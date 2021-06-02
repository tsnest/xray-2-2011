////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xbox360_world.h"
#include <xray/render/api.h>
#include <xray/render/base/api.h>

using xray::render::xbox360::world;

static xray::uninitialized_reference<world>	s_world;

xray::render::world* xray::render::xbox360::create_world	( render::engine::wrapper& wrapper )
{
	XRAY_CONSTRUCT_REFERENCE( s_world, xbox360::world )	( wrapper );
	return					( &*s_world );
}

void xray::render::xbox360::destroy_world							( xray::render::world*& world )
{
	R_ASSERT				( &*s_world == world );
	XRAY_DESTROY_REFERENCE	( s_world );
	world					= 0;
}