////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLIDERS_RAY_OBJECT_H_INCLUDED
#define COLLIDERS_RAY_OBJECT_H_INCLUDED

#include "colliders_ray_aabb.h"

namespace xray {
namespace collision {
namespace colliders {

namespace object {
	template < bool >
	struct vertical_predicate;
} // namespace object

class ray_object	{
public:
					ray_object				(
						query_type const query_type,
						loose_oct_tree const& tree,
						float3 const& origin,
						float3 const& direction,
						float const max_distance,
						ray_triangles_type& triangles,
						triangles_predicate_type const& immediate_predicate
					);
					ray_object				(
						query_type const query_type,
						loose_oct_tree const& tree,
						float3 const& origin,
						float3 const& direction,
						float const max_distance,
						ray_objects_type& objects,
						objects_predicate_type const& immediate_predicate
					);
	inline	bool	result					( ) const { return m_result; }

private:
			void	initialize				( float3 const& origin );
			bool	intersects_aabb			( float3 const& node_center, float3 const& radius, float& distance ) const;
	inline	bool	intersects_aabb_vertical( float3 const& node_center, float3 const& radius, float& distance ) const;
			template < typename T >
			void	query					( non_null<oct_node const>::ptr const node, float3 const& node_center, float const node_radius, T const& predicate );
			bool	query_objects			( );
			bool	query_triangles			( );
			bool	process_query			( );
			bool	false_triangle_predicate( ray_triangle_result const& ) const;

			friend object::vertical_predicate< false >;
			friend object::vertical_predicate< true >;
private:
	ray_aabb_collider				m_ray_aabb_collider;

private:
	loose_oct_tree const&			m_tree;
	ray_objects_type*				m_objects;
	objects_predicate_type const*	m_objects_predicate;
	ray_triangles_type*				m_triangles;
	triangles_predicate_type const*	m_triangles_predicate;
	float const						m_max_distance;
	query_type						m_query_type;
	bool							m_result;
}; // class ray_object

} // namespace colliders
} // namespace collision
} // namespace xray

#endif // #ifndef COLLIDERS_RAY_OBJECT_H_INCLUDED