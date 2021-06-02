////////////////////////////////////////////////////////////////////////////
//	Created		: 24.09.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <stdio.h>
#include <stack>
#include <math.h>
#include <boost/noncopyable.hpp>
#include <boost/crc.hpp>


#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
#include <mmsystem.h>
#pragma comment( lib, "winmm.lib" )

#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )



/*
#define	NOGDICAPMASKS     			// CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define	NOVIRTUALKEYCODES 			// VK_*
#define	NOWINMESSAGES     			// WM_*, EM_*, LB_*, CB_*
#define	NOWINSTYLES       			// WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define	NOSYSMETRICS      			// SM_*
#define	NOMENUS           			// MF_*
#define	NOICONS           			// IDI_*
#define	NOKEYSTATES       			// MK_*
#define	NOSYSCOMMANDS     			// SC_*
#define	NORASTEROPS       			// Binary and Tertiary raster ops
#define	NOSHOWWINDOW      			// SW_*
#define	OEMRESOURCE       			// OEM Resource values
#define	NOATOM            			// Atom Manager routines
#define	NOCLIPBOARD       			// Clipboard routines
#define	NOCOLOR           			// Screen colors
#define	NOCTLMGR          			// Control and Dialog routines
#define	NODRAWTEXT        			// DrawText() and DT_*
#define	NOGDI             			// All GDI defines and routines
#define	NOKERNEL          			// All KERNEL defines and routines
#define	NOUSER            			// All USER defines and routines
#define	NONLS             			// All NLS defines and routines
#define	NOMB              			// MB_* and MessageBox()
#define	NOMEMMGR          			// GMEM_*, LMEM_*, GHND, LHND, associated routines
#define	NOMETAFILE        			// typedef METAFILEPICT
#define	NOMINMAX          			// Macros min(a,b) and max(a,b)
#define	NOMSG             			// typedef MSG and associated routines
#define	NOOPENFILE        			// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define	NOSCROLL          			// SB_* and scrolling routines
#define	NOSERVICE         			// All Service Controller routines, SERVICE_ equates, etc.
#define	NOSOUND           			// Sound driver routines
*/
/*#define	NOWH              			// SetWindowsHook and WH_*
#define	NOWINOFFSETS      			// GWL_*, GCL_*, associated routines
#define	NOCOMM            			// COMM driver routines
#define	NOKANJI           			// Kanji support stuff.
#define	NOHELP            			// Help engine interface.
#define	NOPROFILER        			// Profiler interface.
#define	NODEFERWINDOWPOS  			// DeferWindowPos routines
#define	NOMCX             			// Modem Configuration Extensions
*/
#include <d3dx9shader.h>
#pragma comment( lib, "d3dx9.lib" )

#include <D3D10Shader.h>
#pragma comment( lib, "d3d10.lib" )
#pragma comment( lib, "d3dx10.lib" )

typedef D3D10_SHADER_DESC				D3D_SHADER_DESC;
typedef	D3D10_SHADER_BUFFER_DESC		D3D_SHADER_BUFFER_DESC;
typedef	D3D10_SHADER_VARIABLE_DESC		D3D_SHADER_VARIABLE_DESC;
typedef D3D10_SHADER_INPUT_BIND_DESC	D3D_SHADER_INPUT_BIND_DESC;
typedef	D3D10_SHADER_TYPE_DESC			D3D_SHADER_TYPE_DESC;
typedef D3D10_CBUFFER_TYPE				D3D_CBUFFER_TYPE;

typedef ID3D10ShaderReflection	ID3DShaderReflection;
typedef	ID3D10ShaderReflectionConstantBuffer			ID3DShaderReflectionConstantBuffer;
typedef	ID3D10ShaderReflectionVariable					ID3DShaderReflectionVariable;
typedef	ID3D10ShaderReflectionType						ID3DShaderReflectionType;

namespace temporary
{
	template< class T > T max(T a, T b)
	{
		return a < b ? b : a;
	}
	//colors:
	//12 - red
	//14 - yellow
	//10 - green
	//7, 8 - grey
	//15 - white
	void set_text_color(unsigned short color)
	{
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hCon,color);
	}
	void set_base_color()
	{
		set_text_color(7);
	}
	void set_error_color()
	{
		set_text_color(12);
	}
	void set_warning_color()
	{
		set_text_color(14);
	}
	void set_sucesses_color()
	{
		set_text_color(10);
	}
} // namespace temporary

int print_constants();

void print_d3d_type(D3D10_SHADER_VARIABLE_TYPE type)
{
	switch( type)
	{
	case D3D10_SVT_FLOAT:
		printf("float");
		break;
	case D3D10_SVT_BOOL:
		printf("bool");
		break;
	case D3D10_SVT_INT:
		printf("int");
		break;
	case D3D10_SVT_TEXTURE:
		printf("texture");
		break;
	case D3D10_SVT_TEXTURE1D:
		printf("texture1D");
		break;
	case D3D10_SVT_TEXTURE2D:
		printf("texture2D");
		break;
	case D3D10_SVT_TEXTURE3D:
		printf("texture3D");
		break;
	case D3D10_SVT_TEXTURECUBE:
		printf("textureCube");
		break;
	case D3D10_SVT_SAMPLER:
		printf("sampler");
		break;
	default:
		temporary::set_error_color();
		printf( "print_d3d_type: unexpected d3d shader variable type.");
	}
		temporary::set_base_color();
}

size_t const s_max_value_count	= 6;
typedef char const* values_type[s_max_value_count];

struct definition;
typedef values_type const& (*get_value_ptr)	( definition* const first, definition* const last );

struct definition : private boost::noncopyable {
	inline			definition	(
			char const* const	id,
			char const* const value0,
			char const* const value1,
			char const* const value2,
			char const* const value3,
			char const* const value4,
			char const* const value5,
			get_value_ptr const	getter
		) :
		id				( id ),
		getter			( getter ),
		value			( 0 )
	{
		values[0]		= value0;
		values[1]		= value1;
		values[2]		= value2;
		values[3]		= value3;
		values[4]		= value4;
		values[5]		= value5;

		value_max_length			= 1;
		char const* const* i		= &values[0];
		char const* const* const e	= i + sizeof(values)/sizeof(values[0]);

		// TODO: fix max function, double strlen call!
		for ( ; i != e; ++i  )
			value_max_length		= temporary::max( value_max_length, strlen(*i) );
			//value_max_length		= std::max( value_max_length, strlen(*i) );
	}

	get_value_ptr const	getter;
	char const* const	id;
	values_type			values;
	char const*			value;
	size_t				value_max_length;
}; // struct definition

class includer :
	public ID3DInclude,
	public ID3DXInclude,
	private boost::noncopyable 
{
public:
	inline			includer	( total_info_type const& total_info, char const* const full_path ) :
		m_total_info		( total_info ),
		m_full_path			( full_path )
	{
	}

private:
	HRESULT open_internal		( LPCSTR short_file_name, LPCVOID *ppData, UINT *pBytes )
	{
		string_path			file_name;
		strcpy_s			( file_name, m_full_path );
		strcat_s			( file_name, short_file_name );
		total_info_type::const_iterator	const found	= m_total_info.find( file_name );
		if ( found == m_total_info.end() ) {
			printf			( "cannot find file %s\n", file_name );
			return			S_FALSE;
		}

		file_info_type const& file_info	= (*found).second;
		*ppData				= file_info.body.c_str( );
		*pBytes				= file_info.body.length( );
		return				S_OK;
	}

	HRESULT __stdcall	Open	(D3DXINCLUDE_TYPE IncludeType, LPCSTR short_file_name, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		(void)IncludeType;
		(void)pParentData;

		return				open_internal( short_file_name, ppData, pBytes );
	}

	HRESULT __stdcall	Open	(D3D10_INCLUDE_TYPE IncludeType, LPCSTR short_file_name, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		(void)IncludeType;
		(void)pParentData;

		return				open_internal( short_file_name, ppData, pBytes );
	}

	HRESULT __stdcall	Close	(LPCVOID	pData)
	{
		(void)pData;
		return				S_OK;
	}

private:
	total_info_type const&	m_total_info;
	char const* const		m_full_path;
}; // class includer

