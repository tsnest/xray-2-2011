////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMPOSITE_GEOMETRY_H_INCLUDED
#define COMPOSITE_GEOMETRY_H_INCLUDED

#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

namespace xray {
namespace collision {

class composite_geometry :
	public geometry,
	private boost::noncopyable
{
public:
	typedef buffer_vector< geometry_instance* >	geometry_instances_type;

						composite_geometry		( memory::base_allocator* allocator, geometry_instances_type const& instances );

			void		destroy					( memory::base_allocator* allocator );

	virtual	math::aabb	get_aabb				( ) const;
	virtual	float3		get_random_surface_point( math::random32& randomizer ) const;
	virtual	float		get_surface_area		( ) const;
	virtual	bool		is_inside				( float3 const& position ) const;

	geometry_instance**	begin					( )	{ return ( &*m_geometry_instances.begin() ); }
	geometry_instance**	end						( ) { return ( &*m_geometry_instances.end() ); }

	geometry_instance* const* begin				( )	const { return ( &*m_geometry_instances.begin() ); }
	geometry_instance* const* end				( ) const { return ( &*m_geometry_instances.end() ); }

//	XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS_NOT_IMPLEMENTED

	virtual	void		render					( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& matrix ) const;
			float3		get_closest_point_to	( float3 const& point, float4x4 const& origin ) const;
private:
	virtual	math::float3 const* vertices		( ) const;
	virtual	u32			vertex_count			( ) const;
	virtual	u32 const*	indices					( ) const;
	virtual	u32 const*	indices					( u32 triangle_id ) const;
	virtual	u32			index_count				( ) const;

	virtual	bool		aabb_query				( object const* object, math::aabb const& aabb, triangles_type& triangles ) const ;
	virtual	bool		cuboid_query			( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool		ray_query				(
							object const* object,
							float3 const& origin,
							float3 const& direction,
							float max_distance,
							float& distance,
							ray_triangles_type& triangles,
							triangles_predicate_type const& predicate
						) const				;

	virtual	bool		aabb_test			( math::aabb const& aabb ) const;
	virtual	bool		cuboid_test			( math::cuboid const& cuboid ) const;
	virtual	bool		ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	u32			get_triangles_count	( math::convex const& bounding_convex ) const;

	virtual	void		add_triangles		( triangles_type& triangles ) const;

public:
	virtual	void		enumerate_primitives( enumerate_primitives_callback& cb ) const;
	virtual	void		enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const;
private:
	geometry_instances_type	m_geometry_instances;
}; // class composite_geometry

} // namespace collision
} // namespace xray

#endif // #ifndef COMPOSITE_GEOMETRY_H_INCLUDED