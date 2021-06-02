////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.01.2008
//	Author		: Konstantin Slipchenko
//	Description : space_item
////////////////////////////////////////////////////////////////////////////
#ifndef SPACE_ITEM_H_INCLUDED
#define SPACE_ITEM_H_INCLUDED

#include <xray/collision/object.h>

class collision_space;
class island;
class space_object;

namespace xray {

namespace collision {
	struct  space_partitioning_tree;
} // namespace collision

namespace physics{

struct space_detail;

class space_item:
	private collision::object,
	private boost::noncopyable
{
	xray::collision::space_partitioning_tree*	m_space;
	space_object&								m_space_object;

public:
							space_item		( space_object	&so );
			virtual			~space_item		( );
public:

			void			space_insert	( xray::collision::space_partitioning_tree &space );
			void			space_remove	( );
			void			collide			( island& i  );
			void			on_move			( );

private:
	virtual	void			on_physics_space_collide ( physics::space_detail& sd );

	virtual	void			render			( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;

	virtual	bool			aabb_query		( math::aabb const& aabb, xray::collision::triangles_type& results ) const;
	virtual	bool			cuboid_query	( math::cuboid const& cuboid, xray::collision::triangles_type& results ) const;
	virtual	bool			ray_query		( float3 const& origin, float3 const& direction, float max_distance, float& distance, xray::collision::ray_triangles_type& results, xray::collision::triangles_predicate_type const& predicate ) const;

	virtual	bool			aabb_test		( math::aabb const& aabb ) const;
	virtual	bool			cuboid_test		( math::cuboid const& cuboid ) const;
	virtual	bool			ray_test		( float3 const& origin, float3 const& direction, float max_distance, float& distance ) const;

	virtual	void			add_triangles	( collision::triangles_type& triangles ) const;
	virtual	math::aabb		update_aabb		( float4x4 const& local_to_world );

private:
	virtual	physics::space_item* cast_space_item	( ) { return this; }

}; // class space_item

} // namespace physics
} // namespace xray

#endif // #ifndef SPACE_ITEM_H_INCLUDED