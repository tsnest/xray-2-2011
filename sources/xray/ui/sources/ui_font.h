#ifndef UI_FONT_H_INCLUDED
#define UI_FONT_H_INCLUDED

#include <xray/ui/ui.h>
#include "ui_base.h"

namespace xray {
namespace ui {

class ui_font : public xray::ui::font, public ui_base
{
public:
			ui_font			( memory::base_allocator& a );
							~ui_font		( );
	void					init_font		( pcstr name );
protected:
	f32						m_height;
	float2					m_ts_size;
	u32						m_char_count;
	float3*					m_char_map;
public:
	virtual float3 const&	get_char_tc		( u8 const& ch )	const		{return m_char_map[ch];}
	virtual float3 			get_char_tc_ts	( u8 const& ch )	const;
	virtual float const&	get_height		( )	const						{return m_height;}
	virtual float			get_height_ts	( )	const;
	virtual void			parse_word		( pcstr str, float& out_length, pcstr& out_next_word ) const;
};

class font_manager : public ui_base
{
	ui_font					m_font;
	void					initialize_fonts( );
public:
							font_manager	( memory::base_allocator& a );
							~font_manager	( );
	ui_font const *			get_font		( )	const	{return &m_font;}
	void					on_device_reset	( );
};

f32 calc_string_length		(ui_font const& f, pcstr str);
f32 calc_string_length_n	(ui_font const& f, pcstr str, u16 const str_len);

} // namespace xray
} // namespace ui

#endif //UI_FONT_H_INCLUDED