static values_type const& USE_HBAO_get_value			( definition* const first, definition* const last );
static values_type const& SSAO_OPT_DATA_get_value		( definition* const first, definition* const last );
static values_type const& SSAO_QUALITY_get_value		( definition* const first, definition* const last );
static values_type const& MSAA_SAMPLES_get_value		( definition* const first, definition* const last );
static values_type const& ISAMPLE_get_value				( definition* const first, definition* const last );
static values_type const& MSAA_OPTIMIZATION_get_value	( definition* const first, definition* const last );

static values_type const& SKIN_0_get_value				( definition* const first, definition* const last );
static values_type const& SKIN_1_get_value				( definition* const first, definition* const last );
static values_type const& SKIN_2_get_value				( definition* const first, definition* const last );
static values_type const& SKIN_3_get_value				( definition* const first, definition* const last );
static values_type const& SKIN_4_get_value				( definition* const first, definition* const last );

template <int index>
struct MSAA_ALPHATEST {
	static values_type const& get_value					( definition* const first, definition* const last );
}; // struct MSAA_ALPHATEST

static definition defines_r1[]	= {
	definition( "SKIN_COLOR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -skinw)
	definition( "SKIN_NONE",						"",	   "1",		"",		"",		"",		"",	0 ),							// single value
	definition( "SKIN_0",							"",	   "1",		"",		"",		"",		"",	&SKIN_0_get_value ),			// not defined
	definition( "SKIN_1",							"",	   "1",		"",		"",		"",		"",	&SKIN_1_get_value ),			// not defined
	definition( "SKIN_2",							"",	   "1",		"",		"",		"",		"",	&SKIN_2_get_value ),			// not defined
	definition( "SKIN_3",							"",	   "1",		"",		"",		"",		"",	&SKIN_3_get_value ),			// not defined
	definition( "SKIN_4",							"",	   "1",		"",		"",		"",		"",	&SKIN_4_get_value ),			// not defined
};
static size_t const s_define_r1_count		= sizeof(defines_r1)/sizeof(defines_r1[0]);

static definition defines_r2[]	= {
	definition( "SMAP_size",					"2048",		"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -smapXXXX)
	definition( "FP16_FILTER",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "FP16_BLEND",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "USE_HWSMAP",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "USE_HWSMAP_PCF",				   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx or -nodf24)
	definition( "USE_FETCH4",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -nodf24)
	definition( "USE_SJITTER",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -sjitter)
	definition( "USE_BRANCHING",					"",	   "1",		"",		"",		"",		"",	0 ),							// single value (depends on the hardware: need PS v3.0 and higher)
	definition( "USE_VTF",							"",	   "1",		"",		"",		"",		"",	0 ),							// single value (depends on the hardware: need PS v3.0 and D3DFMT_R32F for vertex texture)
	definition( "USE_TSHADOWS",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -tsh)
	definition( "USE_MBLUR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -mblur)
	definition( "USE_SUNFILTER",					"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -sunfilter)
	definition( "USE_R2_STATIC_SUN",				"",	   "1",		"",		"",		"",		"",	0 ),							// not defined (depends on the renderer type, only in r2a and less)
	definition( "FORCE_GLOSS",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (depends on the renderer type, only in r2a and less)
	definition( "SKIN_COLOR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -skinw)
	definition( "USE_SSAO_BLUR",					"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_blur 1/0)

	definition( "USE_HBAO",							"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_hbao 1/0)
	definition( "SSAO_OPT_DATA",					"",    "1",	   "2",		"",		"",		"",	0 ),							// 2 cases defined with 1 and 2 (could be changed via console command r2_ssao_half_data 1/0)

	definition( "SKIN_NONE",						"",	   "1",		"",		"",		"",		"",	0 ),							// single value
	definition( "SKIN_0",							"",	   "1",		"",		"",		"",		"",	&SKIN_0_get_value ),			// not defined
	definition( "SKIN_1",							"",	   "1",		"",		"",		"",		"",	&SKIN_1_get_value ),			// not defined
	definition( "SKIN_2",							"",	   "1",		"",		"",		"",		"",	&SKIN_2_get_value ),			// not defined
	definition( "SKIN_3",							"",	   "1",		"",		"",		"",		"",	&SKIN_3_get_value ),			// not defined
	definition( "SKIN_4",							"",	   "1",		"",		"",		"",		"",	&SKIN_4_get_value ),			// not defined

	definition( "USE_SOFT_WATER",					"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_SOFT_PARTICLES",				"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_DOF",							"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "SUN_SHAFTS_QUALITY",				"",	   "1",    "2",	   "3",		"",		"",	0 ),
	definition( "SSAO_QUALITY",						"",	   "1",    "2",	   "3",	   "4",		"",	&SSAO_QUALITY_get_value ),
	definition( "SUN_QUALITY",						"",	   "1",    "2",	   "3",	   "4",		"",	0 ),
	definition( "ALLOW_STEEPPARALLAX",				"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_steep_parallax 1/0)
};
static size_t const s_define_r2_count		= sizeof(defines_r2)/sizeof(defines_r2[0]);

static definition defines_r3[]	= {
	definition( "SMAP_size",					"2048",		"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -smapXXXX)
	definition( "FP16_FILTER",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "FP16_BLEND",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "USE_HWSMAP",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "USE_HWSMAP_PCF",				   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx or -nodf24)
	definition( "USE_FETCH4",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -nodf24)
	definition( "USE_SJITTER",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -sjitter)
	definition( "USE_BRANCHING",				   "1",		"",		"",		"",		"",		"",	0 ),							// single value (depends on the hardware: need PS v3.0 and higher)
	definition( "USE_VTF",						   "1",		"",		"",		"",		"",		"",	0 ),							// single value (depends on the hardware: need PS v3.0 and D3DFMT_R32F for vertex texture)
	definition( "USE_TSHADOWS",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -tsh)
	definition( "USE_MBLUR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -mblur)
	definition( "USE_SUNFILTER",					"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -sunfilter)
	definition( "USE_R2_STATIC_SUN",				"",		"",		"",		"",		"",		"",	0 ),							// not defined (depends on the renderer type, only in r2a and less)
	definition( "FORCE_GLOSS",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (depends on the renderer type, only in r2a and less)
	definition( "SKIN_COLOR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -skinw)
	definition( "USE_SSAO_BLUR",					"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_blur 1/0)

	definition( "HDAO",								"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_hdao 1/0)
	definition( "USE_HBAO",							"",    "1",		"",		"",		"",		"",	&USE_HBAO_get_value ),			// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_hbao 1/0)
	definition( "VECTORIZED_CODE",					"",    "1",		"",		"",		"",		"",	&USE_HBAO_get_value ),			// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_hbao 1/0)
	definition( "SSAO_OPT_DATA",					"",    "1",	   "2",		"",		"",		"",	&SSAO_OPT_DATA_get_value ),		// 2 cases defined with 1 and 2 (could be changed via console command r2_ssao_half_data 1/0)

	definition( "SKIN_NONE",						"",	   "1",		"",		"",		"",		"",	0 ),							// single value
	definition( "SKIN_0",							"",	   "1",		"",		"",		"",		"",	&SKIN_0_get_value ),			// not defined
	definition( "SKIN_1",							"",	   "1",		"",		"",		"",		"",	&SKIN_1_get_value ),			// not defined
	definition( "SKIN_2",							"",	   "1",		"",		"",		"",		"",	&SKIN_2_get_value ),			// not defined
	definition( "SKIN_3",							"",	   "1",		"",		"",		"",		"",	&SKIN_3_get_value ),			// not defined
	definition( "SKIN_4",							"",	   "1",		"",		"",		"",		"",	&SKIN_4_get_value ),			// not defined
																							
	definition( "USE_SOFT_WATER",					"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_SOFT_PARTICLES",				"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_DOF",							"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "SUN_SHAFTS_QUALITY",				"",	   "1",    "2",	   "3",		"",		"",	0 ),
	definition( "SSAO_QUALITY",						"",	   "1",    "2",	   "3",	   "4",		"",	&SSAO_QUALITY_get_value ),
	definition( "SUN_QUALITY",						"",	   "1",    "2",	   "3",	   "4",		"",	0 ),
	definition( "ALLOW_STEEPPARALLAX",				"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_steep_parallax 1/0)
	definition( "GBUFFER_OPTIMIZATION",			   "1",		"",		"",		"",		"",		"",	0 ),							// single value (could be changed via console command r3_gbuffer_opt 1/0)
	definition( "SM_4_1",							"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_MINMAX_SM",				   "1",		"",		"",		"",		"",		"",	0 ),							// single value
																							
	definition( "USE_MSAA",							"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "MSAA_SAMPLES",						"",	   "2",    "4",		"",		"",		"",	&MSAA_SAMPLES_get_value ),		// could be changed via console command -r3_msaa
	definition( "ISAMPLE",							"",		"",		"",		"",		"",		"",	&ISAMPLE_get_value ),			// ?
	definition( "MSAA_OPTIMIZATION",				"",		"",		"",		"",		"",		"",	0),								// not defined
	definition( "MSAA_ALPHATEST_DX10_0_ATOC",		"",	   "1",		"",		"",		"",		"",	&MSAA_ALPHATEST<0>::get_value ),// 2 cases defined with 1 and undefined
	definition( "MSAA_ALPHATEST_DX10_1_ATOC",		"",	   "1",		"",		"",		"",		"",	&MSAA_ALPHATEST<1>::get_value ),// 2 cases defined with 1 and undefined
	definition( "MSAA_ALPHATEST_DX10_1",			"",	   "1",		"",		"",		"",		"",	&MSAA_ALPHATEST<2>::get_value ),// 2 cases defined with 1 and undefined
};
static size_t const s_define_r3_count		= sizeof(defines_r3)/sizeof(defines_r3[0]);

static definition defines_r4[]	= {
	definition( "SMAP_size",					"2048",		"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -smapXXXX)
	definition( "FP16_FILTER",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "FP16_BLEND",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "USE_HWSMAP",					   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx)
	definition( "USE_HWSMAP_PCF",				   "1",	   	"",		"",		"",		"",		"",	0 ),							// single value (could be changed only via command line option -r4xx or -nodf24)
	definition( "USE_FETCH4",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -nodf24)
	definition( "USE_SJITTER",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -sjitter)
	definition( "USE_BRANCHING",				   "1",		"",		"",		"",		"",		"",	0 ),							// single value (depends on the hardware: need PS v3.0 and higher)
	definition( "USE_VTF",						   "1",		"",		"",		"",		"",		"",	0 ),							// single value (depends on the hardware: need PS v3.0 and D3DFMT_R32F for vertex texture)
	definition( "USE_TSHADOWS",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -tsh)
	definition( "USE_MBLUR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -mblur)
	definition( "USE_SUNFILTER",					"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -sunfilter)
	definition( "USE_R2_STATIC_SUN",				"",		"",		"",		"",		"",		"",	0 ),							// not defined (depends on the renderer type, only in r2a and less)
	definition( "SKIN_COLOR",						"",		"",		"",		"",		"",		"",	0 ),							// not defined (could be changed only via command line option -skinw)
	definition( "USE_SSAO_BLUR",					"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_blur 1/0)

	definition( "HDAO",								"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_hdao 1/0)
	definition( "USE_HBAO",							"",    "1",		"",		"",		"",		"",	&USE_HBAO_get_value ),			// 2 cases defined with 1 and undefined (could be changed via console command r2_ssao_hbao 1/0)
	definition( "SSAO_OPT_DATA",					"",    "1",	   "2",		"",		"",		"",	&SSAO_OPT_DATA_get_value ),		// 2 cases defined with 1 and 2 (could be changed via console command r2_ssao_half_data 1/0)

	definition( "ISAMPLE",						   "0",		"",		"",		"",		"",		"",	0 ),							// not defined
																							
	definition( "SKIN_NONE",						"",	   "1",		"",		"",		"",		"",	0 ),							// single value
	definition( "SKIN_0",							"",	   "1",		"",		"",		"",		"",	&SKIN_0_get_value ),			// not defined
	definition( "SKIN_1",							"",	   "1",		"",		"",		"",		"",	&SKIN_1_get_value ),			// not defined
	definition( "SKIN_2",							"",	   "1",		"",		"",		"",		"",	&SKIN_2_get_value ),			// not defined
	definition( "SKIN_3",							"",	   "1",		"",		"",		"",		"",	&SKIN_3_get_value ),			// not defined
	definition( "SKIN_4",							"",	   "1",		"",		"",		"",		"",	&SKIN_4_get_value ),			// not defined
																							
	definition( "USE_SOFT_WATER",					"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_SOFT_PARTICLES",				"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "USE_DOF",							"",    "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "SUN_SHAFTS_QUALITY",				"",	   "1",    "2",	   "3",		"",		"",	0 ),
	definition( "SSAO_QUALITY",						"",	   "1",    "2",	   "3",	   "4",		"",	&SSAO_QUALITY_get_value ),
	definition( "SUN_QUALITY",						"",	   "1",    "2",	   "3",	   "4",		"",	0 ),
	definition( "ALLOW_STEEPPARALLAX",				"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined (could be changed via console command r2_steep_parallax 1/0)
	definition( "GBUFFER_OPTIMIZATION",			   "1",		"",		"",		"",		"",		"",	0 ),							// single value (could be changed via console command r3_gbuffer_opt 1/0)
	definition( "SM_4_1",						   "",		"",		"",		"",		"",		"",	0 ),							// not defined
	definition( "SM_5",							   "1",		"",		"",		"",		"",		"",	0 ),							// single value
	definition( "USE_MINMAX_SM",				    "",	   "1",		"",		"",		"",		"",	0 ),							// single value
																							
	definition( "USE_MSAA",							"",	   "1",		"",		"",		"",		"",	0 ),							// 2 cases defined with 1 and undefined
	definition( "MSAA_SAMPLES",						"",	   "2",    "4",		"",		"",		"",	&MSAA_SAMPLES_get_value ),		// could be changed via console command -r3_msaa
	definition( "MSAA_OPTIMIZATION",				"",		"",		"",		"",		"",		"",	&MSAA_OPTIMIZATION_get_value),	// single value
	definition( "MSAA_ALPHATEST_DX10_0_ATOC",		"",	   "1",		"",		"",		"",		"",	0 ),							// not defined (depend on hardware - only for Dx10.0 video cards)
	definition( "MSAA_ALPHATEST_DX10_1_ATOC",		"",	   "1",		"",		"",		"",		"",	0 ),							// not defined (could be changed via console command r3_msaa_alphatest 2/1/0)
	definition( "MSAA_ALPHATEST_DX10_1",			"",	   "1",		"",		"",		"",		"",	0 ),							// not defined (could be changed via console command r3_msaa_alphatest 2/1/0)
};

// TODO: bool operator ==
struct value_name_pair 
{
	bool operator == ( value_name_pair const & other) const
	{
		return value == other.value;
	}
	int value;
	char* name;
};

struct value_name_predicate
{
	value_name_predicate( int value ): m_value (value)	{}

	bool operator () ( value_name_pair const & other )
	{
		return other.value == m_value;
	}

	int m_value;
};


static value_name_pair  parameter_type_names_dx9[]	= {
{ D3DXPT_VOID,				"void" } ,				
{ D3DXPT_BOOL,				"bool" } ,				
{ D3DXPT_INT,				"int" }	,				
{ D3DXPT_FLOAT,				"float" }	,			
{ D3DXPT_STRING,			"string" } ,			
{ D3DXPT_TEXTURE,			"texture" } ,				
{ D3DXPT_TEXTURE1D,			"texture1d" } ,			
{ D3DXPT_TEXTURE2D,			"texture2d" } ,			
{ D3DXPT_TEXTURE3D,			"texture3d" } ,			
{ D3DXPT_TEXTURECUBE,		"texturecube" },			
{ D3DXPT_SAMPLER,			"sampler"	} ,		
{ D3DXPT_SAMPLER1D,			"sampler1D" } ,			
{ D3DXPT_SAMPLER2D,			"sampler2D" } ,			
{ D3DXPT_SAMPLER3D,			"sampler3D" } ,			
{ D3DXPT_SAMPLERCUBE,		"samplercube" } ,			
{ D3DXPT_PIXELSHADER,		"pixelshader" } ,			
{ D3DXPT_VERTEXSHADER,		"vertexshader" } ,		
{ D3DXPT_PIXELFRAGMENT,		"pixelfragment" } ,		
{ D3DXPT_VERTEXFRAGMENT,	"vertexfragment" } ,	
{ D3DXPT_UNSUPPORTED,		"unsupported" }		,	
};

static value_name_pair  parameter_type_names[]	= {
	{ D3DXPT_VOID,				"void" } ,				
	{ D3DXPT_BOOL,				"bool" } ,				
	{ D3DXPT_INT,				"int" }	,				
	{ D3DXPT_FLOAT,				"float" }	,			
	{ D3DXPT_STRING,			"string" } ,			
	{ D3DXPT_TEXTURE,			"texture" } ,				
	{ D3DXPT_TEXTURE1D,			"texture1d" } ,			
	{ D3DXPT_TEXTURE2D,			"texture2d" } ,			
	{ D3DXPT_TEXTURE3D,			"texture3d" } ,			
	{ D3DXPT_TEXTURECUBE,		"texturecube" },			
	{ D3DXPT_SAMPLER,			"sampler"	} ,		
	{ D3DXPT_SAMPLER1D,			"sampler1D" } ,			
	{ D3DXPT_SAMPLER2D,			"sampler2D" } ,			
	{ D3DXPT_SAMPLER3D,			"sampler3D" } ,			
	{ D3DXPT_SAMPLERCUBE,		"samplercube" } ,			
	{ D3DXPT_PIXELSHADER,		"pixelshader" } ,			
	{ D3DXPT_VERTEXSHADER,		"vertexshader" } ,		
	{ D3DXPT_PIXELFRAGMENT,		"pixelfragment" } ,		
	{ D3DXPT_VERTEXFRAGMENT,	"vertexfragment" } ,	
	{ D3DXPT_UNSUPPORTED,		"unsupported" }		,	
};


static size_t const s_define_r4_count		= sizeof(defines_r4)/sizeof(defines_r4[0]);

static values_type const& USE_HBAO_get_value		( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "HDAO" ) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return	result;
		}

		static values_type result = { "", "1", "", "", "", "" };
		return	result;
	}

	__assume(0);
}

static values_type const& SSAO_OPT_DATA_get_value	( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "HDAO" ) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return	result;
		}

		break;
	}

	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "USE_HBAO" ) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "1", "2", "", "", "", "" };
			return	result;
		}

		static values_type result = { "", "1", "2", "", "", "" };
		return	result;
	}

	__assume(0);
}

static values_type const& SSAO_QUALITY_get_value	( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "HDAO" ) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "1", "2", "3", "", "", "" };
			return	result;
		}

		break;
	}

	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "USE_HBAO" ) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "1", "2", "3", "", "", "" };
			return	result;
		}

		break;
	}

	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "SSAO_OPT_DATA" ) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "1", "2", "3", "", "", "" };
			return	result;
		}

		static values_type result = { "", "1", "2", "3", "", "" };
		return	result;
	}

	__assume(0);
}

static values_type const& MSAA_SAMPLES_get_value ( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "USE_MSAA" ) )
			continue;

		if ( !(*i).value || !*(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return		result;
		}

		static values_type result = { "4", "2", "", "", "", "" };
		return			result;
	}

	__assume(0);
}

static values_type const& ISAMPLE_get_value	( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "MSAA_SAMPLES" ) )
			continue;

		if ( !(*i).value || !*(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return		result;
		}

		if ( !strcmp("4",(*i).value) ) {
			static values_type result = { "4", "", "", "", "", "" };
			return			result;
		}

		if ( !strcmp("2",(*i).value) ) {
			static values_type result = { "2", "", "", "", "", "" };
			return			result;
		}

		__assume(0);
	}

	__assume(0);
}

