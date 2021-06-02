////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "solid_visual_collector.h"
#include <d3d9types.h>

namespace xray{
namespace maya{

u32 solid_visual_collector::get_surfaces_count( ) const
{
	return m_render_static_surfaces.size();
}

MStatus	solid_visual_collector::write_properties( configs::lua_config_value& value )
{
	MStatus							status;
	m_bbox.invalidate				( );
	render_static_surfaces_it it	= m_render_static_surfaces.begin();
	render_static_surfaces_it it_e	= m_render_static_surfaces.end();
	for(; it!=it_e; ++it)
	{
		render_static_surface* s	= *it;
		if(s->empty())
			continue;

		m_bbox.modify				( s->get_bbox() );
	}

	math::sphere sphere = m_bbox.sphere( );

	value["type"]						= xray::render::mt_static_mesh;
	value["version"]					= 0;
	value["bounding_box"]["min"]		= m_bbox.min;
	value["bounding_box"]["max"]		= m_bbox.max;
	value["bounding_sphere"]["pos"]		= sphere.center;
	value["bounding_sphere"]["radius"]	= sphere.radius;

	configs::lua_config_value v			= value["locators"];
	write_locators						( v );

	return					MStatus::kSuccess;
}

MStatus	solid_visual_collector::prepare_surfaces( )
{
	render_static_surfaces_it it		= m_render_static_surfaces.begin();
	render_static_surfaces_it it_e		= m_render_static_surfaces.end();
	for(; it!=it_e; ++it)
	{
		render_static_surface* s		= *it;
		MStatus result = s->prepare		( m_arglist );
		if (result != MStatus::kSuccess)
			return result;
	}

	return					MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
