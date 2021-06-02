/****************************************************************************
 *  This file is part of PPMd project                                       *
 *  Written and distributed to public domain by Dmitry Shkarin 1997,        *
 *  1999-2001                                                               *
 *  Contents: PPMII model description and encoding/decoding routines        *
 ****************************************************************************/
#include "pch.h"
#include <xray/compressor_ppmd.h>
#include <xray/os_include.h>
#include <xray/memory_buffer.h>
#include <string.h>
#include <stdio.h>

#pragma message( XRAY_TODO("PS3: PPMD compressor") )

#if !XRAY_PLATFORM_PS3

#pragma warning (push)
#pragma warning (disable: 4244)
#pragma warning (disable: 4189)
#pragma warning (disable: 4389)

#define _WIN32_ENVIRONMENT_
//#define _DOS32_ENVIRONMENT_
//#define _POSIX_ENVIRONMENT_
//#define _UNKNOWN_ENVIRONMENT_
#if defined(_WIN32_ENVIRONMENT_)+defined(_DOS32_ENVIRONMENT_)+defined(_POSIX_ENVIRONMENT_)+defined(_UNKNOWN_ENVIRONMENT_) != 1
#error Only one environment must be defined
#endif /* defined(_WIN32_ENVIRONMENT_)+defined(_DOS32_ENVIRONMENT_)+defined(_POSIX_ENVIRONMENT_)+defined(_UNKNOWN_ENVIRONMENT_) != 1 */

#if defined(_WIN32_ENVIRONMENT_)

//	#include <windows.h>

#else /* _DOS32_ENVIRONMENT_ || _POSIX_ENVIRONMENT_ || _UNKNOWN_ENVIRONMENT_ */
typedef int   BOOL;
#define FALSE 0
#define TRUE  1
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   UINT;
#endif /* defined(_WIN32_ENVIRONMENT_)  */

const DWORD PPMdSignature=0x84ACAF8F, Variant='I';
const int MAX_O=16;                         /* maximum allowed model order  */

#define _USE_PREFETCHING                    /* for puzzling mainly          */

#if !defined(_UNKNOWN_ENVIRONMENT_) && !defined(__GNUC__)
#define _FASTCALL __fastcall
#define _STDCALL  __stdcall
#else
#define _FASTCALL
#define _STDCALL
#endif /* !defined(_UNKNOWN_ENVIRONMENT_) && !defined(__GNUC__) */

#if defined(__GNUC__)
#define _PACK_ATTR __attribute__ ((packed))
#else /* "#pragma pack" is used for other compilers */
#define _PACK_ATTR
#endif /* defined(__GNUC__) */

/* PPMd module works with file streams via ...GETC/...PUTC macros only      */
#ifndef ASSERT
#	define ASSERT(a)
#	define ASSERT(a,b)
#	define ASSERT_is_defined_in_ppmdtype_h
#endif // #ifndef ASSERT
	typedef unsigned char	u8;
	typedef unsigned int	u32;
#	include "compressor_ppmd_stream.h"

#ifdef ASSERT_is_defined_in_ppmdtype_h
#	undef ASSERT
#	undef ASSERT2
#	undef ASSERT_is_defined_in_ppmdtype_h
#endif // #ifdef ASSERT_is_defined_in_ppmdtype_h

#include "compressor_ppmd_stream.h"
typedef compression::ppmd::stream _PPMD_FILE;
#define _PPMD_E_GETC(fp)   fp->get_char()
#define _PPMD_E_PUTC(c,fp) fp->put_char(u8(c))
#define _PPMD_D_GETC(fp)   fp->get_char()
#define _PPMD_D_PUTC(c,fp) fp->put_char(u8(c))

extern "C" {

/****************************************************************************
 * (MaxOrder == 1) parameter value has special meaning, it does not restart *
 * model and can be used for solid mode archives;                           *
 * Call sequence:                                                           *
 *     StartSubAllocator(SubAllocatorSize);                                 *
 *     EncodeFile(SolidArcFile,File1,MaxOrder,MRM_RESTART);                 *
 *     EncodeFile(SolidArcFile,File2,       1,MRM_RESTART);                 *
 *     ...                                                                  *
 *     EncodeFile(SolidArcFile,FileN,       1,MRM_RESTART);                 *
 *     StopSubAllocator();                                                  *
 ****************************************************************************/
/*  imported function                                                       */
void _STDCALL  PrintInfo(_PPMD_FILE* DecodedFile,_PPMD_FILE* EncodedFile);

}

#pragma hdrstop
#include "compressor_ppmd_allocator.h"

#define CP printf("%s : %i\n",__FILE__,__LINE__ );

enum { UP_FREQ=5, INT_BITS=7, PERIOD_BITS=7, TOT_BITS=INT_BITS+PERIOD_BITS,
    INTERVAL=1 << INT_BITS, BIN_SCALE=1 << TOT_BITS, MAX_FREQ=124, O_BOUND=9 };

compression::ppmd::stream*	trained_model = 0;

template <class TMP_TYPE>
inline TMP_TYPE CLAMP(const TMP_TYPE& X,const TMP_TYPE& LoX,const TMP_TYPE& HiX) { return (X < LoX)?(LoX):((HiX < X)?(HiX):(X)); }

void _STDCALL PrintInfo		(_PPMD_FILE* DecodedFile,_PPMD_FILE* EncodedFile)
{
	XRAY_UNREFERENCED_PARAMETERS	(DecodedFile, EncodedFile);
}

template <typename T1, typename T2>
inline void   correct_add (T1 & dest, T2 addic)
{
	dest = ( dest + (addic & T1(-1)) ) & T1(-1);
}

//--------------------------------------------------------
// SEE-contexts for PPM-contexts with masked symbols
//--------------------------------------------------------

#pragma pack(1)
struct SEE2_CONTEXT 
{ 
    WORD Summ;
    BYTE Shift, Count;
    void init(UINT InitVal) { Summ=InitVal << (Shift=PERIOD_BITS-4); Count=7; }
    UINT getMean() {
        UINT RetVal=(Summ >> Shift);        Summ -= RetVal;
        return RetVal+(RetVal == 0);
    }
    void update() {
        if (Shift < PERIOD_BITS && --Count == 0) {
            Summ += Summ;                   Count=3 << Shift++;
        }
    }
};

//--------------------------------------------------------
// struct PPM_CONTEXT
//--------------------------------------------------------

class ppmd_compressor_impl;

struct PPM_CONTEXT 
{
    BYTE NumStats, Flags;                   // 1. NumStats & NumMasked contain
    WORD SummFreq;                          //  number of symbols minus 1
    struct STATE {                          // 2. sizeof(WORD) > sizeof(BYTE)
        BYTE Symbol, Freq;                  // 3. contexts example:
        PPM_CONTEXT* Successor;             // MaxOrder:
    };

    STATE _PACK_ATTR*       Stats;          //  ABCD    context
    PPM_CONTEXT*            Suffix;         //   BCD    suffix
    
    inline void             encodeBinSymbol(ppmd_compressor_impl* impl, int symbol);//   BCDE   successor
    inline void             encodeSymbol1(ppmd_compressor_impl* impl, int symbol);// other orders:
    inline void             encodeSymbol2(ppmd_compressor_impl* impl, int symbol);//   BCD    context
    inline void             decodeBinSymbol(ppmd_compressor_impl* impl) const;//    CD    suffix
    inline void             decodeSymbol1(ppmd_compressor_impl* impl);//   BCDE   successor
    inline void             decodeSymbol2(ppmd_compressor_impl* impl);
    inline void             update1(ppmd_compressor_impl* impl, STATE* p);
    inline void             update2(ppmd_compressor_impl* impl, STATE* p);
    inline SEE2_CONTEXT*    makeEscFreq2(ppmd_compressor_impl* impl) const;
    void                    rescale(ppmd_compressor_impl* impl);
    void                    refresh(ppmd_compressor_impl* impl, int OldNU,DWORD Scale);
    PPM_CONTEXT*            cutOff(ppmd_compressor_impl* impl, int Order);
    PPM_CONTEXT*            removeBinConts(ppmd_compressor_impl* impl, int Order);
    void                    makeSuffix();
    STATE&                  oneState() const { return (STATE&) SummFreq; }