static values_type const& MSAA_OPTIMIZATION_get_value	( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "USE_MSAA" ) )
			continue;

		if ( !(*i).value || !*(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return		result;
		}

		static values_type result = { "1", "", "", "", "", "" };
		return			result;
	}

	__assume(0);
}

template <int count>
static values_type const& SKIN_get_value ( definition* const first, definition* const last, char const* const (&check_keys)[count] )
{
	char const* const* k			= &check_keys[0];
	char const* const * const ke	= k + sizeof(check_keys)/sizeof(check_keys[0]);
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp((*i).id, *k) )
			continue;

		if ( (*i).value && *(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return	result;
		}

		++k;
		if ( k != ke)
			continue;

		break;
	}

	static values_type result = { "", "1", "", "", "", "" };
	return				result;
}

static values_type const& SKIN_0_get_value ( definition* const first, definition* const last )
{
	char const* const check_keys[] = { "SKIN_NONE" };
	return				SKIN_get_value( first, last, check_keys );
}

static values_type const& SKIN_1_get_value	( definition* const first, definition* const last )
{
	char const* check_keys[] = { "SKIN_NONE", "SKIN_0" };
	return				SKIN_get_value( first, last, check_keys );
}

static values_type const& SKIN_2_get_value	( definition* const first, definition* const last )
{
	char const* check_keys[] = { "SKIN_NONE", "SKIN_0", "SKIN_1" };
	return				SKIN_get_value( first, last, check_keys );
}

