////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_RAY_TEST_GEOMETRY_H_INCLUDED
#define COLLIDERS_RAY_TEST_GEOMETRY_H_INCLUDED

#include "colliders_ray_geometry_base.h"

namespace xray {
namespace collision {
namespace colliders {

class ray_test_geometry : public ray_geometry_base {
public:
					ray_test_geometry		(
						triangle_mesh_geometry const& geometry,
						float3 const& origin,
						float3 const& direction,
						float const max_distance,
						triangles_predicate_type const& predicate
					);

private:
	typedef ray_geometry_base				super;

private:
	template < typename T >
			void	query					( Opcode::AABBNoLeafNode const* node, T const& predicate );
			void	test_primitive			( u32 const& triangle );
}; // class ray_test_geometry

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_RAY_TEST_GEOMETRY_H_INCLUDED