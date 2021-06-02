////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_light.h"
#include "project_items.h"

#pragma managed( push, off )
#include <xray/render/facade/debug_renderer.h>
#include <xray/geometry_utils.h>
#include <xray/geometry_primitives.h>
#pragma managed( pop )

namespace xray {
namespace editor {

u16 cross_templ[] = {0,1, 0,2, 0,3};
u16 border_templ[] = {1,5, 6,14, 9,13, 2,10,  3,7, 7,15, 15,11,11,3};

u16 border_side_templ[] = {1,1+16, 5,5+16, 6,6+16, 14,14+16, 13,13+16, 9,9+16, 10,10+16, 2,2+16};

float3 arc_vertices[] ={
	float3(1.000000f, 	0.0f,   0.0f),
	float3(0.923880f, 	0.0f,   0.382683f),
	float3(0.707107f, 	0.0f,   0.707107f),
	float3(0.382683f,	0.0f,   0.923880f),
	float3(0.0f,		0.0f,   1.0f)
};
u16 arc_indices[] = {0,1, 1,2, 2,3, 3,4 };

void fill_indices_template (render::debug_indices_type& indices, u16* templ, u16 size, u16 offset )
{
	for(u16 i=0; i<size; ++i)
		indices.push_back	(templ[i]+offset);
}

void add_border_arc( render::debug_vertices_type& vertices, 
					render::debug_indices_type& indices, 
					math::float3 const& offset, 
					math::float4x4 const& local_tr, 
					math::float4x4 const& world_tr,
					math::color const& clr)
{
	render::vertex_colored		v;
	v.color						= clr;
	math::float3				p;
	u16 vcount					= (u16)array_size(arc_vertices);

	u16 start_idx				= (u16)vertices.size();

	for(u16 i=0; i<vcount; ++i)
	{
		p							= offset + (arc_vertices[i] * local_tr );
		v.position					= p * world_tr;
		vertices.push_back			( v );
	}
	fill_indices_template			( indices, arc_indices, (u16)array_size(arc_indices), start_idx );
}

void draw_corner_cross( render::debug_vertices_type& vertices, 
						render::debug_indices_type& indices, 
						float3 const& p, 
						float3 const& first_dir, 
						float3 const& second_dir, 
						float3 const& third_dir, 
						float4x4 const& world_tr,
						math::color const& clr )
{
	render::vertex_colored		v;
	v.color						= clr;

	u16 start_idx				= (u16)vertices.size();
	v.position					= p * world_tr;
	vertices.push_back			( v );

	v.position					= (p + first_dir) * world_tr;
	vertices.push_back			( v );

	v.position					= (p + second_dir) * world_tr;
	vertices.push_back			( v );

	v.position					= (p + third_dir) * world_tr;
	vertices.push_back			( v );

	fill_indices_template		( indices, cross_templ, (u16)array_size(cross_templ), start_idx );
}

void object_light::draw_octahedron( )
{
	geometry_utils::geom_vertices_type	vertices(g_allocator);
	geometry_utils::geom_indices_type	indices(g_allocator);
	
	geometry_utils::create_octahedron	( vertices, indices, create_translation(get_position() ), float3(0.1f, 0.1f, 0.1f) );
	u32 vcount 							= vertices.size();
	u32 icount 							= indices.size();
	xray::render::debug_vertices_type	draw_vertices(g_allocator);
	draw_vertices.resize				( vcount );
	xray::render::debug_indices_type	draw_indices(g_allocator);
	draw_indices.resize					( icount );
	
	math::color clr(200,200,0,130);

	for(u32 i=0; i<vcount; ++i)
	{
		xray::render::vertex_colored& v = draw_vertices[i];
		v.position						= vertices[i];
		v.color							= clr;
	}

	for(u32 i=0; i<icount; ++i)
	{
		draw_indices[i] = indices[i];
	}

	get_debug_renderer().draw_triangles	( *m_scene, draw_vertices, draw_indices );

	math::color clr_line(200,200,0,255);
	for(u32 i=0; i<vcount; ++i)
	{
		xray::render::vertex_colored& v = draw_vertices[i];
		v.color							= clr_line;
	}


	icount 							= geometry_utils::octahedron_solid::pair_count*2;
	draw_indices.resize				( icount );

	for(u32 i=0; i<icount; ++i)
	{
		draw_indices[i] = geometry_utils::octahedron_solid::pairs[i];
	}

	get_debug_renderer().draw_lines		( *m_scene, draw_vertices, draw_indices );
}

void object_light::render( )
{
	super::render				( );
	//get_debug_renderer().draw_cross(m_transform->c.xyz(), 0.3f, math::color(0xffffff00) ); // center
	
	draw_octahedron				( );

	bool bsel					= m_project_item->get_selected();
	math::color const clr_body	= (bsel) ? math::color(255,255,0) : math::color(0,200,0);
	math::color const clr_range	= (bsel) ? math::color(255,0,0) : math::color(0,150,0);
	switch(m_type)
	{
 		case render::light_type_parallel:
 			{
				// Draw arrow.
				math::float3 const direction	 = math::normalize((*m_transform).k.xyz());
				math::float3 const right_offset  = math::normalize((*m_transform).i.xyz())*0.125f;
				math::float3 const up_offset	 = math::normalize((*m_transform).j.xyz())*0.125f;
				
				math::float3 const light_begin = (*m_transform).c.xyz();
				math::float3 const light_end   = (*m_transform).c.xyz() + direction * 2.0f;
				
 				get_debug_renderer().draw_line( *m_scene, light_begin, light_end, clr_range);
				
				math::float3 const arrows_start = light_end - direction * 0.25f;
				get_debug_renderer().draw_line( *m_scene, arrows_start + right_offset, light_end, clr_range);
				get_debug_renderer().draw_line( *m_scene, arrows_start - right_offset, light_end, clr_range);
				get_debug_renderer().draw_line( *m_scene, arrows_start + up_offset, light_end, clr_range);
				get_debug_renderer().draw_line( *m_scene, arrows_start - up_offset, light_end, clr_range);
				
				// Draw sphere.
				math::float4x4 m	= math::create_translation((*m_transform).c.xyz());
				get_debug_renderer().draw_line_ellipsoid( *m_scene, m, clr_body );
				
 				break;
 			}
		case render::light_type_point:
			{ 
				if(bsel)// draw light range only in selected state
				{
					math::float4x4 m	= math::create_translation((*m_transform).c.xyz());
					set_scale_impl		( float3(m_range, m_range, m_range), m );
					get_debug_renderer().draw_line_ellipsoid( *m_scene, m, clr_range );
				}
			}break;

		case render::light_type_spot:
			{
				if(bsel)// draw light range only in selected state
				{
					math::float4x4 m	= *m_transform;
					set_scale_impl		( float3(1.0f, 1.0f, 1.0f ), m );


					float const penumbra_radius		= m_range * math::tan(m_spot_penumbra_angle/2.0f);
					float3 const penumbra_cone_size	( penumbra_radius, m_range, penumbra_radius);
					get_debug_renderer().draw_cone	( *m_scene, m, penumbra_cone_size, clr_range );

					float const umbra_radius		= m_range * math::tan(m_spot_umbra_angle/2.0f);
					float3 const umbra_cone_size	( umbra_radius, m_range, umbra_radius);
					get_debug_renderer().draw_cone	( *m_scene, m, umbra_cone_size, math::color(255,0,255) );
				}
			}break;

		case render::light_type_obb:
			{
				
				math::float4x4 m		= *m_transform;
				math::float3 box_size	= get_scale();
				set_scale_impl			( float3(1.0f, 1.0f, 1.0f ), m );
			
				//// draw light body
				//get_debug_renderer().draw_obb(m, box_size, clr_body );

			if(bsel)// draw light range only in selected state
			{
				//draw range
				render::debug_vertices_type		vertices(g_allocator);
				render::debug_indices_type		indices(g_allocator);
				render::vertex_colored			v;
				math::float3					p;
				v.color							= clr_range;
				math::float3 up(0,m_range,0), down(0,-m_range,0), left(-m_range,0,0), right(m_range,0,0), fw(0,0,m_range),bk(0,0,-m_range);

				p							= float3(box_size.x, box_size.y, box_size.z);
				draw_corner_cross			( vertices, indices, p, up, right, fw, m, clr_range );

				p							= float3(-box_size.x, box_size.y, box_size.z);
				draw_corner_cross			( vertices, indices, p, up, left, fw, m, clr_range );

				p							= float3(box_size.x, -box_size.y, box_size.z);
				draw_corner_cross			( vertices, indices, p, down, right, fw, m, clr_range );

				p							= float3(-box_size.x, -box_size.y, box_size.z);
				draw_corner_cross			( vertices, indices, p, down, left, fw, m, clr_range );

				p							= float3(box_size.x, box_size.y, -box_size.z);
				draw_corner_cross			( vertices, indices, p, up, right, bk, m, clr_range );

				p							= float3(-box_size.x, box_size.y, -box_size.z);
				draw_corner_cross			( vertices, indices, p, up, left, bk, m, clr_range );

				p							= float3(box_size.x, -box_size.y, -box_size.z);
				draw_corner_cross			( vertices, indices, p, down, right, bk, m, clr_range );

				p							= float3(-box_size.x, -box_size.y, -box_size.z);
				draw_corner_cross			( vertices, indices, p, down, left, bk, m, clr_range );

				// border bound lines
				fill_indices_template		( indices, border_templ, (u16)array_size(border_templ), 0 );
				fill_indices_template		( indices, border_templ, (u16)array_size(border_templ), 16 );

				fill_indices_template		( indices, border_side_templ, (u16)array_size(border_side_templ), 0 );

				// border bound arc
				math::float4x4 range_scale = math::create_scale( math::float3(m_range, m_range, m_range));

				// near +x +y +z
				math::float3 offset			= float3(box_size.x, box_size.y, box_size.z);
				//xy
				math::float4x4 local_tr		= range_scale * math::create_rotation_x( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale;
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//yz
				local_tr					= range_scale * math::create_rotation_z( -math::pi_d2);;
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );


				// near -x +y +z
				offset						= float3(-box_size.x, box_size.y, box_size.z);
				//xy
				local_tr					= range_scale * math::create_rotation_y( math::pi_d2) * math::create_rotation_x( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale * math::create_rotation_y( math::pi_d2);;
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//yz
				local_tr					= range_scale * math::create_rotation_z( -math::pi_d2);;
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );


				// near +x -y +z
				offset						= float3(box_size.x, -box_size.y, box_size.z);
				//xy
				local_tr					= range_scale * math::create_rotation_x( -math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale;
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//yz
				local_tr					= range_scale * math::create_rotation_z( math::pi_d2);;
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );

				// near -x -y +z
				offset						= float3(-box_size.x, -box_size.y, box_size.z);

				//xy
				local_tr					= range_scale * math::create_rotation_y( math::pi_d2) * math::create_rotation_x( -math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale * math::create_rotation_y( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//!yz
				local_tr					= range_scale * math::create_rotation_z( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );

				// near +x +y -z
				offset						= float3(box_size.x, box_size.y, -box_size.z);
				//xy
				local_tr					= range_scale * math::create_rotation_x( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale * math::create_rotation_x( math::pi);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//yz
				local_tr					= range_scale * math::create_rotation_z( -math::pi_d2) * math::create_rotation_y( math::pi );
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );


				// near -x +y -z
				offset						= float3(-box_size.x, box_size.y, -box_size.z);

				//xy
				local_tr					= range_scale * math::create_rotation_y( math::pi_d2) * math::create_rotation_x( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale * math::create_rotation_y( math::pi );
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				////yz
				local_tr					= range_scale * math::create_rotation_y( -math::pi_d2)* math::create_rotation_z( -math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );


				// near +x -y -z
				offset						= float3(box_size.x, -box_size.y, -box_size.z);
				//xy
				local_tr					= range_scale * math::create_rotation_x( -math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale * math::create_rotation_x( math::pi);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//yz
				local_tr					= range_scale * math::create_rotation_x( math::pi) * math::create_rotation_z( math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );

				// near -x -y -z
				offset						= float3(-box_size.x, -box_size.y, -box_size.z);

				//xy
				local_tr					= range_scale * math::create_rotation_x( -math::pi_d2) * math::create_rotation_y( math::pi);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//xz
				local_tr					= range_scale * math::create_rotation_y( math::pi);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );
				//!yz
				local_tr					= range_scale * math::create_rotation_x( -math::pi_d2) * math::create_rotation_y( -math::pi_d2);
				add_border_arc				( vertices, indices, offset, local_tr, m, clr_range );

				get_debug_renderer().draw_lines( *m_scene, vertices, indices );
			}//if(bsel)

			}break;

		case render::light_type_capsule:
			{
				math::float4x4 m			= *m_transform;
				set_scale_impl				( float3(1.0f, 1.0f, 1.0f ), m );

				math::float3 cylinder_size	= 2.f*get_scale();
				//get_debug_renderer().draw_line_capsule(m, cylinder_size, clr_body );
				
				if(bsel)
					get_debug_renderer().draw_line_capsule( *m_scene, m, float3(cylinder_size.x+m_range, cylinder_size.y, cylinder_size.z+m_range), clr_range );

			}break;
		case render::light_type_sphere : {
			if(bsel)// draw light range only in selected state
			{
				{
					math::float4x4 m	= math::create_translation((*m_transform).c.xyz());
					set_scale_impl		( float3(m_range, m_range, m_range) + m_transform->get_scale(), m );
					get_debug_renderer().draw_line_ellipsoid( *m_scene, m, clr_range );
				}
				{
					math::float4x4 m	= math::create_translation((*m_transform).c.xyz());
					set_scale_impl		( m_transform->get_scale(), m );
					get_debug_renderer().draw_line_ellipsoid( *m_scene, m, clr_range );
				}
			}
			break;
		}
		case render::light_type_plane_spot : {
			if ( !bsel )
				break;

			float3 scale					= m_transform->get_scale( );

			get_debug_renderer().draw_rectangle( *m_scene, *m_transform, float3(1.f, 1.f, 1.f), clr_range );

			float4x4 temp					= *m_transform;
			temp.c.xyz()					= m_transform->c.xyz() - normalize(m_transform->j.xyz())*range;

			float const range_X_tan_penumbra_angle_div_2 = range*math::tan(m_spot_penumbra_angle/2.f);
			temp.set_scale					( float3( range_X_tan_penumbra_angle_div_2 + scale.x, 1.f, range_X_tan_penumbra_angle_div_2 + scale.z ) );
			get_debug_renderer().draw_rectangle( *m_scene, temp, float3(1.f, 1.f, 1.f), clr_range );

			float3 const first_corners[]	= {
				m_transform->transform_position( float3(-1.f, 0.f, -1.f) ),
				m_transform->transform_position( float3(-1.f, 0.f, +1.f) ),
				m_transform->transform_position( float3(+1.f, 0.f, +1.f) ),
				m_transform->transform_position( float3(+1.f, 0.f, -1.f) ),
			};
			float3 second_corners[]	= {
				temp.transform_position( float3(-1.f, 0.f, -1.f) ),
				temp.transform_position( float3(-1.f, 0.f, +1.f) ),
				temp.transform_position( float3(+1.f, 0.f, +1.f) ),
				temp.transform_position( float3(+1.f, 0.f, -1.f) ),
			};

			for ( u32 i=0; i<4; ++i )
				get_debug_renderer().draw_line	( *m_scene, first_corners[i], second_corners[i], clr_range );

			float const range_X_tan_umbra_angle_div_2 = range*math::tan(m_spot_umbra_angle/2.f);
			temp.set_scale					( float3( range_X_tan_umbra_angle_div_2 + scale.x, 1.f, range_X_tan_umbra_angle_div_2 + scale.z ) );
			get_debug_renderer().draw_rectangle( *m_scene, temp, float3(1.f, 1.f, 1.f), clr_body );

			float3 third_corners[]	= {
				temp.transform_position( float3(-1.f, 0.f, -1.f) ),
				temp.transform_position( float3(-1.f, 0.f, +1.f) ),
				temp.transform_position( float3(+1.f, 0.f, +1.f) ),
				temp.transform_position( float3(+1.f, 0.f, -1.f) ),
			};

			for ( u32 i=0; i<4; ++i )
				get_debug_renderer().draw_line	( *m_scene, first_corners[i], third_corners[i], clr_body );

			break;
		}
		default:
			UNREACHABLE_CODE();
	}
}

} // namespace editor
} // namespace xray