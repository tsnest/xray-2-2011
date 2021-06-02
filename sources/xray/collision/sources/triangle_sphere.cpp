////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.02.2008
//	Author		: Konstantin Slipchenko
//	Description : sphere - single triangle tests
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "triangle_sphere.h"

#ifdef DEBUG
#	include <xray/render/facade/debug_renderer.h>
#endif // #ifdef DEBUG

namespace xray {
namespace collision {
namespace detail{

void triangle_sphere_test(	triangle_sphere_test_callback &cb_r, buffer_vector<u32> &back_vec, 
							const u32 *	ind, math::float3 const* verts,
							const float3 &p, float radius, float4x4 const& self_transform )
 {
 
	//u32 iv0	= ind[ 0 ], iv1	= ind[ 1 ], iv2	= ind[ 2 ];
	
	u32	local_triangle_id	= u32( cb_r.res_vec.size() );
	triangle_sphere_res r( local_triangle_id, ind );
	cb_r.res_vec.push_back( r );
	triangle_sphere_res &res = cb_r.res_vec.back();

	const float3	v0 = verts[ res.v0 ] * self_transform, &v1 = verts[ res.v1 ] * self_transform, &v2 = verts[ res.v2 ] * self_transform;

	//if( check_neibours( al_tri_res, iv0, iv1, iv2 ) )continue;
	//plane
	//res.side0		=v1-v0	;
	//res.side1		=v2-v1	;
	//res.tri_ax		= res.side0^res.side1;


	const	float3	p_sub_v0	=p-v0;
	base_precalculation( res, v0, v1, v2, p_sub_v0 );
	//res.ax_dist		=p_sub_v0|res.tri_ax;
	
	//res.b_in_plane = res.ax_dist < 0;

#ifdef DEBUG
//	math::color positiv_color( 0u, 0u, 255u, 255u) ;
//	math::color back_color( 0u, 0u, 255u, 255u) ;
//	math::color color = res.b_in_plane ? back_color: positiv_color;
	//cb_r.cb.render_world()->debug(user_id).draw_triangle( v0, v1, v2, color );
	//cb_r.cb.renderer().draw_line( v0, v1, color );
	//cb_r.cb.renderer().draw_line( v1, v2, color );
	//cb_r.cb.renderer().draw_line( v2, v0, color );
#endif // #ifdef DEBUG

    if( res.b_in_plane )
	{
		back_vec.push_back( local_triangle_id );
		return;
	}

	//exit tri plane whith square mag - 
	const float sq_ax_mag = res.tri_ax.squared_length();
	const float sq_ax_dist = sqr( res.ax_dist );
	const float sq_radius = sqr( radius );
	
	res.out = sq_ax_dist > sq_radius * sq_ax_mag;
	
	if( res.out )
			return;
	//points
	const float3	side2		= v0 - v2;
	vx_test_res vx0_res;
	vx0_res.vertex_id = res.v0;
	if( test_vertex( cb_r, vx0_res, res, v0, p_sub_v0, res.side0, side2, radius, sq_radius ) )
		return;
	const	float3	p_sub_v1	= p-v1;
	vx_test_res vx1_res;
	vx1_res.vertex_id = res.v1;
	if( test_vertex( cb_r, vx1_res, res, v1, p_sub_v1, res.side1, res.side0, radius, sq_radius ) )
		return;
	const	float3	p_sub_v2	= p-v2;
	vx_test_res vx2_res;
	vx2_res.vertex_id = res.v2;
	if( test_vertex( cb_r, vx2_res, res, v2, p_sub_v2, side2, res.side1, radius, sq_radius ) )
		return;
	//sides
	side_test_res sd0_res;
	sd0_res.vertex0_id  = res.v0;
	sd0_res.vertex1_id  = res.v1;
	if( test_side( cb_r, sd0_res, res, res.side0, side2, v0, p_sub_v0, vx0_res, vx1_res,  radius, sq_radius ) )
		return;
	side_test_res sd1_res;
	sd0_res.vertex0_id	= res.v1;
	sd0_res.vertex1_id  = res.v2;
	if( test_side( cb_r, sd1_res, res, res.side1, res.side0, v1, p_sub_v1, vx1_res, vx2_res,  radius, sq_radius ) )
		return;
	side_test_res sd2_res;
	sd2_res.vertex0_id  = res.v2;
	sd0_res.vertex1_id  = res.v0;
	if( test_side( cb_r, sd2_res, res,    side2, res.side1, v2, p_sub_v2, vx2_res, vx0_res, radius, sq_radius ) )
		return;

	res.b_tri_sides_internal = true;

	res.use	= true;
	ASSERT( sq_ax_mag>0 );
	//////////////////////////////////////////////
	const float ax_mag = sqrt( sq_ax_mag );/////////
	//////////////////////////////////////////////
	const float rcp_ax_mag = 1.f/ax_mag;
	xray::collision::contact_info	c;
	c.depth		= radius - res.ax_dist * rcp_ax_mag;
	c.normal	= res.tri_ax * rcp_ax_mag;
	c.position	= p	- c.normal * radius;
	cb_r.cb( c );
}

bool test_back_vertex( back_res& cb, vx_test_res &res, const float3 &p, float radius, const float3 &vx_to_point, const float3 &after_vx_side, const float3 &before_vx_side )
{
	if( test_vertex(  res, vx_to_point, after_vx_side, before_vx_side ) )
	{
		xray::collision::contact_info	c;
		float mag = vx_to_point.length();
		ASSERT( mag > 0 );
		c.normal = vx_to_point*(-1.f/mag);
		c.depth = mag + radius;
		c.position = p - c.normal * radius;
		//cb( c );
		cb.update( c );
		return	true;
	}
	return false;
}

bool test_back_side( back_res& cb,  side_test_res& res, const float3& side, 
					const float3	&before_side,
					const float3& v0, const float3& p_sub_v0, const float3 &p, 
					float radius,  const vx_test_res &v0_res, const vx_test_res &v1_res )
{
	XRAY_UNREFERENCED_PARAMETER	(v0);
	const float	sq_side					= side.squared_length();
	const float side_dot_before_side	= side|before_side;
	if( !test_side( res, v0_res, v1_res, sq_side, side_dot_before_side ) )
		return false;

	const float sq_p_sub_v0 = p_sub_v0.squared_length();
	const float	sq_line_dist_mul_sq_side = sq_p_sub_v0*sq_side - sqr( v0_res.det_after_side );
	ASSERT( sq_line_dist_mul_sq_side > 0.f );
	ASSERT( sq_side > 0.f );
	const float	rsp_sq_side	= 1.f/sq_side;
	const float	sq_line_dist = sq_line_dist_mul_sq_side*rsp_sq_side;
	////////////////////////////////////////////////////////////////////
	const float	line_dist	= sqrt( sq_line_dist );//////////////////////
	////////////////////////////////////////////////////////////////////
	xray::collision::contact_info	c;
	c.depth = line_dist + radius;
	const float3	v0_to_line_point = side * ( v0_res.det_after_side * rsp_sq_side );
	
	const float3	line_point_to_p = p_sub_v0 - v0_to_line_point;

	c.normal = line_point_to_p * ( -1.f/line_dist );

	c.position = p	- c.normal * radius;//v0 + v0_to_line_point;

	//cb( c );
	cb.update( c );
	return true;
}

void triangle_sphere_back_test(	back_res &cb_r, 
							    triangle_mesh_primitiv_res &res, 
								math::float3 const* verts,
								const float3 &p, float radius, float4x4 const &mesh_transform )
{
	
	const float3	v0 = verts[ res.v0 ] * mesh_transform;
	const float3	v1 = verts[ res.v1 ] * mesh_transform;
	const float3	v2 = verts[ res.v2 ] * mesh_transform;
	
	//points

	const float3	side2		= v0 - v2;
	const	float3	p_sub_v0	= p-v1;
	vx_test_res vx0_res;
	vx0_res.vertex_id = res.v0;
	if( test_back_vertex( cb_r, vx0_res, p, radius, p_sub_v0, res.side0, side2 ) )
		return;

	const	float3	p_sub_v1	= p-v1;
	vx_test_res vx1_res;
	vx1_res.vertex_id = res.v1;
	if( test_back_vertex( cb_r, vx1_res, p, radius, p_sub_v1, res.side1, res.side0 ) )
		return;

	const	float3	p_sub_v2	= p-v2;
	vx_test_res vx2_res;
	vx2_res.vertex_id = res.v2;
	if( test_back_vertex( cb_r, vx2_res, p, radius, p_sub_v2, side2, res.side1 ) )
		return;
	
	//test_back_side
	//sides
	side_test_res sd0_res;
	sd0_res.vertex0_id  = res.v0;
	sd0_res.vertex1_id  = res.v1;
	if( test_back_side( cb_r, sd0_res, res.side0, side2, v0, p_sub_v0, p, radius, vx0_res, vx1_res) )
		return;
	
	side_test_res sd1_res;
	sd0_res.vertex0_id	= res.v1;
	sd0_res.vertex1_id  = res.v2;
	if( test_back_side( cb_r, sd1_res, res.side1, res.side0, v1, p_sub_v1, p, radius, vx1_res, vx2_res) )
		return;

	side_test_res sd2_res;
	sd2_res.vertex0_id  = res.v2;
	sd0_res.vertex1_id  = res.v0;
	if( test_back_side( cb_r, sd2_res,     side2, res.side1, v2, p_sub_v2, p, radius, vx2_res, vx0_res ) )
		return;
	
	//res.b_tri_sides_internal = true;

	res.use	= true;

	//////////////////////////////////////////////
	const float ax_mag = res.tri_ax.length(); //sqrt( sq_ax_mag );/////////
	//////////////////////////////////////////////
	ASSERT( ax_mag>0 );
	const float rcp_ax_mag = 1.f/ax_mag;
	xray::collision::contact_info	c;
	c.depth		= res.ax_dist * rcp_ax_mag + radius;
	c.normal	= res.tri_ax * rcp_ax_mag;
	c.position	= p	- c.normal * radius;
	//cb_r.cb( c );
	cb_r.update( c );
}

}// xray{
}// collision{
}// detail{
