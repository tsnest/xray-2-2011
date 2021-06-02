////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/physics/api.h>
#include <xray/input/api.h>
#include <xray/animation/api.h>
#include <xray/rtp/api.h>
#include <xray/ui/api.h>
#include <xray/ai_navigation/api.h>
#include <xray/ai/api.h>
#include <xray/game/api.h>
#include "game.h"

using xray::non_null;
using xray::raw;

#if !XRAY_PLATFORM_PS3
namespace boost
{
#ifndef XRAY_STATIC_LIBRARIES
	void throw_exception(std::exception const & e)
	{
		FATAL("boost exception caught: %s", e.what());
	}
#endif // #ifndef XRAY_STATIC_LIBRARIES
	void throw_exception(std::bad_alloc const & e)
	{
		XRAY_UNREFERENCED_PARAMETER	( e );
		FATAL("boost exception caught: %s", e.what());
	}
} // namespace boost
#endif // #if !XRAY_PLATFORM_PS3


static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_input_allocator;
static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_animation_allocator;
static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_rtp_allocator;
static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_ui_allocator;
static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_ai_navigation_allocator;
static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_ai_allocator;
static xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	s_game_allocator;
static xray::uninitialized_reference< stalker2::game >							s_game;

namespace stalker2 {

#ifdef XRAY_STATIC_LIBRARIES
xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	g_physics_allocator;
#endif // #ifdef XRAY_STATIC_LIBRARIES

xray::memory::doug_lea_allocator_type*	g_allocator		= NULL;

xray::engine_user::world* game_module::create_world	(
		xray::engine_user::engine& engine, 
		xray::render::world& render_world, 
		xray::sound::world& sound,
		xray::network::world& network
	)
{
#ifdef XRAY_STATIC_LIBRARIES
	g_physics_allocator->user_current_thread_id			( );
	xray::physics::set_memory_allocator					( g_physics_allocator.c_ptr() );
#else // #ifdef XRAY_STATIC_LIBRARIES
	xray::physics::set_memory_allocator					( memory::g_crt_allocator );
#endif // #ifdef XRAY_STATIC_LIBRARIES

	s_input_allocator->user_current_thread_id			( );
	xray::input::set_memory_allocator					( *s_input_allocator );

	s_animation_allocator->user_current_thread_id		( );
	xray::animation::set_memory_allocator				( *s_animation_allocator );

#if (!XRAY_RTP_LEARNING)
	s_rtp_allocator->user_current_thread_id				( );
	xray::rtp::set_memory_allocator						( *s_rtp_allocator );
#else
	xray::rtp::set_memory_allocator						( debug::g_mt_allocator );
#endif

	s_ui_allocator->user_current_thread_id				( );
//.	xray::ui::set_memory_allocator						( *s_ui_allocator );

	s_ai_navigation_allocator->user_current_thread_id	( );
	xray::ai::navigation::set_memory_allocator			( memory::g_mt_allocator );

	s_ai_allocator->user_current_thread_id				( );
	xray::ai::set_memory_allocator						( *s_ai_allocator );

	s_game_allocator->user_current_thread_id			( );

	XRAY_CONSTRUCT_REFERENCE	( s_game, game )		( engine, render_world, sound, network );

	return												( &*s_game );
}

void game_module::destroy_world						( xray::engine_user::world*& world )
{
	R_ASSERT								( &*s_game == world );
	XRAY_DESTROY_REFERENCE					( s_game );
	world									= 0;
}

static void register_rtp_allocator( )
{
#if (!XRAY_RTP_LEARNING)
	XRAY_CONSTRUCT_REFERENCE				( s_rtp_allocator, xray::memory::doug_lea_allocator_type );
	s_rtp_allocator->do_register			(  3*Mb,	"rtp"		);
#endif // #if (!XRAY_RTP_LEARNING)
}


void game_module::register_memory_allocators	( )
{
#ifdef XRAY_STATIC_LIBRARIES
	XRAY_CONSTRUCT_REFERENCE				( g_physics_allocator, xray::memory::doug_lea_allocator_type );
	g_physics_allocator->do_register		( 16*Mb,	"physics" );
#endif

	XRAY_CONSTRUCT_REFERENCE				( s_input_allocator, xray::memory::doug_lea_allocator_type );
	s_input_allocator->do_register			( 64*Kb,	"input" );

	XRAY_CONSTRUCT_REFERENCE				( s_animation_allocator, xray::memory::doug_lea_allocator_type );
	s_animation_allocator->do_register		(  10*Mb,	"animation" );

	register_rtp_allocator					( );

	XRAY_CONSTRUCT_REFERENCE				( s_ui_allocator, xray::memory::doug_lea_allocator_type );
	s_ui_allocator->do_register				(  64*Kb,	"ui" );

	XRAY_CONSTRUCT_REFERENCE				( s_ai_navigation_allocator, xray::memory::doug_lea_allocator_type );
	s_ai_navigation_allocator->do_register	(  64*Kb,	"ai navigation" );

	XRAY_CONSTRUCT_REFERENCE				( s_ai_allocator, xray::memory::doug_lea_allocator_type );
	s_ai_allocator->do_register				(	8*Mb,	"ai" );


	XRAY_CONSTRUCT_REFERENCE				( s_game_allocator, xray::memory::doug_lea_allocator_type );
	s_game_allocator->do_register			(   8*Mb,	"stalker2" );

//	s_game_allocator->user_current_thread_id( );
	g_allocator								= &*s_game_allocator;
}

xray::memory::doug_lea_allocator_type& game_module::allocator( )
{
	R_ASSERT								( g_allocator );
	return									*g_allocator;
}

} // namespace stalker2
