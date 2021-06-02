////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "navigation_world.h"
#include <xray/ai_navigation/api.h>
#include <xray/linkage_helper.h>

using xray::ai::navigation::navigation_world;

//static xray::uninitialized_reference<navigation_world>	s_world;
xray::ai::navigation::allocator_type*	xray::ai::navigation::g_allocator = 0;

XRAY_DECLARE_LINKAGE_ID(ai_navigation_entry_point)

xray::ai::navigation::world* xray::ai::navigation::create_world	( engine& engine, xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer )
{
	return 				NEW( navigation_world ) ( engine, scene, renderer );
}

void xray::ai::navigation::destroy_world						( xray::ai::navigation::world*& world )
{
	DELETE					(world);
	world						= 0;
}

void xray::ai::navigation::set_memory_allocator					( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}