    void                    read(ppmd_compressor_impl* impl, _PPMD_FILE* fp,UINT PrevSym);
};
#pragma pack()

static char compile_assert_helper[sizeof(PPM_CONTEXT) == UNIT_SIZE];

enum { TOP=1 << 24, BOT=1 << 15 };

struct SUBRANGE 
{ 
    DWORD low, high, scale; 
};

//--------------------------------------------------------
// class ppmd_compressor_impl
//--------------------------------------------------------
class ppmd_compressor_impl
{
public:

	typedef xray::ppmd_compressor::model_restoration_enum	MR_METHOD;

							ppmd_compressor_impl (xray::memory::base_allocator* allocator, 
												  u32 sub_allocator_size,
												  MR_METHOD mr_method);

	virtual void			compress		(xray::const_buffer src, xray::mutable_buffer dest, u32& out_size);
	virtual void			decompress		(xray::const_buffer src, xray::mutable_buffer dest, u32& out_size);

	void _STDCALL			StartModelRare	(int MaxOrder, MR_METHOD MRMethod);
	void					RestoreModelRare (PPM_CONTEXT* pc1,PPM_CONTEXT* MinContext, PPM_CONTEXT* FSuccessor);
	PPM_CONTEXT* _FASTCALL  ReduceOrder		(PPM_CONTEXT::STATE* p,PPM_CONTEXT* pc);
	PPM_CONTEXT* _FASTCALL  CreateSuccessors (BOOL Skip,PPM_CONTEXT::STATE* p, PPM_CONTEXT* pc);
	void					UpdateModel		(PPM_CONTEXT* MinContext);
	void					ClearMask		(_PPMD_FILE* EncodedFile,_PPMD_FILE* DecodedFile);
	void _STDCALL			EncodeFile		(_PPMD_FILE* EncodedFile,_PPMD_FILE* DecodedFile, int MaxOrder,MR_METHOD MRMethod);
	void _STDCALL			DecodeFile		(_PPMD_FILE* DecodedFile,_PPMD_FILE* EncodedFile, int MaxOrder,MR_METHOD MRMethod);

	void					rcEncNormalize	(_PPMD_FILE* stream);
	void					rcInitEncoder	();
	void					rcEncodeSymbol	();
	void					rcFlushEncoder	(_PPMD_FILE* stream);
	void					rcInitDecoder	(_PPMD_FILE* stream);
	void					rcDecNormalize	(_PPMD_FILE* stream);
	UINT					rcGetCurrentCount ();
	void   					rcRemoveSubrange();
	UINT   					rcBinStart		(UINT f0, UINT Shift);
	UINT   					rcBinDecode  	(UINT tmp);
	void   					rcBinCorrect0	(UINT tmp);
	void   					rcBinCorrect1	(UINT tmp, UINT f1);

public:
    bool					StartModelRare_first_time;
    PPM_CONTEXT*			StartModelRare_context;

	ppmd_allocator			m_allocator;
	SEE2_CONTEXT _PACK_ATTR SEE2Cont[24][32], DummySEE2Cont;
	PPM_CONTEXT _PACK_ATTR* MaxContext;
	BYTE					NS2BSIndx[256], QTable[260]; // constants
	PPM_CONTEXT::STATE*		FoundState;      // found next state transition
	int						InitEsc, OrderFall, RunLength, InitRL, MaxOrder;
	BYTE					CharMask[256], NumMasked, PrevSuccess, EscCount, PrintCount;
	WORD					BinSumm[25][64]; // binary SEE-contexts
	MR_METHOD				MRMethod;
	MR_METHOD				m_MRMethod;
	SUBRANGE				m_SubRange;
	DWORD					m_low, m_code, m_range;
};

inline 
void   ppmd_compressor_impl::rcEncNormalize (_PPMD_FILE* stream)
{
#pragma warning (push)
#pragma warning (disable:4146)
    while(      (m_low ^ (m_low+m_range)) < TOP 
            ||  m_range < BOT 
            &&  ((m_range= -m_low & (BOT-1)),1)
         ) 
    {
        _PPMD_E_PUTC( m_low >> 24, stream );
        m_range   <<= 8;                        
        m_low     <<= 8;
    }
#pragma warning (pop)
}

inline 
void   ppmd_compressor_impl::rcInitEncoder() { m_low=0; m_range=DWORD(-1); }
/*
#define RC_ENC_NORMALIZE(stream) {                                          \
    while ((low ^ (low+range)) < TOP || range < BOT &&                      \
            ((range= -low & (BOT-1)),1)) {                                  \
        _PPMD_E_PUTC(low >> 24,stream);                                     \
        range <<= 8;                        low <<= 8;                      \
    }                                                                       \
}
*/

inline 
void   ppmd_compressor_impl::rcEncodeSymbol()
{
    m_low += m_SubRange.low*(m_range/=m_SubRange.scale);  m_range *= m_SubRange.high-m_SubRange.low;
}

inline 
void   ppmd_compressor_impl::rcFlushEncoder (_PPMD_FILE* stream)
{
    for (UINT i=0;i < 4;i++) {
        _PPMD_E_PUTC(m_low >> 24,stream);     m_low <<= 8;
    }
}

inline 
void   ppmd_compressor_impl::rcInitDecoder (_PPMD_FILE* stream)
{
    m_low=m_code=0;                             m_range=DWORD(-1);
    for (UINT i=0;i < 4;i++)
            m_code=(m_code << 8) | _PPMD_D_GETC(stream);
}

inline 
void   ppmd_compressor_impl::rcDecNormalize( _PPMD_FILE* stream )
{
#pragma warning (push)
#pragma warning (disable:4146)
    while(      (m_low ^ (m_low+m_range)) < TOP 
            ||  m_range < BOT 
            &&  ((m_range= -m_low & (BOT-1)),1) 
         ) 
    {
        m_code    = (m_code << 8) | _PPMD_D_GETC(stream);
        m_range <<= 8;                        
        m_low   <<= 8;
    }
#pragma warning (pop)
}

/*
#define RC_DEC_NORMALIZE(stream) {                                          \
    while ((low ^ (low+range)) < TOP || range < BOT &&                      \
            ((range= -low & (BOT-1)),1)) {                                  \
        code=(code << 8) | _PPMD_D_GETC(stream);                            \
        range <<= 8;                        low <<= 8;                      \
    }                                                                       \
}
*/

inline 
UINT   ppmd_compressor_impl::rcGetCurrentCount () { return (m_code-m_low)/(m_range /= m_SubRange.scale); }

inline 
void   ppmd_compressor_impl::rcRemoveSubrange()
{
    m_low     += m_range*m_SubRange.low;                 
    m_range   *= m_SubRange.high-m_SubRange.low;
}

inline 
UINT   ppmd_compressor_impl::rcBinStart(UINT f0,UINT Shift)  { return f0*(m_range >>= Shift); }
inline 
UINT   ppmd_compressor_impl::rcBinDecode  (UINT tmp)         { return (m_code-m_low >= tmp); }
inline 
void   ppmd_compressor_impl::rcBinCorrect0(UINT tmp)         { m_range=tmp; }
inline 
void   ppmd_compressor_impl::rcBinCorrect1(UINT tmp,UINT f1) { m_low += tmp;   m_range *= f1; }

//--------------------------------------------------------
// ppmd_compressor implementation
//--------------------------------------------------------

