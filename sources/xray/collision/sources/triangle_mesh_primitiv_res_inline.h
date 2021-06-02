////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_MESH_PRIMITIVE_INLINE_H_INCLUDED
#define TRIANGLE_MESH_PRIMITIVE_INLINE_H_INCLUDED

namespace xray {
namespace collision {
namespace detail{

inline	triangle_mesh_primitiv_res::triangle_mesh_primitiv_res( u32 _local_id, const u32 *ind ):
v0( ind[ 0 ] ), v1( ind[ 1 ] ), v2	( ind[ 2 ] ),
local_id(_local_id),
use( false ),
b_in_plane( false )
{
		r_vertex.use			= false;
		r_side.use				= false;
}

inline void base_precalculation( triangle_mesh_primitiv_res &res, const float3 &v0, const float3 &v1, const float3 &v2, const float3 &to_v )
{
	res.side0		= v1-v0	;
	res.side1		= v2-v1	;
	res.tri_ax		= res.side0^res.side1;
	res.ax_dist		=to_v|res.tri_ax;
	res.b_in_plane = res.ax_dist < 0;
}


inline  bool test_vertex( vx_test_res &res, const float3 &vx_to_point, const float3 &after_vx_side, const float3 &before_vx_side   )
{
	
	res.det_after_side			= vx_to_point|after_vx_side;
	res.b_det_after_side		= res.det_after_side > 0;
	res.det_before_side			=-vx_to_point|before_vx_side;
	res.b_det_before_side		= res.det_before_side > 0;
	res.use						= !res.b_det_after_side && !res.b_det_before_side;
	return res.use;
}

inline static bool test_side(	side_test_res& res, 
								const vx_test_res &v0_res, const vx_test_res &v1_res,
								float sq_side, float side_dot_before_side
								
							)
{
		float d = - sq_side	* v0_res.det_before_side -  side_dot_before_side *  v0_res.det_after_side; 
		res.use =		v0_res.b_det_after_side	&&	v1_res.b_det_before_side && d > 0;
		return res.use;
}

inline bool	trash_back( const triangle_mesh_primitiv_res &back_tri, const triangle_mesh_primitiv_res &tri, math::float3 const* verts, const float3& p, float4x4 const &mesh_transform )
{
	XRAY_UNREFERENCED_PARAMETER	( p );
	if(tri.b_in_plane)
		return false;

	const float3	v0 = verts[ tri.v0 ] * mesh_transform;
	const float3	v1 = verts[ tri.v1 ] * mesh_transform;
	const float3	v2 = verts[ tri.v2 ] * mesh_transform;

	return	( back_tri.tri_ax | v0 ) < back_tri.ax_dist ||
			( back_tri.tri_ax | v1 ) < back_tri.ax_dist ||
			( back_tri.tri_ax | v2 ) < back_tri.ax_dist  ;
}
template<typename vec_type>
inline bool	trash_back_by_all_res( const triangle_mesh_primitiv_res &back_tri, const vec_type &vec_res, math::float3 const* verts, const float3& p, float4x4 const &mesh_transform )
{
	typename vec_type::const_iterator	j	= vec_res.begin();
	typename vec_type::const_iterator 	je	= vec_res.end();
	for( ;j!=je; ++j )
		if( trash_back( back_tri, *j, verts, p, mesh_transform ) )
			return true;
	return false;
}

inline bool check_side( const side_test_res &sr, u32 v0, u32 v1 )
{
	if( !sr.use )
		return false;
	return
		( sr.vertex1_id == v0 && sr.vertex0_id == v1 ) ||
		( sr.vertex1_id == v1 && sr.vertex0_id == v0 ) ;
}

inline bool check_vertex( const vx_test_res	&vr, u32 v )
{
	if( !vr.use )
		return false;
	return	( vr.vertex_id == v );
}

template<typename vec_type>
inline bool check_neibour_sides( const vec_type & vec, 
						   u32 v0, u32 v1 )
{
	typename vec_type::const_iterator	j	= vec.begin();
	typename vec_type::const_iterator 	je	= vec.end()-1;
	for( ;j!=je; ++j )
		if( check_side((*j).r_side, v0, v1 ) ) 
			return true;
	return false;
}

template<typename vec_type>
inline bool check_neibour_verts( const vec_type &vec, 
						   u32 v )
{
	typename vec_type::const_iterator	j	= vec.begin();
	typename vec_type::const_iterator 	je	= vec.end()-1;
	for( ;j!=je; ++j )
		if( check_vertex((*j).r_vertex, v ) ) 
			return true;
	return false;
}


}// xray{
}// collision{
}// detail{
#endif // #ifndef TRIANGLE_MESH_PRIMITIVE_INLINE_H_INCLUDED