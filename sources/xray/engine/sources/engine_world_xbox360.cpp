////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include <xray/render/api.h>
#include <xray/core/core.h>
#include <xray/resources_fs.h>

using xray::engine::engine_world;

xray::command_line::key		s_force_xbox_log("force_xbox_log", "", "logging", "forces logging even on xbox release and gold", "");

xray::logging::log_file_usage	engine_world::log_file_usage ( ) const
{
#ifdef DEBUG
	return									logging::create_log;
#else // #ifdef DEBUG
	return									(s_force_xbox_log.is_set() ? logging::create_log : logging::no_log);
#endif // #ifdef DEBUG
}

void engine_world::initialize_core	( )
{
	core::initialize		( "../../resources/sources/", "render", core::perform_debug_initialization );
}

void engine_world::create_render	( )
{
	m_render_world			= render::create_world (
		m_engine_user_module_proxy.allocator(),
		m_editor ? &m_editor_allocator : 0
	);
	R_ASSERT				( m_render_world );
}

void engine_world::destroy_render	( )
{
	render::destroy_world	( m_render_world );
}

void engine_world::post_initialize	( bool const show_window )
{
	XRAY_UNREFERENCED_PARAMETER	( show_window );

	on_application_activate	( );
}

void engine_world::run				( )
{
	for (;;) {
		if ( m_destruction_started )
			return;

		tick				( );
	}
}

void engine_world::initialize_editor	( )
{
}

void engine_world::enter_editor_mode	( )
{
	UNREACHABLE_CODE		( );
}

bool engine_world::on_before_editor_tick( )
{
	UNREACHABLE_CODE		( return true );
}

void engine_world::on_after_editor_tick	( )
{
	UNREACHABLE_CODE		( );
}

void engine_world::unload_editor		( )
{
	UNREACHABLE_CODE		( );
}

pcstr engine_world::get_resources_path	( ) const
{
	return					"game:/resources";
}

pcstr engine_world::get_mounts_path		( ) const
{
	return					"game:/mounts";
}

void engine_world::editor_clear_resources	( )
{
	UNREACHABLE_CODE		( );
}

void engine_world::enable_game				( bool value )
{
	enable_game_impl		( value );
}

xray::engine::console* engine_world::create_editor_console( xray::ui::world& )
{
	return					(xray::engine::console*)NULL;
}

void engine_world::try_load_editor			( )
{
	UNREACHABLE_CODE		( );
}

void engine_world::show_window				( bool )
{
	UNREACHABLE_CODE		( );
}

void engine_world::editor_dispatch_callbacks( )
{
}

HWND new_window								( )
{
	return					0;
}
