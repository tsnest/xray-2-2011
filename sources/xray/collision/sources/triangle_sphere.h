////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.03.2008
//	Author		: Konstantin Slipchenko
//	Description : sphere - single triangle tests
////////////////////////////////////////////////////////////////////////////
#ifndef TRIANGLE_SPHERE_H_INCLUDED
#define TRIANGLE_SPHERE_H_INCLUDED

#include "triangle_mesh_primitiv_res.h"

namespace xray {
namespace collision {
	struct on_contact;

namespace detail{


struct triangle_sphere_res: triangle_mesh_primitiv_res
{
	//xray::collision::on_contact		*cb;
	//u32								v0, v1, v2;
	//u32								local_id;
	//bool	out;
	//bool	use;
	//bool	b_in_plane;
	bool	b_tri_sides_internal;
	



	//xray::collision::contact_info	c;

	inline	triangle_sphere_res( u32 _local_id, const u32 *ind ): triangle_mesh_primitiv_res( _local_id, ind )
	{

	}
};



typedef buffer_vector<triangle_sphere_res> tri_sphere_results;

struct triangle_sphere_test_callback:
	public boost::noncopyable
{
	on_contact&		cb;
	back_res		back_res;
	tri_sphere_results &res_vec;
	inline	triangle_sphere_test_callback(on_contact& _cb, tri_sphere_results &_res_vec ):
				cb( _cb ), back_res( _cb ), res_vec( _res_vec ){}
};

void triangle_sphere_test(	triangle_sphere_test_callback &cb_r,  buffer_vector<u32> &back_vec, 
							const u32 *	ind, math::float3 const* verts,
							const float3 &p, float radius, float4x4 const& mesh_transform );

}// xray{
}// collision{
}// detail{

#include "triangle_sphere_inline.h"
#endif