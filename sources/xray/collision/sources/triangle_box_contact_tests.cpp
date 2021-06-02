////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangle_box_contact_tests.h"
#include "box_geometry.h"
#include <xray/math_utils.h>

namespace xray {
namespace collision {
namespace detail{

struct	box_ax:
private	boost::noncopyable
{
	const float3	&ax;
	float			side;
	float			tri_ax_proj;
	bool			b_tri_ax_proj_sign;
	float			f_tri_ax_proj_sign;
	float			tri_ax_positiv_proj;
	float			tri_ax_radius;

	float			tri_ax_side_proj;
	float3			tri_ax_positiv_box_side;
inline		box_ax(	const float3	&a, float sd, const float3	&tri_ax ): ax( a ), side( sd ) 
	{
		tri_ax_proj				= tri_ax|a;
		b_tri_ax_proj_sign		= tri_ax_proj > 0;
		f_tri_ax_proj_sign		= b_tri_ax_proj_sign ? 1.f: -1.f;
		tri_ax_positiv_proj		= b_tri_ax_proj_sign ? tri_ax_proj: -tri_ax_proj;
		tri_ax_radius			= tri_ax_positiv_proj * sd;
	}

inline	void	calc_side_prj()
{
	tri_ax_side_proj	= f_tri_ax_proj_sign * side;

	tri_ax_positiv_box_side = ax * f_tri_ax_proj_sign;
}

};

struct box_ax_one_vert_below:
	boost::noncopyable
{
	const box_ax	&ax;
	box_ax_one_vert_below(const box_ax	&_ax):ax(_ax),init( false ){}
	bool	init;
	bool	b_to_dir;
	float	vert_det;
	inline void	init_to_dir( const float3 &dir, u8 &to_dir_num, const box_ax_one_vert_below*	to_dir[2],
								u8 &from_dir_num, const box_ax_one_vert_below*	from_dir[2], u8	&init_num ) 
	{
		init = true;
		ASSERT( init_num<3 );
		++init_num;
		vert_det	= ax.tri_ax_positiv_box_side|dir;
		if( vert_det<0 )
		{
			b_to_dir = true;
			ASSERT( to_dir_num<2 );
			to_dir[to_dir_num] = this;
			++to_dir_num;
		} else
		{
			b_to_dir = false;
			ASSERT( to_dir_num<2 );
			from_dir[from_dir_num] = this;
			++from_dir_num;
		}
	}

};


struct box_ax_get_to_dir
{
	const box_ax_one_vert_below*	to_dir[2];
	u8		num_to_dir;

	const box_ax_one_vert_below*	from_dir[2];
	u8		num_from_dir;
	u8		num_init;

