////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include <xray/animation/api.h>
#include <xray/render/facade/engine_renderer.h>
#include <xray/resources_fs.h>
#include <xray/core/core.h>
#include "apc.h"
#include <xray/fs_utils.h>
#include <xray/render/world.h>

using xray::engine::engine_world;

static xray::command_line::key			s_editor_key			("editor", "", "", "run editor");
static xray::command_line::key			s_editor_st_key			("editor_single_thread", "editor_st", "", "run editor in single thread");
static xray::command_line::key			s_no_splash_screen_key	("no_splash_screen", "", "application", "suppress splash screen on startup");

engine_world::engine_world				(
		xray::engine_user::module_proxy& proxy,
		pcstr const command_line,
		pcstr const application,
		pcstr const build_date
	) :
	m_engine_user_module_proxy			( proxy ),
	m_render_world						( 0 ),
	m_network_world						( 0 ),
	m_sound_world						( 0 ),
	m_editor							( 0 ),
	m_engine_user_world					( 0 ),
	m_main_window_handle				( 0 ),
	m_render_window_handle				( 0 ),
	m_logic_frame_id					( 0 ),
	m_editor_frame_id					( 0 ),
	m_destruction_started				( 0 ),
	m_file_system_devices_destruction_started	( 0 ),
	m_resources_destruction_started		( 0 ),
	m_resources_cooker_destruction_started	( 0 ),
	m_early_destruction_started			( false ),
	m_exit_code							( 0	),
	m_initialized						( false ),
	m_game_enabled						( true ),
	m_last_game_enabled_value			( true ),
	m_game_paused_last					( false ),
	m_application_active				( false ),
	m_application_activated				( false )
{
	core::preinitialize					(
		this, 
		log_file_usage( ),
		command_line, 
		command_line::contains_application_true, 
		application,
		build_date
	);

	logging::set_format		(
		logging::format_initiator +
		logging::format_separator(" <") +
		logging::format_verbosity + 
		logging::format_separator(">\t") +
		logging::format_separator("[") +
		logging::format_time +
		logging::format_separator("]\t") +
		logging::format_message
	);

	g_allocator.do_register				(  64*Kb,	"engine"	);
	m_render_allocator.do_register		( 128*Mb,	"render"	);
	m_network_allocator.do_register		(  64*Kb,	"network"	);
	m_sound_allocator.do_register		(	1*Mb,	"sound"		);

	m_engine_user_module_proxy.register_memory_allocators	( );

#ifdef XRAY_STATIC_LIBRARIES
	R_ASSERT_BOX						(!s_editor_key && !s_editor_st_key, "Editor is not supported in static configurations!");
#endif // #ifdef XRAY_STATIC_LIBRARIES

	u64 additional_memory_size			= 0;
	bool const is_command_line_editor	= command_line_editor( );
	if ( is_command_line_editor ) {
		// reserve memory for .NET stuff
		additional_memory_size			= 384*Mb;
		m_editor_allocator.do_register	( 8*Mb,	"editor"	);
	}

	memory::allocate_region				( additional_memory_size );
}
	
engine_world::~engine_world		( )
{
	finalize							( );
	core::finalize						( );
}

void engine_world::initialize_render	( )
{
	ASSERT								( m_initialized );

	render::set_memory_allocator		( m_render_allocator, m_engine_user_module_proxy.allocator(), m_editor_allocator );

	ASSERT								( !m_render_world );
	create_render						( );
	ASSERT								( m_render_world );
}

void engine_world::tick					( )
{
	if ( threading::g_debug_single_thread )
		resources::tick					( );

// 	if ( threading::g_debug_single_thread )
// 		file_system_devices_tick		( );

	resources::dispatch_callbacks		( );

	//if ( threading::g_debug_single_thread )
	//	network_tick					( );

	if ( threading::g_debug_single_thread )
		sound_tick						( );

	if ( threading::core_count( ) == 1 ) {
		logic_tick						( );
		build_tick						( );
	}
	
	m_render_world->tick				( );
}

bool engine_world::command_line_editor	( )
{
#ifdef XRAY_STATIC_LIBRARIES
	static bool s_editor_mode			= false;
#else // #ifndef XRAY_STATIC_LIBRARIES
	static bool s_editor_mode			= s_editor_key || s_editor_st_key;
#endif // #ifndef XRAY_STATIC_LIBRARIES

	return								s_editor_mode;
}

namespace xray {
namespace testing {
	bool   run_tests_command_line ( );
} // namespace testing
} // namespace xray

bool engine_world::command_line_no_splash_screen	( )
{
	if ( xray::command_line::show_help() )
		return				true;

	if ( xray::testing::run_tests_command_line() )
		return				true;

	return					s_no_splash_screen_key.is_set( );
}

bool engine_world::command_line_editor_singlethread	( )
{
#ifndef XRAY_STATIC_LIBRARIES
	static bool s_editor_single_thread	=	s_editor_st_key && 
											threading::core_count() == 1;
	return					s_editor_single_thread;
#else // #ifndef XRAY_STATIC_LIBRARIES
	return								false;
#endif // #ifndef XRAY_STATIC_LIBRARIES
}

void engine_world::load_level( pcstr project_resource_name, pcstr project_resource_path )
{
	if ( command_line_editor_singlethread() ) {
		engine_user_world().load			( project_resource_name, project_resource_path );
		return;
	}

	apc::run							(
		apc::logic,
		boost::bind( &xray::engine_user::world::load, &engine_user_world(), project_resource_name, project_resource_path ),
		apc::continue_process_loop,
		apc::wait_for_completion
	);
}

void engine_world::unload_level		( )
{
	if ( command_line_editor_singlethread() ) {
		engine_user_world().unload		( "", false );
		return;
	}

	apc::run							(
		apc::logic,
		boost::bind( &xray::engine_user::world::unload, &engine_user_world(), "", false ),
		apc::continue_process_loop,
		apc::wait_for_completion
	);
}

void engine_world::on_application_activate( )
{
	m_application_active	= true;
	if ( m_application_activated )
		return;

	m_application_activated	= true;

	if ( m_engine_user_world )
		m_engine_user_world->on_application_activate	( );
	else
		m_application_activated	= false;
}

void engine_world::on_application_deactivate( )
{
	m_application_active	= false;
	if ( !m_application_activated )
		return;

	R_ASSERT				( m_engine_user_world );
	m_engine_user_world->on_application_deactivate	( );

	m_application_activated	= false;
}

bool engine_world::is_application_active( )
{
	if ( m_application_activated )
		return				true;

	return					m_application_active;
}

void engine_world::enable_game_impl		( bool const value )
{
	if ( m_engine_user_world )
	{
		if ( command_line_editor_singlethread() ) 
		{
			engine_user_world().enable		( value );
		}
		else
		{
			apc::run	(
				apc::logic,
				boost::bind( &xray::engine_user::world::enable, &engine_user_world(), value ),
				apc::continue_process_loop,
				apc::wait_for_completion
			);
		}
	}

	m_game_enabled		= value;
	m_render_world->enable_logic_thread_safety	( value );

}

int engine_world::get_exit_code			( ) const
{
	return				m_exit_code;
}

void engine_world::set_exit_code		( int const exit_code )
{
	m_exit_code			= exit_code;
}