namespace xray {

ppmd_compressor::ppmd_compressor (memory::base_allocator* const		allocator, 
								  u32	const						sub_allocator_size_mb,
								  model_restoration_enum const		model_restoration)
{
	m_model_restoration	=	model_restoration;
	m_allocator			=	allocator;
	m_impl				=	XRAY_NEW_IMPL(m_allocator, ppmd_compressor_impl) 
							(allocator, sub_allocator_size_mb, model_restoration);
	R_ASSERT				(m_impl);
}

signalling_bool   ppmd_compressor::compress (const_buffer src, mutable_buffer dest, u32& out_size)
{
	m_impl->compress		(src, dest, out_size);
	return					true;
}

signalling_bool   ppmd_compressor::decompress (const_buffer src, mutable_buffer dest, u32& out_size)
{
	m_impl->decompress		(src, dest, out_size);
	return					true;
}

ppmd_compressor::~ppmd_compressor ()
{
	XRAY_DELETE_IMPL		(m_allocator, m_impl);
}

} // namespace xray


//--------------------------------------------------------
// ppmd_compressor_impl implementation
//--------------------------------------------------------

const u32		order_model					=	8;

void   ppmd_compressor_impl::compress (xray::const_buffer src, xray::mutable_buffer dest, u32& out_size)
{
	typedef		compression::ppmd::stream	stream;

	stream	source_stream	(src.c_ptr(), src.size());
	stream	dest_stream		(dest.c_ptr(), dest.size());
	EncodeFile				(&dest_stream, &source_stream, order_model, m_MRMethod);

	out_size			=	dest_stream.tell() + 1;
}

void   ppmd_compressor_impl::decompress (xray::const_buffer src, xray::mutable_buffer dest, u32& out_size)
{
	typedef		compression::ppmd::stream	stream;

	stream	source_stream	(src.c_ptr(), src.size());
	stream	dest_stream		(dest.c_ptr(), dest.size());
	DecodeFile				(&dest_stream, &source_stream, order_model, m_MRMethod);

	out_size			=	dest_stream.tell();
}

//--------------------------------------------------------
// ppmd implementation
//--------------------------------------------------------

inline 
void   SWAP (PPM_CONTEXT::STATE& s1, PPM_CONTEXT::STATE& s2)
{
    WORD t1=(WORD&) s1;                     PPM_CONTEXT* t2=s1.Successor;
    (WORD&) s1 = (WORD&) s2;                s1.Successor=s2.Successor;
    (WORD&) s2 = t1;                        s2.Successor=t2;
}

inline 
void   StateCpy (PPM_CONTEXT::STATE& s1, const PPM_CONTEXT::STATE& s2)
{
    (WORD&) s1=(WORD&) s2;                  
    s1.Successor=s2.Successor;
}

ppmd_compressor_impl::ppmd_compressor_impl (xray::memory::base_allocator* const allocator, 
											u32 const							sub_allocator_size,
											MR_METHOD const						mr_method) 
											: m_allocator(allocator, sub_allocator_size)
{
	m_MRMethod							=	mr_method;
    StartModelRare_first_time			=	true;
    StartModelRare_context				=	0;

	m_SubRange.low						=	0;
	m_SubRange.high						=	0;
	m_SubRange.scale					=	0;
	m_low								=	0;
	m_code								=	0;
	m_range								=	0;

    NS2BSIndx[0]						=	2*0;                       
	NS2BSIndx[1]						=	2*1;
    memset									(NS2BSIndx+2, 2*2, 9); 
	memset									(NS2BSIndx+11, 2*3, 256-11);

	UINT i, k, m, Step;

	for (i=0;i<UP_FREQ;++i)              
	{
		QTable[i]						=	i;
	}
    for (m=i=UP_FREQ, k=Step=1; i<260; ++i) 
	{
        QTable[i]						=	m;
        if ( !--k ) 
		{ 
			k							=	++Step;           
			++m; 
		}
    }
    (DWORD&) DummySEE2Cont				=	PPMdSignature;
}

void   PPM_CONTEXT::makeSuffix ()
{
    STATE* p, * p1;
    if ( !NumStats ) 
    {
        if ( !(p=&oneState())->Successor )  return;

        if ( !Suffix->NumStats )            
        {
            p1=&(Suffix->oneState());
        }
        else
        {
            for (p1=Suffix->Stats;p1->Symbol != p->Symbol;p1++)
                ;
        }
        
        p->Successor->Suffix = p1->Successor; 
        p->Successor->makeSuffix();
    } 
    else 
    {
        for (p=Stats;p <= Stats+NumStats;p++) 
        {
            if ( !p->Successor )            
                continue;

            if ( !Suffix )                  
            {
                p->Successor->Suffix=this;
            }
            else 
            {
                for (p1=Suffix->Stats;p1->Symbol != p->Symbol;p1++)
                    ;
                p->Successor->Suffix=p1->Successor;
            }
            p->Successor->makeSuffix();
        }
    }
}

void   PPM_CONTEXT::read (ppmd_compressor_impl* impl, _PPMD_FILE* fp,UINT PrevSym)
{
    STATE* p;                               Suffix=NULL;
    NumStats=_PPMD_E_GETC(fp);              Flags=0x10*(PrevSym >= 0x40);
    if ( !NumStats ) {
        p=&oneState();                      p->Freq=_PPMD_E_GETC(fp);
        Flags |= 0x08*((p->Symbol=_PPMD_E_GETC(fp)) >= 0x40);
        if ((p->Freq & 0x80) != 0) {
            p->Freq &= ~0x80;
            p->Successor = (PPM_CONTEXT*) impl->m_allocator.AllocContext();
            p->Successor->read(impl, fp,p->Symbol);
        } else                              p->Successor=NULL;
        return;
    }
    Stats = (PPM_CONTEXT::STATE*) impl->m_allocator.AllocUnits((NumStats+2) >> 1);
    for (p=Stats;p <= Stats+NumStats;p++) {
        p->Freq=_PPMD_E_GETC(fp);
        Flags |= 0x08*((p->Symbol=_PPMD_E_GETC(fp)) >= 0x40);
    }
    int EscFreq=SummFreq=(Stats->Freq & ~0x80);
    Flags |= 0x04*(EscFreq < NumStats && EscFreq < 127);
    for (p=Stats;p <= Stats+NumStats;p++) {
        if ((p->Freq & 0x80) != 0) {
            p->Freq &= ~0x80;
            p->Successor = (PPM_CONTEXT*) impl->m_allocator.AllocContext();
            p->Successor->read(impl,fp,p->Symbol);
        } else                              p->Successor=NULL;
        p->Freq=(p == Stats)?(64):(p[-1].Freq-p[0].Freq);
        SummFreq += p->Freq;
    }
    if (EscFreq > 32) {
        SummFreq=(EscFreq >> 1);
        for (p=Stats;p <= Stats+NumStats;p++)
                SummFreq += (p->Freq -= (3*p->Freq) >> 2);
    }
}
/*
void
PPM_CONTEXT::write( int o, FILE* fp )
{
    int EscFreq = EscCount; //(Stats)  ? Stats->Freq & ~0x80  : 0;
    int nc      = 0;

    STATE* p;
    int f, a, b, c;
    if (nc < o)                             
        nc=o;
        
    putc(NumStats,fp);
    if ( !NumStats ) {
        f=(p=&oneState())->Freq;
        if ( EscFreq )                      f=(2*f)/EscFreq;
        f=CLAMP(f,1,127) | 0x80*(p->Successor != NULL);
        putc(f,fp);                         putc(p->Symbol,fp);
        if ( p->Successor )                 p->Successor->write(o+1,fp);
        return;
    }
    for (p=Stats+1;p <= Stats+NumStats;p++) {
        if (p[0].Freq > p[-1].Freq) {
            STATE* p1=p;
            do { SWAP(p1[0],p1[-1]); } while (--p1 != Stats && p1[0].Freq > p1[-1].Freq);
        }
        if (p[0].Freq == p[-1].Freq && p[0].Successor && !p[-1].Successor) {
            STATE* p1=p;
            do { SWAP(p1[0],p1[-1]); } while (--p1 != Stats && p1[0].Freq == p1[-1].Freq && !p1[-1].Successor);
        }
    }
    a=Stats->Freq+!Stats->Freq;             f=(64*EscFreq+(b=a >> 1))/a;
    f=CLAMP(f,1,127) | 0x80*(Stats->Successor != NULL);
    putc(f,fp);                             c=64;
    for (p=Stats;p <= Stats+NumStats;p++) {
        f=(64*p->Freq+b)/a;                 f += !f;
        if (p != Stats)
            putc((c-f) | 0x80*(p->Successor != NULL),fp);
        c=f;                                putc(p->Symbol,fp);
    }
    for (p=Stats;p <= Stats+NumStats;p++)
            if ( p->Successor )             p->Successor->write(o+1,fp);
}
*/
void   PPM_CONTEXT::refresh (ppmd_compressor_impl* impl, int OldNU, DWORD Scale)
{
    int i=NumStats, EscFreq;
    STATE* p = Stats = (STATE*) impl->m_allocator.ShrinkUnits(Stats,OldNU,(i+2) >> 1);
    Flags=(Flags & (0x10+0x04*Scale))+0x08*(p->Symbol >= 0x40);
    EscFreq=SummFreq-p->Freq;
    SummFreq = (p->Freq=(p->Freq+Scale) >> Scale);
    do {
        EscFreq -= (++p)->Freq;
        SummFreq += (p->Freq=(p->Freq+Scale) >> Scale);
        Flags |= 0x08*(p->Symbol >= 0x40);
    } while ( --i );
    SummFreq += (EscFreq=(EscFreq+Scale) >> Scale);
}

