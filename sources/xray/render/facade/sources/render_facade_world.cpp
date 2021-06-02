////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/world.h>
#include <xray/render/engine/api.h>
#include <xray/render/engine/world.h>
#include <xray/render/facade/engine_renderer.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/ui_renderer.h>

#ifndef MASTER_GOLD
#	include <xray/render/facade/editor_renderer.h>
#endif // MASTER_GOLD

using xray::render::world;
using xray::resources::managed_resource_ptr;

world::world								(
		xray::memory::base_allocator& logic_allocator,
		xray::memory::base_allocator* editor_allocator
	) :
	m_logic_channel				( logic_allocator ),
	m_editor_channel			( *editor_allocator ),
	m_last_frame_time_in_ms		( 0 ),
	m_is_logic_enabled			( 1 ),
	m_is_editor					( editor_allocator != 0 ),
	m_is_logic_frame_ended		( 0 ),
	m_is_editor_frame_ended		( editor_allocator == 0 )
{
	XRAY_UNREFERENCED_PARAMETER	( editor_allocator );

	m_render_engine_world		= engine::create_world( );
	R_ASSERT					( m_render_engine_world );

	m_engine_renderer			= NEW( render::engine::renderer ) ( *m_render_engine_world );
	m_game_renderer				= NEW( render::game::renderer ) ( *this, *m_render_engine_world );
#ifndef XRAY_STATIC_LIBRARIES
	m_editor_renderer			= editor_allocator ? NEW( render::editor::renderer ) ( *this, *m_render_engine_world ) : 0;
#else // #ifndef XRAY_STATIC_LIBRARIES
	m_editor_renderer			= 0;
#endif // #ifndef XRAY_STATIC_LIBRARIES

	m_timer.start				( );
}

world::~world								( )
{
#ifndef XRAY_STATIC_LIBRARIES
	DELETE						( m_editor_renderer );
#endif // #ifndef XRAY_STATIC_LIBRARIES
	DELETE						( m_game_renderer );
	DELETE						( m_engine_renderer );

	destroy						( m_render_engine_world );
}

xray::render::engine::renderer&	world::engine_renderer	( )
{
	R_ASSERT					( m_engine_renderer );
	return						*m_engine_renderer;
}

xray::render::game::renderer&	world::game_renderer	( )
{
	R_ASSERT					( m_game_renderer );
	return						*m_game_renderer;
}

#ifndef	MASTER_GOLD

xray::render::editor::renderer&	world::editor_renderer	( )
{
	R_ASSERT					( m_editor_renderer );
	return						*m_editor_renderer;
}

#endif	//MASTER_GOLD 

void world::end_frame			( )
{
	R_ASSERT					( m_is_logic_frame_ended );
	R_ASSERT					( m_is_editor_frame_ended );

	u32 const current_time_in_ms = m_timer.get_elapsed_msec( );
	m_render_engine_world->end_frame	( );
	m_last_frame_time_in_ms		= current_time_in_ms;

	if ( m_is_logic_enabled )
		m_is_logic_frame_ended	= false;

	if ( m_is_editor )
		m_is_editor_frame_ended	= false;
}

void world::end_frame_logic		( )
{
	m_logic_channel.render_on_end_frame	( );

	R_ASSERT					( !m_is_logic_frame_ended || !m_is_logic_enabled );
	m_is_logic_frame_ended		= true;

	if ( !m_is_editor_frame_ended	 )
		return;

	end_frame					( );
}

void world::end_frame_editor	( )
{
#ifndef MASTER_GOLD
	m_editor_channel.render_on_end_frame	( );
#endif // #ifndef MASTER_GOLD

	R_ASSERT					( !m_is_editor_frame_ended );
	m_is_editor_frame_ended		= true;

	if ( !m_is_logic_frame_ended )
		return;

	end_frame					( );
}

void world::tick				( )
{
	if ( !m_is_logic_frame_ended )
		m_logic_channel.render_process_commands	( true );

	if ( !m_is_editor_frame_ended )
		m_editor_channel.render_process_commands( true );
}

void world::clear_resources		( )
{
	m_render_engine_world->clear_resources	( );
}

void world::enable_logic_thread_safety	( bool const value )
{
	R_ASSERT_CMP					( m_is_logic_enabled == 1, !=, value );
	threading::interlocked_exchange	( m_is_logic_enabled, value ? 1 : 0 );
	if ( !value )
		threading::interlocked_exchange	( m_is_logic_frame_ended, 1 );
}