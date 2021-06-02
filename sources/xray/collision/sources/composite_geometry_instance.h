////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COMPOSITE_GEOMETRY_INSTANCE_H_INCLUDED
#define COMPOSITE_GEOMETRY_INSTANCE_H_INCLUDED

#include <xray/collision/geometry_instance.h>

namespace xray {
namespace collision {

class composite_geometry;

class composite_geometry_instance : public geometry_instance
{
public:
							composite_geometry_instance	( memory::base_allocator* allocator, float4x4 const& matrix, non_null<composite_geometry const>::ptr geometry );
	virtual					~composite_geometry_instance( );


	virtual void			destroy				( memory::base_allocator* allocator );

	virtual	void			set_matrix			( math::float4x4 const& matrix ) { m_matrix = matrix; m_inverted_matrix	= math::invert4x3( matrix ); }
	virtual	float4x4 const&	get_matrix			( ) const { return m_matrix; };

	virtual	bool			aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const;
	virtual	bool			cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool			ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const;

	virtual	bool			aabb_test			( math::aabb const& aabb ) const;
	virtual	bool			cuboid_test			( math::cuboid const& cuboid ) const;
	virtual	bool			ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	math::aabb		get_aabb			( ) const;
	virtual	math::aabb		get_geometry_aabb	( ) const;

	virtual	bool			is_valid			( ) const;
	virtual	void			render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;
	virtual	void			render		( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const;

	virtual	void			enumerate_primitives( enumerate_primitives_callback& cb ) const;
	virtual	void			enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const;

	virtual	void			add_triangles		( triangles_type& triangles ) const;

	virtual	math::float3 const*		vertices			( ) const;
	virtual	u32						vertex_count		( ) const;
	virtual	u32 const*				indices				( ) const;
	virtual	u32 const*				indices				( u32 triangle_id ) const;
	virtual	u32						index_count			( ) const;

	virtual	float					get_surface_area	( ) const;
	virtual	float3				get_random_surface_point( math::random32& randomizer ) const;
	virtual	float3					get_closest_point_to( float3 const& point, float4x4 const& origin = float4x4().identity() ) const;

	non_null<composite_geometry const>::ptr get_geometry( ) const { return m_geometry; }

	XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS_NOT_IMPLEMENTED
private:
	float4x4								m_matrix;
	float4x4								m_inverted_matrix;
	non_null<composite_geometry const>::ptr	m_geometry;
}; // class composite_geometry_instance

} // namespace collision
} // namespace xray

#endif // #ifndef COMPOSITE_GEOMETRY_INSTANCE_H_INCLUDED