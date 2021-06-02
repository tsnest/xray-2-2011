////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CAPSULE_GEOMETRY_INSTANCE_H_INCLUDED
#define CAPSULE_GEOMETRY_INSTANCE_H_INCLUDED

#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry_double_dispatcher.h>

namespace xray {
namespace collision {

class capsule_geometry_instance:
	public geometry_instance,
	private boost::noncopyable
{
public:
						capsule_geometry_instance	( float4x4 const& matrix, float radius, float half_length );
	virtual				~capsule_geometry_instance	( )			{ }
	
	virtual	void			destroy				( memory::base_allocator* allocator );

	inline	float			radius				( ) const	{ return m_radius; }
	inline	float			half_length			( ) const	{ return m_half_length; }

	virtual	void			set_matrix		( math::float4x4 const& matrix );
	virtual	float4x4 const&	get_matrix		( ) const { return m_matrix; };

	virtual	math::aabb		get_aabb			( ) const;
	virtual	math::aabb		get_geometry_aabb	( ) const;

	virtual	bool			is_valid			( ) const;

//	XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS

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
	virtual	float3		get_random_surface_point( math::random32& randomizer ) const;
	virtual	float3		get_closest_point_to( float3 const& point, float4x4 const& origin = float4x4().identity() ) const;
	virtual	float		get_surface_area	( ) const;

	virtual	void		enumerate_primitives( enumerate_primitives_callback& cb ) const;
	virtual	void		enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const;

	XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS
private:
	float4x4			m_matrix;
	float4x4			m_inverted_matrix;
	float				m_radius;
	float				m_half_length;
	float				m_true_half_length;

}; // class capsule_geometry_instance

} // namespace collision
} // namespace xray

#endif // #ifndef CAPSULE_GEOMETRY_INSTANCE_H_INCLUDED