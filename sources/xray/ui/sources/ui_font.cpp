////////////////////////////////////////////////////////////////////////////
//	Created		: 30.01.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ui_font.h"
#include "ui_text.h"

namespace xray {
namespace ui {

extern float4 arial_21_symb [];

ui_font::ui_font(memory::base_allocator& a)
:ui_base		( a ),
m_char_map		( NULL )
{}

ui_font::~ui_font( )
{
	XRAY_FREE_IMPL		( allocator(), m_char_map );
}

void ui_font::init_font(pcstr name)
{
	XRAY_UNREFERENCED_PARAMETER	(name);
	//. create render-specific resource(shader+geom)
	m_height			= 21.0f;
	m_char_count		= 256;
	
	m_ts_size.x			= 256.0f;
	m_ts_size.y			= 256.0f;

	XRAY_FREE_IMPL		( allocator(), m_char_map );
	m_char_map			= XRAY_ALLOC_IMPL( allocator(), math::float3, m_char_count );
//	m_char_map			= (float3*)REALLOC(m_char_map, sizeof(float3) * m_char_count, "font charmap");

	for (u32 idx=0; idx<m_char_count; ++idx)
	{
		float3& ch		= m_char_map[idx];
		ch.x			= arial_21_symb[idx].x;
		ch.y			= arial_21_symb[idx].y;
		ch.z			= arial_21_symb[idx].z - arial_21_symb[idx].x;
	}
}

float3 ui_font::get_char_tc_ts(u8 const& ch)	const
{
	float3	result		= get_char_tc(ch);
	result.x 			/= m_ts_size.x;
	result.y 			/= m_ts_size.y;
	result.z 			/= m_ts_size.x;

// 	result.x			+= (0.5f / m_ts_size.x);
// 	result.y			+= (0.5f / m_ts_size.y);
	
	return				result;
}

void ui_font::parse_word( pcstr str, float& out_length, pcstr& out_next_word ) const
{
	xray::ui::parse_word(str, this, out_length, out_next_word);
}

float ui_font::get_height_ts()	const
{
	return get_height()/m_ts_size.y;
}

font_manager::font_manager( memory::base_allocator& a )
:ui_base( a ),
m_font	( a )
{
	initialize_fonts();
}

font_manager::~font_manager()
{}

void font_manager::initialize_fonts()
{
	m_font.init_font("arial_14");
}

void font_manager::on_device_reset()
{
	initialize_fonts();
}

f32 calc_string_length(ui_font const& f, pcstr str)
{
	pcstr p			= str;
	f32	result		= 0.0f;
	while (*p)
	{
		result		+= f.get_char_tc(*p).z;
		++p;
	}
	return			result;
}

f32 calc_string_length_n	(ui_font const& f, pcstr str, u16 const str_len)
{
	pcstr p				= str;
	f32	result			= 0.0f;
	u16 char_counter	= str_len;
	while (*p && char_counter)
	{
		result		+= f.get_char_tc(*p).z;
		++p;
		--char_counter;
	}
	return			result;
}

/*
[symbol_coords]
height = 21
*/
float4 arial_21_symb [] = {
float4(    0,    0,    0,    0),
float4(	   0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    0,    0),
float4(    0,    0,    8,   21),
float4(    8,    0,   12,   21),
float4(   12,    0,   17,   21),
float4(   17,    0,   25,   21),
float4(   25,    0,   33,   21),
float4(   33,    0,   45,   21),
float4(   45,    0,   54,   21),
float4(   54,    0,   57,   21),
float4(   57,    0,   62,   21),
float4(   62,    0,   67,   21),
float4(   67,    0,   73,   21),
float4(   73,    0,   81,   21),
float4(   81,    0,   85,   21),
float4(   85,    0,   90,   21),
float4(   90,    0,   94,   21),
float4(   94,    0,   98,   21),
float4(   98,    0,  106,   21),
float4(  106,    0,  114,   21),
float4(  114,    0,  122,   21),
float4(  122,    0,  130,   21),
float4(  130,    0,  138,   21),
float4(  138,    0,  146,   21),
float4(  146,    0,  154,   21),
float4(  154,    0,  162,   21),
float4(  162,    0,  170,   21),
float4(  170,    0,  178,   21),
float4(  178,    0,  182,   21),
float4(  182,    0,  186,   21),
float4(  186,    0,  194,   21),
float4(  194,    0,  202,   21),
float4(  202,    0,  210,   21),
float4(  210,    0,  218,   21),
float4(  218,    0,  231,   21),
float4(  231,    0,  239,   21),
float4(  239,    0,  248,   21),
float4(    0,   21,   10,   42),
float4(   10,   21,   20,   42),
float4(   20,   21,   29,   42),
float4(   29,   21,   38,   42),
float4(   38,   21,   49,   42),
float4(   49,   21,   59,   42),
float4(   59,   21,   63,   42),
float4(   63,   21,   70,   42),
float4(   70,   21,   79,   42),
float4(   79,   21,   87,   42),
float4(   87,   21,   99,   42),
float4(   99,   21,  109,   42),
float4(  109,   21,  120,   42),
float4(  120,   21,  129,   42),
float4(  129,   21,  140,   42),
float4(  140,   21,  150,   42),
float4(  150,   21,  159,   42),
float4(  159,   21,  169,   42),
float4(  169,   21,  179,   42),
float4(  179,   21,  187,   42),
float4(  187,   21,  199,   42),
float4(  199,   21,  208,   42),
float4(  208,   21,  216,   42),
float4(  216,   21,  225,   42),
float4(  225,   21,  229,   42),
float4(  229,   21,  233,   42),
float4(  233,   21,  237,   42),
float4(  237,   21,  244,   42),
float4(  244,   21,  252,   42),
float4(    0,   42,    5,   63),
float4(    5,   42,   13,   63),
float4(   13,   42,   21,   63),
float4(   21,   42,   29,   63),
float4(   29,   42,   37,   63),
float4(   37,   42,   45,   63),
float4(   45,   42,   49,   63),
float4(   49,   42,   57,   63),
float4(   57,   42,   65,   63),
float4(   65,   42,   69,   63),
float4(   69,   42,   73,   63),
float4(   73,   42,   80,   63),
float4(   80,   42,   84,   63),
float4(   84,   42,   96,   63),
float4(   96,   42,  104,   63),
float4(  104,   42,  112,   63),
float4(  112,   42,  120,   63),
float4(  120,   42,  128,   63),
float4(  128,   42,  133,   63),
float4(  133,   42,  140,   63),
float4(  140,   42,  144,   63),
float4(  144,   42,  152,   63),
float4(  152,   42,  158,   63),
float4(  158,   42,  168,   63),
float4(  168,   42,  175,   63),
float4(  175,   42,  181,   63),
float4(  181,   42,  187,   63),
float4(  187,   42,  192,   63),
float4(  192,   42,  196,   63),
float4(  196,   42,  201,   63),
float4(  201,   42,  209,   63),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(  209,   42,  213,   63),
float4(    0,    0,    8,   21),
float4(  213,   42,  218,   63),
float4(  218,   42,  231,   63),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(  231,   42,  239,   63),
float4(  239,   42,  254,   63),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,   63,    8,   84),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    8,   63,   16,   84),
float4(   16,   63,   20,   84),
float4(   20,   63,   24,   84),
float4(   24,   63,   29,   84),
float4(   29,   63,   34,   84),
float4(   34,   63,   39,   84),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(   39,   63,   52,   84),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(   52,   63,   58,   84),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(   58,   63,   67,   84),
float4(   67,   63,   73,   84),
float4(   73,   63,   80,   84),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(    0,    0,    8,   21),
float4(   80,   63,   88,   84),
float4(   88,   63,   97,   84),
float4(   97,   63,  107,   84),
float4(  107,   63,  117,   84),
float4(  117,   63,  125,   84),
float4(    0,    0,    8,   21),
float4(  125,   63,  130,   84),
float4(  130,   63,  140,   84),
float4(  140,   63,  144,   84),
float4(  144,   63,  151,   84),
float4(  151,   63,  160,   84),
float4(  160,   63,  164,   84),
float4(  164,   63,  168,   84),
float4(    0,    0,    8,   21),
float4(  168,   63,  178,   84),
float4(  178,   63,  185,   84),
float4(  185,   63,  190,   84),
float4(  190,   63,  198,   84),
float4(  198,   63,  212,   84),
float4(  212,   63,  219,   84),
float4(  219,   63,  227,   84),
float4(    0,    0,    8,   21),
float4(  227,   63,  236,   84),
float4(  236,   63,  243,   84),
float4(  243,   63,  247,   84),
float4(  247,   63,  255,   84),
float4(    0,   84,    9,  105),
float4(    9,   84,   18,  105),
float4(   18,   84,   26,  105),
float4(   26,   84,   35,  105),
float4(   35,   84,   44,  105),
float4(   44,   84,   56,  105),
float4(   56,   84,   64,  105),
float4(   64,   84,   74,  105),
float4(   74,   84,   84,  105),
float4(   84,   84,   92,  105),
float4(   92,   84,  101,  105),
float4(  101,   84,  113,  105),
float4(  113,   84,  123,  105),
float4(  123,   84,  134,  105),
float4(  134,   84,  144,  105),
float4(  144,   84,  153,  105),
float4(  153,   84,  163,  105),
float4(  163,   84,  173,  105),
float4(  173,   84,  182,  105),
float4(  182,   84,  192,  105),
float4(  192,   84,  201,  105),
float4(  201,   84,  211,  105),
float4(  211,   84,  220,  105),
float4(  220,   84,  232,  105),
float4(  232,   84,  244,  105),
float4(  244,   84,  255,  105),
float4(    0,  105,   12,  126),
float4(   12,  105,   21,  126),
float4(   21,  105,   31,  126),
float4(   31,  105,   44,  126),
float4(   44,  105,   54,  126),
float4(   54,  105,   62,  126),
float4(   62,  105,   70,  126),
float4(   70,  105,   78,  126),
float4(   78,  105,   83,  126),
float4(   83,  105,   91,  126),
float4(   91,  105,   99,  126),
float4(   99,  105,  108,  126),
float4(  108,  105,  115,  126),
float4(  115,  105,  123,  126),
float4(  123,  105,  131,  126),
float4(  131,  105,  137,  126),
float4(  137,  105,  145,  126),
float4(  145,  105,  154,  126),
float4(  154,  105,  162,  126),
float4(  162,  105,  170,  126),
float4(  170,  105,  178,  126),
float4(  178,  105,  186,  126),
float4(  186,  105,  194,  126),
float4(  194,  105,  200,  126),
float4(  200,  105,  206,  126),
float4(  206,  105,  217,  126),
float4(  217,  105,  224,  126),
float4(  224,  105,  232,  126),
float4(  232,  105,  239,  126),
float4(  239,  105,  249,  126),
float4(    0,  126,   11,  147),
float4(   11,  126,   20,  147),
float4(   20,  126,   30,  147),
float4(   30,  126,   37,  147),
float4(   37,  126,   44,  147),
float4(   44,  126,   54,  147),
float4(   54,  126,   62,  147)
};						

} // namespace xray
} // namespace ui
