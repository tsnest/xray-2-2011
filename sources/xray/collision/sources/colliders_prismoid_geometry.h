////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_PRISMOID_GEOMETRY_H_INCLUDED
#define COLLIDERS_PRISMOID_GEOMETRY_H_INCLUDED

#include <xray/collision/object.h>

namespace Opcode {
	class AABBNoLeafNode;
} // namespace Opcode

namespace xray {
namespace collision {

class triangle_mesh_base;

namespace colliders {

class prismoid_geometry : private boost::noncopyable {
public:
						prismoid_geometry	(
							triangle_mesh_base const& geometry,
							object const* object,
							math::prismoid const& prismoid,
							Results& results
						);
	inline	bool		result				( ) const { return m_result; }

private:
	typedef xray::math::intersection		intersection;

private:
		intersection	intersects_aabb		( Opcode::AABBNoLeafNode const* node ) const;
			void		add_triangle		( u32 triangle_id ) const;
			void		add_triangles		( Opcode::AABBNoLeafNode const* node ) const;
			void		query				( Opcode::AABBNoLeafNode const* node ) const;

private:
	triangle_mesh_base const&	m_geometry;
	math::prismoid const&		m_prismoid;
	Results&					m_results;
	object const*				m_object;
	bool						m_result;
}; // class colliders_prismoid_query

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_PRISMOID_GEOMETRY_H_INCLUDED