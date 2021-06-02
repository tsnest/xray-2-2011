////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_OBJECT_H_INCLUDED
#define COLLISION_OBJECT_H_INCLUDED

#include <xray/collision/object.h>
#include <xray/collision/geometry_instance.h>

namespace xray {
namespace collision {

class XRAY_COLLISION_API collision_object : public object {
public:
								collision_object	(
									memory::base_allocator* allocator,
									collision::object_type type_object,
									geometry_instance* geometry,
									pvoid const user_data = 0
								);
	virtual						~collision_object	( );
	
	virtual void				destroy						( memory::base_allocator* allocator );

private:
								collision_object	( collision_object const& );
	collision_object&				operator =		( collision_object const& );

public:
	virtual	void							render			( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;
			void					dbg_render_nearest_point( render::scene_ptr const& scene, render::debug::renderer& r, float3 const& point ) const;
	virtual	bool							aabb_query		( math::aabb const& aabb, triangles_type& results ) const;	
	virtual	bool							cuboid_query	( math::cuboid const& cuboid, triangles_type& results ) const;
	virtual	bool							ray_query		(
												math::float3 const& origin, 
												math::float3 const& direction, 
												float max_distance, 
												float& distance, 
												ray_triangles_type& results, 
												triangles_predicate_type const& predicate
											) const;

	virtual	bool							aabb_test		( math::aabb const& aabb ) const;	
	virtual	bool							cuboid_test		( math::cuboid const& cuboid ) const;	
	virtual	bool							ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	void							add_triangles	( triangles_type& triangles ) const;

	geometry_instance* const				get_geometry_instance	( ) const { return m_geometry_instance; };

	virtual	math::aabb						update_aabb	( float4x4 const& local_to_world );

	virtual	math::float4x4 const&			get_matrix		( ) const;

protected:
			void							set_matrix		( math::float4x4 const& matrix );
public:
			math::aabb				get_aabb			( ) const;
			math::float3			get_center			( ) const;
			math::float3			get_extents			( ) const;
private:
	geometry_instance* const				m_geometry_instance;
}; // class collision_object

} // namespace collision
} // namespace xray

#endif // #ifndef COLLISION_OBJECT_H_INCLUDED
