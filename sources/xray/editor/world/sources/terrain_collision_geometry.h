////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_COLLISION_GEOMETRY_H_INCLUDED
#define TERRAIN_COLLISION_GEOMETRY_H_INCLUDED

#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry_double_dispatcher.h>

namespace xray {
namespace collision
{
	struct enumerate_primitives_callback;
	struct geometry_double_dispatcher;
} // namespace collision
namespace editor {
ref class terrain_node;

struct terrain_data
{
	 
					terrain_data	( terrain_node^ terrain ):m_terrain_node( terrain ){}
	
inline		float	physical_size	( )const;
inline	 	float	min_height		( )const;	
inline	 	float	max_height		( )const;	
inline	 	u32		dimension		( )const;	

inline	 	bool	get_row_col		( float3 const& position_local, int& x, int& z )const;

			bool	ray_test_quad	(	int const y1, int const x1, 
										float3 const& ray_point, 
										float3 const& ray_dir,
										float const max_distance,
										float& range,
										bool log_out ) const;		

private:
	 gcroot<terrain_node^>			m_terrain_node;

};




class terrain_collision_geometry:	public collision::geometry_instance,
									private boost::noncopyable
{
private:
	typedef collision::geometry_double_dispatcher			geometry_double_dispatcher;
	typedef collision::box_geometry_instance				box_geometry_instance;
	typedef collision::sphere_geometry_instance				sphere_geometry_instance;
	typedef collision::cylinder_geometry_instance			cylinder_geometry_instance;
	typedef collision::capsule_geometry_instance			capsule_geometry_instance;
	typedef collision::truncated_sphere_geometry_instance	truncated_sphere_geometry_instance;
	
public:
						terrain_collision_geometry	( terrain_node^ terrain, float4x4 const& transform );
	virtual				~terrain_collision_geometry	( );

	virtual	void		destroy						( memory::base_allocator* allocator );
	
	virtual	math::aabb	get_aabb			( ) const;
	virtual	math::aabb	get_geometry_aabb	( ) const;
	virtual	float3	get_random_surface_point( math::random32& randomizer ) const;
	virtual float3		get_closest_point_to( float3 const& point, float4x4 const& origin = float4x4().identity() ) const;
	virtual	float		get_surface_area	( ) const;

	virtual	void					set_matrix	( math::float4x4 const& matrix ) { m_matrix = matrix; *m_inverted_matrix	= math::invert4x3( matrix ); }
	virtual	math::float4x4 const&	get_matrix	( ) const { return m_matrix; }

	virtual	bool		is_valid			( ) const { return true; }

	XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS_NOT_IMPLEMENTED
private:
	virtual	void		render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;
	virtual	void		render				( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const;
	virtual	math::float3 const* vertices	( ) const;
	virtual	u32			vertex_count		( ) const;
	virtual	u32 const*	indices				( ) const;
	virtual	u32 const*	indices				( u32 triangle_id ) const;
	virtual	u32			index_count			( ) const;
	virtual	bool		aabb_query			( collision::object const* object, math::aabb const& aabb, collision::triangles_type& triangles )	const;
	virtual	bool		cuboid_query		( collision::object const* object, math::cuboid const& cuboid, collision::triangles_type& triangles )	const;
	virtual	bool		ray_query			(
							collision::object const* object,
							float3 const& pos,
							float3 const& dir,
							float max_distance,
							float& distance,
							collision::ray_triangles_type& triangles,
							collision::triangles_predicate_type const& predicate ) const;
	virtual	bool		aabb_test			( math::aabb const& aabb ) const;
	virtual	bool		cuboid_test			( math::cuboid const& cuboid ) const;
	virtual	bool		ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;
	
	virtual	u32			get_triangles_count	( math::convex const& bounding_convex ) const;
	
	virtual	void		add_triangles		( collision::triangles_type& triangles ) const;
	virtual	void		enumerate_primitives( collision::enumerate_primitives_callback&  ) const {};
	virtual	void		enumerate_primitives( float4x4 const& , collision::enumerate_primitives_callback&  ) const{};
private:
	float4x4			m_matrix;
	terrain_data		m_data;
	float4x4*			m_inverted_matrix;
};

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_COLLISION_GEOMETRY_H_INCLUDED
