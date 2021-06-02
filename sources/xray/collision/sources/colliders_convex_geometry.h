////////////////////////////////////////////////////////////////////////////
//	Created		: 26.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_CONVEX_GEOMETRY_H_INCLUDED
#define COLLIDERS_CONVEX_GEOMETRY_H_INCLUDED

namespace Opcode {
	class AABBNoLeafNode;
} // namespace Opcode

namespace xray {

namespace math {
	class convex;
} // namespace math 

namespace collision {
class triangle_mesh_geometry;
namespace colliders {


class convex_geometry : boost::noncopyable
{
public:
							convex_geometry		( triangle_mesh_geometry const& geometry, math::convex const& convex );
	inline	u32				result				( ) const { return m_result; }

private:
	typedef xray::math::intersection	intersection;

private:
			intersection	intersects_aabb		( Opcode::AABBNoLeafNode const* node ) const;
			void			calculate_triangles	( Opcode::AABBNoLeafNode const* node );
			void			query				( Opcode::AABBNoLeafNode const* node );
private:
	triangle_mesh_geometry const&	m_geometry;
	math::convex const&				m_convex;
	u32								m_result;
}; // class convex_geometry

} // namespace colliders 
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_CONVEX_GEOMERTY_H_INCLUDED