static values_type const& SKIN_3_get_value	( definition* const first, definition* const last )
{
	char const* check_keys[] = { "SKIN_NONE", "SKIN_0", "SKIN_1", "SKIN_2" };
	return				SKIN_get_value( first, last, check_keys );
}

static values_type const& SKIN_4_get_value	( definition* const first, definition* const last )
{
	char const* check_keys[] = { "SKIN_NONE", "SKIN_0", "SKIN_1", "SKIN_2", "SKIN_3" };
	values_type const& temp_result = SKIN_get_value( first, last, check_keys );
	if ( strcmp(temp_result[1], "1") )
		return			temp_result;

	static values_type result = { "1", "", "", "", "", "" };
	return				result;
}

inline bool identity_value	( bool const value )
{
	return				value;
}

template <int index>
values_type const& MSAA_ALPHATEST<index>::get_value	( definition* const first, definition* const last )
{
	for (definition* i = first; i != last; ++i ) {
		if ( strcmp( (*i).id, "USE_MSAA" ) )
			continue;

		if ( !(*i).value || !*(*i).value ) {
			static values_type result = { "", "", "", "", "", "" };
			return		result;
		}

		if ( identity_value(!index) ) {
			static values_type result = { "", "1", "", "", "", "" };
			return		result;
		}

		char const* check_keys1[] = { "MSAA_ALPHATEST_DX10_0_ATOC" };
		char const* check_keys2[] = { "MSAA_ALPHATEST_DX10_0_ATOC", "MSAA_ALPHATEST_DX10_1_ATOC" };
		if ( identity_value(index == 1) )
			return		SKIN_get_value( first, last, check_keys1 );
		else
			return		SKIN_get_value( first, last, check_keys2 );
	}

	__assume(0);
}

