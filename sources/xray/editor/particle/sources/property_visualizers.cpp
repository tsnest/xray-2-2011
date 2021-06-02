////////////////////////////////////////////////////////////////////////////
//	Created		: 12.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_visualizers.h"
#include <xray/render/facade/debug_renderer.h>

namespace xray{
namespace particle_editor{

property_visualizer_domain::property_visualizer_domain( configs::lua_config_value const& v )
:m_cfg( NEW(configs::lua_config_value)(v))
{}

property_visualizer_domain::~property_visualizer_domain()
{
	DELETE(m_cfg);
}

void property_visualizer_domain::draw( math::float4x4 const& world_matrix, xray::render::debug::renderer& r, render::scene_pointer scene )
{
	if (!(bool)((*m_cfg)["DrawGizmo"]))
		return;

	pcstr type							= (*m_cfg)["DomainType"]["selected_value"];
	configs::lua_config_value curr		= (*m_cfg)["DomainType"][type];



	color clr(255u,0u,0u,255u);//rgba
	color clr_solid(255u,0u,0u,80u);
	math::float3 const scale = (*m_cfg)["Scale"];
	math::float3 const rotate = (*m_cfg)["Rotate"];
	math::float3 const translate = (*m_cfg)["Translate"];
	math::float4x4 const m = math::create_scale(scale) * math::create_rotation(rotate) * math::create_translation(translate);

	if(0==strings::compare(type, "Point"))
	{
	}else
	if(0==strings::compare(type, "Line"))
	{
		float line_length = curr["Length"];
		r.draw_line( render::scene_ptr(scene), float3(-line_length/2, 0,0)*m*world_matrix,float3(line_length/2, 0,0)*m*world_matrix, clr);
	}else
	if(0==strings::compare(type, "Triangle"))
	{	
// 		float side_A = curr["SideLengthA"];
// 		float side_B = curr["SideLengthB"];
// 		float side_C = curr["SideLengthC"];
// 		float p3x = (math::sqr(side_B) + math::sqr(side_A) - math::sqr(side_C))/(2*side_A);
// 		float p3y = math::sqrt(math::sqr(side_B) - math::sqr(p3x));
		math::float3 point_1 = math::float3(curr["PointA"]);//math::float3(0,0,0);
		math::float3 point_2 = math::float3(curr["PointB"]);//math::float3(0,side_A,0);
		math::float3 point_3 = math::float3(curr["PointC"]);//math::float3(p3x,p3y,0);
		//r.draw_line(point_1*m*world_matrix,point_3*m*world_matrix, clr);
		//r.draw_line(point_2*m*world_matrix,point_3*m*world_matrix, clr);
		//r.draw_line(point_1*m*world_matrix,point_2*m*world_matrix, clr);
		r.draw_triangle(render::scene_ptr(scene), point_1*m*world_matrix,point_2*m*world_matrix,point_3*m*world_matrix, clr_solid);			
		r.draw_triangle(render::scene_ptr(scene), point_3*m*world_matrix,point_2*m*world_matrix,point_1*m*world_matrix, clr_solid);			
	}else
	if(0==strings::compare(type, "Plane"))
	{
	}else
	if(0==strings::compare(type, "Box"))
	{
		math::float3 size;
		size.x = float(curr["Width"]) * 0.5f;
		size.y = float(curr["Height"]) * 0.5f;
		size.z = float(curr["Depth"]) * 0.5f;
		r.draw_cube( render::scene_ptr(scene), m*world_matrix, size, clr);
	}else
	if(0==strings::compare(type, "Sphere"))
	{
		float inner_radius	= curr["RadiusInner"];
		float outer_radius	= curr["RadiusOuter"];

		math::float4x4 const m = math::create_rotation(rotate) * math::create_translation(translate);


		r.draw_ellipsoid(render::scene_ptr(scene), m, scale*inner_radius, clr);
		r.draw_ellipsoid(render::scene_ptr(scene), m, scale*outer_radius, clr);

//		r.draw_sphere(float3(0,0,0)*m*world_matrix, inner_radius, clr);
//		r.draw_sphere(float3(0,0,0)*m*world_matrix, outer_radius, clr);
	}else
	if(0==strings::compare(type, "Cylinder"))
	{
		float inner_radius	= curr["RadiusInner"];
		float outer_radius	= curr["RadiusOuter"];
		float height		= curr["Height"];
		r.draw_cylinder( render::scene_ptr(scene), m*world_matrix, float3(inner_radius,height,inner_radius), clr);
		r.draw_cylinder( render::scene_ptr(scene), m*world_matrix, float3(outer_radius,height,outer_radius), clr);
	}else
	if(0==strings::compare(type, "Cone"))
	{
		math::float3 size;
		size.x = curr["Radius"];
		size.y = curr["Height"];
		size.z = curr["Radius"];
		r.draw_cone( render::scene_ptr(scene), world_matrix*m, size, clr);
	}else
	if(0==strings::compare(type, "Blob"))
	{
	}else
	if(0==strings::compare(type, "Disc"))
	{
// 		System::String^ vert = gcnew System::String("");
// 		configs::lua_config_value cfg = configs::create_lua_config()->get_root()["vertexes"].create_table();
// 		int vertex_count = 32;
// 		for (int i = 0; i<=vertex_count;i++)
// 		{
// 			vert += (i + "," + (i+vertex_count) + ",");		
// 			vert += ((i+vertex_count) + "," + (i+1) + ",");
// 		}
// 		cfg["verts"] = unmanaged_string(vert).c_str();
// 		cfg.save_as("d:/vert");
		float inner_radius = curr["RadiusInner"];
		float outer_radius = curr["RadiusOuter"];
		r.draw_circle( render::scene_ptr(scene), m*world_matrix, float3(inner_radius,0,inner_radius),clr);
		r.draw_circle( render::scene_ptr(scene), m*world_matrix, float3(outer_radius,0,outer_radius),clr);
	}
	else
	if(0==strings::compare(type, "Rectangle"))
	{
		math::float3 size;
		size.x = float(curr["Width"]) * 0.5f;
		size.y = 0;
		size.z = float(curr["Height"]) * 0.5f;
		r.draw_rectangle( render::scene_ptr(scene), world_matrix*m, size, clr);
	}else
		UNREACHABLE_CODE();
}

}
}