////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "console_commands.h"
#include <xray/console_command.h>
#include <xray/sound/world_user.h>
#include <xray/sound/sound_debug_stats.h>
#include "sound_world.h"

namespace xray {
namespace sound {

console_commands::console_commands		( sound_world const& world ) :
	m_time_factor						( 1.0f ),
	m_debug_draw_mode					( 0 ),
	m_is_debug_stream_writing_enabled	( false ),
	m_sound_world						( world )
{
	static xray::console_commands::cc_float	s_time_factor	( "sound_time_factor", m_time_factor, 0.0f, 2.0f, false, xray::console_commands::command_type_engine_internal );
	s_time_factor.subscribe_on_change						( boost::bind( &console_commands::on_time_factor_changed, this ) );

	static xray::console_commands::cc_bool s_stream_writing	( "sound_stream_writing", m_is_debug_stream_writing_enabled, false, xray::console_commands::command_type_engine_internal );
	s_stream_writing.subscribe_on_change					( boost::bind( &console_commands::on_stream_writing_changed, this ) );

	static xray::console_commands::cc_delegate s_dump		( "sound_dump_stream_writing", boost::bind( &console_commands::on_dump_stream_writing, this ), false );

	static xray::console_commands::cc_u32 s_debug_draw_mode	( "sound_debug_draw_mode", m_debug_draw_mode, 0, 2, false, xray::console_commands::command_type_engine_internal );
	s_debug_draw_mode.subscribe_on_change					( boost::bind( &console_commands::on_debug_draw_mode_changed, this ) );

	static xray::console_commands::cc_u32 s_proxy_stats		( "sound_show_proxy_stats", m_proxy_stats, 0, (u32)-1, false, xray::console_commands::command_type_engine_internal );
	s_proxy_stats.subscribe_on_change						( boost::bind( &console_commands::on_show_proxy_stats_changed, this ) );
}

void console_commands::on_time_factor_changed		( ) const
{
	world_user* const user				= get_thread_world_user	( );
	R_ASSERT							( user );
	user->set_time_scale_factor			( m_time_factor );
}


void console_commands::on_stream_writing_changed	( ) const
{
	world_user* const user				= get_thread_world_user	( );
	R_ASSERT							( user );

	m_is_debug_stream_writing_enabled ?
		user->enable_current_scene_stream_writing( ) :
		user->disable_current_scene_stream_writing( );
}

void console_commands::on_dump_stream_writing		( ) const
{
	world_user* const user				= get_thread_world_user	( );
	R_ASSERT							( user );
	
	user->dump_current_scene_stream_writing			( );
}

void console_commands::on_debug_draw_mode_changed	( ) const
{
	sound_debug_stats::set_debug_draw_mode		( sound_debug_stats::mode( m_debug_draw_mode ) );
}

void console_commands::on_show_proxy_stats_changed	( ) const
{
	sound_debug_stats::set_detail_view_proxy_id	( m_proxy_stats );
}


world_user* console_commands::get_thread_world_user	( ) const
{
#pragma message( XRAY_TODO("dimetcm 2 dimetcm: use thread local storage this") )
	pcstr thread_name					= threading::current_thread_logging_name( );

#ifndef MASTER_GOLD
	if ( strings::equal( "editor", thread_name ) )
		return &m_sound_world.get_editor_world_user	( );
	else 
#endif //#ifndef MASTER_GOLD
	if ( strings::equal( "logic", thread_name ) )
		return &m_sound_world.get_logic_world_user	( );
	else
		NOT_IMPLEMENTED					( return 0; );
}

} // namespace sound
} // namespace xray