class compile_parameters : private boost::noncopyable {
public:
	inline					compile_parameters	(
			char const* const file_name,
			char const* const new_file_name,
			char const* const buffer,
			size_t const buffer_size,
			includer& includer,
			char const* const main_function_id,
			char const* const shader_version_id,
			D3D_SHADER_MACRO const (*in_macros),
			size_t const macro_index,
			unsigned int& shader_count,
			unsigned int& error_count,
			unsigned int procedure_time_start,
			unsigned int& total_size,
			CRITICAL_SECTION& section,
			bool const legacy_compiler
		) :
		next				( 0 ),
		file_name			( file_name				),
		new_file_name		( new_file_name			),
		buffer				( buffer				),
		buffer_size			( buffer_size			),
		includer			( includer				),
		main_function_id	( main_function_id		),
		shader_version_id	( shader_version_id		),
		shader_count		( shader_count			),
		error_count			( error_count			),
		procedure_time_start( procedure_time_start	),
		total_size			( total_size			),
		section				( section ),
		m_multithreaded		( false ),
		m_legacy_compiler	( legacy_compiler )
	{
		memcpy				( macros, in_macros, (macro_index+1)*sizeof(definition) );
	}

	static void __stdcall	run					( void* const data )
	{
		compile_parameters* const self = (compile_parameters*)data;
		self->m_multithreaded	= true;
//		_ReadWriteBarrier	( );
		self->run			( );
		delete				self;
	}

	void print_header							( char const* const short_new_file_name, char const* const name )
	{
		printf				( "\n%4d. %-42s \n\t %s ...", shader_count+1, short_new_file_name, name );
	}

	void  			check_for					( char const* const vs_ps, char const* const target, std::string& main_function, std::string& shader_version ) const
	{
		char temp[256];
		strcpy_s			( temp, vs_ps );
		strcat_s			( temp, "_" );
		strcat_s			( temp, target );

		char temp2[256];
		strcpy_s			( temp2, "main_" );
		strcat_s			( temp2, temp );
		if ( !strstr(buffer, temp2) )
			return;

		shader_version		= temp;
		main_function		= temp2;
	}

	void					run					( )
	{
		string_path	drive, folder, name, extension;

		_splitpath_s			( new_file_name.c_str(), drive, folder, name, extension );

		char* short_new_file_name = folder;
		size_t const length		= strlen(folder);
		if ( length ) {
			folder[length - 1] = 0;
			short_new_file_name	= strrchr( folder, '\\' );
			if ( !short_new_file_name )
				short_new_file_name	= folder;
			else
				++short_new_file_name;
		}

		if ( !m_multithreaded )
			print_header		( short_new_file_name, name );

		size_t const time_start	= timeGetTime( );

		FILE* output;
		const char* command_line = GetCommandLine();

		if ( strstr(command_line,"-c") ) {
			if ( !fopen_s( &output, new_file_name.c_str(), "rb" ) ) {
				fclose			( output );

				EnterCriticalSection	( &section );
				if ( m_multithreaded )
					print_header( short_new_file_name, name );
				printf			( " skipped! [errors : %d]\n", error_count );
				++shader_count;
				LeaveCriticalSection	( &section );

				return;
			}
		}
		
		unsigned long legacy_compiler_options = D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;
		unsigned int compiler_options = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
		
		if ( strstr(command_line,"-d") )
		{
			if (!m_legacy_compiler)
				compiler_options|=D3D10_SHADER_DEBUG;
			else
				legacy_compiler_options|=D3DXSHADER_DEBUG;
		}
		if ( strstr(command_line,"-cm") )
		{
			if (!m_legacy_compiler)
				compiler_options&=~D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
			else
				legacy_compiler_options&=~D3DXSHADER_PACKMATRIX_ROWMAJOR;
		}
		
		ID3DBlob* shader_buffer	= 0;
		ID3DBlob* error_buffer	= 0;
		ID3DXBuffer* shader_buffer_legacy = 0;
		ID3DXBuffer* error_buffer_legacy = 0;

		LPD3DXCONSTANTTABLE	constants_legacy = 0;
		if ( !m_legacy_compiler ) {
			D3DCompile				(
				buffer,
				buffer_size,
				file_name.c_str(),
				macros,
				&includer,
				main_function_id.c_str(),
				shader_version_id.c_str(),
				compiler_options,
				0,
				&shader_buffer,
				&error_buffer
			);
		}
		else {
			if ( shader_version_id.c_str()[0] == 'v' ) {
				check_for				( "vs", "1_1", main_function_id, shader_version_id );
				check_for				( "vs", "2_0", main_function_id, shader_version_id );
			}
			else {
				check_for				( "ps", "1_1", main_function_id, shader_version_id );
				check_for				( "ps", "1_2", main_function_id, shader_version_id );
				check_for				( "ps", "1_3", main_function_id, shader_version_id );
				check_for				( "ps", "1_4", main_function_id, shader_version_id );
				check_for				( "ps", "2_0", main_function_id, shader_version_id );
			}

			
			D3DXCompileShader(
				buffer,
				buffer_size,
				(D3DXMACRO*)macros,
				&includer,
				main_function_id.c_str(),
				shader_version_id.c_str(),
				legacy_compiler_options,
				&shader_buffer_legacy,
				&error_buffer_legacy,
				&constants_legacy
			);
		}
		
		if ( !shader_buffer && !shader_buffer_legacy ) {
			++error_count;

			EnterCriticalSection( &section );
			if ( m_multithreaded )
				print_header	( short_new_file_name, name );

			++shader_count;
			if ( !error_buffer && !error_buffer_legacy ) {
				printf			( "failed!\nunknown error during compilation %s\n", new_file_name.c_str() );
				LeaveCriticalSection( &section );
				return;
			}
			
			// TODO: text color for errors and warnings
			
			std::string error;
			if ( error_buffer )
				error			= std::string( (char*)error_buffer->GetBufferPointer(), ((char*)error_buffer->GetBufferPointer()) + error_buffer->GetBufferSize() );
			else
				error			= std::string( (char*)error_buffer_legacy->GetBufferPointer(), ((char*)error_buffer_legacy->GetBufferPointer()) + error_buffer_legacy->GetBufferSize() );
			
			printf				( "failed!");
			
			if (strstr(error.c_str(), ": warning"))
				temporary::set_warning_color();
			else
				temporary::set_error_color();

			printf				( "\n%s\n", error.c_str() );
			temporary::set_base_color();

			LeaveCriticalSection( &section );

			
			return;
		}

		EnterCriticalSection	( &section );
		if ( m_multithreaded )
			print_header		( short_new_file_name, name );

		++shader_count;

		if ( fopen_s( &output, new_file_name.c_str(), "wb" ) ) {
			printf				( "failed!\ncannot open file for writing %s\n", new_file_name.c_str() );
			LeaveCriticalSection( &section );
			return;
		}

		// write crc
		boost::crc_32_type		processor;

		if ( shader_buffer )
			processor.process_block	( shader_buffer->GetBufferPointer(), ((char*)shader_buffer->GetBufferPointer()) + shader_buffer->GetBufferSize() );
		else
			processor.process_block	( shader_buffer_legacy->GetBufferPointer(), ((char*)shader_buffer_legacy->GetBufferPointer()) + shader_buffer_legacy->GetBufferSize() );

		unsigned int const crc	= processor.checksum( );
		size_t count			= fwrite( &crc, 1, sizeof(crc), output );
		if ( count != sizeof(crc) ) {
			printf				( "failed!\ncannot write to file %s (only partially)\n", new_file_name.c_str() );
		}

		// write body
		if ( shader_buffer )
			count				= fwrite( shader_buffer->GetBufferPointer(), 1, shader_buffer->GetBufferSize(), output );
		else
			count				= fwrite( shader_buffer_legacy->GetBufferPointer(), 1, shader_buffer_legacy->GetBufferSize(), output );

		if ( (shader_buffer && (count != shader_buffer->GetBufferSize())) || (shader_buffer_legacy && (count != shader_buffer_legacy->GetBufferSize())) ) {
			printf				( "failed!\ncannot write to file %s (only partially)\n", new_file_name.c_str() );
		}

		fclose					( output );

		if( constants_legacy )
			print_constants_legacy( constants_legacy->GetBufferPointer());
		else
			print_constants( shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize());

		size_t const time_stop	= timeGetTime( );
		unsigned int const size	= count + sizeof(crc);
		total_size				+= size;
		temporary::set_sucesses_color();
		printf					( " \n\n\t OK! %6.2fs  %8.2fs  %6.2f kb  %7.2f kb  [errors : %d]\n", float(time_stop - time_start)/1000.f, float(time_stop - procedure_time_start)/1000.f, float(size)/1024.f, total_size/1024.f, error_count );
		temporary::set_base_color();
		LeaveCriticalSection	( &section );
	}


