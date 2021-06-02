////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_add_static.h"
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

using xray::render::base_world;
using xray::render::command_add_static;
using xray::resources::managed_resource_ptr;

command_add_static::command_add_static	(
		base_world& world,
		managed_resource_ptr const& collision,
		managed_resource_ptr const& geometry,
		managed_resource_ptr const& visuals
	) :
	m_collision			( collision ),
	m_geometry			( geometry ),
	m_visuals			( visuals ),
	m_world				( world )
{
}

void command_add_static::execute		( )
{
	m_world.platform().add_static	( m_collision, m_geometry, m_visuals );
}