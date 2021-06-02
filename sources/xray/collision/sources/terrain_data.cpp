////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_data.h"
#include <xray/collision/terrain_collision_utils.h>

namespace xray {
namespace collision {

terrain_data::terrain_data	( float phisical_size, u32 vertex_row_size, const float* heightfield ) :
	m_physical_size		( phisical_size ),
	m_vertex_row_size	( vertex_row_size ),
	m_heightfield		( heightfield )
{
}


float terrain_data::quad_size	( ) const	
{ 
	 return m_physical_size / ( quad_dimension( ) ); 
}


u32 terrain_data::quad_dimension	( ) const	
{ 
	 return vert_row_size( ) - 1; 
}

float terrain_data::min_height	( ) const	
{ 
	const u32 heights_count		= vert_row_size( ) * vert_row_size( );
	
	float ret					= math::infinity;
	for( u32 i = 0; i < heights_count; ++i )
		ret						= math::min( ret, m_heightfield[i] );

	return ret; 
}

float terrain_data::max_height	( ) const
{ 
	const u32 heights_count		=  vert_row_size( ) * vert_row_size( );

	float ret					= -math::infinity;
	for( u32 i = 0; i < heights_count; ++i )
		ret						= math::max( ret, m_heightfield[i] );

	return ret; 
}

bool	terrain_data::get_row_col	( float3 const& position_local, int& x, int& z ) const
{ 
	return collision::get_row_col( quad_size( ), m_vertex_row_size, position_local, x, z );
}

float3 terrain_data::position	( u16 vertex_id ) const
{
	ASSERT( vertex_id <  vert_row_size( ) * vert_row_size( ) );

	float3 result;

	terrain_vertex_xz( vert_row_size(), quad_size(), vertex_id, result.x, result.z );
	
	result.y		= m_heightfield[vertex_id];
	return result;
}

bool terrain_data::ray_test_quad		(	int const y, int const x, 
											float3 const& ray_point, 
											float3 const& ray_dir,
											float const max_distance,
											float& range,
											bool log_out ) const
{ 
	XRAY_UNREFERENCED_PARAMETERS( y, x, &ray_point, &ray_dir, max_distance, range, log_out );
	
	if(x<0 || y<0)
		return false;

	if(x >= (int)quad_dimension() || y >= (int)quad_dimension() )
		return false;

	if(log_out)
		LOG_INFO("%d %d", x, y);

	terrain_quad	quad;
	get_quad( quad, quad_dimension(), x, y );

	return collision::ray_test_quad(	position( quad.index_lt ),  position( quad.index_rt ),
								position( quad.index_lb ),  position( quad.index_rb ),
								ray_point, ray_dir, max_distance, range );
	//m_terrain_node->get_quad	( quad, x, y );
	//return quad.ray_test		( ray_point, ray_dir, max_distance, range );
}


} // namespace collision
} // namespace xray