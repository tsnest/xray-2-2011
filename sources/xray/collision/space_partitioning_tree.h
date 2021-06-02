////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SPACE_PARTITIONING_TREE_H_INCLUDED
#define XRAY_SPACE_PARTITIONING_TREE_H_INCLUDED

#include <xray/collision/common_types.h>
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {

namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace collision {

struct XRAY_NOVTABLE space_partitioning_tree
{
	virtual	void	insert			( non_null<object>::ptr object, math::float4x4 const& local_to_world ) = 0;
	virtual	void	erase			( non_null<object>::ptr object ) = 0;
	virtual	void	move			( non_null<object>::ptr object, math::float4x4 const& new_local_to_world ) = 0;
	virtual	void	render			( render::scene_ptr const& scene, render::debug::renderer& renderer ) = 0;

	virtual	bool	aabb_query		( object_type query_mask, math::aabb const& aabb, triangles_type& results ) = 0;
	virtual	bool	aabb_query		( object_type query_mask, math::aabb const& aabb, objects_type& results ) = 0;
	
	virtual	bool	cuboid_query	( object_type query_mask, math::cuboid const& cuboid, triangles_type& results ) = 0;
	virtual	bool	cuboid_query	( object_type query_mask, math::cuboid const& cuboid, objects_type& results ) = 0;
	virtual	bool	cuboid_query	( object_type query_mask, math::cuboid const& cuboid, results_callback_type& callback ) = 0;

	virtual	bool	ray_query		(
						object_type query_mask,
						math::float3 const& origin,
						math::float3 const& direction,
						float max_distance,
						ray_triangles_type& triangles,
						triangles_predicate_type const& predicate
					)				= 0;
	virtual	bool	ray_query		(
						object_type query_mask,
						math::float3 const& origin,
						math::float3 const& direction,
						float max_distance,
						ray_objects_type& objects,
						objects_predicate_type const& predicate
					)				= 0;

	virtual	bool	aabb_test		( math::aabb const& aabb, triangles_predicate_type const& predicate ) = 0;
	virtual	bool	cuboid_test		( math::cuboid const& cuboid, triangles_predicate_type const& predicate ) = 0;
	virtual	bool	ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, triangles_predicate_type const& predicate ) = 0;

	typedef fastdelegate::FastDelegate< void ( object const* )>	predicate_type;
	virtual	void	for_each		( predicate_type const& predicate ) const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( space_partitioning_tree )
}; // struct space_partitioning_tree

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_SPACE_PARTITIONING_TREE_H_INCLUDED