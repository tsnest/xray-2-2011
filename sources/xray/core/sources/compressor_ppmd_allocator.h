#ifndef XRAY_PPMD_ALLOCATOR_H_INCLUDED
#define XRAY_PPMD_ALLOCATOR_H_INCLUDED

/****************************************************************************
 *  This file is part of PPMd project                                       *
 *  Written and distributed to public domain by Dmitry Shkarin 1997,        *
 *  1999-2001                                                               *
 *  Contents: memory allocation routines                                    *
 ****************************************************************************/

#ifdef _WIN64
enum { UNIT_SIZE=20 };
#else // #ifdef _WIN64
enum { UNIT_SIZE=12 };
#endif // #ifdef _WIN64

enum { N1=4, N2=4, N3=4, N4=(128+3-1*N1-2*N2-3*N3)/4,
        N_INDEXES=N1+N2+N3+N4 };

#pragma pack(1)
struct BLK_NODE 
{
    DWORD		Stamp;
    BLK_NODE*	next;

    BOOL		avail	() const		{ return (next != NULL); }
    void		link	(BLK_NODE* p)	{ p->next=next; next=p; }
    void		unlink	()				{ next=next->next; }
    void*		remove	()
	{
        BLK_NODE* p	=	next;                   
		unlink			();
        --Stamp;                            
		return			p;
    }
    inline void insert	(void* pv, int NU);
};

struct MEM_BLK : public BLK_NODE 
{
	DWORD NU; 
} _PACK_ATTR;
#pragma pack()

class ppmd_allocator
{
public:
	ppmd_allocator(xray::memory::base_allocator* const allocator, u32 SASize) : m_allocator(allocator)
	{
		SubAllocatorSize				=	0;
		StartSubAllocator					(SASize);

		UINT i, k;
		for ( i=0,k=1; i < N1    ;i++,k += 1 )  Indx2Units[i]=k;
		for ( k++;i < N1+N2      ;i++,k += 2 )  Indx2Units[i]=k;
		for ( k++;i < N1+N2+N3   ;i++,k += 3 )  Indx2Units[i]=k;
		for ( k++;i < N1+N2+N3+N4;i++,k += 4 )  Indx2Units[i]=k;

		for ( k=i=0; k<128; ++k )
		{
			i								+=	(Indx2Units[i] < k+1);         
			Units2Indx[k]		=	i;
		}
	}

	~ppmd_allocator()
	{
		StopSubAllocator					();
	}

public:
	void				SplitBlock			(void* pv, UINT OldIndx, UINT NewIndx);
	DWORD _STDCALL		GetUsedMemory		();
	void _STDCALL		StopSubAllocator	();
	BOOL _STDCALL		StartSubAllocator	(UINT SASize);
	void				InitSubAllocator	();
	void				GlueFreeBlocks		();
	void* _STDCALL		AllocUnitsRare		(UINT indx);
	void*				AllocUnits			(UINT NU);
	void*				AllocContext		();
	void				UnitsCpy			(void* Dest,void* Src,UINT NU);
	void*				ExpandUnits			(void* OldPtr,UINT OldNU);
	void*  				ShrinkUnits			(void* OldPtr, UINT OldNU, UINT NewNU);
	void   				FreeUnits			(void* ptr, UINT NU);
	void   				SpecialFreeUnit 	(void* ptr);
	void*  				MoveUnitsUp			(void* OldPtr, UINT NU);
	void   				ExpandTextArea		();
	
public:
	xray::memory::base_allocator*	m_allocator;
	BLK_NODE			BList[N_INDEXES];
	BYTE				Indx2Units[N_INDEXES], Units2Indx[128]; // constants
	DWORD				GlueCount, SubAllocatorSize;
	BYTE				*HeapStart, *pText, *UnitsStart, *LoUnit, *HiUnit;

	friend class		ppmd_compressor;
};

inline UINT   U2B (UINT NU) { return UNIT_SIZE*NU; }

inline 
void   PrefetchData(void* Addr)
{
#if defined(_USE_PREFETCHING)
    BYTE PrefetchByte = *(volatile BYTE*) Addr;
#endif /* defined(_USE_PREFETCHING) */
}

