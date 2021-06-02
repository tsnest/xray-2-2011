////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_COLLISION_UTILS_INLINE_H_INCLUDED
#define TERRAIN_COLLISION_UTILS_INLINE_H_INCLUDED

#include <xray/collision/ray_collision_utils.h>

namespace xray {
namespace collision {

inline u16	terrain_vertex_id( int vert_in_row, int const _x_idx, int const _z_idx )
{
	ASSERT(_x_idx>=0 && _x_idx<=vert_in_row && _z_idx>=0 && _z_idx<=vert_in_row);
	return (u16)(vert_in_row*_z_idx + _x_idx);
}


inline void terrain_vertex_xz( int vert_in_row, u16 vertex_id, int& _x_idx, int& _z_idx )
{
	_z_idx			= vertex_id / (vert_in_row);
	_x_idx			= vertex_id - (vert_in_row) * _z_idx;
}

inline void terrain_vertex_xz( int vert_in_row, float cell_size, u16 vertex_id, float& _x, float& _z )
{
	int _iz		= vertex_id / vert_in_row;
	_x			= cell_size * (vertex_id - vert_in_row * _iz);
	_z			= _iz * -cell_size;
}

inline u16 quad_index( int quad_in_row, int x, int y )
{
	ASSERT(x>=0 && x<=quad_in_row);
	ASSERT(y>=0 && y<=quad_in_row);

	return u16(quad_in_row*y + x);
}

inline bool	 terrain_quad::setup( u16 id, u32 quad_in_row )
{
	quad_id			= id;
	R_ASSERT(quad_id<=quad_in_row*quad_in_row);

	// fill indises here
	int _y			= id / quad_in_row;
	int _x			= id - quad_in_row*_y;

	index_lt		= u16( (quad_in_row+1)*_y + _x);
	index_lb		= u16(index_lt + quad_in_row + 1);
	index_rt		= index_lt + 1;
	index_rb		= index_lb + 1;


	R_ASSERT(index_lt<=(quad_in_row+1)*(quad_in_row+1));
	R_ASSERT(index_lb<=(quad_in_row+1)*(quad_in_row+1));
	R_ASSERT(index_rt<=(quad_in_row+1)*(quad_in_row+1));
	R_ASSERT(index_rb<=(quad_in_row+1)*(quad_in_row+1));
	return true;
}

inline void get_quad	(  terrain_quad &quad, u32 quad_in_row, int x, int y )
{
	quad.setup( quad_index( quad_in_row, x, y ), quad_in_row ) ;
}



inline bool ray_test_quad( float3 const& v_lt, float3 const& v_rt, float3 const& v_lb, float3 const& v_rb,
				   float3 const& pos, float3 const& dir, float const max_distance, float& range )
{
	
	float3	v0 = v_lb ;//owner->position(index_lb);
	float3	v1 = v_lt ;//owner->position(index_lt);
	float3	v2 = v_rt ;//owner->position(index_rt);
	
	bool result = test_triangle(	v0,
									v1,
									v2,
									pos,
									dir,
									max_distance,
									range );

	if(!result)
	{
		v1 = v_rt; //owner->position(index_rt);
		v2 = v_rb; //owner->position(index_rb);

		result	= test_triangle(	v0,
									v1,
									v2,
									pos,
									dir,
									max_distance,
									range );
	}
	return result;
}



inline bool get_row_col( float cell_size, int dimension,  float3 const& position_local, int& x, int& z )
{
	x	= int(position_local.x / (int)cell_size);
	z	= - int(position_local.z / (int)cell_size);

	math::clamp(x, 0, dimension-1);
	math::clamp(z, 0, dimension-1);

	ASSERT(x>=0 && x<dimension);
	ASSERT(z>=0 && z<dimension);

	return true;
}



template< class terrain_data_type >
inline math::aabb	terrain_aabb( terrain_data_type const &terrain_data ) 
{
	float const node_size	= terrain_data.physical_size( );
	return
		math::create_aabb_min_max(
			float3( 0.0f, terrain_data.min_height(), -node_size ),
			float3( node_size, terrain_data.max_height(), 0 )
		);
}


template< class terrain_data_type >
bool bresenham(	terrain_data_type const &terrain_data, 	int const Yd, int const Xd, 
				int const Yf, int const Xf, 
				float3 const& ray_point, 
				float3 const& ray_dir,
				float const max_distance,
				float& range, bool log_out )
{
  int dX, dY;
  int XInc, YInc;
  int S;
  int dX2, dY2, dXY;
  int X, Y;
  int i;

	dX = math::abs(Xd - Xf); 
	dY = math::abs(Yd - Yf);
	dX2 = dX + dX; 
	dY2 = dY + dY;
	if( Xd < Xf)
		XInc = 1;
	else
		XInc = -1;

	if(Yd < Yf)
		YInc = 1;
	else
		YInc = -1;

	X = Xd; 
	Y = Yd;

	if( terrain_data.ray_test_quad( Y, X, ray_point, ray_dir, max_distance, range, log_out ))
		return true;

	if( terrain_data.ray_test_quad( Y-1, X, ray_point, ray_dir, max_distance, range, log_out ))
		return true;
	if( terrain_data.ray_test_quad( Y+1, X, ray_point, ray_dir, max_distance, range, log_out ))
		return true;
	if( terrain_data.ray_test_quad( Y, X-1, ray_point, ray_dir, max_distance, range, log_out ))
		return true;
	if( terrain_data.ray_test_quad( Y, X+1, ray_point, ray_dir, max_distance, range, log_out ))
		return true;

	if(dX > dY)
	{
		S		= dY2 - dX;
		dXY		= dY2 - dX2;
		for( i = 1; i<=dX; ++i)
		{
			if(S >= 0)
			{
				Y	+= YInc;
				S	+= dXY;
			}else
				S	+= dY2;

			X	+= XInc;

			if( terrain_data.ray_test_quad( Y, X, ray_point, ray_dir, max_distance, range, log_out ))
				return true;

			if( terrain_data.ray_test_quad( Y-1, X, ray_point, ray_dir, max_distance, range, log_out ))
				return true;

			if( terrain_data.ray_test_quad( Y+1, X, ray_point, ray_dir, max_distance, range, log_out ))
				return true;
		}
	}else
	{
		S		= dX2 - dY;
		dXY		= dX2 - dY2;

		for (i = 1; i<=dY; ++i)
		{
			if(S >= 0)
			{
				X += XInc;
				S +=dXY;
			}else
				S += dX2;

			Y	+= YInc;

			if( terrain_data.ray_test_quad( Y, X, ray_point, ray_dir, max_distance, range, log_out ))
				return true;

			if( terrain_data.ray_test_quad( Y, X-1, ray_point, ray_dir, max_distance, range, log_out ))
				return true;

			if( terrain_data.ray_test_quad( Y, X+1, ray_point, ray_dir, max_distance, range, log_out ))
				return true;
		};
	}
  return false;
}


template< class terrain_data_type >
bool terrain_ray_test(  terrain_data_type const &terrain_data, math::float3 const& p_os, math::float3 const& d_ir, float max_distance, float& distance )
{ 
	// 1. Find EntryPoint for terrain node
	float	terrain_size	= terrain_data.physical_size	( ); // 20

	float3 ray_start		= p_os; // float3	( 10.450005f, 10.000f, 31.900002f );		
	float3 ray_direction	= d_ir; // float3	( 1.0f, 0.0f, 0.0f );						

	if(math::is_zero(ray_direction.x))
		ray_direction.x			= 0.0001f;

	if(math::is_zero(ray_direction.y))
		ray_direction.y			= 0.0001f;

	if(math::is_zero(ray_direction.z))
		ray_direction.z			= 0.0001f;

	float min_x		= (-ray_start.x) / ray_direction.x;
	float max_x		= (terrain_size-ray_start.x) / ray_direction.x;

	if(min_x>max_x)
		std::swap(min_x, max_x);

	float min_z		= ( -ray_start.z) / ray_direction.z;
	float max_z		= -( terrain_size + ray_start.z) / ray_direction.z;
	if(min_z>max_z)
		std::swap(min_z, max_z);

	float dist_to_enter	= math::max( min_x, min_z );
	dist_to_enter		= math::max( dist_to_enter, 0.0f );// start point is inside node

	float dist_to_exit	= max_x;
	dist_to_exit		= math::max( dist_to_exit, 0.0f );

	if(max_z>=0.0f)
		dist_to_exit	= math::min( dist_to_exit, max_z );

	R_ASSERT			(dist_to_exit >=0.0f);

  	float3 pt_enter		= ray_start + ray_direction*(dist_to_enter);
	math::clamp			( pt_enter.x, 0.0f, terrain_size ); // tmp
	math::clamp			( pt_enter.z, -terrain_size, 0.0f ); // tmp

	float3 pt_exit		= ray_start + ray_direction*(dist_to_exit-0.00001f);
	
	math::int2 quad_enter;
	math::int2 quad_exit;
	terrain_data.get_row_col(pt_enter, quad_enter.x, quad_enter.y );
	terrain_data.get_row_col(pt_exit, quad_exit.x, quad_exit.y );

	//2. walk on line pt_enter->pt_exit and test quads for intersection
	// using Bresenham algorithm

	bool walker_result = bresenham( terrain_data,	quad_enter.y, quad_enter.x,
											quad_exit.y, quad_exit.x,
											ray_start,
											ray_direction,
											max_distance,
											distance, false );
	if(walker_result)
	{
		return true;
	}
	//else
	//{		
	//	LOG_INFO("---");
	//	bresenham(	quad_enter.y, quad_enter.x,
	//				quad_exit.y, quad_exit.x,
	//				ray_start,
	//				ray_direction,
	//				max_distance,
	//				distance, true );
	//}


	return				false;
}

} // namespace collision
} // namespace xray

#endif // #ifndef TERRAIN_COLLISION_UTILS_INLINE_H_INCLUDED