	// prints constant buffer entry to output
	int print_constants( const void* buffer, unsigned int buffer_size )
	{
		ID3DShaderReflection *pReflection = 0;
		HRESULT result; 

		#ifdef USE_DX11
			result	= D3DReflect		( buffer, buffer_size, IID_ID3D10ShaderReflection, (void**)&pReflection);
		#else
			result	= D3D10ReflectShader( buffer, buffer_size, &pReflection);
		#endif
		
		if (result!=S_OK)
			return 0;

		D3D_SHADER_DESC	ShaderDesc;
		pReflection->GetDesc(&ShaderDesc);

		if( ShaderDesc.ConstantBuffers )
		{
			//	Parse single constant table
			ID3DShaderReflectionConstantBuffer *pTable=0;

			for( unsigned int iBuf = 0; iBuf<ShaderDesc.ConstantBuffers; ++iBuf )
			{
				pTable = pReflection->GetConstantBufferByIndex(iBuf);
				if (pTable)
				{
					D3D_SHADER_BUFFER_DESC	TableDesc;
					if( FAILED(pTable->GetDesc(&TableDesc)))
						continue;
	
					for (unsigned int i = 0; i < TableDesc.Variables; ++i)
					{
						ID3DShaderReflectionVariable* pVar;
						D3D_SHADER_VARIABLE_DESC		VarDesc;
						ID3DShaderReflectionType*		pType;
						D3D_SHADER_TYPE_DESC			TypeDesc;

						pVar = pTable->GetVariableByIndex(i);
						//VERIFY(pVar);
						pVar->GetDesc(&VarDesc);
						pType = pVar->GetType();
						//VERIFY(pType);
						
						pType->GetDesc(&TypeDesc);
						
						// Print type
						printf("\n");
						print_d3d_type(TypeDesc.Type);
						LPCSTR	name		=	VarDesc.Name;

						// Print name
						printf(" %s", name);

						// Print [] if an array
						if (TypeDesc.Elements)
							printf("[%d] %d",TypeDesc.Elements, VarDesc.StartOffset);
						printf(" {%d}",VarDesc.StartOffset);
					}
				}
			}
		}
		
		return 0;
// 		if (ShaderDesc.BoundResources)
// 		{
// 			parseResources(pReflection, ShaderDesc.BoundResources, destination);
// 		}
	}

	// prints constant buffer entry to output
	int print_constants_legacy( const void *shader_desc)
	{
		printf				( "\n" );
		
		D3DXSHADER_CONSTANTTABLE* desc	= (D3DXSHADER_CONSTANTTABLE*) shader_desc;
		D3DXSHADER_CONSTANTINFO* it		= (D3DXSHADER_CONSTANTINFO*) (LPBYTE(desc)+desc->ConstantInfo);
		LPBYTE					 ptr	= LPBYTE(desc);
		for ( unsigned int dwCount = desc->Constants; dwCount; dwCount--,it++)
		{
			// Name
			LPCSTR	name		=	LPCSTR(ptr+it->Name);

			// TypeInfo + class
			D3DXSHADER_TYPEINFO*	T	= (D3DXSHADER_TYPEINFO*)(ptr+it->TypeInfo);

			string_path constant_str;
			string_path tmp_str;

			value_name_pair tmp_pair;
			tmp_pair.value = T->Type;
			value_name_pair* res = std::find_if(&parameter_type_names_dx9[0], &parameter_type_names_dx9[sizeof(parameter_type_names_dx9)], value_name_predicate( T->Type));
			if( res == &parameter_type_names_dx9[sizeof(parameter_type_names_dx9)])
				continue;

			strcpy_s( constant_str, res->name );

			if( T->Rows > 1)
			{
				_itoa_s( T->Rows, tmp_str, sizeof(tmp_str), 10);
				strcat_s( constant_str, tmp_str);
			}

			if( T->Columns > 1)
			{
				if( T->Rows > 1)
					strcat_s( constant_str, "x");

				_itoa_s( T->Columns, tmp_str, sizeof(tmp_str), 10);
				strcat_s( constant_str, tmp_str);
			}

			strcat_s( constant_str, "\t");
			strcat_s( constant_str, name);

			if( T->Elements > 1)
			{
				strcat_s( constant_str, "[");
				_itoa_s( T->Elements, tmp_str, sizeof(tmp_str), 10);
				strcat_s( constant_str, tmp_str);
				strcat_s( constant_str, "]");
			}
			strcat_s( constant_str, ";");
			printf("\n\t%s ", constant_str);
		}
		return desc->Constants;
	}
	
private:
	D3D_SHADER_MACRO			macros[260];
	std::string	const			file_name;
	std::string	const			new_file_name;

public:
	compile_parameters*			next;

private:
	char const* const			buffer;
	includer&					includer;
	std::string					main_function_id;
	std::string					shader_version_id;
	unsigned int&				shader_count;
	unsigned int&				error_count;
	unsigned int&				total_size;
	CRITICAL_SECTION&			section;
	size_t const				buffer_size;
	unsigned int				procedure_time_start;
	bool						m_multithreaded;
	bool						m_legacy_compiler;

	friend class thread_pool;
}; // class compile_parameters

class thread;

class thread_pool {
public:
	inline						thread_pool					( int reserve_thread_count );
	inline void					add_query					( compile_parameters* const query );
	inline compile_parameters*	get_query					( );
	inline void					wait_for_queries_completion	( );
	inline void					run_reserved_threads		( );

private:
	typedef std::vector<thread>	threads_type;
	threads_type				m_threads;
	compile_parameters*			m_head;
	compile_parameters*			m_tail;
	CRITICAL_SECTION			m_section;
//	int							m_count;
}; // class thread_pool

