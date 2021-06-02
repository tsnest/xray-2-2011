////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "network_world.h"
#include <xray/network/api.h>

using xray::network::network_world;

static xray::uninitialized_reference<network_world>	s_world;
xray::network::allocator_type*	xray::network::g_allocator = 0;

xray::network::world* xray::network::create_world	( engine& engine )
{
	XRAY_CONSTRUCT_REFERENCE	( s_world, network_world )	( engine );
	return						( &*s_world );
}

void xray::network::destroy_world					( xray::network::world*& world )
{
	R_ASSERT					( &*s_world == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world						= 0;
}

void xray::network::set_memory_allocator			( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}

void xray::network::dispatch_callbacks				( )
{
}

// here is _STLP_DECLSPEC
_STLP_DECLSPEC