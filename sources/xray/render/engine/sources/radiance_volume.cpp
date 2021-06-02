////////////////////////////////////////////////////////////////////////////
//	Created		: 21.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "radiance_volume.h"
#include "system_renderer.h"

#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/render_target.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/options.h>
#include <xray/render/core/gpu_timer.h>
#include "renderer_context_targets.h"

#include <xray/math_half.h>
#include "renderer_context.h"

//#include "effect_radiance_volume_inject_lighting.h"
//#include "effect_radiance_volume_propagate_lighting.h"
#include "effect_light_propagation_volumes.h"

#include "shared_names.h"

#include <xray/console_command.h>

static bool s_draw_radiance_debug_value				= false;
static bool s_draw_propagated_radiance_debug_value	= false;
static bool s_draw_radiance_occluders_debug_value	= false;

static xray::console_commands::cc_bool s_draw_radiance_debug_cc				("draw_radiance_debug",				s_draw_radiance_debug_value, false, xray::console_commands::command_type_engine_internal);
static xray::console_commands::cc_bool s_draw_propagated_radiance_debug_cc	("draw_propagated_radiance_debug",	s_draw_propagated_radiance_debug_value, false, xray::console_commands::command_type_engine_internal);
static xray::console_commands::cc_bool s_draw_radiance_occluders_debug_cc	("draw_radiance_occluders_debug",	s_draw_radiance_occluders_debug_value, false, xray::console_commands::command_type_engine_internal);