#define P_CALL(F) ( PrefetchData(p->Successor), \
                    p->Successor=p->Successor->F(impl, Order+1))

PPM_CONTEXT*   PPM_CONTEXT::cutOff (ppmd_compressor_impl* impl, int Order)
{
    int i, tmp;
    STATE* p;
    if ( !NumStats ) {
        if ((BYTE*) (p=&oneState())->Successor >= impl->m_allocator.UnitsStart) {
            if (Order < impl->MaxOrder)           P_CALL(cutOff);
            else                            p->Successor=NULL;
            if (!p->Successor && Order > O_BOUND)
                    goto REMOVE;
            return this;
        } else {
REMOVE:     impl->m_allocator.SpecialFreeUnit(this);          return NULL;
        }
    }
    PrefetchData(Stats);
    Stats = (STATE*) impl->m_allocator.MoveUnitsUp(Stats,tmp=(NumStats+2) >> 1);
    for (p=Stats+(i=NumStats);p >= Stats;p--)
            if ((BYTE*) p->Successor < impl->m_allocator.UnitsStart) {
                p->Successor=NULL;          SWAP(*p,Stats[i--]);
            } else if (Order < impl->MaxOrder)    P_CALL(cutOff);
            else                            p->Successor=NULL;
    if (i != NumStats && Order) {
        NumStats=i;                         p=Stats;
        if (i < 0) { impl->m_allocator.FreeUnits(p,tmp);      goto REMOVE; }
        else if (i == 0) {
            Flags=(Flags & 0x10)+0x08*(p->Symbol >= 0x40);
            StateCpy(oneState(),*p);        impl->m_allocator.FreeUnits(p,tmp);
            oneState().Freq=(oneState().Freq+11) >> 3;
        } else                              refresh(impl, tmp,SummFreq > 16*i);
    }
    return this;
}

PPM_CONTEXT*   PPM_CONTEXT::removeBinConts (ppmd_compressor_impl* impl, int Order)
{
    STATE* p;
    if ( !NumStats ) {
        p=&oneState();
        if ((BYTE*) p->Successor >= impl->m_allocator.UnitsStart && Order < impl->MaxOrder)
                P_CALL(removeBinConts);
        else                                p->Successor=NULL;
        if (!p->Successor && (!Suffix->NumStats || Suffix->Flags == 0xFF)) {
            impl->m_allocator.FreeUnits(this,1);              return NULL;
        } else                              return this;
    }
    PrefetchData(Stats);
    for (p=Stats+NumStats;p >= Stats;p--)
            if ((BYTE*) p->Successor >= impl->m_allocator.UnitsStart && Order < impl->MaxOrder)
                    P_CALL(removeBinConts);
            else                            p->Successor=NULL;
    return this;
}

void   ppmd_compressor_impl::RestoreModelRare (PPM_CONTEXT* pc1,PPM_CONTEXT* MinContext, PPM_CONTEXT* FSuccessor)
{
    PPM_CONTEXT* pc;
    PPM_CONTEXT::STATE* p;
    for (pc=MaxContext, m_allocator.pText=m_allocator.HeapStart;pc != pc1;pc=pc->Suffix)
            if (--(pc->NumStats) == 0) {
                pc->Flags=(pc->Flags & 0x10)+0x08*(pc->Stats->Symbol >= 0x40);
                p=pc->Stats;                StateCpy(pc->oneState(),*p);
                m_allocator.SpecialFreeUnit(p);
                pc->oneState().Freq=(pc->oneState().Freq+11) >> 3;
            } else
                    pc->refresh(this, (pc->NumStats+3) >> 1,FALSE);
    for ( ;pc != MinContext;pc=pc->Suffix)
            if ( !pc->NumStats )
                    pc->oneState().Freq -= pc->oneState().Freq >> 1;
            else if ((pc->SummFreq += 4) > 128+4*pc->NumStats)
                    pc->refresh(this, (pc->NumStats+2) >> 1,TRUE);
	if (MRMethod > xray::ppmd_compressor::model_restoration_freeze ) {
        MaxContext=FSuccessor;              m_allocator.GlueCount += !( m_allocator.BList[1].Stamp & 1);
    } else if (MRMethod == xray::ppmd_compressor::model_restoration_freeze ) {
        while ( MaxContext->Suffix )        MaxContext=MaxContext->Suffix;
        MaxContext->removeBinConts(this, 0);      MRMethod=MR_METHOD(MRMethod+1);
        m_allocator.GlueCount=0;                        OrderFall=MaxOrder;
	} else if (MRMethod == xray::ppmd_compressor::model_restoration_restart || m_allocator.GetUsedMemory() < (m_allocator.SubAllocatorSize >> 1)) {
        StartModelRare(MaxOrder,MRMethod);
        EscCount=0;                         PrintCount=0xFF;
    } else {
        while ( MaxContext->Suffix )        MaxContext=MaxContext->Suffix;
        do {
            MaxContext->cutOff(this, 0);          m_allocator.ExpandTextArea();
		} while (m_allocator.GetUsedMemory() > 3*(m_allocator.SubAllocatorSize >> 2));
        m_allocator.GlueCount=0;                        OrderFall=MaxOrder;
    }
}