inline 
void   BLK_NODE::insert(void* pv,int NU) 
{
    MEM_BLK* p	=	(MEM_BLK*) pv;               
	link			(p);
    p->Stamp	=	~0UL; 
	p->NU		=	NU;
    ++Stamp;
}

inline 
void   ppmd_allocator::SplitBlock (void* pv,UINT OldIndx,UINT NewIndx)
{
    UINT i, k, UDiff=Indx2Units[OldIndx]-Indx2Units[NewIndx];
    BYTE* p=((BYTE*) pv)+U2B(Indx2Units[NewIndx]);
    if (Indx2Units[i=Units2Indx[UDiff-1]] != UDiff) {
        k=Indx2Units[--i];                  BList[i].insert(p,k);
        p += U2B(k);                        UDiff -= k;
    }

	R_ASSERT	( UDiff-1 < 128 );
    BList[Units2Indx[UDiff-1]].insert(p,UDiff);
}

inline
DWORD _STDCALL   ppmd_allocator::GetUsedMemory ()
{
    DWORD i, RetVal=SubAllocatorSize-(HiUnit-LoUnit)-(UnitsStart-pText);
    for (i=0;i < N_INDEXES;i++)
            RetVal -= UNIT_SIZE*Indx2Units[i]*BList[i].Stamp;
    return RetVal;
}

inline
void _STDCALL   ppmd_allocator::StopSubAllocator () 
{
    if ( SubAllocatorSize ) 
	{
        SubAllocatorSize	=	0;                 
		
		XRAY_FREE_IMPL						(m_allocator, HeapStart);
    }
}

inline
BOOL _STDCALL   ppmd_allocator::StartSubAllocator (UINT SASize)
{
    DWORD t								=	SASize << 20U;
    if ( SubAllocatorSize == t )			return TRUE;
    StopSubAllocator						();
	HeapStart							=	XRAY_ALLOC_IMPL(m_allocator, BYTE, t);

    if ( HeapStart == NULL)					return FALSE;
    SubAllocatorSize					=	t;
	return									TRUE;
}

inline 
void   ppmd_allocator::InitSubAllocator ()
{
    memset(BList,0,sizeof(BList));
    HiUnit=(pText=HeapStart)+SubAllocatorSize;
    UINT Diff=UNIT_SIZE*(SubAllocatorSize/8/UNIT_SIZE*7);
    LoUnit=UnitsStart=HiUnit-Diff;          GlueCount=0;
}

inline
void   ppmd_allocator::GlueFreeBlocks ()
{
    UINT i, k, sz;
    MEM_BLK s0, * p, * p0, * p1;
    if (LoUnit != HiUnit)                   *LoUnit=0;
    for (i=0, (p0=&s0)->next=NULL;i < N_INDEXES;i++)
            while ( BList[i].avail() ) {
                p=(MEM_BLK*) BList[i].remove();
                if ( !p->NU )               continue;
                while ((p1=p+p->NU)->Stamp == ~0UL) {
                    p->NU += p1->NU;        p1->NU=0;
                }
                p0->link(p);                p0=p;
            }
    while ( s0.avail() ) {
        p=(MEM_BLK*) s0.remove();           sz=p->NU;
        if ( !sz )                          continue;
        for ( ;sz > 128;sz -= 128, p += 128)
                BList[N_INDEXES-1].insert(p,128);
		
		R_ASSERT	( sz-1 < 128 );
        if (Indx2Units[i=Units2Indx[sz-1]] != sz) {
            k=sz-Indx2Units[--i];           BList[k-1].insert(p+(sz-k),k);
        }
        BList[i].insert(p,Indx2Units[i]);
    }
    GlueCount=1 << 13;
}

inline
void* _STDCALL   ppmd_allocator::AllocUnitsRare (UINT indx)
{
    INT i=indx;
    if ( !GlueCount ) {
        GlueFreeBlocks();
        if ( BList[i].avail() )             return BList[i].remove();
    }
    do {
        if (++i == N_INDEXES) {
            GlueCount--;                    i=U2B(Indx2Units[indx]);
            return (UnitsStart-pText > i)?(UnitsStart -= i):(NULL);
        }
		R_ASSERT	( i < N_INDEXES );
    } while ( !BList[i].avail() );
    void* RetVal=BList[i].remove();         SplitBlock(RetVal,i,indx);
    return RetVal;
}

