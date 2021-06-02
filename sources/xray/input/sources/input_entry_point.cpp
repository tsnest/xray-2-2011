////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_world.h"
#include <xray/input/api.h>
#include <xray/linkage_helper.h>

using xray::input::input_world;

XRAY_DECLARE_LINKAGE_ID(input_entry_point)

static xray::uninitialized_reference<input_world>	s_world;
xray::input::allocator_type*	xray::input::g_allocator = 0;

xray::input::world* xray::input::create_world	( engine& engine, HWND const window_handle )
{
	XRAY_CONSTRUCT_REFERENCE	( s_world, input_world )	( engine, window_handle );
	return						( &*s_world );
}

void xray::input::destroy_world					( xray::input::world*& world )
{
	R_ASSERT					( &*s_world == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world						= 0;
}

void xray::input::set_memory_allocator			( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}