PPM_CONTEXT* _FASTCALL   ppmd_compressor_impl::ReduceOrder (PPM_CONTEXT::STATE* p,PPM_CONTEXT* pc)
{
    PPM_CONTEXT::STATE* p1,  * ps[MAX_O], ** pps=ps;
    PPM_CONTEXT* pc1=pc, * UpBranch = (PPM_CONTEXT*) m_allocator.pText;
    BYTE tmp, sym=FoundState->Symbol;
    *pps++ = FoundState;                    FoundState->Successor=UpBranch;
    OrderFall++;
    if ( p ) { pc=pc->Suffix;               goto LOOP_ENTRY; }
    for ( ; ; ) {
        if ( !pc->Suffix ) {
            if (MRMethod > xray::ppmd_compressor::model_restoration_freeze) {
FROZEN:         do { (*--pps)->Successor = pc; } while (pps != ps);
                m_allocator.pText=m_allocator.HeapStart+1;          OrderFall=1;
            }
            return pc;
        }
        pc=pc->Suffix;
        if ( pc->NumStats ) {
            if ((p=pc->Stats)->Symbol != sym)
                    do { tmp=p[1].Symbol;   p++; } while (tmp != sym);
            tmp=2*(p->Freq < MAX_FREQ-9);
            p->Freq += tmp;                 pc->SummFreq += tmp;
        } else { p=&(pc->oneState());       p->Freq += (p->Freq < 32); }
LOOP_ENTRY:
        if ( p->Successor )                 break;
        *pps++ = p;                         p->Successor=UpBranch;
        OrderFall++;
    }
    if (MRMethod > xray::ppmd_compressor::model_restoration_freeze) {
        pc = p->Successor;                  goto FROZEN;
    } else if (p->Successor <= UpBranch) {
        p1=FoundState;                      FoundState=p;
        p->Successor=CreateSuccessors(FALSE,NULL,pc);
        FoundState=p1;
    }
    if (OrderFall == 1 && pc1 == MaxContext) {
        FoundState->Successor=p->Successor; m_allocator.pText--;
    }
    return p->Successor;
}

void   PPM_CONTEXT::rescale (ppmd_compressor_impl* impl)
{
    UINT OldNU, Adder, EscFreq, i=NumStats;
    STATE tmp, * p1, * p;
    for (p=impl->FoundState;p != Stats;p--)       SWAP(p[0],p[-1]);
    p->Freq += 4;                           SummFreq += 4;
    EscFreq=SummFreq-p->Freq;
    Adder=(impl->OrderFall != 0 || impl->MRMethod > xray::ppmd_compressor::model_restoration_freeze);
    SummFreq = (p->Freq=(p->Freq+Adder) >> 1);
    do {
        EscFreq -= (++p)->Freq;
        SummFreq += (p->Freq=(p->Freq+Adder) >> 1);
        if (p[0].Freq > p[-1].Freq) {
            StateCpy(tmp,*(p1=p));
            do StateCpy(p1[0],p1[-1]); while (tmp.Freq > (--p1)[-1].Freq);
            StateCpy(*p1,tmp);
        }
    } while ( --i );
    if (p->Freq == 0) {
        do { i++; } while ((--p)->Freq == 0);
        EscFreq += i;                       OldNU=(NumStats+2) >> 1;
        if ((NumStats -= i) == 0) {
            StateCpy(tmp,*Stats);
            tmp.Freq=(2*tmp.Freq+EscFreq-1)/EscFreq;
            if (tmp.Freq > MAX_FREQ/3)      tmp.Freq=MAX_FREQ/3;
            impl->m_allocator.FreeUnits(Stats,OldNU);         StateCpy(oneState(),tmp);
            Flags=(Flags & 0x10)+0x08*(tmp.Symbol >= 0x40);
            impl->FoundState=&oneState();         return;
        }
        Stats = (STATE*) impl->m_allocator.ShrinkUnits(Stats,OldNU,(NumStats+2) >> 1);
        Flags &= ~0x08;                     i=NumStats;
        Flags |= 0x08*((p=Stats)->Symbol >= 0x40);
        do { Flags |= 0x08*((++p)->Symbol >= 0x40); } while ( --i );
    }
    SummFreq += (EscFreq -= (EscFreq >> 1));
    Flags |= 0x04;                          impl->FoundState=Stats;
}

PPM_CONTEXT* _FASTCALL   ppmd_compressor_impl::CreateSuccessors (BOOL Skip,PPM_CONTEXT::STATE* p, PPM_CONTEXT* pc)
{
    PPM_CONTEXT ct, * UpBranch=FoundState->Successor;
    PPM_CONTEXT::STATE* ps[MAX_O], ** pps=ps;
    UINT cf, s0;
    BYTE tmp, sym=FoundState->Symbol;
    if ( !Skip ) {
        *pps++ = FoundState;
        if ( !pc->Suffix )                  goto NO_LOOP;
    }
    if ( p ) { pc=pc->Suffix;               goto LOOP_ENTRY; }
    do {
        pc=pc->Suffix;
        if ( pc->NumStats ) {
            if ((p=pc->Stats)->Symbol != sym)
                    do { tmp=p[1].Symbol;   p++; } while (tmp != sym);
            tmp=(p->Freq < MAX_FREQ-9);
            p->Freq += tmp;                 pc->SummFreq += tmp;
        } else {
            p=&(pc->oneState());
            p->Freq += ((!pc->Suffix->NumStats) & (p->Freq < 24));
        }
LOOP_ENTRY:
        if (p->Successor != UpBranch) {
            pc=p->Successor;                break;
        }
        *pps++ = p;
    } while ( pc->Suffix );
NO_LOOP:
    if (pps == ps)                          return pc;
    ct.NumStats=0;                          ct.Flags=0x10*(sym >= 0x40);
    ct.oneState().Symbol=sym=*(BYTE*) UpBranch;
    ct.oneState().Successor=(PPM_CONTEXT*) (((BYTE*) UpBranch)+1);
    ct.Flags |= 0x08*(sym >= 0x40);
    if ( pc->NumStats ) {
        if ((p=pc->Stats)->Symbol != sym)
                do { tmp=p[1].Symbol;       p++; } while (tmp != sym);
        s0=pc->SummFreq-pc->NumStats-(cf=p->Freq-1);
        ct.oneState().Freq=1+((2*cf <= s0)?(5*cf > s0):((cf+2*s0-3)/s0));
    } else
            ct.oneState().Freq=pc->oneState().Freq;
    do {
        PPM_CONTEXT* pc1 = (PPM_CONTEXT*) m_allocator.AllocContext();
        if ( !pc1 )                         return NULL;

//         ((DWORD*) pc1)[0] = ((DWORD*) &ct)[0];
//         ((DWORD*) pc1)[1] = ((DWORD*) &ct)[1];

		pc1->NumStats	=	ct.NumStats;
		pc1->Flags		=	ct.Flags;
		pc1->SummFreq	=	ct.SummFreq;
		pc1->Stats		=	ct.Stats;

        pc1->Suffix		=	pc;                     
		(*--pps)->Successor=pc=pc1;
    } while (pps != ps);
    return pc;
}

