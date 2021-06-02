////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/world.h>
#include <xray/render/api.h>
#include <xray/render/core/memory.h>

typedef xray::render::render_allocator_type	render_allocator_type;
typedef xray::render::logic_allocator_type	logic_allocator_type;
typedef xray::render::editor_allocator_type	editor_allocator_type;

using xray::render::world;

static xray::uninitialized_reference<world>	s_world;

logic_allocator_type*	xray::render::logic::g_allocator = 0;
editor_allocator_type*	xray::render::editor::g_allocator = 0;

void xray::render::set_memory_allocator	(
		render_allocator_type&	render_allocator,
		logic_allocator_type&	logic_allocator,
		editor_allocator_type&	editor_allocator
	)
{
	R_ASSERT			( !xray::render::g_allocator );
	g_allocator			= &render_allocator;

	R_ASSERT			( !logic::g_allocator );
	logic::g_allocator	= &logic_allocator;

	R_ASSERT			( !editor::g_allocator );
	editor::g_allocator	= &editor_allocator;
}

xray::render::world* xray::render::create_world	(
		xray::memory::base_allocator& logic_allocator,
		xray::memory::base_allocator* editor_allocator
	)
{
	XRAY_CONSTRUCT_REFERENCE( s_world, world )	( logic_allocator, editor_allocator );
	return					( &*s_world );
}

void xray::render::destroy_world					( xray::render::world*& world )
{
	R_ASSERT				( &*s_world == world );
	XRAY_DESTROY_REFERENCE	( s_world );
	world					= 0;
}