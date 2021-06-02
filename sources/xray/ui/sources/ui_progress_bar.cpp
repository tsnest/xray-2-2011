////////////////////////////////////////////////////////////////////////////
//	Created		: 14.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ui_progress_bar.h"
#include <xray/render/facade/ui_renderer.h>
namespace xray {
namespace ui {

ui_progress_bar::ui_progress_bar		( ui_world& world ) :
	ui_window			( world.allocator( ) ),
	m_ui_world			( world ),
	m_minimum			( 0 ),
	m_maximum			( 100 ),
	m_value				( 0 ),
	m_border_width		( 1 ),
	m_border_height		( 1 ),
	m_back_color		( 23, 14,  143 ),
	m_front_color		( 17, 104, 236 ),
	m_text_color		( 206, 210, 43 ),
	m_draw_text			( false )
{}

ui_progress_bar::~ui_progress_bar		( )
{}

void ui_progress_bar::set_range			( u32 minimum, u32 maximum )
{
	R_ASSERT			( maximum > minimum );
	m_minimum			= minimum;
	m_maximum			= maximum;
}

void ui_progress_bar::set_minimum		( u32 minimum )
{
	R_ASSERT			( m_maximum > minimum );
	m_minimum			= minimum;
}

u32 ui_progress_bar::get_minimum		( ) const
{
	return m_minimum;
}

void ui_progress_bar::set_maximum		( u32 maximum )
{
	R_ASSERT			( maximum > m_minimum );
	m_maximum			= maximum;
}

u32 ui_progress_bar::get_maximum		( ) const
{
	return m_maximum;
}

void ui_progress_bar::set_value			( u32 value )
{
	R_ASSERT			( value >= m_minimum && value <= m_maximum );
	m_value				= value;
}

u32 ui_progress_bar::get_value			( ) const
{
	return m_value;
}

void ui_progress_bar::reset				( )
{
	m_value				= m_minimum;
}

void ui_progress_bar::set_border_width	( u32 width )
{
	R_ASSERT			( width < m_size.x / 2 );
	m_border_width		= width;
}

u32 ui_progress_bar::get_border_width	( ) const
{
	return m_border_width;
}

void ui_progress_bar::set_border_height	( u32 height )
{
	R_ASSERT			( height < m_size.y / 2 );
	m_border_height		= height;
}

u32 ui_progress_bar::get_border_height	( ) const
{
	return m_border_height;
}

void ui_progress_bar::set_back_color	( math::color color )
{
	m_back_color		= color;
}

math::color ui_progress_bar::get_back_color		( ) const
{
	return m_back_color;
}

void ui_progress_bar::set_front_color			( math::color color )
{
	m_front_color		= color;
}

math::color ui_progress_bar::get_front_color	( ) const
{
	return m_front_color;
}

void ui_progress_bar::draw_text					( bool value )
{
	m_draw_text			= value;
}

void ui_progress_bar::set_text_color			( math::color color )
{
	m_text_color		= color;
}

math::color ui_progress_bar::get_text_color	( ) const
{
	return m_text_color;
}

void ui_progress_bar::draw				(
											xray::render::ui::renderer& renderer,
											xray::render::scene_view_ptr const& scene_view
										)
{
	ui_window::draw					( renderer, scene_view );
	draw_back_rectangle				( renderer, scene_view );
	if ( m_value != m_minimum )
		draw_front_rectangle		( renderer, scene_view );
	if ( m_draw_text )
		draw_text					( renderer, scene_view );
}

void ui_progress_bar::draw_back_rectangle			(
														xray::render::ui::renderer& renderer,
														xray::render::scene_view_ptr const& scene_view
													) const
{
	float2 pos				= get_position( );
	client_to_screen		( *this, pos );		//need absolute position
	float2 size				= get_size( );

	xray::render::ui::vertex const back_vertices[] = {
		xray::render::ui::vertex( pos.x,			pos.y + size.y,	0.0f, m_back_color.m_value, 0, 1 ),
		xray::render::ui::vertex( pos.x,			pos.y,			0.0f, m_back_color.m_value, 0, 0 ),
		xray::render::ui::vertex( pos.x + size.x,	pos.y + size.y,	0.0f, m_back_color.m_value, 1, 1 ),
		xray::render::ui::vertex( pos.x + size.x,	pos.y,			0.0f, m_back_color.m_value, 1, 0 )
	};

	u32 const primitives_type	= 0; // 0-tri-list, 1-line-list
	u32 const points_type		= 2; // rect

	renderer.draw_vertices	( scene_view, back_vertices, primitives_type, points_type );
}

void ui_progress_bar::draw_front_rectangle		(
													xray::render::ui::renderer& renderer,
													xray::render::scene_view_ptr const& scene_view
												) const
{
	// inner rect

	float2 pos										= get_position( );
	client_to_screen								( *this, pos );		//need absolute position
	float2 size										= get_size( );
	u32 diff										= m_maximum - m_minimum;
	float2 inner_size								= float2( size.x - 2.0f * m_border_width, size.y - 2.0f * m_border_height );
	inner_size.x									*= (float)(m_value - m_minimum)/(float)(diff);
	float2 inner_pos								= float2( pos.x + m_border_width, pos.y + m_border_height );
	xray::render::ui::vertex const top_vertices[] = {
		xray::render::ui::vertex( inner_pos.x,					inner_pos.y + inner_size.y,	0.0f, m_front_color.m_value, 0,	1 ),
		xray::render::ui::vertex( inner_pos.x,					inner_pos.y,				0.0f, m_front_color.m_value, 0,	0 ),
		xray::render::ui::vertex( inner_pos.x + inner_size.x,	inner_pos.y + inner_size.y,	0.0f, m_front_color.m_value, 1,	1 ),
		xray::render::ui::vertex( inner_pos.x + inner_size.x,	inner_pos.y,				0.0f, m_front_color.m_value, 1,	0 )
	};

	u32 const primitives_type	= 0; // 0-tri-list, 1-line-list
	u32 const points_type		= 2; // rect

	renderer.draw_vertices	( scene_view, top_vertices, primitives_type, points_type );
}

void ui_progress_bar::draw_text					(
													xray::render::ui::renderer& renderer,
													xray::render::scene_view_ptr const& scene_view
												) const
{
	float2 pos					= get_position( );
	client_to_screen			( *this, pos );		//need absolute position
	float2 size					= get_size( );

	fixed_string<32> str;
	str.assignf					( "%.2f %%", ((float)(m_value - m_minimum)/(float)(m_maximum - m_minimum)) * 100.f );
	pcstr next_word				= NULL;
	float text_width			= 0.0f;
	ui_font const* fnt			= m_ui_world.get_font_manager().get_font();
	fnt->parse_word				( str.c_str(), text_width, next_word );
	float text_height			= fnt->get_height( );
	float2 font_pos				= pos;
	font_pos.x					= ( size.x / 2.0f ) - text_width / 2.0f + pos.x;
	font_pos.y					= ( size.y / 2.0f ) - text_height / 2.0f + pos.y;

	renderer.draw_text			(
		scene_view,
		str.c_str( ), 
		*fnt, 
		font_pos, 
		m_text_color, 
		m_text_color, 
		math::floor( size.x ),
		false, 
		0, 
		0
		);
}

} // namespace ui
} // namespace xray
