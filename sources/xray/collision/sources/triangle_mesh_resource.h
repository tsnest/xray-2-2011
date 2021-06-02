////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_MESH_RESOURCE_H_INCLUDED
#define TRIANGLE_MESH_RESOURCE_H_INCLUDED

#include <xray/resources_resource.h>
#include "triangle_mesh_geometry.h"

namespace xray {
namespace collision {

class triangle_mesh_resource : public triangle_mesh_geometry {
public:
						triangle_mesh_resource	( memory::base_allocator* allocator, resources::managed_resource_ptr resource );
	
	virtual	bool		aabb_query				( object const* object, math::aabb const& aabb, triangles_type& triangles ) const;
	virtual	bool		cuboid_query			( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool		ray_query				(
							object const* object,
							float3 const& origin,
							float3 const& direction,
							float max_distance,
							float& distance,
							ray_triangles_type& triangles,
							triangles_predicate_type const& predicate
						) const;
	virtual	void		render					( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& matrix ) const;

public:
	typedef vectora< u32 >						indices_type;

private:
	typedef triangle_mesh_geometry				super;

private:
	indices_type			m_indices;
	resources::managed_resource_ptr	m_resource;
}; // class world

} // namespace collision
} // namespace xray

#endif // #ifndef TRIANGLE_MESH_RESOURCE_H_INCLUDED