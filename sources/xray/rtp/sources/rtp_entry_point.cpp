////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "rtp_world.h"
#include <xray/rtp/api.h>
#include <xray/linkage_helper.h>

using xray::rtp::rtp_world;

XRAY_DECLARE_LINKAGE_ID(rtp_entry_point)

static xray::uninitialized_reference<rtp_world>	s_world;
xray::rtp::allocator_type*	xray::rtp::g_allocator = 0;

xray::rtp::world* xray::rtp::create_world	(
		engine& engine,
		animation::world& animation,
		xray::render::scene_ptr const& scene,
		xray::render::game::renderer* renderer,
		pcstr const resources_xray_path
	)
{
	XRAY_CONSTRUCT_REFERENCE	( s_world, rtp_world )	( engine, animation, scene, renderer, resources_xray_path );
	return						( &*s_world );
}

void xray::rtp::destroy_world					( xray::rtp::world*& world )
{
	R_ASSERT					( &*s_world == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world						= 0;
}

void xray::rtp::set_memory_allocator			( memory::base_allocator& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}