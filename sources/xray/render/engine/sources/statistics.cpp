////////////////////////////////////////////////////////////////////////////
//	Created		: 23.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/ui/world.h>
#include <xray/render/core/gpu_timer.h>
#include "system_renderer.h"
#include "statistics.h"
#include <xray/render/core/backend.h>
#include <xray/ui/ui.h>
#include <xray/render/facade/ui_renderer.h>
#include "material.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>

#include "material_manager.h"

namespace xray {
namespace render {


statistics_group::statistics_group(pcstr group_name):
	first_statistics(0),
	m_name(group_name)
{
	m_next							= statistics::ref().first_group;
	statistics::ref().first_group	= this;
}


statistics_group::~statistics_group()
{

}

void statistics_group::start()
{
	for (statistics_base* it = first_statistics; it != 0; it = it->m_next)
	{
		it->start();
	}
}

u32 statistics_group::get_num_text_lines() const
{
	u32 num_lines = 1;
	
	for (statistics_base* it = first_statistics; it != 0; it = it->m_next)
	{
		num_lines++;
	}
	
	return num_lines;
}

static void push_point(xray::vectora<xray::render::ui::vertex>& out_vertices, u32 index, float x, float y, float z, u32 c, float u, float v)
{
	xray::render::ui::vertex& vertex_item	= out_vertices[index];
	vertex_item.m_position.set				(x, y, z, 1);
	vertex_item.m_uv.set					(u, v);
	vertex_item.m_color						= c;
}

void make_ui_vertices(
	xray::vectora<xray::render::ui::vertex>& out_vertices,
	pcstr in_text, 
	xray::ui::font const& in_font,
	xray::math::float2 const& in_position,
	xray::math::color const& in_color,
	xray::math::color const& in_selection_color,
	u32 max_line_width,
	bool is_multiline,
	u32 start_selection_index,
	u32 end_selection_index)
{
	using namespace xray::math;
	
	float2 pos_rt			(0, 0);
	float2 pos				= in_position;
	u32 symb_count			= strlen(in_text);
//	u32 max_verts			= symb_count*4;
	
	pcstr ch				= in_text;
	float const height		= in_font.get_height();
	float height_ts			= in_font.get_height_ts();
	
	float curr_word_len		= 0.0f;
	pcstr next_word			= NULL;
	
	if(is_multiline)
		in_font.parse_word	(ch, curr_word_len, next_word);
	
	u32 index = 0;
	
	for(u32 i=0; i<symb_count; ++i, ++ch)
	{
		u32 clr = (i>=start_selection_index && i<end_selection_index) ? in_selection_color.m_value : in_color.m_value;
		
		float3 uv			= in_font.get_char_tc_ts(*ch);
		float3 const& tc	= in_font.get_char_tc(*ch);
		
		if (in_text[i]=='\n' || in_text[i]=='\r\n')
		{
			in_font.parse_word	(ch, curr_word_len, next_word);
			pos_rt.x			= 0.0f;
			pos_rt.y			+= height;
		}

		if(is_multiline && (ch==next_word))
		{
			in_font.parse_word		(ch, curr_word_len, next_word);
			if(pos_rt.x + curr_word_len > max_line_width)
			{
				pos_rt.x		= 0.0f;
				pos_rt.y		+= height;
			}
		}
		
		push_point(out_vertices, index++, pos.x+pos_rt.x,		pos.y+pos_rt.y+height,	0.0f, clr, uv.x,		uv.y+height_ts);
		push_point(out_vertices, index++, pos.x+pos_rt.x,		pos.y+pos_rt.y,			0.0f, clr, uv.x,		uv.y);
		push_point(out_vertices, index++, pos.x+pos_rt.x+tc.z,	pos.y+pos_rt.y+height,	0.0f, clr, uv.x+uv.z,	uv.y+height_ts);
		push_point(out_vertices, index++, pos.x+pos_rt.x+tc.z,	pos.y+pos_rt.y,			0.0f, clr, uv.x+uv.z,	uv.y);
		
		pos_rt.x			+= tc.z;
	}
}

static void draw_text(xray::ui::world& ui_world, pcstr str, u32 pos_x, u32 pos_y, u32 clr)
{
	u32 const string_length		= strlen(str);
	math::color string_color	(clr);
	
	xray::vectora<xray::render::ui::vertex> out_vertices(g_allocator);
	out_vertices.resize(string_length * 4);
	
	make_ui_vertices(
		out_vertices,
		str,
		*ui_world.default_font(),
		float2(float(pos_x), float(pos_y)),
		string_color,
		string_color,
		400,
		false,
		0,
		string_length
	);
	
	system_renderer::ref().draw_ui_vertices((vertex_formats::TL*)&out_vertices.front(), out_vertices.size(), 0, 0);
}

static void draw_text_shadowed(xray::ui::world& ui_world, pcstr str, u32 pos_x, u32 pos_y, u32 clr)
{
	draw_text(ui_world, str, pos_x + 1, pos_y + 1, xray::math::color(0, 0, 0, 220).m_value);
	//draw_text(ui_world, str, pos_x - 1, pos_y - 1, xray::math::color(0, 0, 0, 220).m_value);
	//draw_text(ui_world, str, pos_x + 1, pos_y - 1, xray::math::color(0, 0, 0, 220).m_value);
	//draw_text(ui_world, str, pos_x - 1, pos_y + 1, xray::math::color(0, 0, 0, 220).m_value);
	draw_text(ui_world, str, pos_x, pos_y, clr);
}

u32 statistics_group::render(xray::ui::world& ui_world, u32 x, u32 y)
{
 	u32 y_offset = 15;
 	
 	draw_text_shadowed(ui_world, m_name.c_str(), x, y, xray::math::color(25, 255, 25, 220).m_value);
 	
 	for (statistics_base* it = first_statistics; it != 0; it = it->m_next)
 	{
 		fs_new::virtual_path_string value;
 		it->print(value);
 		
 		draw_text_shadowed(ui_world, value.c_str(), x + 10, y + y_offset, xray::math::color(255, 255, 255, 220).m_value);
 		
 		y_offset+=15;
 	}
	return y_offset;
}

statistics_base::statistics_base(statistics_group* group, pcstr name)
{
	// Not for inner statistics values.
	if (group)
	{
		m_name					= name;
		m_next					= group->first_statistics;
		group->first_statistics = this;
	}
}

statistics_base::~statistics_base()
{

}

void statistics_float::print(fs_new::virtual_path_string& out_result)
{
	out_result.assignf("%s: %f", m_name.c_str(), average());
}

void statistics_int::print(fs_new::virtual_path_string& out_result)
{
	out_result.assignf("%s: %d", m_name.c_str(), average());
}


void statistics_cpu_gpu::print(fs_new::virtual_path_string& out_result)
{
	out_result.assignf("%s: CPU:%.3f, GPU:%.3f", m_name.c_str(), cpu_time.average(), gpu_time.average());
}

void statistics_cpu_gpu::start() 
{ 
	cpu_time.start();
	gpu_time.start();
}

void statistics::render(xray::ui::world& ui_world, u32 x, u32 y)
{
 	u32 y_offset = 0;
	u32 x_offset = 0;
	
 	for (statistics_group* it = first_group; it != 0; it = it->m_next)
 	{
		if (y_offset > 800)
		{
			x_offset	+= 350;
			y_offset	= 0;
		}
		
 		y_offset += it->render(ui_world, x + x_offset, y + y_offset);
 	}
	
//	fs_new::virtual_path_string out_text[100];
//	u32 num_lines = 0;
//	material_manager::ref().get_output_text(out_text, num_lines);
//
//	for (u32 i = 0; i < num_lines; i++)
//		draw_text_shadowed(ui_world, out_text[i].c_str(), 10, 10 + i * 20, xray::math::color(255, 255, 255, 220).m_value);
}



#pragma warning( push )
#pragma warning( disable : 4355 )

gbuffer_statistics_group::gbuffer_statistics_group	(pcstr group_name):
	statistics_group		(group_name),
	pre_pass_execute_time	(this, "pre pass execute time"),
	material_pass_execute_time (this, "material pass execute time")
{}

visibility_statistics_group::visibility_statistics_group	(pcstr group_name):
	statistics_group		(group_name),
	num_draw_calls			(this, "num draw calls"),
	culling_time			(this, "culling time"),
	models_updating_time	(this, "models updating time"),
	num_triangles			(this, "num visible triangles"),
	num_models				(this, "num models"),
	num_total_rendered_triangles (this, "num total rendered triangles"),
	num_total_rendered_points (this, "num total rendered points"),
	num_lights				(this, "num lights"),
	num_particles			(this, "num particles"),
	num_speedtree_instances	(this, "num total rendered speedtree instances")
{}

lights_statistics_group::lights_statistics_group		(pcstr group_name):
	statistics_group		(group_name),
	accumulate_lighting_time(this, "accumulate lighting time"),
	forward_lighting_time	(this, "forward lighting time"),
	shadow_map_time			(this, "shadow map time")
{}

particles_statistics_group::particles_statistics_group	(pcstr group_name):
	statistics_group		(group_name),
	execute_time			(this, "execute time"),
	sprites_execute_time	(this, "sprites execute time"),
	beamtrails_execute_time	(this, "beams and trails execute time"),
	meshes_execute_time		(this, "meshes execute time"),
	num_total_instances		(this, "num total instances")
{}

cascaded_sun_shadow_statistics_group::cascaded_sun_shadow_statistics_group	(pcstr group_name):
	statistics_group		(group_name),
	execute_time_cascade_1	(this, "execute time cascade # 1"),
	execute_time_cascade_2	(this, "execute time cascade # 2"),
	execute_time_cascade_3	(this, "execute time cascade # 3"),
	execute_time_cascade_4	(this, "execute time cascade # 4"),
	num_models_cascade_1	(this, "num rendered models cascade # 1"),
	num_models_cascade_2	(this, "num rendered models cascade # 2"),
	num_models_cascade_3	(this, "num rendered models cascade # 3"),
	num_models_cascade_4	(this, "num rendered models cascade # 4")
{}

postprocess_statistics_group::postprocess_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	execute_time			(this, "execute time")
{}

forward_stage_statistics_group::forward_stage_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	execute_time			(this, "execute time")
{}

general_statistics_group::general_statistics_group	(pcstr group_name):
	statistics_group		(group_name),
	render_frame_time		(this, "render frame time"),
	cpu_render_frame_time	(this, "cpu render frame time"),
	fps						(this, "FPS"),
	cpu_fps					(this, "FPS (CPU)"),
	num_setted_shader_constants (this, "num setted shader constants")
{}

speedtree_statistics_group::speedtree_statistics_group	(pcstr group_name):
	statistics_group		(group_name),
	render_time				(this, "render time"),
	culling_time			(this, "culling time"),
	//num_last_lod_billboards	(this, "num last lod billboards"),
	num_instances			(this, "num instances")
	//num_unique_trees		(this, "num unique trees")
{}

forward_decals_statistics_group::forward_decals_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	execute_time			(this, "execute time"),
	num_decals				(this, "num decals"),
	num_decal_draw_calls	(this, "num decal draw calls")
{}

deferred_decals_statistics_group::deferred_decals_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	execute_time			(this, "execute time"),
	num_decals				(this, "num decals"),
	num_decal_draw_calls	(this, "num decal draw calls")
{}

distortion_pass_statistics_group::distortion_pass_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	accumulate_time			(this, "accumulate time"),
	apply_time				(this, "apply time")
{}

ssao_statistics_group::ssao_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	ssao_accumulate_time	(this, "ssao accumulate time"),
	ssao_blurring_time		(this, "ssao blurring time")
{}

sky_statistics_group::sky_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	execute_time			(this, "execute time")
{}

lpv_statistics_group::lpv_statistics_group(pcstr group_name):
	statistics_group		(group_name),
	rsm_rendering_time		(this, "RSM rendering time"),
	rsm_downsample_time		(this, "RSM downsample time"),
	vpl_injection_time		(this, "VPL injection time"),
	gv_injection_time		(this, "GV injection time"),
	propagation_time		(this, "propagation time"),
	lpv_lookup_time			(this, "LPV lookup time")
{}

#pragma warning(pop)

statistics::statistics():
	first_group						(0),
	lpv_stat_group					("light propagation volumes statistics"),
	postprocess_stat_group			("post-process statistics"),
	ssao_stat_group					("ssao statistics"),
	distortion_pass_stat_group		("distortion pass statistics"),
	particles_stat_group			("particles statistics"),
	sky_stat_group					("sky statistics"),
	speedtree_stat_group			("speedtree statistics"),
	forward_stage_stat_group		("forward stage statistics"),
	cascaded_sun_shadow_stat_group	("cascaded sun shadow statistics"),
	gbuffer_stat_group				("g-buffer statistics"),
	lights_stat_group				("lights statistics"),
	forward_decals_stat_group		("forward decals statistics"),
	deferred_decals_stat_group		("deferred decals statistics"),
	visibility_stat_group			("visibility statistics"),
	general_stat_group				("general statistics")
{}

u32 statistics::get_num_text_lines() const
{
	u32 num_lines = 0;
	for (statistics_group* it = first_group; it != 0; it = it->m_next)
	{
		num_lines += get_num_text_lines();
	}
	return num_lines;
}

void statistics::start()
{
	for (statistics_group* it = first_group; it != 0; it = it->m_next)
	{
		it->start();
	}
	
	backend::ref().num_total_rendered_triangles		= 0;
	backend::ref().num_total_rendered_points		= 0;
	backend::ref().num_setted_shader_constants		= 0;
	backend::ref().num_draw_calls					= 0;
}

statistics::~statistics()
{
	
}

} // namespace render
} // namespace xray