static void generate_shader_combinations	(
		definition* const defines,
		size_t const define_count,
		file_info_type const& file_info,
		D3D_SHADER_MACRO (*macros),
		size_t const define_index,
		size_t const macro_index,
		char const* const file_name,
		char* const new_file_name,
		char const* const buffer,
		size_t const buffer_size,
		includer& includer,
		char const* const main_function_id,
		char const* const shader_version_id,
		unsigned int& shader_count,
		unsigned int& error_count,
		unsigned int procedure_time_start,
		unsigned int& total_size,
		CRITICAL_SECTION& section,
		thread_pool* thread_pool,
		bool const legacy_compiler
	)
{
	definition& define			= defines[define_index];
	size_t const previous_length = strlen(new_file_name);

	if ( define_index >= define_count ) {
		macros[macro_index].Name		= 0;
		macros[macro_index].Definition	= 0;

		if ( !thread_pool ) {
			compile_parameters		(
				file_name,
				new_file_name,
				file_info.body.c_str(),
				file_info.body.length(),
				includer,
				"main",
				shader_version_id,
				macros,
				macro_index,
				shader_count,
				error_count,
				procedure_time_start,
				total_size,
				section,
				legacy_compiler
			).run( );
		}
		else {
			thread_pool->add_query			(
				new compile_parameters	(
					file_name,
					new_file_name,
					file_info.body.c_str(),
					file_info.body.length(),
					includer,
					"main",
					shader_version_id,
					macros,
					macro_index,
					shader_count,
					error_count,
					procedure_time_start,
					total_size,
					section,
					legacy_compiler
				)
			);
		}

		new_file_name[previous_length]	= 0;
		return;
	}

	if ( file_info.external_definitions.find(define.id) == file_info.external_definitions.end() ) {
		char* i					= new_file_name + previous_length;
		char* const e			= new_file_name + previous_length + define.value_max_length;
		for ( ; i != e; ++i )
			*i					= '_';
		*i						= 0;
		define.value			= 0;

		generate_shader_combinations(
			defines,
			define_count,
			file_info,
			macros,
			define_index + 1,
			macro_index,
			file_name,
			new_file_name,
			buffer,
			buffer_size,
			includer,
			main_function_id,
			shader_version_id,
			shader_count,
			error_count,
			procedure_time_start,
			total_size,
			section,
			thread_pool,
			legacy_compiler
		);
		new_file_name[previous_length]	= 0;
		return;
	}

	char const* const* values	= define.values;
	if ( define.getter ) {
		values					= (*define.getter)( defines, defines + define_count );
	}
	char const* const* i		= values;
	char const* const* const e	= values + s_max_value_count;
	for ( ; i != e; ++i ) {
		if ( i != values ) {
			if ( !**i )
				break;
		}

		define.value			= *i;
		char const* j			= define.value;
		char* k					= new_file_name + previous_length;
		for ( ; *j; ++j, ++k )
			*k					= *j;

		char* e					= new_file_name + previous_length + define.value_max_length;
		for ( ; k != e; ++k )
			*k					= '0';

		*k						= 0;

		if ( *define.value ) {
			macros[macro_index].Name		= define.id;
			macros[macro_index].Definition	= define.value;
		}

		generate_shader_combinations(
			defines,
			define_count,
			file_info,
			macros,
			define_index + 1,
			macro_index + ((*define.value) ? 1 : 0),
			file_name,
			new_file_name,
			buffer,
			buffer_size,
			includer,
			main_function_id,
			shader_version_id,
			shader_count,
			error_count,
			procedure_time_start,
			total_size,
			section,
			thread_pool,
			legacy_compiler
		);

		new_file_name[previous_length]	= 0;
	}
}

static void compile_shader	(
		definition* const defines,
		size_t const define_count,
		total_info_type const& total_info,
		includer& includer,
		D3D_SHADER_MACRO* macros,
		char const* const full_path,
		char const* const output_path,
		char const* const short_file_name,
		char const* const shader_version_id,
		unsigned int& shader_count,
		unsigned int& error_count,
		unsigned int procedure_time_start,
		unsigned int& total_size,
		CRITICAL_SECTION& section,
		thread_pool* const thread_pool,
		bool const legacy_compiler
	)
{
	string_path				file_name;
	strcpy_s				( file_name, full_path );
	strcat_s				( file_name, short_file_name );

	total_info_type::const_iterator	const found	= total_info.find( file_name );
	if ( found == total_info.end() ) {
		printf				( "cannot find file %s\n", file_name );
		return;
	}

	string_path				new_file_name;
	strcpy_s				( new_file_name, output_path );
	strcat_s				( new_file_name, short_file_name );
	strcat_s				( new_file_name, "\\" );
	_mkdir					( new_file_name );

	file_info_type const& file_info	= (*found).second;
	generate_shader_combinations(
		defines,
		define_count,
		file_info,
		macros,
		0,
		0,
		file_name,
		new_file_name,
		file_info.body.c_str(),
		file_info.body.length(),
		includer,
		"main",
		shader_version_id,
		shader_count,
		error_count,
		procedure_time_start,
		total_size,
		section,
		thread_pool,
		legacy_compiler
	);
}

class thread {
public:
	inline			thread		( thread_pool* const pool ) :
		m_pool		( pool ),
		m_is_ready	( true ),
		m_started	( false )
	{
	}

	inline			~thread		( )
	{
		if ( m_thread_handle == INVALID_HANDLE_VALUE )
			return;

		TerminateThread	( m_thread_handle, 0 );
	}

	void			run			( )
	{
		if ( m_started )
			return;

		m_started			= true;
		m_thread_handle		= CreateThread( 0, 0, &run_thread, this, 0, 0);
	}

	inline bool		is_ready	( ) const
	{
		return			m_is_ready;
	}

	inline bool		started		( ) const
	{
		return			m_started;
	}

private:
	static DWORD __stdcall	run_thread	( void* data )
	{
		thread* const self	= (thread*)data;
		self->process		( );
		return				1;
	}

	inline	void	process		( )
	{
		for (;;) {
			compile_parameters* const query = m_pool->get_query( );
			if ( !query ) {
				m_is_ready			= true;
				Sleep				( 1 );
				continue;
			}

			m_is_ready				= false;
			compile_parameters::run	( query );
		}
	}

private:
	thread_pool*	m_pool;
	HANDLE			m_thread_handle;
	bool			m_is_ready;
	bool			m_started;
}; // class thread

static inline unsigned int get_core_count	( )
{
	static unsigned int core_count	= unsigned int(-1);
	if ( core_count != unsigned int(-1) )
		return			core_count;

	_SYSTEM_INFO		system_info;
	GetSystemInfo		( &system_info );
	core_count			= system_info.dwNumberOfProcessors == 1 ? 1 : system_info.dwNumberOfProcessors + 0;
	return				core_count;
}

inline thread_pool::thread_pool	( int reserve_thread_count ) :
	m_head				( 0 ),
	m_tail				( 0 )
{
	InitializeCriticalSection	( &m_section );

	timeBeginPeriod		( 1 );

	unsigned int const core_count	= get_core_count( );
	m_threads.reserve	( core_count );
	for (unsigned int i=0; i<core_count; ++i )
		m_threads.push_back	( thread(this) );

	threads_type::iterator i		= m_threads.begin();
	threads_type::iterator const e	= m_threads.begin() + std::max( (int)core_count - reserve_thread_count, 0 );
	for ( ; i != e; ++i ) {
		(*i).run		( );
	}
}

inline void thread_pool::run_reserved_threads	( )
{
	threads_type::iterator i		= m_threads.begin();
	threads_type::iterator const e	= m_threads.end();
	for ( ; i != e; ++i ) {
		if ( !(*i).started() )
			(*i).run	( );
	}
}

inline void	thread_pool::add_query	( compile_parameters* const query )
{
	EnterCriticalSection		( &m_section );

	query->next					= 0;

	if ( m_head ) {
		m_tail->next			= query;
		m_tail					= query;
	}
	else {
		m_head = m_tail			= query;
	}

	LeaveCriticalSection		( &m_section );
}

inline compile_parameters* thread_pool::get_query	( )
{
	EnterCriticalSection		( &m_section );

	compile_parameters* const result = m_head;
	if ( result ) {
		m_head					= m_head->next;
	}

	LeaveCriticalSection		( &m_section );

	return						result;
}

inline void	thread_pool::wait_for_queries_completion	( )
{
	for (;;) {
		for (;;) {
			if ( !m_head )
				break;

			Sleep						( 1 );
		}

		bool found						= false;
		threads_type::iterator i		= m_threads.begin();
		threads_type::iterator const e	= m_threads.end();
		for ( ; i != e; ++i ) {
			if ( !(*i).is_ready() ) {
				found					= true;
				break;
			}
		}

		if ( !found )
			return;

		Sleep							( 1 );
	}
}