inline 
void   ppmd_compressor_impl::UpdateModel (PPM_CONTEXT* MinContext)
{
    PPM_CONTEXT::STATE* p           = NULL;
    PPM_CONTEXT*        FSuccessor  = FoundState->Successor;
    PPM_CONTEXT*        pc          = MinContext->Suffix;
    PPM_CONTEXT*        pc1         = MaxContext;
    
    UINT ns1, ns, cf, sf, s0, FFreq=FoundState->Freq;
    BYTE Flag, sym, FSymbol=FoundState->Symbol;


    if( FFreq < MAX_FREQ/4  &&  pc ) 
    {
        if ( pc->NumStats ) 
        {
            if ((p=pc->Stats)->Symbol != FSymbol) 
            {
                do { sym=p[1].Symbol;       p++; } while (sym != FSymbol);
                if (p[0].Freq >= p[-1].Freq) 
                {
                    SWAP(p[0],p[-1]);       
                    p--;
                }
            }
            cf=2*(p->Freq < MAX_FREQ-9);
            p->Freq      += cf;                  
            pc->SummFreq += cf;
        } 
        else 
        { 
            p=&(pc->oneState());       
            p->Freq += (p->Freq < 32); 
        }
    }
    
    if( !OrderFall && FSuccessor) 
    {
        FoundState->Successor=CreateSuccessors(TRUE,p,MinContext);
        if ( !FoundState->Successor )       
            goto RESTART_MODEL;
        MaxContext=FoundState->Successor;   
        return;
    }

    *m_allocator.pText++ = FSymbol;                     

    PPM_CONTEXT*    Successor = (PPM_CONTEXT*) m_allocator.pText;
    
    if (m_allocator.pText >= m_allocator.UnitsStart)                
        goto RESTART_MODEL;


    if( FSuccessor ) 
    {
        if ((BYTE*) FSuccessor < m_allocator.UnitsStart)
            FSuccessor=CreateSuccessors(FALSE,p,MinContext);
    } 
    else
    {
        FSuccessor=ReduceOrder(p,MinContext);
    }
    
    if( !FSuccessor )                          
        goto RESTART_MODEL;


    if( !--OrderFall ) 
    {
        Successor=FSuccessor;               
        m_allocator.pText -= (MaxContext != MinContext);
    } 
    else if( MRMethod > xray::ppmd_compressor::model_restoration_freeze) 
    {
        Successor=FSuccessor;               
        m_allocator.pText=m_allocator.HeapStart;
        OrderFall=0;
    }
    
    s0=MinContext->SummFreq-(ns=MinContext->NumStats)-FFreq;
    for (Flag=0x08*(FSymbol >= 0x40);pc1 != MinContext;pc1=pc1->Suffix) 
    {
        if ((ns1=pc1->NumStats) != 0) 
        {
            if ((ns1 & 1) != 0) 
            {
                p=(PPM_CONTEXT::STATE*) m_allocator.ExpandUnits(pc1->Stats,(ns1+1) >> 1);
                if ( !p )                   
                    goto RESTART_MODEL;
                pc1->Stats=p;
            }
            pc1->SummFreq += (3*ns1+1 < ns);
        } 
        else 
        {
            p=(PPM_CONTEXT::STATE*) m_allocator.AllocUnits(1);
            if ( !p )                       
                goto RESTART_MODEL;
                
            StateCpy(*p,pc1->oneState());   
            pc1->Stats=p;
            
            if (p->Freq < MAX_FREQ/4-1)     p->Freq += p->Freq;
            else                            p->Freq  = MAX_FREQ-4;
            
            pc1->SummFreq=p->Freq+InitEsc+(ns > 2);
        }

        cf=2*FFreq*(pc1->SummFreq+6);       
        sf=s0+pc1->SummFreq;

        if (cf < 6*sf) 
        {
            cf=1+(cf > sf)+(cf >= 4*sf);
            pc1->SummFreq += 4;
        } 
        else 
        {
            cf=4+(cf > 9*sf)+(cf > 12*sf)+(cf > 15*sf);
            pc1->SummFreq += cf;
        }
        p=pc1->Stats+(++pc1->NumStats);     
        p->Successor=Successor;
        p->Symbol = FSymbol;                
        p->Freq = cf;
        pc1->Flags |= Flag;
    }

    MaxContext=FSuccessor;
    return;

RESTART_MODEL:
    RestoreModelRare(pc1,MinContext,FSuccessor);
}

// Tabulated escapes for exponential symbol distribution
static const BYTE ExpEscape[16]={ 25,14, 9, 7, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2 };
#define GET_MEAN(SUMM,SHIFT,ROUND) ((SUMM+(1 << (SHIFT-ROUND))) >> (SHIFT))

inline 
void   PPM_CONTEXT::encodeBinSymbol (ppmd_compressor_impl* impl, int symbol)
{
    BYTE indx=impl->NS2BSIndx[Suffix->NumStats]+impl->PrevSuccess+Flags;
    STATE& rs=oneState();
    WORD& bs=impl->BinSumm[impl->QTable[rs.Freq-1]][indx+((impl->RunLength >> 26) & 0x20)];
    UINT tmp=impl->rcBinStart(bs,TOT_BITS);
    if (rs.Symbol == symbol) {
        impl->FoundState=&rs;                     rs.Freq += (rs.Freq < 196);
        impl->rcBinCorrect0(tmp);                 bs += INTERVAL-GET_MEAN(bs,PERIOD_BITS,2);
        impl->PrevSuccess=1;                      impl->RunLength++;
    } else {
        impl->rcBinCorrect1(tmp,BIN_SCALE-bs);    bs -= GET_MEAN(bs,PERIOD_BITS,2);
        impl->InitEsc=ExpEscape[bs >> 10];        impl->CharMask[rs.Symbol]=impl->EscCount;
        impl->NumMasked=impl->PrevSuccess=0;            impl->FoundState=NULL;
    }
}

inline 
void   PPM_CONTEXT::decodeBinSymbol (ppmd_compressor_impl* impl) const
{
    BYTE indx=impl->NS2BSIndx[Suffix->NumStats]+impl->PrevSuccess+Flags;
    STATE& rs=oneState();

    WORD& bs=impl->BinSumm[impl->QTable[rs.Freq-1]][indx+((impl->RunLength >> 26) & 0x20)];
    UINT tmp=impl->rcBinStart(bs,TOT_BITS);
    if ( !impl->rcBinDecode(tmp) ) {
        impl->FoundState=&rs;                     rs.Freq += (rs.Freq < 196);
        impl->rcBinCorrect0(tmp);                 bs += INTERVAL-GET_MEAN(bs,PERIOD_BITS,2);
        impl->PrevSuccess=1;                      impl->RunLength++;
    } else {
        impl->rcBinCorrect1(tmp,BIN_SCALE-bs);    bs -= GET_MEAN(bs,PERIOD_BITS,2);
        impl->InitEsc=ExpEscape[bs >> 10];        impl->CharMask[rs.Symbol]=impl->EscCount;
        impl->NumMasked=impl->PrevSuccess=0;            impl->FoundState=NULL;
    }
}

inline 
void   PPM_CONTEXT::update1 (ppmd_compressor_impl* impl, STATE* p) 
{
    (impl->FoundState=p)->Freq += 4;              SummFreq += 4;
    if (p[0].Freq > p[-1].Freq) {
        SWAP(p[0],p[-1]);                   impl->FoundState=--p;
        if (p->Freq > MAX_FREQ)             rescale(impl);
    }
}

inline 
void   PPM_CONTEXT::encodeSymbol1 (ppmd_compressor_impl* impl, int symbol)
{
    UINT LoCnt, i=Stats->Symbol;
    STATE* p=Stats;                         impl->m_SubRange.scale=SummFreq;
    if (i == symbol) {
        impl->PrevSuccess=(2*(impl->m_SubRange.high=p->Freq) >= impl->m_SubRange.scale);
        (impl->FoundState=p)->Freq += 4;          SummFreq += 4;
        impl->RunLength += impl->PrevSuccess;
        if (p->Freq > MAX_FREQ)             rescale(impl);
        impl->m_SubRange.low=0;                        return;
    }
    LoCnt=p->Freq;
    i=NumStats;                             impl->PrevSuccess=0;
    while ((++p)->Symbol != symbol) {
        LoCnt += p->Freq;
        if (--i == 0) {
            if ( Suffix )                   PrefetchData(Suffix);
            impl->m_SubRange.low=LoCnt;                impl->CharMask[p->Symbol]=impl->EscCount;
            i=impl->NumMasked=NumStats;           impl->FoundState=NULL;
            do { impl->CharMask[(--p)->Symbol]=impl->EscCount; } while ( --i );
            impl->m_SubRange.high=impl->m_SubRange.scale;         return;
        }
    }
    impl->m_SubRange.high=(impl->m_SubRange.low=LoCnt)+p->Freq;   update1(impl, p);
}

