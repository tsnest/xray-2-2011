////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skin_visual_writer.h"

namespace xray{
namespace maya{


MStatus skin_visual_collector::write_bones( memory::writer& w ) const
{
	MStatus				result;
	w.write_u16			( (u16)m_bones.size() );

	bones_vec::const_iterator it	= m_bones.begin();
	bones_vec::const_iterator it_e	= m_bones.end();
	for( ;it!=it_e; ++it)
	{
		sk_bone const& bone		= *it;
		result					= bone.save	( w );
		CHK_STAT_R				( result );
	}

	return				result;
}

u32 skin_visual_collector::get_surfaces_count( ) const
{
	return m_render_skinned_surfaces.size();
}


MStatus skin_visual_collector::write_skeleton_properties( configs::lua_config_value& value )
{
	value["skeleton"]		= m_skeleton_name.asChar();
	return					MStatus::kSuccess;
}

MStatus skin_visual_collector::write_properties( configs::lua_config_value& value )
{
	math::aabb			bbox =  math::create_identity_aabb() ;
	
	math::sphere sphere = bbox.sphere( );

	value["type"]						= xray::render::mt_skinned_mesh;
	value["version"]					= 0;
	value["bounding_box"]["min"]		= bbox.min;
	value["bounding_box"]["max"]		= bbox.max;
	value["bounding_sphere"]["pos"]		= sphere.center;
	value["bounding_sphere"]["radius"]	= sphere.radius;

	configs::lua_config_value v			= value["locators"];
	write_locators						( v );

	return					MStatus::kSuccess;
}

} //namespace maya
} //namespace xray
