////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOOSE_OCT_TREE_H_INCLUDED
#define LOOSE_OCT_TREE_H_INCLUDED

#include <xray/collision/space_partitioning_tree.h>

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace collision {

struct oct_node {
	oct_node*	octants[8];
	oct_node*	parent;
	object*		objects;
}; // struct oct_node

class vertex_allocator;

class loose_oct_tree : public space_partitioning_tree
{
public:
					loose_oct_tree			( memory::base_allocator* allocator, float min_aabb_extents, u32 reserve_node_count );
					~loose_oct_tree			( );
	virtual	void	insert					( non_null<object>::ptr object, float4x4 const& local_to_world );
	virtual	void	erase					( non_null<object>::ptr object );
	virtual	void	move					( non_null<object>::ptr object, float4x4 const& new_local_to_world );
	virtual	void	render					( render::scene_ptr const& scene, render::debug::renderer& renderer );

public:
	virtual	bool	aabb_query				( object_type query_mask, math::aabb const& aabb, objects_type& results );
	virtual	bool	aabb_query				( object_type query_mask, math::aabb const& aabb, triangles_type& results );

	virtual	bool	cuboid_query			( object_type query_mask, math::cuboid const& cuboid, objects_type& results );
	virtual	bool	cuboid_query			( object_type query_mask, math::cuboid const& cuboid, triangles_type& results );
	virtual	bool	cuboid_query			( object_type query_mask, math::cuboid const& cuboid, results_callback_type& callback );

	virtual	bool	ray_query				(
						object_type query_mask,
						float3 const& origin,
						float3 const& direction,
						float max_distance,
						ray_triangles_type& results,
						triangles_predicate_type const& predicate
					);
	virtual	bool	ray_query				(
						object_type query_mask,
						float3 const& origin,
						float3 const& direction,
						float max_distance,
						ray_objects_type& objects,
						objects_predicate_type const& predicate
					);

	inline non_null<oct_node const>::ptr root ( ) const;
	inline float3 const& aabb_center		( ) const;
	inline float	aabb_extents				( ) const;
	memory::base_allocator& get_allocator	( ) const;

public:
			u32		object_count			( ) const;
			u32		node_count				( ) const;

public:
	template < typename P >
	inline	void	for_each				( P const& predicate ) const;

private:
	template < typename P >
	inline void		for_each_iterate		(
						P const& predicate,
						non_null< oct_node const>::ptr const node,
						float3 const& aabb_center,
						float const aabb_extents
					) const;
	virtual	void	for_each				( predicate_type const& predicate ) const;

private:
			void	initialize				( non_null<object>::ptr object, float3 const& aabb_center, float3 const& aabb_extents );
			void	uninitialize			( );

private:
	inline	bool	out_of_bounds			( float3 const& aabb_center, float3 const& aabb_extents ) const;
			void	update_bounds			( float3 const& aabb_center, float3 const& aabb_extents );
			void	insert					( non_null< oct_node >::ptr node, non_null<object>::ptr object, float3 const& node_center, float const node_radius );
			void	render_iterate			( render::scene_ptr const& scene, render::debug::renderer& renderer, non_null< oct_node const>::ptr const node, float3 const& aabb_center, float const aabb_extents );

private:
			bool	try_collapse_bottom_top	( non_null< oct_node >::ptr node );
			void	try_collapse_top_bottom	( );
			void	remove_octant			( non_null< oct_node >::ptr node, oct_node* const child );
			void	remove_nodes			( non_null< oct_node >::ptr node );
	template <typename T>
	inline	void	remove_impl				( non_null<object>::ptr object, T const& predicate );

private:
	virtual	bool	aabb_test				( math::aabb const& aabb, triangles_predicate_type const& predicate );
	virtual	bool	cuboid_test				( math::cuboid const& cuboid, triangles_predicate_type const& predicate );
	virtual	bool	ray_test				( math::float3 const& origin, math::float3 const& direction, float max_distance, triangles_predicate_type const& predicate );
			void	add_objects				( non_null<oct_node const>::ptr const node, objects_type& objects ) const;

private:
			void	validate				( oct_node* node, u32* object_count = 0 ) const;
			void	insert_impl				( non_null<object>::ptr const object );

private:
	static inline void add					( non_null< oct_node >::ptr node, non_null<object>::ptr object );

private:
	float3				m_aabb_center;
	vertex_allocator*	m_allocator;
	oct_node*			m_root;
	float				m_aabb_extents;
	float				m_min_aabb_extents;
	u32					m_object_count;
	bool				m_initialized;
}; // class loose_oct_tree

	inline	u8		octant_index			( float3 const& signs );
	inline	float3	octant_vector			( ptrdiff_t const octant_index );

} // namespace collision
} // namespace xray

#include "loose_oct_tree_inline.h"

#endif // #ifndef LOOSE_OCT_TREE_H_INCLUDED