inline 
void   PPM_CONTEXT::decodeSymbol1 (ppmd_compressor_impl* impl)
{
    UINT i, count, HiCnt=Stats->Freq;
    STATE* p=Stats;                         impl->m_SubRange.scale=SummFreq;
    if ((count=impl->rcGetCurrentCount()) < HiCnt) {
        impl->PrevSuccess=(2*(impl->m_SubRange.high=HiCnt) >= impl->m_SubRange.scale);

        (impl->FoundState=p)->Freq=(HiCnt += 4);  
        SummFreq += 4;

        impl->RunLength += impl->PrevSuccess;
        if (HiCnt > MAX_FREQ)               rescale(impl);
        impl->m_SubRange.low=0;                        return;
    }
    i=NumStats;                             impl->PrevSuccess=0;
    while ((HiCnt += (++p)->Freq) <= count)
        if (--i == 0) {
            if ( Suffix )                   PrefetchData(Suffix);
            impl->m_SubRange.low=HiCnt;                impl->CharMask[p->Symbol]=impl->EscCount;
            i=impl->NumMasked=NumStats;           impl->FoundState=NULL;
            do { impl->CharMask[(--p)->Symbol]=impl->EscCount; } while ( --i );
            impl->m_SubRange.high=impl->m_SubRange.scale;         return;
        }
    impl->m_SubRange.low=(impl->m_SubRange.high=HiCnt)-p->Freq;   
    ((PPM_CONTEXT*)this)->update1(impl, p);
}

inline 
void   PPM_CONTEXT::update2 (ppmd_compressor_impl* impl, STATE* p)
{
/*
    ++EscCount;
    RunLength = InitRL;
*/
    (impl->FoundState=p)->Freq += 4;              SummFreq += 4;
    if (p->Freq > MAX_FREQ)                 rescale(impl);
    impl->EscCount++;                             impl->RunLength=impl->InitRL;
}

inline 
SEE2_CONTEXT*   PPM_CONTEXT::makeEscFreq2 (ppmd_compressor_impl* impl) const
{
    BYTE* pb=(BYTE*) Stats;                 UINT t=2*NumStats;
    PrefetchData(pb);                       PrefetchData(pb+t);
    PrefetchData(pb += 2*t);                PrefetchData(pb+t);
    SEE2_CONTEXT* psee2c;
    if (NumStats != 0xFF) {
        t=Suffix->NumStats;
        psee2c=impl->SEE2Cont[impl->QTable[NumStats+2]-3]+(SummFreq > 11*(NumStats+1));
        psee2c += 2*((UINT)2*NumStats < t+impl->NumMasked)+Flags;
        impl->m_SubRange.scale=psee2c->getMean();
    } else {
        psee2c=&impl->DummySEE2Cont;              impl->m_SubRange.scale=1;
    }
    return psee2c;
}

inline 
void   PPM_CONTEXT::encodeSymbol2 (ppmd_compressor_impl* impl, int symbol)
{
    SEE2_CONTEXT* psee2c=makeEscFreq2(impl);
    UINT Sym, LoCnt=0, i=NumStats-impl->NumMasked;
    STATE* p1, * p=Stats-1;
    do {
        do { Sym=p[1].Symbol;   p++; } while (impl->CharMask[Sym] == impl->EscCount);
        impl->CharMask[Sym]=impl->EscCount;
        if (Sym == symbol)                  goto SYMBOL_FOUND;
        LoCnt += p->Freq;
    } while ( --i );
    impl->m_SubRange.high=(impl->m_SubRange.scale += (impl->m_SubRange.low=LoCnt));
    //psee2c->Summ += (WORD)impl->m_SubRange.scale;            
	correct_add(psee2c->Summ, impl->m_SubRange.scale);
	
	impl->NumMasked = NumStats;
    return;
SYMBOL_FOUND:
    impl->m_SubRange.low=LoCnt;                        impl->m_SubRange.high=(LoCnt += p->Freq);
    for (p1=p; --i ; ) {
        do { Sym=p1[1].Symbol;  p1++; } while (impl->CharMask[Sym] == impl->EscCount);
        LoCnt += p1->Freq;
    }
    impl->m_SubRange.scale += LoCnt;
    psee2c->update();                       update2(impl, p);
}

inline 
void   PPM_CONTEXT::decodeSymbol2 (ppmd_compressor_impl* impl)
{
    SEE2_CONTEXT* psee2c=makeEscFreq2(impl);
    UINT Sym, count, HiCnt=0, i=NumStats-impl->NumMasked;
    STATE* ps[256], ** pps=ps, * p=Stats-1;
    do {
        do { Sym=p[1].Symbol;   p++; } while (impl->CharMask[Sym] == impl->EscCount);
        HiCnt += p->Freq;                   *pps++ = p;
    } while ( --i );
    impl->m_SubRange.scale += HiCnt;                   count=impl->rcGetCurrentCount();
    p=*(pps=ps);
    if (count < HiCnt) {
        HiCnt=0;
        while ((HiCnt += p->Freq) <= count) p=*++pps;
        impl->m_SubRange.low = (impl->m_SubRange.high=HiCnt)-p->Freq;
        psee2c->update();                   
        update2(impl, p);
    } else {
        impl->m_SubRange.low=HiCnt;                    impl->m_SubRange.high=impl->m_SubRange.scale;
        i=NumStats-impl->NumMasked;               impl->NumMasked = NumStats;
        do { impl->CharMask[(*pps)->Symbol]=impl->EscCount; pps++; } while ( --i );

		correct_add(psee2c->Summ, impl->m_SubRange.scale);
        //psee2c->Summ = (WORD)(((DWORD)psee2c->Summ + impl->m_SubRange.scale) & 0xffff);
    }
}

inline 
void   ppmd_compressor_impl::ClearMask (_PPMD_FILE* EncodedFile,_PPMD_FILE* DecodedFile)
{
    EscCount=1;                             memset(CharMask,0,sizeof(CharMask));
    if (++PrintCount == 0)                  PrintInfo(DecodedFile,EncodedFile);
}


void _STDCALL   ppmd_compressor_impl::EncodeFile (_PPMD_FILE* EncodedFile,_PPMD_FILE* DecodedFile, int MaxOrder,MR_METHOD MRMethod)
{
    rcInitEncoder();
    StartModelRare(MaxOrder,MRMethod);

    for (PPM_CONTEXT* MinContext; ; ) 
    {
        BYTE    ns  =(MinContext=MaxContext)->NumStats;
        int     c   = _PPMD_E_GETC(DecodedFile);

        if( ns ) 
        {
            MinContext->encodeSymbol1(this, c);   
            rcEncodeSymbol();
        } 
        else
        {
            MinContext->encodeBinSymbol(this, c);
        }
        
        while( !FoundState ) 
        {
            rcEncNormalize(EncodedFile);
            do
            {
                OrderFall++;                
                MinContext=MinContext->Suffix;

                if( !MinContext )          
                    goto STOP_ENCODING;
            } while (MinContext->NumStats == NumMasked);

            MinContext->encodeSymbol2(this, c);   
            rcEncodeSymbol();
        }
        
        if (!OrderFall && (BYTE*) FoundState->Successor >= m_allocator.UnitsStart)
        {
            PrefetchData(MaxContext=FoundState->Successor);
        }
        else 
        {
            UpdateModel(MinContext);        
            PrefetchData(MaxContext);

            if (EscCount == 0 )
                ClearMask(EncodedFile,DecodedFile);
        }
        rcEncNormalize(EncodedFile);
    } // for (PPM_CONTEXT* MinContext; ; )
    
STOP_ENCODING:
    rcFlushEncoder(EncodedFile);            
    PrintInfo(DecodedFile,EncodedFile);
}

