////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_MESH_PRIMITIV_RES_H_INCLUDED
#define TRIANGLE_MESH_PRIMITIV_RES_H_INCLUDED
#include <xray/collision/contact_info.h>
namespace xray {
namespace collision {
namespace detail{

struct vx_test_res
{
	bool	use;
	float	det_after_side;
	bool	b_det_after_side;
	float	det_before_side;
	bool	b_det_before_side;
	u32		vertex_id;

//	inline	vx_test_res( u32 id ): 
//	use( false ), vertex_id( id ){}
};



struct side_test_res
{
	bool	use;
	u32		vertex0_id;
	u32		vertex1_id;
//	side_test_res( u32 iv0, u32 iv1 ): use( false ), vertex0_id(iv0), vertex1_id( iv1 )
//	{}
};


struct triangle_mesh_primitiv_res 
{
public:
	
	inline	triangle_mesh_primitiv_res( u32 _local_id, const u32 *	ind );
	u32				v0, v1, v2;
	u32				local_id;
	bool			out;
	bool			use;
	bool			b_in_plane;
	float3			side0;
	float3			side1;
	float3			tri_ax;
	float			ax_dist;
	vx_test_res		r_vertex;
	side_test_res	r_side;
private:

}; // class triangle_mesh_primitiv_res




struct back_res:
	boost::noncopyable
{
	bool			b_back;
	contact_info	back;
	on_contact		&cb;
	inline void			update( const contact_info & c )
	{
	//cb( c );
		if( !b_back )
		{
			back = c;
			return;
		}
		if( back.depth > c.depth )
			back = c;
	}
	back_res(on_contact& _cb):b_back(false),cb (_cb) {}
	inline ~back_res()
	{ 
		if( b_back )
			cb( back ); 
	}
};

void triangle_sphere_back_test(	back_res &cb_r, 
							    triangle_mesh_primitiv_res &res, 
								math::float3 const* verts,
								const float3 &p, float radius, const float4x4 &mesh_transform );

}// xray{
}// collision{
}// detail{

#include "triangle_mesh_primitiv_res_inline.h"

#endif // #ifndef TRIANGLE_MESH_PRIMITIV_RES_H_INCLUDED