////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_CUBOID_H_INCLUDED
#define COLLIDERS_CUBOID_H_INCLUDED

#include <xray/collision/object.h>

namespace Opcode {
	class AABBNoLeafNode;
} // namespace Opcode

namespace xray {
namespace collision {

class triangle_mesh_geometry;

namespace colliders {

class cuboid_geometry : private boost::noncopyable
{
public:
						cuboid_geometry	( triangle_mesh_geometry const& geometry, object const* object, math::cuboid const& cuboid, triangles_type& triangles );
	inline	bool		result			( ) const { return m_result; }

private:
	typedef xray::math::intersection	intersection;

private:
			intersection intersects_aabb( Opcode::AABBNoLeafNode const* node ) const;
			void		add_triangle	( u32 triangle_id ) const;
			void		add_triangles	( Opcode::AABBNoLeafNode const* node ) const;
			void		query			( Opcode::AABBNoLeafNode const* node ) const;

private:
	triangle_mesh_geometry const&	m_geometry;
	math::cuboid const&				m_cuboid;
	triangles_type&					m_triangles;
	object const*					m_object;
	bool							m_result;
}; // class cuboid_geometry

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_CUBOID_H_INCLUDED