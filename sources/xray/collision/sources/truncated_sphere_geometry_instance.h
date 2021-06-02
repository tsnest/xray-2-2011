////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef truncated_sphere_geometry_instance_INSTANCE_H_INCLUDED
#define truncated_sphere_geometry_instance_INSTANCE_H_INCLUDED

#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry_double_dispatcher.h>

namespace xray {
namespace collision {

class truncated_sphere_geometry_instance:
	public geometry_instance,
	private boost::noncopyable
{
public:
						truncated_sphere_geometry_instance	( float4x4 const& matrix, float radius, float4* planes, u32 planes_count );
	virtual				~truncated_sphere_geometry_instance	( )			{ }
	
	virtual	void		destroy				( memory::base_allocator* allocator );
	
	inline	float		radius				( ) const	{ return m_radius; }
	inline	buffer_vector< float4 > const &	planes	( ) const	{ return m_planes; }

	//XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS
	virtual	void			set_matrix		( math::float4x4 const& matrix ) { m_matrix = matrix; m_inverted_matrix	= math::invert4x3( matrix ); }
	virtual	float4x4 const&	get_matrix		( ) const { return m_matrix; };

	virtual	math::aabb	get_aabb			( ) const;
	virtual	math::aabb	get_geometry_aabb	( ) const;

	virtual	bool		is_valid			( ) const { return true; }
private:
	virtual	void		render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;
	virtual	void		render		( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const;
	virtual	math::float3 const* vertices	( ) const;
	virtual	u32			vertex_count		( ) const;
	virtual	u32 const*	indices				( ) const;
	virtual	u32 const*	indices				( u32 triangle_id ) const;
	virtual	u32			index_count			( ) const;

	virtual	bool		aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const;
	virtual	bool		cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool		ray_query			(
							object const* object,
							float3 const& origin,
							float3 const& direction,
							float max_distance,
							float& distance,
							ray_triangles_type& triangles,
							triangles_predicate_type const& predicate
						) const;

	virtual	bool		aabb_test			( math::aabb const& aabb ) const;
	virtual	bool		cuboid_test			( math::cuboid const& cuboid ) const;
	virtual	bool		ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	void		add_triangles		( triangles_type& triangles ) const;

public:
	virtual	math::aabb&	get_aabb			( math::aabb& result ) const;
	virtual	void		get_aabb			( math::aabb& bbox, float4x4 const& self_transform ) const;
	virtual	float3		get_random_surface_point( math::random32& randomizer ) const;
	virtual float3		get_closest_point_to( float3 const& point, float4x4 const& origin = float4x4().identity() ) const;
	virtual	float		get_surface_area	( ) const;

	virtual	void		enumerate_primitives( enumerate_primitives_callback& cb ) const;
	virtual	void		enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const;
	
	XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS
private:
	float4x4					m_matrix;
	float4x4					m_inverted_matrix;
	buffer_vector< float4 >		m_planes;
	float						m_radius;
}; // class truncated_sphere

} // namespace collision
} // namespace xray

#endif // #ifndef truncated_sphere_geometry_instance_INSTANCE_H_INCLUDED