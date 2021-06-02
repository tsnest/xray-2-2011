////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include "game_console.h"
#include "apc.h"
#include <xray/input/api.h>
#include <xray/input/world.h>
#include <xray/physics/api.h>
#include <xray/physics/world.h>
#include <xray/network/api.h>
#include <xray/sound/api.h>
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/engine_renderer.h>
#include <boost/bind.hpp>
#include <xray/rtp/api.h>
#include <xray/engine/engine_user.h>
#include <xray/animation/api.h>
#include <xray/ai_navigation/api.h>
#include <xray/render/world.h>

using xray::engine::engine_world;

void engine_world::initialize_logic_thread	( )
{
	m_engine_user_module_proxy.allocator().user_current_thread_id ( );
}

void engine_world::initialize_logic_modules( )
{
	ASSERT						( m_initialized );
	ASSERT						( !m_engine_user_world );

	m_engine_user_world			=
		m_engine_user_module_proxy.create_world( 
			*this,
			*m_render_world,
			*m_sound_world,
			*m_network_world
		);

	engine_user_world().enable	( m_game_enabled );
}

HWND new_window	( );

void engine_world::initialize_logic		( )
{
	apc::wait					( apc::editor );

	if ( !m_editor ) {
		m_main_window_handle	= new_window( );
		m_render_window_handle	= m_main_window_handle;
	}

	m_logic_frame_id			= 0;

	if ( threading::core_count() == 1 )
		return;

	if ( m_editor && command_line_editor_singlethread() )
		return;

	apc::assign_thread_id		( apc::logic, u32(-1) );

	threading::spawn			(
		boost::bind( &engine_world::logic, this ), 
		"logic", 
		"logic", 
		0,
		1 % threading::core_count()
	);
}

void engine_world::logic_finalize_modules	( )
{
	on_application_deactivate			( );
	m_engine_user_module_proxy.destroy_world(m_engine_user_world);
}

void engine_world::logic_dispatch_callbacks	( )
{
	xray::resources::dispatch_callbacks		( );
	xray::network::dispatch_callbacks		( );
	m_sound_world->get_logic_world_user().dispatch_callbacks	( );
	m_render_world->logic_channel().owner_delete_processed_items( );
}

void engine_world::logic_tick				( )
{
	logic_dispatch_callbacks				( );

	m_last_game_enabled_value			= m_game_enabled;

	if ( !m_game_enabled || !is_application_active()) {
		if ( !m_game_enabled )
			++m_logic_frame_id;

 		return;
	}
	
	m_engine_user_world->tick			( m_logic_frame_id );
	++m_logic_frame_id;
}

void engine_world::logic				( )
{
	apc::assign_thread_id		( apc::logic, threading::current_thread_id( ) );
	apc::process				( apc::logic );

	for (;;) {
		if ( m_destruction_started )
			break;

		logic_tick				( );

		if ( ( m_logic_frame_id > m_render_world->engine_renderer().frame_id( ) + 1 ) && !m_destruction_started ) {
			do {
				logic_dispatch_callbacks( );
				if ( m_game_enabled && !m_last_game_enabled_value ) {
					// m_logic_frame_id could be even 0
					// all will work correctly
					--m_logic_frame_id;
					continue;
				}

				if ( !apc::try_process_single_call( apc::logic ) )
					threading::yield		( 1 );
			} while ( ( m_logic_frame_id > m_render_world->engine_renderer().frame_id( ) + 1 ) && !m_destruction_started );
		}
		else
			apc::try_process_single_call( apc::logic );
	}

	apc::process				( apc::logic );
}

void engine_world::logic_clear_resources		( )
{
	logic_dispatch_callbacks		( );
	m_engine_user_world->clear_resources( );
}

xray::engine::console* engine_world::create_game_console( xray::ui::world& uw, xray::input::world& iw )
{
	return XRAY_NEW_IMPL(m_engine_user_module_proxy.allocator(), game_console)(	uw, iw, m_engine_user_module_proxy.allocator() );
}