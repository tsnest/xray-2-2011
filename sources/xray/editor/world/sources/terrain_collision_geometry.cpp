////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_collision_geometry.h"
#include "terrain_object.h"
#include <xray/collision/terrain_collision_utils.h>

namespace xray {
namespace editor {


inline		float		terrain_data::physical_size	( )const	
{ 
	return (float)m_terrain_node->size();
}

inline	 	float	terrain_data::min_height	( )const	
{ 
	return m_terrain_node->m_min_height; 
}

inline	 	float	terrain_data::max_height	( )const	
{ 
	return m_terrain_node->m_max_height; 
}

inline	 	u32		terrain_data::dimension		( )const	
{ 
	return m_terrain_node->m_vertices.m_dimension; 
}

inline bool terrain_data::get_row_col( float3 const& position_local, int& x, int& z )const
{ 
	return m_terrain_node->get_row_col( position_local, x, z );
}

bool terrain_data::ray_test_quad(	int const y, int const x, 
									float3 const& ray_point, 
									float3 const& ray_dir,
									float const max_distance,
									float& range,
									bool log_out ) const
{
	if(x<0 || y<0)
	return false;

	if( x>=(int)dimension() || y>=(int)dimension() )
		return false;

	if(log_out)
		LOG_INFO("%d %d", x, y);

	terrain_quad				quad;

	m_terrain_node->get_quad	( quad, x, y );

	return quad.ray_test		( ray_point, ray_dir, max_distance, range );
}


terrain_collision_geometry::terrain_collision_geometry	( terrain_node^ terrain, float4x4 const& transform ) :
	m_data									( terrain ),
	m_matrix								( transform ),
	m_inverted_matrix						( NEW( float4x4 ) ( math::invert4x3(transform) ) )
{
}

terrain_collision_geometry::~terrain_collision_geometry	( )
{
	DELETE									( m_inverted_matrix );
}

void terrain_collision_geometry::destroy	( memory::base_allocator* allocator )
{
	XRAY_UNREFERENCED_PARAMETER				( allocator );
}

math::aabb terrain_collision_geometry::get_aabb( ) const
{
	math::aabb tmp				= collision::terrain_aabb( m_data );
	return						tmp.modify( m_matrix ); 
}

math::aabb terrain_collision_geometry::get_geometry_aabb( ) const
{
	return collision::terrain_aabb( m_data );
}


float3 terrain_collision_geometry::get_random_surface_point ( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETER	( randomizer );
	UNREACHABLE_CODE			( return float3() );
}

float3 terrain_collision_geometry::get_closest_point_to	( float3 const& point, float4x4 const& origin ) const
{
	XRAY_UNREFERENCED_PARAMETER	( point );
	float4x4 transform			= origin * m_matrix;
	NOT_IMPLEMENTED				( return transform.c.xyz( ) );
}

float terrain_collision_geometry::get_surface_area	( ) const
{
	NOT_IMPLEMENTED				( );
	return						0.f;
}

void terrain_collision_geometry::render( render::scene_ptr const& /*scene*/, render::debug::renderer& /*renderer*/ ) const
{
	/* NOT_IMPLEMENTED();*/
}

void terrain_collision_geometry::render( render::scene_ptr const& /*scene*/, render::debug::renderer& /*renderer*/, float4x4 const& /*transform*/ ) const
{
	/* NOT_IMPLEMENTED();*/
}

math::float3 const*  terrain_collision_geometry::vertices( ) const											
{ NOT_IMPLEMENTED( return NULL );}

u32 terrain_collision_geometry::vertex_count( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32	terrain_collision_geometry::index_count	( ) const
{
	NOT_IMPLEMENTED(return 0);
}

u32 const* terrain_collision_geometry::indices( ) const							
{
	NOT_IMPLEMENTED( return NULL );
}

u32 const* terrain_collision_geometry::indices( u32 ) const							
{ NOT_IMPLEMENTED( return NULL );}

bool terrain_collision_geometry::aabb_query( collision::object const*, math::aabb const&, collision::triangles_type& ) const
{ NOT_IMPLEMENTED( return false );}

bool terrain_collision_geometry::cuboid_query( collision::object const*, math::cuboid const&, collision::triangles_type&) const 
{ NOT_IMPLEMENTED( return false );}

bool terrain_collision_geometry::ray_query(	collision::object const*,
											float3 const&,
											float3 const&,
											float,
											float&,
											collision::ray_triangles_type&,
											collision::triangles_predicate_type const&) const
{ return false; /*NOT_IMPLEMENTED(return false);*/ }

bool terrain_collision_geometry::aabb_test( math::aabb const& ) const
{ return false;}

bool terrain_collision_geometry::cuboid_test( math::cuboid const& ) const	
{ return false;}

bool terrain_collision_geometry::ray_test( math::float3 const& p_os, 
										  math::float3 const& d_ir, 
										  float max_distance, 
										  float& distance ) const
{
	float3 const new_origin		= m_inverted_matrix->transform_position(p_os);
	float3 const new_direction	= m_inverted_matrix->transform_direction(d_ir);
	float const new_max_distance = length(m_inverted_matrix->transform_position(p_os + d_ir*max_distance) - new_origin);
	bool const result			=
		collision::terrain_ray_test( m_data, new_origin, new_direction, new_max_distance, distance );
	distance					*= max_distance/new_max_distance;
	return						result;
}

void terrain_collision_geometry::add_triangles( collision::triangles_type& )const
{ NOT_IMPLEMENTED(); }


u32 terrain_collision_geometry::get_triangles_count	( math::convex const& bounding_convex ) const
{
	XRAY_UNREFERENCED_PARAMETER		( bounding_convex );
	NOT_IMPLEMENTED(return 0);
}

} // namespace editor
} // namespace xray
