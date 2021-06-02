////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_AABB_GEOMETRY_H_INCLUDED
#define COLLIDERS_AABB_GEOMETRY_H_INCLUDED

#include <xray/collision/object.h>
#include "colliders_aabb_base.h"

#define XRAY_USE_OLD_TRIANGLE_AABB_TEST	0

namespace Opcode {
	class AABBNoLeafNode;
} // namespace Opcode

namespace xray {
namespace collision {

class triangle_mesh_geometry;

namespace colliders {

class aabb_geometry : public aabb_base {
public:
							aabb_geometry		( triangle_mesh_geometry const& geometry, object const* object, math::aabb const& aabb, triangles_type& triangles );

private:
			void			add_triangle		( u32 triangle) const;
#if XRAY_USE_OLD_TRIANGLE_AABB_TEST
			bool			test_triangle_planes( float3 const& e0, float3 const& e1, float3 const& v0 ) const;
			bool			test_triangle_axis	( float3 const& e0, float3 const& e1, float3 const& v0, float3 const& v1, float3 const& v2 ) const;
#endif // #else // #if XRAY_USE_OLD_TRIANGLE_AABB_TEST
			bool			test_triangle		( u32 triangle_id ) const;
			void			test_primitive		( u32 triangle_id ) const;
			void			add_triangles		( Opcode::AABBNoLeafNode const* node ) const;
			void			query				( Opcode::AABBNoLeafNode const* node ) const;
			void			process				( );

private:
	typedef aabb_base		super;

private:
	float3							m_center;
	float3							m_extents;
	mutable float3					m_leaf_vertices[3];
	object const*					m_object;
	triangle_mesh_geometry const&	 m_geometry;
	triangles_type&				m_triangles;
}; // class aabb_geometry

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_AABB_GEOMETRY_H_INCLUDED