	box_ax_get_to_dir( const float3 &dir, box_ax_one_vert_below	&a0, box_ax_one_vert_below	&a1, box_ax_one_vert_below	&a2 ):
		num_to_dir(0),num_from_dir(0), num_init(0)
	{
		a0.init_to_dir( dir, num_to_dir, to_dir, num_from_dir, from_dir, num_init );
		a1.init_to_dir( dir, num_to_dir, to_dir, num_from_dir, from_dir, num_init );
		a2.init_to_dir( dir, num_to_dir, to_dir, num_from_dir, from_dir, num_init );
	}
};

void line_contact( const float3 &cross, float cross_mag, float scaled_cross_depth, on_contact& cb )
{
		contact_info c;
		//float mag = cross.length();
		ASSERT(cross_mag>0);
		float rp_mag = 1.f/cross_mag;
		c.depth = scaled_cross_depth * rp_mag;
		c.normal= cross * rp_mag;
		ASSERT(0);//define point
		cb(c);
}

bool test_one_bvert_below_vertex( on_contact& cb, vx_test_res &v0_res, const float3 &boxv0_sub_v0,const float3 &v0, const box_ax &x_ax, const box_ax &y_ax, const box_ax &z_ax,  const float3	&after_side,  const float3	&before_side )
{
		
	if( !test_vertex( v0_res, boxv0_sub_v0, before_side, after_side ) )
		return false;

	
	box_ax_one_vert_below	lx_ax( x_ax ); 
	box_ax_one_vert_below	ly_ax( y_ax ); 
	box_ax_one_vert_below	lz_ax( z_ax );
	box_ax_get_to_dir tovert( -boxv0_sub_v0, lx_ax, ly_ax, lz_ax );
	ASSERT( tovert.num_to_dir == 1 || tovert.num_to_dir ==2 );
	if( tovert.num_to_dir == 2 )
	{
		ASSERT( tovert.num_from_dir == 1 );
		ASSERT( tovert.num_to_dir == 2 );
		contact_info c;
		c.depth = -tovert.from_dir[0]->ax.tri_ax_positiv_box_side|boxv0_sub_v0;
		if( c.depth >0 )
		{
			c.normal = tovert.from_dir[0]->ax.tri_ax_positiv_box_side;
			c.position = v0;
			//side point ?
			cb(c);
		}
		return true;
	} else { //if( tovert.num_to_vert == 1 )
		ASSERT( tovert.num_to_dir == 1 );
		ASSERT( tovert.num_from_dir == 2 );
		const float3 cross0 = tovert.to_dir[0]->ax.tri_ax_positiv_box_side^before_side;//+-?
		float depth_scaled0 = -cross0|boxv0_sub_v0;
		//contact_info c0;
		if( depth_scaled0 < 0 )
			return true;
		const float3 cross1 = -tovert.to_dir[0]->ax.tri_ax_positiv_box_side^after_side;//+-?
		const float depth_scaled1 = -cross1|boxv0_sub_v0;
		if( depth_scaled1 < 0 )
			return true;
		
		const float sq_mag0 = cross0.squared_length();
		const float sq_mag1 = cross1.squared_length();

		if( math::sqr( depth_scaled0 ) * sq_mag1 < math::sqr( depth_scaled1 ) * sq_mag0 )
		{
			ASSERT( sq_mag0 > 0 );
			float mag = sqrt( sq_mag0 );
			line_contact( cross0, mag, depth_scaled0, cb );
		} else {
			ASSERT( sq_mag1 > 0 );
			float mag = sqrt( sq_mag1 );
			line_contact( cross1, mag, depth_scaled1, cb );
		}	
		return true;
	}	
	
}


bool line_contact_sep( const float3 &cross, const float3 &sub_points, on_contact& cb )
{
		float	scaled_cross_depth = cross|sub_points;
		if( scaled_cross_depth < 0 )
			return true;
		float mag = cross.length();
		line_contact( cross, mag, scaled_cross_depth, cb );
		//c.position = 
		return false;
}

bool test_one_bvert_below_side( on_contact& cb, vx_test_res &v0_res, vx_test_res &v1_res, const float3 &box_v, const float3 &v0, const float3 &v1, const float3 &side, const float3 &before_side, const box_ax &x_ax, const box_ax &y_ax, const box_ax &z_ax )
{
	XRAY_UNREFERENCED_PARAMETER	( v1 );

	side_test_res side_res;
	const float sq_side = side.squared_length();
	const float side_dot_before_side = side|before_side;
	if( !test_side( side_res, v0_res, v1_res, sq_side, side_dot_before_side  ) )
		return false;
	
	const float		rsp_sq_side = 1.f/sq_side;
	const float3	v0_to_line_point = side * ( v0_res.det_after_side * rsp_sq_side );
	const float3	side_p = v0 + v0_to_line_point;
	const float3	boxv_sub_side_p = box_v - side_p;

	box_ax_one_vert_below	lx_ax( x_ax ); 
	box_ax_one_vert_below	ly_ax( y_ax ); 
	box_ax_one_vert_below	lz_ax( z_ax );
	box_ax_get_to_dir toside( -boxv_sub_side_p, lx_ax, ly_ax, lz_ax );
	ASSERT( toside.num_to_dir == 1 || toside.num_to_dir ==2 );
	if( toside.num_to_dir == 2 )
	{		
//side separation
		ASSERT( toside.num_from_dir == 1 );
		const float3 boxv_v_sub_v0 = box_v - v0;
		//const float3 boxv_v_sub_v1 = box_v - v1;
		
		float v0_depth = -(toside.from_dir[0]->ax.tri_ax_positiv_box_side|boxv_v_sub_v0);
		float d0_dot_side = toside.from_dir[0]->ax.tri_ax_positiv_box_side|side; 
		if( v0_depth < 0.f	&& d0_dot_side< 0.f )
			return true;
		//float v1_depth = -(toside.from_dir[0]->ax.tri_ax_positiv_box_side|boxv_v_sub_v1);
//////////////////////////////////////////////////////////////////////////////
		const box_ax_one_vert_below *side_nearest = 0;
		const box_ax_one_vert_below *side_not_nearest = 0;
		if(toside.to_dir[0]->vert_det > toside.to_dir[1]->vert_det)
		{
			side_nearest = toside.to_dir[0];
			side_not_nearest = toside.to_dir[1];
		} else
		{
			side_nearest = toside.to_dir[1];
			side_not_nearest = toside.to_dir[0];
		}

		const float3 nearest_cross = side_nearest->ax.tri_ax_positiv_box_side^side;//+-?
		if( line_contact_sep(nearest_cross, boxv_v_sub_v0, cb ) )
			return true;



		const float3 not_nearest_cross = side_not_nearest->ax.tri_ax_positiv_box_side^side;//+-?
		line_contact_sep(not_nearest_cross, boxv_v_sub_v0, cb );



		//other points ?

		//if( !v0_sep && !v1_sep )
		//{
		//	bool v0_sd0_sep = -(boxv_v_sub_v0|toside.to_dir[0]->ax.tri_ax_positiv_box_side) < 0;
		//	if( v0_sd0_sep )
		//	{

		//	}

		//	bool v0_sd1_sep = -(boxv_v_sub_v0|toside.to_dir[1]->ax.tri_ax_positiv_box_side) < 0;
			
			
			


		//}

		//if( toside.to_dir[0]->vert_det >  )
	}
	
	return true;

}

void	triangle_box_contact_tests::test( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, float4x4 const &mesh_transform )
{
/////////////////////////////////////////////////////////////////////////////////////	
	u32	local_triangle_id	= u32( res_vec.size() );
	triangle_box_res r( local_triangle_id, ind );
	res_vec.push_back( r );
	triangle_box_res &res = res_vec.back();
	const float3	v0 = verts[ res.v0 ] * mesh_transform, v1 = verts[ res.v1 ] * mesh_transform, v2 = verts[ res.v2 ] * mesh_transform;
	const float3	&p	= m_transform.c.xyz();

	const	float3	p_sub_v0	=p-v0;
	base_precalculation( res, v0, v1, v2, p_sub_v0 ); 
	if( res.b_in_plane )
	{
		backs.push_back( local_triangle_id );
		return;
	}
/////////////////////////////////////////////////////////////////////////////////////
	
	const	float3	&box_ax0 = m_transform.i.xyz();
	const	float3	&box_ax1 = m_transform.j.xyz();
	const	float3	&box_ax2 = m_transform.k.xyz();
	
	const	float3	&half_sides = m_geometry.half_sides();
	box_ax	x_ax( box_ax0, half_sides.x, res.tri_ax );
	box_ax	y_ax( box_ax1, half_sides.y, res.tri_ax );
	box_ax	z_ax( box_ax2, half_sides.z, res.tri_ax );

	float	tri_ax_radius = x_ax.tri_ax_radius + y_ax.tri_ax_radius + z_ax.tri_ax_radius;
	
	//float	tri_ax_depth = 
	res.out	= res.ax_dist > tri_ax_radius;

	if( res.out )
		return;
	x_ax.calc_side_prj();
	y_ax.calc_side_prj();
	z_ax.calc_side_prj();
	
	math::float_sorted<box_ax> box_ax_sorted( x_ax, y_ax, z_ax, x_ax.tri_ax_radius, y_ax.tri_ax_radius, z_ax.tri_ax_radius );	

	if( res.ax_dist > tri_ax_radius - box_ax_sorted.r2().tri_ax_radius )
	{	//one box vert below tri
		//
		float3	lbox_v0	= p - x_ax.tri_ax_positiv_box_side - y_ax.tri_ax_positiv_box_side - z_ax.tri_ax_positiv_box_side;
		
		const float3	boxv0_sub_v0 = lbox_v0 - v0;
		const float3	side2 = v0 - v2;
		vx_test_res v0_res; v0_res.vertex_id = res.v0;
		if( test_one_bvert_below_vertex( cb, v0_res,boxv0_sub_v0, v0, x_ax, y_ax, z_ax, res.side0, side2 ) )
			return;

		const float3	boxv0_sub_v1 = lbox_v0 - v1;
		vx_test_res v1_res; v1_res.vertex_id = res.v1;
		if( test_one_bvert_below_vertex( cb, v1_res,boxv0_sub_v1, v1, x_ax, y_ax, z_ax, res.side1, res.side0 ) )
			return;
	
		const float3	boxv0_sub_v2 = lbox_v0 - v2;
//		vx_test_res v2_res; v2_res.vertex_id = res.v2;
		if( test_one_bvert_below_vertex( cb, v1_res,boxv0_sub_v2, v2, x_ax, y_ax, z_ax, side2, res.side1 ) )
			return;
		


	}

	
	//float3	lbox_v1 = lbox_v0 + box_ax_sorted.r2().tri_ax_positiv_box_side * 2;


//	sort by tri ax
	
	//if( side_prg0 )
	

}

//void	triangle_box_contact_tests::back_test			( result &back_tri, float3 const* verts )
//{
//
//}

}//xray{
}//collision{
}//detail{