inline 
void*   ppmd_allocator::AllocUnits (UINT NU)
{
    UINT indx=Units2Indx[NU-1];
    if ( BList[indx].avail() )              return BList[indx].remove();
    void* RetVal=LoUnit;                    LoUnit += U2B(Indx2Units[indx]);
    if (LoUnit <= HiUnit)                   return RetVal;
    LoUnit -= U2B(Indx2Units[indx]);        return AllocUnitsRare(indx);
}

inline 
void*   ppmd_allocator::AllocContext ()
{
    if (HiUnit != LoUnit)                   return (HiUnit -= UNIT_SIZE);
    else if ( BList->avail() )              return BList->remove();
    else                                    return AllocUnitsRare(0);
}

inline
void   ppmd_allocator::UnitsCpy (void* Dest,void* Src,UINT NU)
{
    char* p1=(char*) Dest, * p2=(char*) Src;
    do {
		memcpy(p1, p2, UNIT_SIZE);
		p1 += UNIT_SIZE;
		p2 += UNIT_SIZE;
    } while ( --NU );
}

inline 
void*   ppmd_allocator::ExpandUnits (void* OldPtr,UINT OldNU)
{
    UINT i0=Units2Indx[OldNU-1], i1=Units2Indx[OldNU-1+1];
    if (i0 == i1)                           return OldPtr;
    void* ptr=AllocUnits(OldNU+1);
    if ( ptr ) {
        UnitsCpy(ptr,OldPtr,OldNU);         BList[i0].insert(OldPtr,OldNU);
    }
    return ptr;
}

inline 
void*   ppmd_allocator::ShrinkUnits (void* OldPtr,UINT OldNU,UINT NewNU)
{
    UINT i0=Units2Indx[OldNU-1], i1=Units2Indx[NewNU-1];
    if (i0 == i1)                           return OldPtr;
    if ( BList[i1].avail() ) {
        void* ptr=BList[i1].remove();       UnitsCpy(ptr,OldPtr,NewNU);
        BList[i0].insert(OldPtr,Indx2Units[i0]);
        return ptr;
    } else {
        SplitBlock(OldPtr,i0,i1);           return OldPtr;
    }
}

inline 
void   ppmd_allocator::FreeUnits (void* ptr,UINT NU) 
{
    UINT indx=Units2Indx[NU-1];
    BList[indx].insert(ptr,Indx2Units[indx]);
}

inline 
void   ppmd_allocator::SpecialFreeUnit (void* ptr)
{
    if ((BYTE*) ptr != UnitsStart)          BList->insert(ptr,1);
    else { *(DWORD*) ptr=~0UL;              UnitsStart += UNIT_SIZE; }
}

inline 
void*   ppmd_allocator::MoveUnitsUp (void* OldPtr,UINT NU)
{
    UINT indx=Units2Indx[NU-1];
    if ((BYTE*) OldPtr > UnitsStart+16*1024 || (BLK_NODE*) OldPtr > BList[indx].next)
            return OldPtr;
    void* ptr=BList[indx].remove();
    UnitsCpy(ptr,OldPtr,NU);                NU=Indx2Units[indx];
    if ((BYTE*) OldPtr != UnitsStart)       BList[indx].insert(OldPtr,NU);
    else                                    UnitsStart += U2B(NU);
    return ptr;
}


inline void   ppmd_allocator::ExpandTextArea ()
{
    BLK_NODE* p;
    UINT Count[N_INDEXES];                  memset(Count,0,sizeof(Count));
    while ((p=(BLK_NODE*) UnitsStart)->Stamp == ~0UL) {
        MEM_BLK* pm=(MEM_BLK*) p;           UnitsStart=(BYTE*) (pm+pm->NU);
        Count[Units2Indx[pm->NU-1]]++;      pm->Stamp=0;
    }
    for (UINT i=0;i < N_INDEXES;i++)
        for (p=BList+i;Count[i] != 0;p=p->next)
            while ( !p->next->Stamp ) {
                p->unlink();                BList[i].Stamp--;
                if ( !--Count[i] )          break;
            }
}


#endif // #define XRAY_PPMD_ALLOCATOR_H_INCLUDED