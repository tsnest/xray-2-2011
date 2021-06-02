////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.01.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "maya_animation_world.h"
#include <xray/maya_animation/api.h>
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(maya_animation_entry_point)

using xray::maya_animation::maya_animation_world;

static xray::uninitialized_reference< maya_animation_world >	s_world;

xray::maya_animation::allocator_type*	xray::maya_animation::g_allocator = 0;

xray::maya_animation::world* xray::maya_animation::create_world( engine& engine )
{
	XRAY_CONSTRUCT_REFERENCE	( s_world, maya_animation_world )	( engine );
	return						( &*s_world );
}

void xray::maya_animation::destroy_world( xray::maya_animation::world*& world )
{
	R_ASSERT					( &*s_world == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world						= 0;
}

void xray::maya_animation::set_memory_allocator( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}