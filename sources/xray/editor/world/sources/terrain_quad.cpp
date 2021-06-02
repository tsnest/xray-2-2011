////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_quad.h"
#include "terrain_object.h"

namespace xray {
namespace editor {

void terrain_vertex::set_default( )
{
	height			= 0.0f;
	t0_infl			= 255;
	t1_infl			= 0;
	t2_infl			= 0;
	t0_index		= 0;
	t1_index		= 1;
	t2_index		= 2;

	vertex_color	= math::color_rgba( 128u, 128, 128, 255 );

	tc_shift_x		= 0.f;
	tc_shift_z		= 0.f;
}

u8 terrain_vertex::get_texture_index( int layer )
{
	if(layer==0)
		return t0_index;
	else
	if(layer==1)
		return t1_index;
	else
		return t2_index;
}

void terrain_vertex::set_texture_index( int layer, u8 index )
{
	if(layer==0)
		t0_index = index;
	else
	if(layer==1)
		t1_index = index;
	else
		t2_index = index;
}

bool terrain_quad::setup(u16 id, terrain_node^ o)
{
	owner			= o;
	
	return super::setup( id, owner->m_vertices.m_dimension );
	//return collision_terrain_quad.setup( id, owner->m_dimension );

}

terrain_vertex^ terrain_quad::vertex(u16 index) 
{
	return owner->m_vertices.get(index);
}

// xz plane distance, height component is ignored
float terrain_quad::min_distance_to_sqr(float3 const& point)
{
	float result					= math::float_max;

	float const dist_xz_sqr_lt		= owner->distance_xz_sqr(index_lt, point);
	float const dist_xz_sqr_lb		= owner->distance_xz_sqr(index_lb, point);
	float const dist_xz_sqr_rt		= owner->distance_xz_sqr(index_rt, point);
	float const dist_xz_sqr_rb		= owner->distance_xz_sqr(index_rb, point);

	result 							= math::min(result, dist_xz_sqr_lt);
	result 							= math::min(result, dist_xz_sqr_lb);
	result 							= math::min(result, dist_xz_sqr_rt);
	result 							= math::min(result, dist_xz_sqr_rb);

	return 							result;
}

void terrain_quad::export_lines(xray::vectora<u16>& dest_indices, u32& idx)
{
	int	dim					= owner->m_vertices.m_dimension;

	int _y					= quad_id / dim;
	int _x					= quad_id - dim*_y;

	dest_indices[idx++]		= index_lt;
	dest_indices[idx++]		= index_rt;

	dest_indices[idx++]		= index_rt;
	dest_indices[idx++]		= index_lb;

	dest_indices[idx++]		= index_lb;
	dest_indices[idx++]		= index_lt;

	if(_x==dim-1)
	{
		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_rb;
	}
	if(_y==dim-1)
	{
		dest_indices[idx++]		= index_rb;
		dest_indices[idx++]		= index_lb;
	}

}

void terrain_quad::export_tris(xray::vectora<u16>& dest_indices, u32& idx, bool clockwise)
{
	if(clockwise)
	{
	// CW
		dest_indices[idx++]		= index_lb;
		dest_indices[idx++]		= index_lt;
		dest_indices[idx++]		= index_rt;

		dest_indices[idx++]		= index_lb;
		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_rb;
	}else
	{
	// CCW
		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_lt;
		dest_indices[idx++]		= index_lb;

		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_lb;
		dest_indices[idx++]		= index_rb;
	}
}

// collision use 32bit- based index
void terrain_quad::export_tris(xray::vectora<u32>& dest_indices, u32& idx, bool clockwise)
{
	if(clockwise)
	{
	// CW
		dest_indices[idx++]		= index_lb;
		dest_indices[idx++]		= index_lt;
		dest_indices[idx++]		= index_rt;

		dest_indices[idx++]		= index_lb;
		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_rb;
	}else
	{
	// CCW
		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_lt;
		dest_indices[idx++]		= index_lb;

		dest_indices[idx++]		= index_rt;
		dest_indices[idx++]		= index_lb;
		dest_indices[idx++]		= index_rb;
	}
}





bool terrain_quad::ray_test( float3 const& pos, float3 const& dir, float const max_distance, float& range )
{
	
	float3 v_lt = owner->position_l(index_lt);
	float3 v_rt = owner->position_l(index_rt);

	float3 v_lb = owner->position_l(index_lb);
	float3 v_rb = owner->position_l(index_rb);
	
	return collision::ray_test_quad( v_lt, v_rt, v_lb, v_rb, pos, dir, max_distance, range );
	
}

void terrain_quad::debug_draw(	render::debug_vertices_type& vertices, 
								render::debug_indices_type& indices, 
								u32 clr,
								math::float3 const& camera_pos )
{
	u16 rt_index_lb, rt_index_lt, rt_index_rt, rt_index_rb;
	render::vertex_colored	v;
	v.color.m_value			= clr;
	static float dist		= 0.3f;
	math::float3 p			= owner->position_g(index_lb);
	v.position				= p + (camera_pos-p).normalize() * dist;
	rt_index_lb				= (u16)vertices.size();
	vertices.push_back		( v );

	p						= owner->position_g(index_lt);
	v.position				= p + (camera_pos-p).normalize() * dist;
	rt_index_lt				= rt_index_lb+1;
	vertices.push_back		( v );

	p						= owner->position_g(index_rt);
	v.position				= p + (camera_pos-p).normalize() * dist;
	rt_index_rt				= rt_index_lb+2;
	vertices.push_back		( v );

	p						= owner->position_g(index_rb);
	v.position				= p + (camera_pos-p).normalize() * dist;
	rt_index_rb				= rt_index_lb+3;
	vertices.push_back		( v );

	indices.push_back		( rt_index_lb );
	indices.push_back		( rt_index_lt );
	indices.push_back		( rt_index_rt );

	indices.push_back		( rt_index_lb );
	indices.push_back		( rt_index_rt );
	indices.push_back		( rt_index_rb );
}

get_neighbour_result get_neighbour(	terrain_node^ src_node,
									u16 const origin_vert_idx, 
									int const& offset_x_idx, 
									int const& offset_z_idx,
									terrain_node^% result_node,
									u16& result_vert_idx )
{
	int dim			= src_node->m_vertices.m_dimension;

	int _x, _z;
	src_node->vertex_xz(origin_vert_idx, _x, _z);
	int x_new		= _x + offset_x_idx;
	int z_new		= _z + offset_z_idx;

	if((x_new<0) || (x_new>dim) || (z_new<0) || (z_new>dim))
	{
		terrain_node_key thiskey = src_node->m_tmp_key;

		if(x_new<0)
			thiskey.x	-=1;
		else
		if(x_new>dim)
			thiskey.x	+=1;

		if(z_new<0)
			thiskey.z	+=1;
		else
		if(z_new>dim)
			thiskey.z	-=1;

		if(src_node->get_terrain_core()->m_nodes.ContainsKey(thiskey))
		{
			result_node		= src_node->get_terrain_core()->m_nodes[thiskey];
			int t_dim		= result_node->m_vertices.m_dimension;
			ASSERT_CMP		( t_dim, ==, dim );
			int dx			= x_new;
			int dz			= z_new;

			if(x_new<0)
				dx = (t_dim - math::abs(x_new));
			else
			if(x_new>dim)
				dx = (x_new - dim);

			if(z_new<0)
				dz = (t_dim - math::abs(z_new));
			else
			if(z_new>dim)
				dz = (z_new-dim);

			result_vert_idx	= result_node->vertex_id(dx, dz);
			return			get_neighbour_result::ok;
		}else
			return			get_neighbour_result::missing;
	}


	result_vert_idx		= src_node->vertex_id(x_new, z_new);
	result_node			= src_node;
	
	return get_neighbour_result::ok;
}

} // namespace editor
} // namespace xray