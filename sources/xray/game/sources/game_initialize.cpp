////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game.h"
#include <xray/physics/api.h>
#include <xray/input/api.h>
#include <xray/animation/api.h>
#include <xray/rtp/api.h>
#include <xray/ui/api.h>
#include <xray/ai_navigation/api.h>
#include <xray/ai/api.h>
#include <xray/render/world.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/game_test_suite.h>
#include <xray/collision/api.h>
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>
#include <xray/physics/world.h>

using stalker2::game;

void game::initialize_physics		( )
{
	ASSERT							( !m_initialized );

	ASSERT							( !m_physics_world );
#ifdef MASTER_GOLD
	m_physics_world					= physics::create_world( g_allocator, *this );
#else
	m_physics_world					= physics::create_world( memory::g_crt_allocator, *this );
#endif
	ASSERT							( m_physics_world );

	//m_physics_world->create_test_primitives( );
}

void game::initialize_input			( )
{
	ASSERT							( !m_initialized );

	ASSERT							( !m_input_world );
	m_input_world					= input::create_world( *this, m_engine.get_main_window_handle() );
	ASSERT							( m_input_world );
}

void game::initialize_animation		( )
{
	ASSERT							( !m_initialized );

	ASSERT							( !m_animation_world );
	m_animation_world				= animation::create_world( *this, m_scene, &m_renderer.scene(), &m_renderer.debug() );
	ASSERT							( m_animation_world );
}

void game::initialize_rtp			( )
{
	ASSERT							( !m_initialized );
	ASSERT							( m_animation_world );
	ASSERT							( !m_rtp_world );
	m_rtp_world						= rtp::create_world( *this, *m_animation_world, m_scene, &m_renderer, m_engine.get_resources_path() );

	game_test_suite::singleton()->set_rtp_world( m_rtp_world );

	ASSERT							( m_rtp_world );
}

void game::initialize_ui			( )
{
	ASSERT							( !m_initialized );

	ASSERT							( !m_ui_world );
	m_ui_world						= ui::create_world( *this, m_renderer.ui(), *g_allocator, m_scene_view );
	ASSERT							( m_ui_world );
}

void game::initialize_ai_navigation	( )
{
	ASSERT							( !m_initialized );

	ASSERT							( !m_ai_navigation_world );
	m_ai_navigation_world			= ai::navigation::create_world( *this, m_scene, m_renderer.debug() );
	ASSERT							( m_ai_navigation_world );
}

void game::initialize_ai			( )
{
	ASSERT							( !m_initialized );

	ASSERT							( !m_spatial_tree );
	m_spatial_tree					= &*collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 );
	ASSERT							( m_spatial_tree );
	
	ASSERT							( !m_ai_world );
	m_ai_world						= ai::create_world( *this );
	ASSERT							( m_ai_world );
}

void game::initialize_modules		( )
{
	initialize_physics				( );
	initialize_input				( );
	initialize_animation			( );
	initialize_rtp					( );
	initialize_ui					( );
	initialize_ai_navigation		( );
	initialize_ai					( );
}

void game::deinitialize_modules		( )
{
	ai::destroy_world				( m_ai_world );
	ai::navigation::destroy_world	( m_ai_navigation_world );
	ui::destroy_world				( m_ui_world );
	rtp::destroy_world				( m_rtp_world );
	animation::destroy_world		( m_animation_world );
	input::destroy_world			( m_input_world );
#ifdef MASTER_GOLD
	physics::destroy_world			( g_allocator, m_physics_world );
#else
	physics::destroy_world			( memory::g_crt_allocator, m_physics_world );
#endif

}