namespace xray {
namespace render {


const D3D_INPUT_ELEMENT_DESC injection_geometry_vertex_layout[] =
{
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 0,  D3D_INPUT_PER_VERTEX_DATA, 0 },
};
	
const D3D_INPUT_ELEMENT_DESC sliced_cube_vertex_layout[] = 
{
	{"POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0}, // grid normalized x, y, z
	{"TEXCOORD",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0}, // transformed xy rt position, and slice index
};

injection_geometry::injection_geometry(u32 rsm_size):
	m_num_points(rsm_size * rsm_size),
	m_stride(sizeof(float2))
{
	float2* vertices					= ALLOC(float2, m_num_points);
	float2* vertices_begin				= vertices;
	
	for (u32 u = 0; u < rsm_size; u++)
		for (u32 v = 0; v < rsm_size; v++)
			*(vertices++) = float2(float(u), float(v)) / float(rsm_size);
	
	m_vertext_declaration				= resource_manager::ref().create_declaration(injection_geometry_vertex_layout);
	m_vertex_buffer						= resource_manager::ref().create_buffer	(
		m_num_points * m_stride,
		vertices_begin,
		enum_buffer_type_vertex, 
		false
	);
	FREE								(vertices_begin);
}

injection_geometry::injection_geometry(u32 rsm_size_x, u32 rsm_size_y):
	m_stride(sizeof(float2)),
	m_rsm_size_x(0),
	m_rsm_size_y(0)
{
	prepare(rsm_size_x, rsm_size_y);
}


void injection_geometry::prepare(u32 rsm_size_x, u32 rsm_size_y)
{
	if (m_rsm_size_x == rsm_size_x && m_rsm_size_y == rsm_size_y)
		return;
	
	m_num_points						= rsm_size_x * rsm_size_y;
	m_rsm_size_x						= rsm_size_x;
	m_rsm_size_y						= rsm_size_y;
	
	float2* vertices					= ALLOC(float2, m_num_points);
	float2* vertices_begin				= vertices;
	
	for (u32 u = 0; u < rsm_size_x; u++)
		for (u32 v = 0; v < rsm_size_y; v++)
			*(vertices++) = float2(float(u) / float(rsm_size_x), float(v) / float(rsm_size_y));
	
	m_vertext_declaration				= resource_manager::ref().create_declaration(injection_geometry_vertex_layout);
	m_vertex_buffer						= resource_manager::ref().create_buffer	(
		m_num_points * m_stride,
		vertices_begin,
		enum_buffer_type_vertex, 
		false
	);
	FREE								(vertices_begin);
}

void injection_geometry::draw()
{
	backend::ref().set_declaration			(m_vertext_declaration.c_ptr());
	backend::ref().set_vb					(m_vertex_buffer.c_ptr(), m_stride);
	backend::ref().set_ib					(NULL);
	backend::ref().render					(D3D_PRIMITIVE_TOPOLOGY_POINTLIST, m_num_points, 0);
}

sliced_cube_geometry::sliced_cube_geometry(u32 const in_num_cells):
	m_slices(in_num_cells),
	m_stride(sizeof(sliced_cube_geometry_vertex))
{
	u32 const						num_vertices	= m_slices * 4;
	u32 const						num_indices		= m_slices * 6;
	
	sliced_cube_geometry_vertex*	vertices		= (sliced_cube_geometry_vertex*)ALLOCA(m_stride * num_vertices);
	u16*							indices			= (u16*)ALLOCA(sizeof(u16) * num_indices);
	
	sliced_cube_geometry_vertex*	vertices_begin	= vertices;
	u16*							indices_begin	= indices;
	
	for (u32 slice_index = 0; slice_index < m_slices; slice_index++)
	{
		float const slice_z				= float(slice_index) / float(m_slices);
		
		new(vertices)sliced_cube_geometry_vertex;
		vertices->position				= float4(0.0f, 0.0f, slice_z, float(slice_index));
		vertices->xy_and_slice_index	= float4(vertices->position.x * 2.0f - 1.0f, (1.0f - vertices->position.y) * 2.0f - 1.0f, 0.0f, 1.0f);
		vertices++;
		
		new(vertices)sliced_cube_geometry_vertex;
		vertices->position				= float4(1.0f, 0.0f, slice_z, float(slice_index));
		vertices->xy_and_slice_index	= float4(vertices->position.x * 2.0f - 1.0f, (1.0f - vertices->position.y) * 2.0f - 1.0f, 0.0f, 1.0f);
		vertices++;
		
		new(vertices)sliced_cube_geometry_vertex;
		vertices->position				= float4(1.0f, 1.0f, slice_z, float(slice_index));
		vertices->xy_and_slice_index	= float4(vertices->position.x * 2.0f - 1.0f, (1.0f - vertices->position.y) * 2.0f - 1.0f, 0.0f, 1.0f);
		vertices++;
		
		new(vertices)sliced_cube_geometry_vertex;
		vertices->position				= float4(0.0f, 1.0f, slice_z, float(slice_index));
		vertices->xy_and_slice_index	= float4(vertices->position.x * 2.0f - 1.0f, (1.0f - vertices->position.y) * 2.0f - 1.0f, 0.0f, 1.0f);
		vertices++;
		
		u16 const offset				= static_cast<u16>(slice_index * 4);
		
		// clockwise
		*(indices++)					= 0 + offset;
		*(indices++)					= 1 + offset;
		*(indices++)					= 2 + offset;
		*(indices++)					= 0 + offset;
		*(indices++)					= 2 + offset;
		*(indices++)					= 3 + offset;
	}
	
	m_vertext_declaration				= resource_manager::ref().create_declaration(sliced_cube_vertex_layout);
	m_vertex_buffer						= resource_manager::ref().create_buffer(num_vertices * m_stride, vertices_begin, enum_buffer_type_vertex, false);
	m_index_buffer						= resource_manager::ref().create_buffer(num_indices * sizeof(u16), indices_begin, enum_buffer_type_index,  false);
}

void sliced_cube_geometry::draw()
{
	backend::ref().set_declaration		(m_vertext_declaration.c_ptr());
	backend::ref().set_vb				(m_vertex_buffer.c_ptr(), m_stride);
	backend::ref().set_ib				(m_index_buffer.c_ptr(), 0);	
	backend::ref().render_indexed		(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_slices * 6, 0, 0);
}

radiance_volume::radiance_volume(u32 const		in_rsm_size, 
								 u32 const		in_num_cells, 
								 u32 const		in_num_propagate_iterations,
								 float const	in_scale,
								 float const	in_flux_amplifier): 
	m_injection_geometry			(in_rsm_size),
	m_injection_geometry_from_camera(128, 128),//1.0 * 1056, 1.0 * 959),
	m_sliced_cube_geometry			(in_num_cells),
	m_num_cells						(in_num_cells),
	m_num_propagate_iterations		(in_num_propagate_iterations),
	m_scale							(in_scale),
	m_flux_amplifier				(in_flux_amplifier),
	m_is_position_changed			(true),
	m_prev_position					(math::float3(0.0f, 0.0f, 0.0f)),
	m_bbox							(math::create_zero_aabb())
{
	//DXGI_FORMAT_R16G16B16A16_FLOAT
	//DXGI_FORMAT_R8G8B8A8_UNORM
	
	DXGI_FORMAT rv_format				=	DXGI_FORMAT_R16G16B16A16_FLOAT;
	
	m_3d_rt_previous_radiance_r			=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_previous_radiance_g			=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_previous_radiance_b			=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	
	m_3d_rt_accumulated_propagation_r	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_accumulated_propagation_g	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_accumulated_propagation_b	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	
	m_3d_t_accumulated_propagation_r	=	m_3d_rt_accumulated_propagation_r->get_texture();
	m_3d_t_accumulated_propagation_g	=	m_3d_rt_accumulated_propagation_g->get_texture();
	m_3d_t_accumulated_propagation_b	=	m_3d_rt_accumulated_propagation_b->get_texture();
	
	
	m_3d_rt_radiance_r	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_radiance_g	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_radiance_b	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	
	m_3d_t_radiance_r	=	m_3d_rt_radiance_r->get_texture();
	m_3d_t_radiance_g	=	m_3d_rt_radiance_g->get_texture();
	m_3d_t_radiance_b	=	m_3d_rt_radiance_b->get_texture();
	
	m_3d_t_previous_radiance_r	=	m_3d_rt_previous_radiance_r->get_texture();
	m_3d_t_previous_radiance_g	=	m_3d_rt_previous_radiance_g->get_texture();
	m_3d_t_previous_radiance_b	=	m_3d_rt_previous_radiance_b->get_texture();
	
	
	m_3d_rt_occluders	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_t_occluders	=	m_3d_rt_occluders->get_texture();
	
	// This buffers needs for propagation.
	m_3d_rt_radiance_intermediate_r	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_radiance_intermediate_g	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	m_3d_rt_radiance_intermediate_b	=	resource_manager::ref().create_volume_render_target(NULL, in_num_cells, in_num_cells, in_num_cells, rv_format, enum_rt_usage_render_target, D3D_USAGE_DEFAULT);
	
	m_3d_t_radiance_intermediate_r	=	m_3d_rt_radiance_intermediate_r->get_texture();
	m_3d_t_radiance_intermediate_g	=	m_3d_rt_radiance_intermediate_g->get_texture();
	m_3d_t_radiance_intermediate_b	=	m_3d_rt_radiance_intermediate_b->get_texture();
	
#ifndef MASTER_GOLD
	//m_3d_t_radiance_r_lockable				=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
	//m_3d_t_radiance_g_lockable				=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
	//m_3d_t_radiance_b_lockable				=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
	//m_3d_t_propagated_radiance_r_lockable	=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
	//m_3d_t_propagated_radiance_g_lockable	=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
	//m_3d_t_propagated_radiance_b_lockable	=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
	//m_3d_t_occluders_lockable				=	resource_manager::ref().create_texture3d(NULL, in_num_cells, in_num_cells, in_num_cells, 0, rv_format, D3D_USAGE_STAGING, 0);
#endif // #ifndef MASTER_GOLD
	
	m_radiance_depth_stencil = resource_manager::ref().create_render_target(NULL, in_num_cells, in_num_cells, DXGI_FORMAT_R24G8_TYPELESS, enum_rt_usage_depth_stencil);
	
	m_c_rsm_size					=	backend::ref().register_constant_host("rsm_size", rc_int);
	m_c_grid_size					=	backend::ref().register_constant_host("grid_size", rc_float);
	m_c_grid_origin					=	backend::ref().register_constant_host("grid_origin", rc_float);
	m_c_grid_origin_and_inv_grid_scale	=	backend::ref().register_constant_host("grid_origin_and_inv_grid_scale", rc_float);
	m_c_grid_cell_size				=	backend::ref().register_constant_host("grid_cell_size", rc_float);
	m_c_num_grid_cells				=	backend::ref().register_constant_host("num_grid_cells", rc_float);
	m_c_light_direction				=	backend::ref().register_constant_host("light_direction", rc_float);
	m_c_light_position				=	backend::ref().register_constant_host("light_position", rc_float);
	m_c_propagate_iteration_index	=	backend::ref().register_constant_host("propagate_iteration_index", rc_int);
	m_c_inject_flux_weight			=	backend::ref().register_constant_host("inject_flux_weight", rc_float);
	m_c_flux_amplifier				=	backend::ref().register_constant_host("flux_amplifier", rc_float);
	m_c_eye_ray_corner				=	backend::ref().register_constant_host("s_eye_ray_corner", rc_float);
	m_c_cascade_index				=	backend::ref().register_constant_host("cascade_index", rc_int);
	m_c_occlusion_amplifier			=	backend::ref().register_constant_host("occlusion_amplifier", rc_float);
	
	//effect_manager::ref().create_effect<effect_radiance_volume_inject_lighting>(&m_inject_lighting_effect);
	//effect_manager::ref().create_effect<effect_radiance_volume_propagate_lighting>(&m_propagate_lighting_effect);
	
	effect_manager::ref().create_effect<effect_light_propagation_volumes>(&m_lpv_effect);
	
	
}

radiance_volume::~radiance_volume()
{
	
}

bool radiance_volume::is_effects_ready() const
{
	return m_lpv_effect.c_ptr() != NULL;
}

void radiance_volume::set_origin(xray::math::float3 const& in_origin)
{
	m_bbox.zero			();
	m_bbox.max			+= m_scale;
	m_bbox.move			(in_origin);
}

xray::math::float3 const& radiance_volume::get_origin() const 
{ 
	return m_bbox.min;
}

float radiance_volume::get_scale() const
{
	return m_scale;
}

u32 radiance_volume::get_num_cells() const
{
	return m_num_cells;
}

void radiance_volume::prepare_gv()
{
	m_is_position_changed	= false;
	
//	if (!math::is_similar(m_prev_position, m_bbox.min, math::epsilon_3) || !options::ref().m_lpv_gather_occluders_from_camera_view)
	{
		m_is_position_changed	= true;
		m_prev_position			= m_bbox.min;
		
		backend::ref().set_render_targets		(&*m_3d_rt_occluders, 0, 0, 0);
		backend::ref().clear_render_targets		(0.0f, 0.0f, 0.0f, 0.0f);
		backend::ref().reset_render_targets		();
	}
}

void radiance_volume::prepare(math::float3 const& view_position, math::float3 const& view_direction, float offset_from_center)
{
	backend::ref().set_render_targets		(&*m_3d_rt_radiance_r, &*m_3d_rt_radiance_g, &*m_3d_rt_radiance_b, 0);
	backend::ref().clear_render_targets		(0.0f, 0.0f, 0.0f, 0.0f);
	
	backend::ref().set_render_targets		(&*m_3d_rt_radiance_intermediate_r, &*m_3d_rt_radiance_intermediate_g, &*m_3d_rt_radiance_intermediate_b, 0);
	backend::ref().clear_render_targets		(0.0f, 0.0f, 0.0f, 0.0f);
	
	backend::ref().set_render_targets		(&*m_3d_rt_accumulated_propagation_r, &*m_3d_rt_accumulated_propagation_g, &*m_3d_rt_accumulated_propagation_b, 0);
	backend::ref().clear_render_targets		(0.0f, 0.0f, 0.0f, 0.0f);
	
	float const cell_size					=	get_scale() / float(get_num_cells());
	
	float3 cascade_origin					=	view_position - float3(0.5f, 0.5f, 0.5f) * get_scale();
	cascade_origin							+=	view_direction * get_scale() * offset_from_center;
	cascade_origin							/=  cell_size;
	cascade_origin.x						=	float(math::floor(cascade_origin.x));
	cascade_origin.y						=	float(math::floor(cascade_origin.y));
	cascade_origin.z						=	float(math::floor(cascade_origin.z));
	cascade_origin							*=  cell_size;
	
	if (!options::ref().m_lpv_movable)
		cascade_origin						=	float3(0.0f, 0.0f, 0.0f);
	
	set_origin								(cascade_origin);
}

void radiance_volume::begin_render_to_cells()
{
	backend::ref().get_viewport				(m_saved_viewport);
	backend::ref().set_depth_stencil_target	(NULL);
	
	D3D_VIEWPORT							cells_viewport;
	
	cells_viewport.TopLeftX					= 0.0f;
	cells_viewport.TopLeftY					= 0.0f;
	cells_viewport.Width					= float(m_num_cells);
	cells_viewport.Height					= float(m_num_cells);
	cells_viewport.MinDepth					= 0;
	cells_viewport.MaxDepth					= 1.f;
	
	backend::ref().set_viewport				(cells_viewport);
}

void radiance_volume::end_render_to_cells()
{
	backend::ref().reset_render_targets		();
	backend::ref().reset_depth_stencil_target();
	backend::ref().set_viewport				(m_saved_viewport);
}

void radiance_volume::fill_previous_result()
{
	//resource_manager::ref().copy3D(&*m_3d_t_previous_radiance_r, 0, 0, 0, &*m_3d_t_accumulated_propagation_r, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D(&*m_3d_t_previous_radiance_g, 0, 0, 0, &*m_3d_t_accumulated_propagation_g, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D(&*m_3d_t_previous_radiance_b, 0, 0, 0, &*m_3d_t_accumulated_propagation_b, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
}

void radiance_volume::propagate_lighting(u32 const cascade_index)
{
	//resource_manager::ref().copy3D			(&*m_3d_t_accumulated_propagation_r, 0, 0, 0, &*m_3d_t_radiance_r, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_accumulated_propagation_g, 0, 0, 0, &*m_3d_t_radiance_g, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_accumulated_propagation_b, 0, 0, 0, &*m_3d_t_radiance_b, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	
	begin_render_to_cells					();
	
	for (u32 iteration_index = 0; iteration_index < m_num_propagate_iterations; iteration_index++)
	{
		if (iteration_index % 2 == 0)
		{
			backend::ref().flush_rt_shader_resources();
			
			backend::ref().set_render_targets		(&*m_3d_rt_radiance_intermediate_r, &*m_3d_rt_radiance_intermediate_g, &*m_3d_rt_radiance_intermediate_b, 0);
			m_lpv_effect->apply						(effect_light_propagation_volumes::propagate_lighting_stage);
			//backend::ref().set_gs_constant			(m_c_grid_size, float(m_num_cells));
			backend::ref().set_ps_texture			("t_radiance_r", &*m_3d_t_radiance_r);
			backend::ref().set_ps_texture			("t_radiance_g", &*m_3d_t_radiance_g);
			backend::ref().set_ps_texture			("t_radiance_b", &*m_3d_t_radiance_b);
			backend::ref().set_ps_texture			("t_occluders", &*m_3d_t_occluders);
			backend::ref().set_ps_constant			(m_c_num_grid_cells, float(m_num_cells));
			backend::ref().set_ps_constant			(m_c_propagate_iteration_index, iteration_index);
			backend::ref().set_ps_constant			(m_c_flux_amplifier, m_flux_amplifier);
			backend::ref().set_ps_constant			(m_c_cascade_index, cascade_index);
			backend::ref().set_ps_constant			(m_c_occlusion_amplifier, options::ref().m_lpv_occlusion_amplifier);
			
			m_sliced_cube_geometry.draw				();
			
			// Accumulate.
			backend::ref().set_render_targets		(&*m_3d_rt_accumulated_propagation_r, &*m_3d_rt_accumulated_propagation_g, &*m_3d_rt_accumulated_propagation_b, 0);
			m_lpv_effect->apply						(effect_light_propagation_volumes::accumulate_propagation_stage);
			backend::ref().set_ps_texture			("t_radiance_r", &*m_3d_t_radiance_intermediate_r);
			backend::ref().set_ps_texture			("t_radiance_g", &*m_3d_t_radiance_intermediate_g);
			backend::ref().set_ps_texture			("t_radiance_b", &*m_3d_t_radiance_intermediate_b);
			m_sliced_cube_geometry.draw				();
		}
		else
		{
			backend::ref().flush_rt_shader_resources();
			
			backend::ref().set_render_targets		(&*m_3d_rt_radiance_r, &*m_3d_rt_radiance_g, &*m_3d_rt_radiance_b, 0);
			m_lpv_effect->apply						(effect_light_propagation_volumes::propagate_lighting_stage);
			//backend::ref().set_gs_constant			(m_c_grid_size, float(m_num_cells));
			backend::ref().set_ps_texture			("t_radiance_r", &*m_3d_t_radiance_intermediate_r);
			backend::ref().set_ps_texture			("t_radiance_g", &*m_3d_t_radiance_intermediate_g);
			backend::ref().set_ps_texture			("t_radiance_b", &*m_3d_t_radiance_intermediate_b);
			backend::ref().set_ps_texture			("t_occluders", &*m_3d_t_occluders);
			backend::ref().set_ps_constant			(m_c_num_grid_cells, float(m_num_cells));
			backend::ref().set_ps_constant			(m_c_propagate_iteration_index, iteration_index);
			backend::ref().set_ps_constant			(m_c_flux_amplifier, m_flux_amplifier);
			backend::ref().set_ps_constant			(m_c_cascade_index, cascade_index);
			backend::ref().set_ps_constant			(m_c_occlusion_amplifier, options::ref().m_lpv_occlusion_amplifier);
			
			m_sliced_cube_geometry.draw				();
			
			// Accumulate.
			backend::ref().set_render_targets		(&*m_3d_rt_accumulated_propagation_r, &*m_3d_rt_accumulated_propagation_g, &*m_3d_rt_accumulated_propagation_b, 0);
			m_lpv_effect->apply						(effect_light_propagation_volumes::accumulate_propagation_stage);
			backend::ref().set_ps_texture			("t_radiance_r", &*m_3d_t_radiance_r);
			backend::ref().set_ps_texture			("t_radiance_g", &*m_3d_t_radiance_g);
			backend::ref().set_ps_texture			("t_radiance_b", &*m_3d_t_radiance_b);
			m_sliced_cube_geometry.draw				();
		}
	}
	
// 	if (m_num_propagate_iterations % 2 != 0)
// 	{
// 		resource_manager::ref().copy3D			(&*m_3d_t_radiance_r, 0, 0, 0, &*m_3d_t_radiance_intermediate_r, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
// 		resource_manager::ref().copy3D			(&*m_3d_t_radiance_g, 0, 0, 0, &*m_3d_t_radiance_intermediate_g, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
// 		resource_manager::ref().copy3D			(&*m_3d_t_radiance_b, 0, 0, 0, &*m_3d_t_radiance_intermediate_b, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
// 	}
	
	//resource_manager::ref().copy3D			(&*m_3d_t_radiance_r, 0, 0, 0, &*m_3d_t_accumulated_propagation_r, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_radiance_g, 0, 0, 0, &*m_3d_t_accumulated_propagation_g, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_radiance_b, 0, 0, 0, &*m_3d_t_accumulated_propagation_b, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	
	end_render_to_cells						();
	
#ifndef MASTER_GOLD	
	//resource_manager::ref().copy3D			(&*m_3d_t_propagated_radiance_r_lockable, 0, 0, 0, &*m_3d_t_radiance_r, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_propagated_radiance_g_lockable, 0, 0, 0, &*m_3d_t_radiance_g, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_propagated_radiance_b_lockable, 0, 0, 0, &*m_3d_t_radiance_b, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
#endif // #ifndef MASTER_GOLD
}

void radiance_volume::inject_occluders(renderer_context* context, xray::math::float3 const& light_position, xray::math::float3 const& light_direction, u32 rsm_size)
{
	XRAY_UNREFERENCED_PARAMETER(rsm_size);
	
	// Inject occluders from light view.
	if (options::ref().m_lpv_gather_occluders_from_light_view)
	{
		backend::ref().set_render_targets		(&*m_3d_rt_occluders, 0, 0, 0);
		begin_render_to_cells					();
		
		m_lpv_effect->apply						(effect_light_propagation_volumes::inject_occluders_light_view_stage);
		
		backend::ref().set_vs_constant			(m_c_grid_origin_and_inv_grid_scale, float4(m_bbox.min, 1.0f / get_scale()));
		
		backend::ref().set_gs_constant			(m_c_grid_size, float(m_num_cells));
		backend::ref().set_gs_constant			(m_c_light_position, light_position);
		backend::ref().set_gs_constant			(m_c_light_direction, light_direction);
		backend::ref().set_ps_constant			(m_c_light_position, light_position);
		
		m_injection_geometry.draw				();
		
		end_render_to_cells						();
	}
/*
	
	// Inject occluders from camera view.
	if (m_is_position_changed && options::ref().m_lpv_gather_occluders_from_camera_view)
	{
		backend::ref().set_render_targets		(&*m_3d_rt_occluders, 0, 0, 0);
		begin_render_to_cells					();
		
		m_lpv_effect->apply						(effect_light_propagation_volumes::inject_occluders_camera_view_stage);
		
		backend::ref().set_vs_constant			(m_c_grid_origin, m_bbox.min);
		backend::ref().set_vs_constant			(m_c_grid_cell_size, m_scale / float(m_num_cells));
		backend::ref().set_vs_constant			(m_c_num_grid_cells, float(m_num_cells));
		
		backend::ref().set_gs_constant			(m_c_grid_size, float(m_num_cells));
		backend::ref().set_gs_constant			(m_c_light_position, light_position);
		backend::ref().set_gs_constant			(m_c_light_direction, light_direction);
		
		float3 const* const eye_rays			= context->get_eye_rays();
		backend::ref().set_vs_constant			(m_c_eye_ray_corner,	((float4*)eye_rays)[0]);
		
		m_injection_geometry_from_camera.draw	();
		
		end_render_to_cells						();
	}
	*/
#ifndef MASTER_GOLD	
	if (options::ref().m_lpv_gather_occluders_from_light_view ||
		options::ref().m_lpv_gather_occluders_from_camera_view)
	{
		//resource_manager::ref().copy3D			(&*m_3d_t_occluders_lockable, 0, 0, 0, &*m_3d_t_occluders, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	}
#endif // #ifndef MASTER_GOLD
}

void radiance_volume::inject_camera_occluders(renderer_context* context)
{
	// Inject occluders from camera view.
	if (m_is_position_changed && options::ref().m_lpv_gather_occluders_from_camera_view)
	{
		backend::ref().set_render_targets		(&*m_3d_rt_occluders, 0, 0, 0);
		begin_render_to_cells					();
		
		m_lpv_effect->apply						(effect_light_propagation_volumes::inject_occluders_camera_view_stage);
		
		backend::ref().set_vs_constant			(m_c_grid_origin, m_bbox.min);
		backend::ref().set_vs_constant			(m_c_grid_cell_size, m_scale / float(m_num_cells));
		backend::ref().set_vs_constant			(m_c_num_grid_cells, float(m_num_cells));
		
		backend::ref().set_gs_constant			(m_c_grid_size, float(m_num_cells));
		
		float3 const* const eye_rays			= context->get_eye_rays();
		backend::ref().set_vs_constant			(m_c_eye_ray_corner,	((float4*)eye_rays)[0]);
		
		context->set_v(context->get_v());
		context->set_p(context->get_p());
		
		m_injection_geometry_from_camera.prepare(
			context->m_targets->m_t_gbuffer_position_downsampled->width(),
			context->m_targets->m_t_gbuffer_position_downsampled->height()
		);
		
		m_injection_geometry_from_camera.draw	();
		
		end_render_to_cells						();
	}
	
// #ifndef MASTER_GOLD	
// 	if (options::ref().m_lpv_gather_occluders_from_light_view ||
// 		options::ref().m_lpv_gather_occluders_from_camera_view)
// 	{
// 		resource_manager::ref().copy3D			(&*m_3d_t_occluders_lockable, 0, 0, 0, &*m_3d_t_occluders, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
// 	}
// #endif // #ifndef MASTER_GOLD
}

void radiance_volume::inject_lighting(xray::math::float3 const& light_position, xray::math::float3 const& light_direction, float light_fov, u32 rsm_size)
{
	backend::ref().set_render_targets		(&*m_3d_rt_radiance_r, &*m_3d_rt_radiance_g, &*m_3d_rt_radiance_b, 0);
	
	begin_render_to_cells					();
	
	m_lpv_effect->apply						(effect_light_propagation_volumes::inject_lighting_stage);
	
	backend::ref().set_vs_constant			(m_c_grid_origin_and_inv_grid_scale, float4(m_bbox.min, 1.0f / get_scale()));
	backend::ref().set_gs_constant			(m_c_grid_size, float(m_num_cells));
	backend::ref().set_gs_constant			(m_c_light_position, light_position);
	backend::ref().set_gs_constant			(m_c_light_direction, light_direction);
	
	// use or not?
	backend::ref().set_ps_constant			(m_c_light_position, light_position);
	backend::ref().set_ps_constant			(m_c_light_direction, light_direction);
	
	float aspect_ratio						= 1.0f;
	
	float tan_fovy_half						= math::tan(light_fov / 2.0f);
	float tan_fovx_half						= math::tan(light_fov / 2.0f) * aspect_ratio;
	
	float flux_weight						= 4.0f * tan_fovy_half * tan_fovx_half / float(rsm_size * rsm_size);
	
	flux_weight								*= 30.0f;
	
	backend::ref().set_ps_constant			(m_c_inject_flux_weight, flux_weight);
	
	m_injection_geometry.draw				();
	
#ifndef MASTER_GOLD
	// for show VPL
	//resource_manager::ref().copy3D			(&*m_3d_t_radiance_r_lockable, 0, 0, 0, &*m_3d_t_radiance_r, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_radiance_g_lockable, 0, 0, 0, &*m_3d_t_radiance_g, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
	//resource_manager::ref().copy3D			(&*m_3d_t_radiance_b_lockable, 0, 0, 0, &*m_3d_t_radiance_b, 0, 0, 0, m_num_cells, m_num_cells, m_num_cells);
#endif // #ifndef MASTER_GOLD
	
	end_render_to_cells						();
}

static void draw_line(float3 const& start, float3 const& end, xray::math::color clr, bool use_depth)
{
	float3 line[2];
	line[0] = start;
	line[1] = end;
	system_renderer::ref().draw_screen_lines(line, 2, clr, 1, 0xffffffff, use_depth, false);
}

static void draw_sh(render::vector<vertex_colored>& out_vertices, 
					render::vector<u16>& out_indices, 
					math::float3 const& position, 
					render::renderer_context* context, 
					math::color const& clr,
					float width)
{
	u32 screen_width  = xray::render::backend::ref().target_width();
	
	float4x4 view_matrix = context->get_v(),
			 proj_matrix = context->get_p();
	
	float4x4 inv_view_proj_matrix = math::mul4x4( view_matrix, proj_matrix);
	
	inv_view_proj_matrix.try_invert( inv_view_proj_matrix);
	
	float4x4 inv_view_matrix = view_matrix;
	inv_view_matrix.try_invert(inv_view_matrix);
	
	// Distance to view pos.
	float dist = ( float3( inv_view_matrix.e30, inv_view_matrix.e31, inv_view_matrix.e32) - position).length();
	
	// Calc quad vertex offsets.
	float3 offset_by_x = inv_view_proj_matrix.transform_direction( float3( 1000, 0, 0 ) ).normalize() * 1.0f / (float)screen_width * width * 0.5f * dist,
		   offset_by_y = inv_view_proj_matrix.transform_direction( float3( 0,-1000, 0 ) ).normalize() * 1.0f / (float)screen_width * width * 0.5f * dist;
	
	float3 quad_position[4] = { 
		position - offset_by_x - offset_by_y,
		position - offset_by_x + offset_by_y, 
		position + offset_by_x + offset_by_y,
		position + offset_by_x - offset_by_y
	};
	
	u16 const num_prev_vertices = static_cast<u16>(out_vertices.size());
	
	for ( u32 i = 0; i < 4; ++i)
		out_vertices.push_back	( vertex_colored( quad_position[i], clr ) );
	
	out_indices.push_back(num_prev_vertices + 2); 
	out_indices.push_back(num_prev_vertices + 1); 
	out_indices.push_back(num_prev_vertices + 0);
	out_indices.push_back(num_prev_vertices + 3); 
	out_indices.push_back(num_prev_vertices + 2); 
	out_indices.push_back(num_prev_vertices + 0);
}

#ifndef MASTER_GOLD

void radiance_volume::draw_debug_radiance(render::renderer_context* context)
{
	u32 row_pitch_r = 0;
	u32 row_pitch_g = 0;
	u32 row_pitch_b = 0;
	
	math::half4* data_r = (math::half4*)m_3d_t_radiance_r_lockable->map3D(D3D_MAP_READ, 0, row_pitch_r);
	math::half4* data_g = (math::half4*)m_3d_t_radiance_g_lockable->map3D(D3D_MAP_READ, 0, row_pitch_g);
	math::half4* data_b = (math::half4*)m_3d_t_radiance_b_lockable->map3D(D3D_MAP_READ, 0, row_pitch_b);
	
	render::vector<vertex_colored> vertices;
	render::vector<u16> indices;
	
	if (data_r && data_g && data_b)
	{
		for (u32 z = 0; z < m_num_cells; z++)
		{
			vertices.clear();
			indices.clear();
			
			bool has_values = false;
			for (u32 y = 0; y < m_num_cells; y++)
			{
				for (u32 x = 0; x < m_num_cells; x++)
				{
					float4 value_r						= *(data_r + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					float4 value_g						= *(data_g + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					float4 value_b						= *(data_b + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					
					float const	 cell_size				= m_scale / float(m_num_cells);
					float const	 half_cell_offset		= 0.5f * m_scale / float(m_num_cells);
					float3 const half_cell_offset_xyz	= float3(half_cell_offset, half_cell_offset, half_cell_offset);
					
					float4 value						= value_r + value_g + value_b;
					
					clamp								(value, float4(-100.0f, -100.0f, -100.0f, -100.0f), float4(100.0f, 100.0f, 100.0f, 100.0f));
					
					float3 for_length					= value.xyz();
					
					if (for_length.length() > math::epsilon_5)
					{
						float3 const sh_position		= m_bbox.min + float3(float(x), float(y), float(z)) * cell_size + half_cell_offset_xyz;
						
						if (identity(0))
						draw_sh	(
							vertices,
							indices,
							sh_position,
							context,
							math::color(value.y, value.z, value.w, 1.0f),
							10.0f
						);
						
						math::aabb sh_aabb(math::create_aabb_min_max(
												sh_position - half_cell_offset_xyz * 0.9f,
												sh_position + half_cell_offset_xyz * 0.9f)
												);

						system_renderer::ref().draw_aabb(sh_aabb, math::color(1.0f, 0.5f, 0.5f, 0.6f));
						
						has_values = true;
					}
					else if (identity(0))
					{
						draw_sh	(
							vertices, 
							indices, 
							m_bbox.min + float3(float(x), float(y), float(z)) * cell_size + half_cell_offset_xyz, 
							context, 
							math::color(value.y, value.z, value.w, 0.25f), 
							10.0f
						);
						has_values = true;
					}
				}
			}
			if (identity(0) && has_values)
			system_renderer::ref().draw_triangles(
				&*vertices.begin(),
				&*vertices.end(),
				&*indices.begin(),
				&*indices.end()
			);
		}
	}
	
	m_3d_t_radiance_b_lockable->unmap3D(0);
	m_3d_t_radiance_g_lockable->unmap3D(0);
	m_3d_t_radiance_r_lockable->unmap3D(0);
}

// static float dot4(math::float4 const& left, math::float4 const& right)
// {
// 	return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
// }

void radiance_volume::draw_debug_prapagated_radiance(render::renderer_context* context)
{
	u32 row_pitch_r = 0;
	u32 row_pitch_g = 0;
	u32 row_pitch_b = 0;
	
	math::half4* data_r = (math::half4*)m_3d_t_propagated_radiance_r_lockable->map3D(D3D_MAP_READ, 0, row_pitch_r);
	math::half4* data_g = (math::half4*)m_3d_t_propagated_radiance_g_lockable->map3D(D3D_MAP_READ, 0, row_pitch_g);
	math::half4* data_b = (math::half4*)m_3d_t_propagated_radiance_b_lockable->map3D(D3D_MAP_READ, 0, row_pitch_b);
	
	render::vector<vertex_colored> vertices;
	render::vector<u16> indices;
	
	if (data_r && data_g && data_b)
	{
		for (u32 z = 0; z < m_num_cells; z++)
		{
			vertices.clear();
			indices.clear();
			
			bool has_values = false;
			for (u32 y = 0; y < m_num_cells; y++)
			{
				for (u32 x = 0; x < m_num_cells; x++)
				{
					float4 value_r						= *(data_r + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					float4 value_g						= *(data_g + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					float4 value_b						= *(data_b + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					
					float const	 cell_size				= m_scale / float(m_num_cells);
					float const	 half_cell_offset		= 0.5f * m_scale / float(m_num_cells);
					float3 const half_cell_offset_xyz	= float3(half_cell_offset, half_cell_offset, half_cell_offset);
					
					float4 ident						= float4(1.0f, 1.0f, 1.0f, 1.0f);
					float4 value						= float4(math::abs(value_r.x), math::abs(value_g.x), math::abs(value_b.x), 1.0f);
					
					clamp								(value, float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 1.0f, 1.0f, 1.0f));
					
					float3 for_length					= value.xyz();
					
					if (for_length.length() > math::epsilon_5)
					{
						float3 const sh_position		= m_bbox.min + float3(float(x), float(y), float(z)) * cell_size + half_cell_offset_xyz;
						
						if (identity(0))
						draw_sh	(
							vertices,
							indices,
							sh_position,
							context,
							math::color(value.x, value.y, value.z, 1.0f),
							10.0f
						);
						
						math::aabb sh_aabb(math::create_aabb_min_max(
												sh_position - half_cell_offset_xyz * 0.3f,
												sh_position + half_cell_offset_xyz * 0.3f)
												);
						system_renderer::ref().draw_aabb(sh_aabb, math::color(for_length.x, for_length.y, for_length.z, 1.0f));
						
						has_values = true;
					}
				}
			}
			if (identity(0) && has_values)
			system_renderer::ref().draw_triangles(
				&*vertices.begin(),
				&*vertices.end(),
				&*indices.begin(),
				&*indices.end()
			);
		}
	}
	
	m_3d_t_propagated_radiance_b_lockable->unmap3D(0);
	m_3d_t_propagated_radiance_g_lockable->unmap3D(0);
	m_3d_t_propagated_radiance_r_lockable->unmap3D(0);
}

void radiance_volume::draw_debug_radiance_occluders(render::renderer_context* context)
{
	XRAY_UNREFERENCED_PARAMETER(context);
	
	u32 row_pitch = 0;
	
	math::half4* data = (math::half4*)m_3d_t_occluders_lockable->map3D(D3D_MAP_READ, 0, row_pitch);
	
	if (data)
	{
		for (u32 z = 0; z < m_num_cells; z++)
		{
			bool has_values = false;
			for (u32 y = 0; y < m_num_cells; y++)
			{
				for (u32 x = 0; x < m_num_cells; x++)
				{
					float4 value						= *(data + x + y * m_num_cells + z * m_num_cells * m_num_cells);
					
					float const	 cell_size				= m_scale / float(m_num_cells);
					float const	 half_cell_offset		= 0.5f * m_scale / float(m_num_cells);
					float3 const half_cell_offset_xyz	= float3(half_cell_offset, half_cell_offset, half_cell_offset);
					
					clamp								(value, float4(-100.0f, -100.0f, -100.0f, -100.0f), float4(100.0f, 100.0f, 100.0f, 100.0f));
					
					float3 for_length					= value.xyz();
					
					if (for_length.length() > math::epsilon_5)
					{
						float3 const sh_position		= m_bbox.min + float3(float(x), float(y), float(z)) * cell_size + half_cell_offset_xyz;
						
						math::aabb sh_aabb(math::create_aabb_min_max(
							sh_position - half_cell_offset_xyz * 0.9f,
							sh_position + half_cell_offset_xyz * 0.9f
							));
						system_renderer::ref().draw_aabb(sh_aabb, math::color(0.0f, 0.0f, 1.0f, 0.75f));
						
						has_values = true;
					}
				}
			}
		}
	}
	
	m_3d_t_occluders_lockable->unmap3D(0);
}

void radiance_volume::draw_debug(render::renderer_context* context)
{
	if (!s_draw_radiance_debug_value && 
		!s_draw_propagated_radiance_debug_value &&
		!s_draw_radiance_occluders_debug_value)
		return;
	
	system_renderer::ref().draw_aabb(m_bbox, math::color(0.75f, 0.1f, 1.1f, 1.0f));
	
	draw_line(m_bbox.min, m_bbox.min + float3(m_bbox.max.x, 0, 0), math::color(1.0f, 0.0f, 0.0f, 1.0f), false);
	draw_line(m_bbox.min, m_bbox.min + float3(0, m_bbox.max.y, 0), math::color(0.0f, 1.0f, 0.0f, 1.0f), false);
	draw_line(m_bbox.min, m_bbox.min + float3(0, 0, m_bbox.max.z), math::color(0.0f, 0.0f, 1.0f, 1.0f), false);
	
	if (s_draw_radiance_debug_value)
		draw_debug_radiance(context);

	if (s_draw_propagated_radiance_debug_value)
		draw_debug_prapagated_radiance(context);

	if (s_draw_radiance_occluders_debug_value)
		draw_debug_radiance_occluders(context);
}

#endif // #ifndef MASTER_GOLD

} // namespace render
} // namespace xray
