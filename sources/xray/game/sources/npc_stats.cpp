////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "npc_stats.h"
#include <xray/ui/ui.h>
#include <xray/ui/world.h>
#include <xray/ai/npc_statistics.h>
#include "human_npc.h"

namespace stalker2 {

npc_stats::npc_stats					( xray::ui::world& ui_world ) :
	m_ui_world							( ui_world ),
	m_caption_color						( xray::math::color( 255, 255, 128 ).m_value ),
	m_text_color						( xray::math::color( 128, 255, 255 ).m_value ),
	m_line_height						( 20.f ),
	m_column_width						( 180.f )
{
	m_main_window						= m_ui_world.create_window();
	m_main_window->set_visible			( true );
	m_main_window->set_position			( float2( 0.f, 0.f ) );
	m_main_window->set_size				( float2( 1024.f, 768.f ) );
}

ui::text* npc_stats::create_new_group	(
		column_types_enum column_number,
		u32 font_color,
		pcstr text,
		ui::window* upper_window
	)
{
	R_ASSERT							( m_main_window );
	
	ui::text* title_text				= m_ui_world.create_text();
	title_text->w()->set_visible		( true );
	float2 const position				( column_number * m_column_width,
										  upper_window ? upper_window->get_position().y + upper_window->get_size().y : 0 );
	title_text->w()->set_position		( position );
	title_text->w()->set_size			( float2( m_column_width, m_line_height ) );
	title_text->set_font				( xray::ui::fnt_arial );
	title_text->set_text_mode			( xray::ui::tm_default );
	title_text->set_color				( font_color );
	m_main_window->add_child			( title_text->w(), true );
	title_text->set_text				( text );
	return								title_text;
}

npc_stats::~npc_stats					( )
{
	m_ui_world.destroy_window			( m_main_window );
}

void npc_stats::draw					( xray::render::ui::renderer& ui_renderer, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER			( scene_view );
	m_main_window->draw					( ui_renderer, m_ui_world.get_scene_view() );
}

void npc_stats::set_stats				( human_npc const* const owner )
{
	m_main_window->remove_all_childs	( );

	if ( !owner )
		return;
	
	ai::npc_statistics					stats;
	owner->fill_stats					( stats );
	
	ui::text* last_item					= 0;

	// 1st column: sensors and target selectors
	for ( u32 i = 0; i < stats.sensors_state.size(); ++i )
	{
		last_item						= create_new_group(
											column_1,
											m_caption_color,
											stats.sensors_state[i].caption.c_str(),
											last_item ? last_item->w() : 0
										);
		for ( u32 j = 0; j < stats.sensors_state[i].content.size(); ++j )
		{
			last_item					= create_new_group(
											column_1,
											m_text_color,
											stats.sensors_state[i].content[j].c_str(),
											last_item ? last_item->w() : 0
										);
		}
	}

	for ( u32 i = 0; i < stats.selectors_state.size(); ++i )
	{
		last_item						= create_new_group(
											column_1,
											m_caption_color,
											stats.selectors_state[i].caption.c_str(),
											last_item ? last_item->w() : 0
										);
		for ( u32 j = 0; j < stats.selectors_state[i].content.size(); ++j )
		{
			last_item					= create_new_group(
											column_1,
											m_text_color,
											stats.selectors_state[i].content[j].c_str(),
											last_item ? last_item->w() : 0
										);
		}
	}

	last_item							= 0;
	
	// 2nd column: working memory and blackboard
	last_item							= create_new_group(
											column_2,
											m_caption_color,
											stats.working_memory_state.caption.c_str(),
											last_item ? last_item->w() : 0
										);
	for ( u32 j = 0; j < stats.working_memory_state.content.size(); ++j )
	{
		last_item						= create_new_group(
											column_2,
											m_text_color,
											stats.working_memory_state.content[j].c_str(),
											last_item ? last_item->w() : 0
										);
	}

	last_item							= create_new_group(
											column_2,
											m_caption_color,
											stats.blackboard_state.caption.c_str(),
											last_item ? last_item->w() : 0
										);
	for ( u32 j = 0; j < stats.blackboard_state.content.size(); ++j )
	{
		last_item						= create_new_group(
											column_2,
											m_text_color,
											stats.blackboard_state.content[j].c_str(),
											last_item ? last_item->w() : 0
										);
	}
	
	last_item							= 0;
	
	// 3rd column: general info and planning
	last_item							= create_new_group(
											column_3,
											m_caption_color,
											stats.general_state.caption.c_str(),
											last_item ? last_item->w() : 0
										);
	for ( u32 j = 0; j < stats.general_state.content.size(); ++j )
	{
		last_item						= create_new_group(
											column_3,
											m_text_color,
											stats.general_state.content[j].c_str(),
											last_item ? last_item->w() : 0
										);
	}
}

} //namespace stalker2
