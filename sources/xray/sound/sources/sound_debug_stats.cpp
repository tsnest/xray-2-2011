////////////////////////////////////////////////////////////////////////////
//	Created		: 24.10.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_debug_stats.h>
#include <xray/ui/ui.h>
#include <xray/ui/world.h>
#include <xray/linkage_helper.h>
#include <xray/ui/text_tree_draw_helper.h>
#include <xray/render/facade/ui_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/text_tree.h>
#include <xray/sound/sound_rms.h>
#include <xray/sound/world_user.h>
#include "sound_scene.h"

XRAY_DECLARE_LINKAGE_ID					( sound_debug_stats_id );

namespace xray {
namespace sound {

threading::atomic32_type sound_debug_stats::m_s_debug_draw_mode		= 0;
threading::atomic32_type sound_debug_stats::m_s_proxy_id			= 0;

u32 const max_propagator_statistics_count = 32;

void sound_debug_stats::set_debug_draw_mode			( mode debug_draw_mode )
{
	R_ASSERT							( debug_draw_mode < mode_count );
	threading::interlocked_exchange		( m_s_debug_draw_mode, debug_draw_mode );
}

void sound_debug_stats::set_detail_view_proxy_id	( u32 proxy_id )
{
	set_debug_draw_mode					( detail );
	threading::interlocked_exchange		( m_s_proxy_id, proxy_id );
}

sound_debug_stats::sound_debug_stats	( memory::base_allocator* allocator, world_user& user, sound_scene_ptr scene, ui::world& ui_world ) :
	m_ui_world					( ui_world ),
	m_world_user				( user ),
	m_scene						( static_cast_checked<sound_scene*>( scene.c_ptr() ) ),
	m_allocator					( *allocator ),
	m_actual_statistic			( -1 )
{
	m_statistic[0]				= 0;
	m_statistic[1]				= 0;

	m_main_window								= m_ui_world.create_window( );
	m_main_window->set_visible					( true );
	m_main_window->set_position					( float2( 0.f, 0.f ) );
	m_main_window->set_size						( float2( 2024.f, 768.f ) );

	m_progress_bars								= XRAY_NEW_ARRAY_IMPL( allocator, ui::progress_bar*, max_propagator_statistics_count );
	for ( u32 i = 0; i < max_propagator_statistics_count; ++i )
	{
		m_progress_bars[i]								= m_ui_world.create_progress_bar( );
		m_progress_bars[i]->get_window( )->set_parent	( m_main_window );
		m_progress_bars[i]->get_window( )->set_visible	( false );
		m_progress_bars[i]->get_window( )->set_size		( float2( 300.0f, 14.0f ) );
		m_progress_bars[i]->get_window( )->set_position	( float2( 400, 15.0f * ( i + 1 ) * 4 ) + 45.0f );
		m_progress_bars[i]->draw_text					( true );
		m_main_window->add_child						( m_progress_bars[i]->get_window( ), false );
	}

	sound_order* const order			= XRAY_NEW_IMPL( m_world_user.get_allocator(), functor_order )
		( boost::bind( &sound_debug_stats::create_statistic, this ) );
	m_world_user.add_order				( order );
}

sound_debug_stats::~sound_debug_stats	( )
{
}

void sound_debug_stats::clear_resources	( world_user& user )
{

	R_ASSERT					( m_main_window );
	m_ui_world.destroy_window	( m_main_window );
	m_main_window				= 0;

	R_ASSERT					( m_statistic );
	R_ASSERT					( m_scene );

	XRAY_DELETE_ARRAY_IMPL		( m_allocator, m_progress_bars );

	m_actual_statistic			= -1;

#ifndef MASTER_GOLD
	for ( u32 i = 0; i < array_size( m_statistic ); ++i )
	{
		functor_order* order		= XRAY_NEW_IMPL( m_allocator, functor_order )
		( boost::bind( &sound_scene::delete_statistic, m_scene, m_statistic[i] ) );

		user.add_order				( order );
	}
#endif //#ifndef MASTER_GOLD

}

#ifndef MASTER_GOLD
void sound_debug_stats::create_statistic	( )
{
	R_ASSERT							( m_actual_statistic == -1 );
	m_statistic[0]						= m_scene->create_statistic( );
	m_statistic[1]						= m_scene->create_statistic( );
	threading::interlocked_exchange		( m_actual_statistic, 0 );

	sound_response* const response		= XRAY_NEW_IMPL( g_allocator, functor_response )
		( boost::bind(&sound_debug_stats::on_statistic_updated, this ) );
	m_world_user.add_response			( response );
}

void sound_debug_stats::update_statistic( )
{
	R_ASSERT							( m_scene );
	if ( m_actual_statistic == -1 )
		return;

	u32 const idx						= m_actual_statistic^1;
	m_scene->delete_statistic			( m_statistic[idx] );
	m_statistic[idx]					= m_scene->create_statistic( );
	threading::interlocked_exchange		( m_actual_statistic, idx );

	sound_response* const response		= XRAY_NEW_IMPL( g_allocator, functor_response )
		( boost::bind(&sound_debug_stats::on_statistic_updated, this ) );
	m_world_user.add_response			( response );

}

void sound_debug_stats::on_statistic_updated( )
{
	if ( m_actual_statistic == -1 )
		return;

	sound_order* const order			= XRAY_NEW_IMPL( m_world_user.get_allocator( ), functor_order )
		( boost::bind( &sound_debug_stats::update_statistic, this ) );
	m_world_user.add_order				( order );
}
#endif //#ifndef MASTER_GOLD

void sound_debug_stats::draw				( render::scene_ptr scene )
{
	R_ASSERT								( m_actual_statistic != -1 );

	switch ( m_s_debug_draw_mode )
	{
	case none:		return;
	case overall:	draw_overall_stats		( scene );				break;
	case detail:	draw_detail_stats		( scene, m_s_proxy_id );break;
	case hdr:		draw_hdr_stats			( scene );				break;
	default:		NODEFAULT				( );
	};
}

#ifndef MASTER_GOLD
void sound_debug_stats::draw_overall_stats	( render::scene_ptr scene )
{
	R_ASSERT										( m_actual_statistic != -1 );

	pvoid buffer									= XRAY_ALLOCA_IMPL( 64 * xray::Kb );
	text_tree text_tree_view						( buffer, 64 * xray::Kb, "sound scene statistic" );

	m_statistic[m_actual_statistic]->fill_text_tree	( &text_tree_view.root( ) );
	update_window									( &text_tree_view.root( ) );
}

void sound_debug_stats::draw_hdr_stats		( render::scene_ptr scene )
{
	R_ASSERT								( m_actual_statistic != -1 );
}

void sound_debug_stats::draw_detail_stats	( render::scene_ptr scene, u32 proxy_id )
{
	R_ASSERT								( m_actual_statistic != -1 );

	proxy_statistic* stats					= m_statistic[m_actual_statistic]->m_proxies_statistic.front( );
	while ( stats )
	{
		if ( stats->m_id == proxy_id )
		{
			pvoid buffer							= XRAY_ALLOCA_IMPL( 64 * xray::Kb );
			text_tree text_tree_view				( buffer, 64 * xray::Kb, "sound proxy statistic" );
			stats->fill_text_tree					( &text_tree_view.root( ), true );
			for ( u32 i = 0; i < max_propagator_statistics_count; ++i )
				m_progress_bars[i]->get_window( )->set_visible	( false );

			m_main_window->remove_all_childs	( );
			u32 i = 0;
			propagator_statistic* prop				= stats->m_propagator_statistics.front( );
			while ( prop )
			{
				m_progress_bars[i]->get_window( )->set_visible	( true );
				m_progress_bars[i]->set_range					( 0, prop->m_length );
				m_progress_bars[i]->set_value					( prop->m_current_playing_time );
				m_main_window->add_child						( m_progress_bars[i]->get_window( ), false );
				prop								= stats->m_propagator_statistics.get_next_of_object( prop );
				++i;
			};
			ui::text_tree_draw_helper_params	params;	
			params.color1						= math::color( 77, 109, 243 ).m_value;
			params.color2						= math::color( 91, 192, 247 ).m_value;
			params.is_multipaged				= false;
			params.fnt							= ui::fnt_arial;
			params.row_height					= 15.0f;
			params.space_between_pages			= 10.0f;
			params.start_pos					= float2(0.0f, 0.0f);
			ui::text_tree_draw_helper h			( m_ui_world, m_main_window, params, debug::g_mt_allocator );

			h.output							( &text_tree_view.root( ) );
			m_main_window->draw					( m_ui_world.get_renderer(), m_ui_world.get_scene_view() );
//			update_window							( &text_tree_view.root( ) );
			return;
		}
		stats									= m_statistic[m_actual_statistic]->m_proxies_statistic.get_next_of_object( stats );
	}

	pvoid buffer							= XRAY_ALLOCA_IMPL( 64 * xray::Kb );
	text_tree text_tree_view				( buffer, 64 * xray::Kb, "sound proxy statistic" );
	text_tree_view.root( ).new_child		( "proxy with current id not exist" );
	update_window							( &text_tree_view.root( ) );
}

void sound_debug_stats::update_window		( xray::strings::text_tree_item* item )
{
	R_ASSERT								( m_main_window );
	R_ASSERT								( item );

	m_main_window->remove_all_childs();

	ui::text_tree_draw_helper_params	params;	
	params.color1						= math::color( 77, 109, 243 ).m_value;
	params.color2						= math::color( 91, 192, 247 ).m_value;
	params.is_multipaged				= false;
	params.fnt							= ui::fnt_arial;
	params.row_height					= 15.0f;
	params.space_between_pages			= 10.0f;
	params.start_pos					= float2(0.0f, 0.0f);
	ui::text_tree_draw_helper h			( m_ui_world, m_main_window, params, debug::g_mt_allocator );

	h.output							( item );
	m_main_window->draw					( m_ui_world.get_renderer(), m_ui_world.get_scene_view() );
}
#endif //#ifndef MASTER_GOLD


} // namespace sound
} // namespace xray