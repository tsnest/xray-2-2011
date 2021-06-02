////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_RAY_GEOMETRY_H_INCLUDED
#define COLLIDERS_RAY_GEOMETRY_H_INCLUDED

#include "colliders_ray_geometry_base.h"

namespace xray {
namespace collision {
namespace colliders {

class ray_query_geometry : public ray_geometry_base {
public:
					ray_query_geometry		(
						triangle_mesh_geometry const& geometry,
						object const* object,
						float3 const& origin,
						float3 const& direction,
						float const max_distance,
						ray_triangles_type& triangles,
						triangles_predicate_type const& predicate
					);

private:
	template < typename T >
			void	query					( Opcode::AABBNoLeafNode const* node, T const& predicate );
			void	test_primitive			( u32 const& triangle );

private:
	typedef ray_geometry_base super;

private:
	ray_triangles_type&				m_triangles;
	object const*					m_object;
}; // class ray_geometry

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_RAY_GEOMETRY_H_INCLUDED