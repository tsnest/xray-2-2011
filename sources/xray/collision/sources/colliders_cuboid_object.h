////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_CUBOID_OBJECT_H_INCLUDED
#define COLLIDERS_CUBOID_OBJECT_H_INCLUDED

#include <xray/collision/object.h>

namespace xray {
namespace collision {

class object;
struct oct_node;

namespace colliders {

class cuboid_object : private boost::noncopyable {
public:
							cuboid_object	( query_type const query_type, loose_oct_tree const& tree, math::cuboid const& cuboid, objects_type& objects );
							cuboid_object	( query_type const query_type, loose_oct_tree const& tree, math::cuboid const& cuboid, triangles_type& triangles );
							cuboid_object	( query_type const query_type, loose_oct_tree const& tree, math::cuboid const& cuboid, results_callback_type& callback );
	inline	bool			result			( ) const { return m_result; }

private:
	typedef xray::math::intersection		intersection;

private:
			intersection	intersects_aabb	( float3 const& aabb_center, float3 const& aabb_extents ) const;
			void			add_objects		( non_null<collision::oct_node const>::ptr node ) const;
			void			add_triangles	( non_null<oct_node const>::ptr const node ) const;
			void	add_objects_by_callback	( non_null<oct_node const>::ptr const node ) const;
			void			query			( non_null<collision::oct_node const>::ptr node, float3 const& aabb_center, float const aabb_extents ) const;
			void			process			( );

private:
	loose_oct_tree const&	m_tree;
	math::cuboid const&		m_cuboid;
	objects_type*			m_objects;
	triangles_type*			m_triangles;
	results_callback_type*	m_callback;
	query_type				m_query_type;
	bool					m_result;
}; // class cuboid_object

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_CUBOID_OBJECT_H_INCLUDED