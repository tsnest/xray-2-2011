////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include "apc.h"
#include <xray/render/facade/engine_renderer.h>
#include <xray/editor/world/api.h>
#include <xray/editor/world/world.h>
#include <xray/editor/world/library_linkage.h>
#include <boost/bind.hpp>
#include <xray/core/core.h>
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>
#include "editor_console.h"
#include <xray/render/world.h>
#include <xray/debug/debug.h>

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
#	include <xray/os_preinclude.h>
#	undef NOUSER
#	undef NOMSG
#	undef NOMB
#	include <xray/os_include.h>
#	include <objbase.h>							// for COINIT_MULTITHREADED

	static HMODULE								s_editor_module	= 0;
	static xray::editor::create_world_ptr		s_create_world = 0;
	static xray::editor::destroy_world_ptr		s_destroy_world = 0;
	static xray::editor::memory_allocator_ptr	s_memory_allocator = 0;
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

using xray::engine::engine_world;

void engine_world::try_load_editor			( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
#	if defined(DEBUG) && XRAY_PLATFORM_32_BIT
		xray::debug::enable_fpe	( false );
#	endif

	CoInitializeEx		( 0, COINIT_APARTMENTTHREADED );

	R_ASSERT			( !s_editor_module );
	s_editor_module		= LoadLibrary( XRAY_EDITOR_FILE_NAME );

	if (!s_editor_module) {
		LOG_WARNING		( "cannot load library \"%s\"", XRAY_EDITOR_FILE_NAME );
		CHECK_OR_EXIT	( s_editor_module != 0, "Cannot load editor library - exiting.\r\nSee log file for details." );
		return;
	}

	R_ASSERT			( !s_create_world );
	s_create_world		= (xray::editor::create_world_ptr)GetProcAddress(s_editor_module, "create_world");
	R_ASSERT			( s_create_world );

	R_ASSERT			( !s_destroy_world );
	s_destroy_world		= (xray::editor::destroy_world_ptr)GetProcAddress(s_editor_module, "destroy_world");
	R_ASSERT			( s_destroy_world );

	R_ASSERT			( !s_memory_allocator);
	s_memory_allocator	= (xray::editor::memory_allocator_ptr)GetProcAddress(s_editor_module, "set_memory_allocator");
	R_ASSERT			( s_memory_allocator );

	m_editor_allocator.user_current_thread_id	( );
	s_memory_allocator	( m_editor_allocator );

	// this function cannot be called before s_memory_allocator function called
	// because of a workaround of BugTrapN initialization from unmanaged code
	debug::change_bugtrap_usage	( debug::error_mode_verbose, debug::managed_bugtrap );

	R_ASSERT			( !m_editor );
	m_editor			= s_create_world ( *this );
	R_ASSERT			( m_editor );

	R_ASSERT			( !m_render_window_handle );
	m_render_window_handle = m_editor->view_handle( );
	R_ASSERT			( m_render_window_handle );

	R_ASSERT			( !m_main_window_handle );
	m_main_window_handle = m_editor->main_handle( );
	R_ASSERT			( m_main_window_handle );

	m_editor->load		( );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	UNREACHABLE_CODE	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void engine_world::unload_editor			( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	ASSERT				( m_editor );

	ASSERT				( s_destroy_world );
	s_destroy_world		( m_editor );
	ASSERT				( !m_editor );

	ASSERT				( s_editor_module );
	FreeLibrary			( s_editor_module );
	s_editor_module		= 0;

	s_destroy_world		= 0;
	s_create_world		= 0;
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	UNREACHABLE_CODE	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void engine_world::initialize_editor		( )
{
	R_ASSERT					( command_line_editor() );
	m_game_enabled				= false;

	if( threading::core_count( ) == 1 ) {
#if defined(DEBUG) && XRAY_PLATFORM_WINDOWS_32
		xray::debug::enable_fpe	( false );
#endif
		return;
	}

	apc::assign_thread_id		( apc::editor, u32(-1) );

	threading::spawn			(
		boost::bind( &engine_world::editor, this ),
		!command_line_editor_singlethread( ) ? "editor" : "editor + logic",
		!command_line_editor_singlethread( ) ? "editor" : "editor + logic",
		0,
		0
	);
}

void engine_world::editor				( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	CoInitializeEx			( 0, COINIT_APARTMENTTHREADED );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	R_ASSERT_U				( !_controlfp_s( 0, _PC_53, MCW_PC ) );

	apc::assign_thread_id	( apc::editor, threading::current_thread_id( ) );
	apc::process			( apc::editor );

	m_editor->run			( );

	if ( !m_destruction_started )
		exit				( m_editor->exit_code() );

	apc::process			( apc::editor );
}

#ifndef MASTER_GOLD
void engine_world::editor_dispatch_callbacks( )
{
	m_sound_world->get_editor_world_user().dispatch_callbacks	( );
	m_render_world->editor_channel().owner_delete_processed_items( );
}
#endif //#ifndef MASTER_GOLD

bool engine_world::on_before_editor_tick	( )
{
	if ( threading::core_count( ) == 1 ) 	
		tick						( );
	else {
		static bool editor_singlethreaded = command_line_editor_singlethread ( ); 
		if ( editor_singlethreaded ) {
			logic_tick				( );
		}
		else {
			if ( m_editor_frame_id > m_render_world->engine_renderer().frame_id() + 1 )
				return				false;
		}
	}

#ifndef MASTER_GOLD
	editor_dispatch_callbacks		( );
#endif //#ifndef MASTER_GOLD
	return							true;
}

void engine_world::on_after_editor_tick_impl( )
{
	if ( threading::core_count( ) == 1 )
		return;

	if( !command_line_editor_singlethread() )
		return;

	while ( ( m_logic_frame_id > m_render_world->engine_renderer().frame_id( ) + 1 ) && !m_destruction_started )
		threading::yield			( 0 );
}

void engine_world::on_after_editor_tick	( )
{
	on_after_editor_tick_impl	( );
	++m_editor_frame_id;
}

void engine_world::enter_editor_mode	( )
{
	if ( !m_editor )
		return;


	m_editor->editor_mode	( true );
}

#ifndef MASTER_GOLD
void engine_world::editor_clear_resources	( )
{
	if ( !m_editor )
		return;

	m_sound_world->get_editor_world_user().dispatch_callbacks	( );
	resources::dispatch_callbacks	( );
}
#endif //#ifndef MASTER_GOLD

void engine_world::editor_message_out( pcstr message )
{
	if ( !m_editor )
		return;
	m_editor->message_out( message );
}

xray::engine::console* engine_world::create_editor_console( xray::ui::world& uw )
{
	return XRAY_NEW_IMPL(m_editor_allocator, xray::engine::editor_console)( uw, m_editor_allocator );
}