void _STDCALL   ppmd_compressor_impl::DecodeFile(_PPMD_FILE* DecodedFile,_PPMD_FILE* EncodedFile, int MaxOrder,MR_METHOD MRMethod)
{
    rcInitDecoder(EncodedFile);
    StartModelRare(MaxOrder,MRMethod);
    
    PPM_CONTEXT* MinContext=MaxContext;
    for( BYTE ns=MinContext->NumStats; ; )
    {
        if( ns )
        {
            MinContext->decodeSymbol1(this);    
            rcRemoveSubrange();
        } 
        else                             
        {
            MinContext->decodeBinSymbol(this);
        }
        
        while( !FoundState ) 
        {
            rcDecNormalize(EncodedFile);
            do 
            {
                OrderFall++;                
                MinContext=MinContext->Suffix;
                if( !MinContext )          
                    goto STOP_DECODING;
            } 
            while( MinContext->NumStats == NumMasked );

            MinContext->decodeSymbol2(this);    
            rcRemoveSubrange();
        }
        _PPMD_D_PUTC(FoundState->Symbol,DecodedFile);
        if (!OrderFall && (BYTE*) FoundState->Successor >= m_allocator.UnitsStart)
        {
            PrefetchData(MaxContext=FoundState->Successor);
        }
        else 
        {
            UpdateModel(MinContext);
            PrefetchData(MaxContext);
            if (EscCount == 0)              
                ClearMask(EncodedFile,DecodedFile);
        }
        ns=(MinContext=MaxContext)->NumStats;
        rcDecNormalize(EncodedFile);
    }
    
STOP_DECODING:
    PrintInfo(DecodedFile,EncodedFile);
}

void   _STDCALL   ppmd_compressor_impl::StartModelRare (int MaxOrder, MR_METHOD MRMethod)
{
    if( StartModelRare_first_time )
    {
        UINT i, k, m;

        memset( CharMask, 0, sizeof(CharMask) );
        EscCount=PrintCount=1;
        if( MaxOrder < 2 ) // we are in solid mode
        {                     
            OrderFall = this->MaxOrder;
            for( PPM_CONTEXT* pc=MaxContext; pc&&pc->Suffix; pc=pc->Suffix )
                OrderFall--;
            return;
        }

        this->OrderFall = this->MaxOrder = MaxOrder;          
        this->MRMethod  = MRMethod;
        
        m_allocator.InitSubAllocator();
        RunLength = InitRL = -((MaxOrder < 12) ? MaxOrder : 12) - 1;

        static const WORD InitBinEsc[] = {0x3CDD,0x1F3F,0x59BF,0x48F3,0x64A1,0x5ABC,0x6632,0x6051};

        for( i=m=0; m<25; m++ ) 
        {
            while( QTable[i] == m )              
                i++;
                
            for( k=0; k<8; k++ )
                BinSumm[m][k] = BIN_SCALE - InitBinEsc[k]/(i+1);
            for( k=8; k<64; k+=8 )
                memcpy( BinSumm[m]+k, BinSumm[m], 8*sizeof(WORD) );
        }
        for( i=m=0; m<24; m++ ) 
        {
            while( QTable[i+3] == m+3 )          
                i++;
            SEE2Cont[m][0].init(2*i+5);
            for( k=1; k<32; k++ )
                SEE2Cont[m][k] = SEE2Cont[m][0];
        }
        
        MaxContext = (PPM_CONTEXT*) m_allocator.AllocContext();
        MaxContext->Suffix = NULL;

        if( !trained_model || _PPMD_E_GETC(trained_model) > MaxOrder ) 
        {
            MaxContext->SummFreq=(MaxContext->NumStats=255)+2;
            MaxContext->Stats = (PPM_CONTEXT::STATE*) m_allocator.AllocUnits(256/2);
            for (PrevSuccess=i=0;i < 256;i++) 
            {
                MaxContext->Stats[i].Symbol=i;  MaxContext->Stats[i].Freq=1;
                MaxContext->Stats[i].Successor=NULL;
            }
        } 
        else 
        {
            MaxContext->read(this, trained_model,0xFF);
            MaxContext->makeSuffix();
        }

//        StartModelRare_first_time  = false;
        StartModelRare_context     = MaxContext;
    }
    else
    {    
    
        memset( CharMask, 0, sizeof(CharMask) );
        EscCount=PrintCount=1;
        this->OrderFall = this->MaxOrder = MaxOrder;          
        this->MRMethod  = MRMethod;
        
///        InitSubAllocator();
        RunLength = InitRL = -((MaxOrder < 12) ? MaxOrder : 12) - 1;
///        MaxContext = (PPM_CONTEXT*) m_allocator.AllocContext();
///        MaxContext->Suffix = NULL;

        MaxContext  = StartModelRare_context;
        FoundState  = 0;

/*
        if( !trained_model || _PPMD_E_GETC(trained_model) > MaxOrder ) 
        {
            MaxContext->SummFreq=(MaxContext->NumStats=255)+2;
            MaxContext->Stats = (PPM_CONTEXT::STATE*) AllocUnits(256/2);
            for( PrevSuccess=i=0;i < 256;i++) 
            {
                MaxContext->Stats[i].Symbol=i;  MaxContext->Stats[i].Freq=1;
                MaxContext->Stats[i].Successor=NULL;
            }
        } 
        else 
        {
            MaxContext->read(trained_model,0xFF);
            MaxContext->makeSuffix();
        }
*/
    }
}




/****************************************************************************
 *  This file is part of PPMd project                                       *
 *  Contents: 'Carryless rangecoder' by Dmitry Subbotin                     *
 *  Comments: this implementation is claimed to be a public domain          *
 ****************************************************************************/
/**********************  Original text  *************************************
////////   Carryless rangecoder (c) 1999 by Dmitry Subbotin   ////////

typedef unsigned int  uint;
typedef unsigned char uc;

#define  DO(n)     for (int _=0; _<n; _++)
#define  TOP       (1<<24)
#define  BOT       (1<<16)


class RangeCoder
{
 uint  low, code, range, passed;
 FILE  *f;

 void OutByte (uc c)           { passed++; fputc(c,f); }
 uc   InByte ()                { passed++; return fgetc(f); }

public:

 uint GetPassed ()             { return passed; }
 void StartEncode (FILE *F)    { f=F; passed=low=0;  range= (uint) -1; }
 void FinishEncode ()          { DO(4)  OutByte(low>>24), low<<=8; }
 void StartDecode (FILE *F)    { passed=low=code=0;  range= (uint) -1;
                                 f=F; DO(4) code= code<<8 | InByte();
                               }

 void Encode (uint cumFreq, uint freq, uint totFreq) {
    assert(cumFreq+freq<totFreq && freq && totFreq<=BOT);
    low  += cumFreq * (range/= totFreq);
    range*= freq;
    while ((low ^ low+range)<TOP || range<BOT && ((range= -low & BOT-1),1))
       OutByte(low>>24), range<<=8, low<<=8;
 }

 uint GetFreq (uint totFreq) {
   uint tmp= (code-low) / (range/= totFreq);
   if (tmp >= totFreq)  throw ("Input data corrupt"); // or force it to return
   return tmp;                                         // a valid value :)
 }

 void Decode (uint cumFreq, uint freq, uint totFreq) {
    assert(cumFreq+freq<totFreq && freq && totFreq<=BOT);
    low  += cumFreq*range;
    range*= freq;
    while ((low ^ low+range)<TOP || range<BOT && ((range= -low & BOT-1),1))
       code= code<<8 | InByte(), range<<=8, low<<=8;
 }
};
*****************************************************************************/

#pragma warning (pop)

#else // #if !XRAY_PLATFORM_PS3

namespace xray {

ppmd_compressor::ppmd_compressor (memory::base_allocator* const		allocator, 
								  u32	const						sub_allocator_size_mb,
								  model_restoration_enum const		model_restoration)
{
	m_model_restoration	=	model_restoration;
	m_allocator			=	allocator;
}

ppmd_compressor::~ppmd_compressor ()
{
}

signalling_bool   ppmd_compressor::compress (const_buffer src, mutable_buffer dest, u32& out_size)
{
	return					true;
}

signalling_bool   ppmd_compressor::decompress (const_buffer src, mutable_buffer dest, u32& out_size)
{
	return					true;
}

} // namespace xray

#endif // #if !XRAY_PLATFORM_PS3