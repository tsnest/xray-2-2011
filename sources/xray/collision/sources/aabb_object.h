////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef AABB_OBJECT_H_INCLUDED
#define AABB_OBJECT_H_INCLUDED

#include <xray/collision/object.h>

namespace xray {
namespace render {

namespace debug {
	class renderer;
} // namespace debug 

} // namespace render 

namespace collision {

class aabb_object : public object
{
public:
					aabb_object		( 
						object_type object_type,
						math::aabb const& aabb,
						pvoid user_data
					);
	virtual			~aabb_object	( );
	
	virtual	void	render			( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;

	virtual	bool	aabb_query		( math::aabb const& aabb, triangles_type& triangles ) const;
	virtual	bool	cuboid_query	( math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool	ray_query		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, collision::ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const;

	virtual	bool	aabb_test		( math::aabb const& aabb ) const;
	virtual	bool	cuboid_test		( math::cuboid const& cuboid ) const;
	virtual	bool	ray_test		( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	void	add_triangles	( triangles_type& triangles ) const;

private:
	virtual	math::aabb	update_aabb( float4x4 const& local_to_world );
private:
}; // class aabb_object

} // namespace collision
} // namespace xray

#endif // #ifndef AABB_OBJECT_H_INCLUDED
