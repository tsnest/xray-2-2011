////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.03.2008
//	Author		: Konstantin Slipchenko
//	Description : sphere - single triangle tests
////////////////////////////////////////////////////////////////////////////
#include <xray/collision/contact_info.h>
using xray::math::float3;
using xray::math::sqr;
namespace xray {
namespace collision {
namespace detail{



inline bool check_vertex( const vx_test_res	&vr, u32 v0, u32 v1, u32 v2 )
{
	if( !vr.use )
		return false;
	return	( vr.vertex_id == v0 ) || 
			( vr.vertex_id == v1 ) || 
			( vr.vertex_id == v2 ) ;
}

inline bool check_side( const side_test_res &sr, u32 v0, u32 v1, u32 v2 )
{
	if( !sr.use )
		return false;
	bool rsd0_0 = sr.vertex0_id == v0 ;
	bool rsd0_1 = sr.vertex0_id == v1 ;
	bool rsd0_2 = sr.vertex0_id == v2 ;

	bool rsd1_0 = sr.vertex1_id == v0 ;
	bool rsd1_1 = sr.vertex1_id == v1 ;
	bool rsd1_2 = sr.vertex1_id == v2 ;
	return ( rsd0_0||rsd0_1||rsd0_2 )  && ( rsd1_0||rsd1_1||rsd1_2 );
}

inline bool check_neighbour( const triangle_sphere_res &tri, u32 v0, u32 v1, u32 v2 )
{
	if( check_vertex( tri.r_vertex, v0, v1, v2 ) )
		return true;
	if( check_side( tri.r_side, v0, v1, v2 ) )
		return true;
	return false;
}

inline bool check_neighbours( const xray::buffer_vector<triangle_sphere_res> & vec, 
						   u32 v0, u32 v1, u32 v2 )
{
	if ( vec.empty() )
		return		false;

	xray::buffer_vector<triangle_sphere_res>::const_iterator i			= vec.begin();
	xray::buffer_vector<triangle_sphere_res>::const_iterator const e	= vec.begin() + vec.size() - 1;
	for( ; i != e; ++i )
		if( check_neighbour(*i, v0, v1, v2) ) 
			return	true;

	return			false;
}

inline static bool test_vertex( triangle_sphere_test_callback& cb, vx_test_res &res,  triangle_sphere_res &tri_res, 
							   const	float3 &v,
							   const float3 &vx_to_point, 
							   const float3 &after_vx_side, const float3 &before_vx_side, 
							   float radius, float sq_radius   )
{
	if( !test_vertex( res, vx_to_point, after_vx_side, before_vx_side ) )
			return false;
	res.use				= true;
	tri_res.r_vertex	= res;
	float	sq_vx_to_point	= vx_to_point.squared_length();
	tri_res.out = sq_vx_to_point > sq_radius;
	if(!tri_res.out && !check_neibour_verts( cb.res_vec, res.vertex_id ) )
	{
		ASSERT( sq_vx_to_point>0.f );
		////////////////////////////////////////////////////////////////////
		float dist	=	sqrt( sq_vx_to_point );/////////////////////////////
		////////////////////////////////////////////////////////////////////
		xray::collision::contact_info	c;
		c.depth		=	radius - dist;
		c.position	=	v;

		ASSERT( dist>0.f );
		c.normal	=	vx_to_point*(1.f/dist);
		cb.cb( c );
	}

	return true;
}

inline bool test_side( triangle_sphere_test_callback& cb, side_test_res& res, triangle_sphere_res &tri_res, const float3& side, 
							 const float3	&before_side, 
							 const float3& v0, const float3& p_sub_v0, const vx_test_res &v0_res, const vx_test_res &v1_res,  float radius, float sq_radius )//const vx_test_res &v_oposit_res,
{
	const float	sq_side					= side.squared_length();
	const float side_dot_before_side	= side|before_side;

	if(  !test_side( res, v0_res, v1_res, sq_side, side_dot_before_side ) ) //
		return false;
	res.use			= true;
	tri_res.r_side	= res;
	const float sq_p_sub_v0 = p_sub_v0.squared_length();

	float	sq_line_dist_mul_sq_side = sq_p_sub_v0*sq_side - sqr( v0_res.det_after_side );

	tri_res.out = sq_line_dist_mul_sq_side > sq_radius * sq_side;
	if( !tri_res.out && !check_neibour_sides( cb.res_vec, res.vertex0_id, res.vertex1_id ) )
	{
		ASSERT( sq_line_dist_mul_sq_side > 0.f );
		//sq_line_dist_mul_sq_side = abs( sq_line_dist_mul_sq_side );
		ASSERT( sq_line_dist_mul_sq_side > 0.f );
		ASSERT( sq_side > 0.f );

		const float	rsp_sq_side	= 1.f/sq_side;
		const float	sq_line_dist = sq_line_dist_mul_sq_side*rsp_sq_side;
		////////////////////////////////////////////////////////////////////
		const float	line_dist	= sqrt( sq_line_dist );//////////////////////
		////////////////////////////////////////////////////////////////////
		xray::collision::contact_info	c;
		c.depth = radius - line_dist;
		const float3	v0_to_line_point = side * ( v0_res.det_after_side * rsp_sq_side );
		c.position = v0 + v0_to_line_point;
		const float3	line_point_to_p = p_sub_v0 - v0_to_line_point;
		c.normal = line_point_to_p * ( 1.f/line_dist );
		cb.cb( c );
	}
	return true;
}



}// xray{
}// collision{
}// detail{