void compile_shaders		(
		total_info_type const& total_info,
		char const* const full_path,
		char const* const render,
		char const* const target_version,
		char const* const ignored_extensions,
		char const* const* ignore_shader_id_first,
		char const* const* ignore_shader_id_last,
		definition* const defines,
		size_t const define_count,
		D3D_SHADER_MACRO* macros,
		size_t const& time_start,
		CRITICAL_SECTION& section,
		unsigned int& shader_count,
		unsigned int& error_count,
		unsigned int& total_size,
		thread_pool* pool,
		bool const legacy_compiler
	)
{
	string_path					output_folder;
	strcpy_s					( output_folder, full_path );
	strcat_s					( output_folder, "objects\\" );
	_mkdir						( output_folder );

	strcat_s					( output_folder, render );
	strcat_s					( output_folder, "\\" );
	_mkdir						( output_folder );

	includer					includer( total_info, full_path );

	printf						( "\nStarted shaders compilation for %s:\n", render );

	total_info_type::const_iterator	i		= total_info.begin();
	total_info_type::const_iterator const e	= total_info.end();
	for ( ; i != e; ++i ) {
		string_path				drive, folder, name, extension;
		_splitpath_s			( (*i).first.c_str(), drive, folder, name, extension );
		char const* temp_target	= 0;
		if ( !strcmp(extension, ".ps") ) {
			if ( strstr(ignored_extensions,".ps") )
				continue;

			temp_target			= "ps_";
		}
		else if ( !strcmp(extension, ".vs") ) {
			if ( strstr(ignored_extensions,".vs") )
				continue;

			temp_target			= "vs_";
		}
		else if ( !strcmp(extension, ".gs") ) {
			if ( strstr(ignored_extensions,".gs") )
				continue;

			temp_target			= "gs_";
		}
		else if ( !strcmp(extension, ".cs") ) {
			if ( strstr(ignored_extensions,".cs") )
				continue;

			temp_target			= "cs_";
		}
		else if ( !strcmp(extension, ".hs") ) {
			if ( strstr(ignored_extensions,".hs") )
				continue;

			temp_target			= "hs_";
		}
		else if ( !strcmp(extension, ".ds") ) {
			if ( strstr(ignored_extensions,".ds") )
				continue;

			temp_target			= "ds_";
		}
		else
			continue;

		char target[16];
		strcpy_s				(target, temp_target);
		strcat_s				(target, target_version);

		strcat_s				( name, extension );

		bool found				= false;
		for (char const* const* i = ignore_shader_id_first; i != ignore_shader_id_last; ++i ) {
			if ( !strcmp(*i,name) ) {
				EnterCriticalSection( &section );
				++shader_count;
				printf			( "%4d. %-42s skipped due to specified ignore list\n", shader_count, name );
				LeaveCriticalSection( &section );
				found			= true;
				break;
			}
		}

		if ( !found ) {
			strcat_s			( drive, folder );
			compile_shader		( defines, define_count, total_info, includer, macros, drive, output_folder, name, target, shader_count, error_count, time_start, total_size, section, pool, legacy_compiler );
		}
	}
}

void compile_shaders		( total_info_type const& total_info, char const* const full_path )
{
	size_t const define_r12_count	= s_define_r2_count > s_define_r1_count ? s_define_r2_count : s_define_r1_count;
	size_t const define_r123_count	= s_define_r3_count > define_r12_count ? s_define_r3_count : define_r12_count;
	size_t const define_count		= s_define_r4_count > define_r123_count ? s_define_r4_count : define_r123_count;
	D3D_SHADER_MACRO			macros[define_count];

	size_t const time_start		= timeGetTime( );

	CRITICAL_SECTION			section;
	InitializeCriticalSection	( &section );

	thread_pool* pool			= 0;
	if ( strstr(GetCommandLine(), "-mp") && get_core_count() > 1 ) {
		pool					= new thread_pool( 1 );
	}

	unsigned int shader_count	= 0;
	unsigned int error_count	= 0;
	unsigned int total_size		= 0;

// 	if ( strstr(GetCommandLine(), "-dx9") ) {
// 		compile_shaders			( total_info, full_path, "r2", "2_0", ".gs;.cs;.hs;.ds", 0, 0, defines_r1, s_define_r1_count, macros, time_start, section, shader_count, error_count, total_size, pool, true );
// 	}
// 	else if ( strstr(GetCommandLine(), "-dx10") ) {
// 		compile_shaders			( total_info, full_path, "dx10", "4_0", ".cs;.hs;.ds", 0, 0, defines_r1, s_define_r1_count, macros, time_start, section, shader_count, error_count, total_size, pool, true );
// 	}
// 	else if ( strstr(GetCommandLine(), "-dx11") ) {
// 		compile_shaders			( total_info, full_path, "dx11", "5_0", "", 0, 0, defines_r1, s_define_r1_count, macros, time_start, section, shader_count, error_count, total_size, pool, false );
// 	}
// 	else 
	if ( strstr(GetCommandLine(), "-r1") ) {
		compile_shaders			( total_info, full_path, "r1", "2_0", ".gs;.cs;.hs;.ds", 0, 0, defines_r1, s_define_r1_count, macros, time_start, section, shader_count, error_count, total_size, pool, false );
	}
	else if ( strstr(GetCommandLine(), "-r2") ) {
		static char const* const ignore_shader_ids[]	= {
			"accum_sun_near_new.ps",
			"accum_sun_near_var1.ps",
			"accum_sun_near_var2.ps",
			"combine_2.ps",
			"deffer_base_aref_steep-hq.ps",
			"deffer_base_aref_steep_d-hq.ps",
			"deffer_base_aref_steep_db-hq.ps",
			"deffer_base_lmh_aref_steep_d-hq.ps",
			"deffer_base_lmh_aref_steep_db-hq.ps",
			"deffer_base_lmh_aref_steep-hq.ps",
			"deffer_base_lmh_steep-hq.ps",
			"deffer_base_lmh_steep_d-hq.ps",
			"deffer_base_lmh_steep_db-hq.ps",
			"deffer_base_steep-hq.ps",
			"deffer_base_steep_d-hq.ps",
			"deffer_base_steep_db-hq.ps",
			"ssao.ps",
			"ssao_blur.ps",
			"ssao_hbao.ps",
			"water_soft.ps"
		};
		compile_shaders			( total_info, full_path, "r2", "2_0", ".gs;.cs;.hs;.ds", ignore_shader_ids, ignore_shader_ids + sizeof(ignore_shader_ids)/sizeof(ignore_shader_ids[0]), defines_r2, s_define_r2_count, macros, time_start, section, shader_count, error_count, total_size, pool, true );
	}
	else if ( strstr(GetCommandLine(), "-r3") ) {
		static char const* const ignore_shader_ids[]	= {
			"fluid_draw_texture.ps",
			"fluid_obststaticbox.ps",
			"gather.ps",
			"ssao.ps",
			"ssao_blur.ps",
			"ssao_hbao.ps",
			"ssao_hdao.ps",
			"ssao_hdao_new.ps"
		};

		compile_shaders			( total_info, full_path, "r3", "4_0", ".cs;.hs;.ds", ignore_shader_ids, ignore_shader_ids + sizeof(ignore_shader_ids)/sizeof(ignore_shader_ids[0]), defines_r3, s_define_r3_count, macros, time_start, section, shader_count, error_count, total_size, pool, false );
	}
	else if ( strstr(GetCommandLine(), "-r4") ) {
		static char const* const ignore_shader_ids[]	= {
			"fluid_draw_texture.ps",
			"fluid_obststaticbox.ps",
			"gather.ps",
			"ssao.ps",
			"ssao_blur.ps",
			"ssao_hbao.ps",
			"ssao_hdao.ps",
			"ssao_hdao_new.ps"
		};

		compile_shaders			( total_info, full_path, "r4", "5_0", "", ignore_shader_ids, ignore_shader_ids + sizeof(ignore_shader_ids)/sizeof(ignore_shader_ids[0]), defines_r4, s_define_r4_count, macros, time_start, section, shader_count, error_count, total_size, pool, false );
	}
	else {
		printf					( "specify compilation parameters(-r1, -r2, -r3 or -r4)\n" );
	}

	if ( pool ) {
		pool->run_reserved_threads			( );
		pool->wait_for_queries_completion	( );
		delete					pool;
	}

	size_t const time_stop		= timeGetTime( );
	float const time			= float(time_stop - time_start)/1000.f;
	if ( error_count )
		printf					(
			"\n%d shaders has been generated with %d errors! (%9.2fs)\n",
			shader_count,
			error_count,
			time
		);
	else
		printf					(
			"\n%d shaders has been generated with no errors! (%9.2fs)\n",
			shader_count,
			time
		);
}