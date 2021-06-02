////////////////////////////////////////////////////////////////////////////
//	Created		: 22.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include "apc.h"
#include <xray/sound/world.h>
#include <xray/sound/api.h>
#include <xray/render/facade/engine_renderer.h>
#include <xray/render/world.h>

#if XRAY_PLATFORM_WINDOWS
#	include <xray/os_preinclude.h>
#	undef NOUSER
#	undef NOMSG
#	undef NOMB
#	include <xray/os_include.h>
#	include <objbase.h>						// for COINIT_MULTITHREADED
#endif // #if XRAY_PLATFORM_WINDOWS

using xray::engine::engine_world;

void engine_world::initialize_sound_modules	( )
{
#if XRAY_PLATFORM_WINDOWS
	CoInitializeEx					( 0, COINIT_APARTMENTTHREADED );
#endif // #if XRAY_PLATFORM_WINDOWS

	m_sound_allocator.user_current_thread_id( );
	sound::set_memory_allocator		( m_sound_allocator );

	threading::interlocked_exchange_pointer	(
		m_sound_world,
		sound::create_world(
			*this,
			m_engine_user_module_proxy.allocator( ),
			command_line_editor() ? &m_editor_allocator : 0
		)
	);
}

void engine_world::initialize_sound	( )
{
	if ( threading::g_debug_single_thread )
		return;

	apc::assign_thread_id			( apc::sound, u32(-1) );

	threading::spawn				(
		boost::bind( &engine_world::sound, this ), 
		"sound", 
		"sound", 
		0,
		5 % threading::core_count()
	);
}

void engine_world::sound_tick			( )
{
	xray::resources::dispatch_callbacks	( );
	m_sound_world->tick				( );
}

void engine_world::sound			( )
{
	apc::assign_thread_id			( apc::sound, threading::current_thread_id( ) );
	apc::process					( apc::sound );

	for (;;) {
		if ( m_destruction_started )
			break;

		sound_tick					( );

		//while ( ( math::max( m_editor_frame_id, m_logic_frame_id ) > m_render_world->engine_renderer().frame_id( ) + 1 ) && !m_destruction_started ) {
		//	if ( !apc::try_process_single_call( apc::sound ) )
		//		threading::yield	( 1 );
		//}
		threading::yield			( 10 );
	}
	
	apc::process					( apc::sound );
}

void engine_world::sound_clear_resources	( )
{
	resources::dispatch_callbacks	( );
	m_sound_world->clear_resources	( );
}
