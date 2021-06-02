////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "system_renderer.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/resource_manager.h>
#include "vertex_formats.h"
#include <xray/render/core/effect_manager.h>
#include "effect_system_colored.h"
#include "effect_system_line.h"
#include "renderer_context.h"
#include "effect_system_ui.h"
#include "effect_wireframe_colored.h"
#include "effect_editor_selection.h"
#include "effect_editor_model_ghost.h"
#include "effect_particle_selection.h"
#include "effect_speedtree_selection.h"
#include "material_manager.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/pix_event_wrapper.h>
#include "aabb_indices.h"
#include <xray/render/core/render_target.h>
#include "renderer_context_targets.h"
#include "terrain_render_model.h"
#include "scene.h"
#include "scene_view.h"
#include "terrain.h"
#include "render_output_window.h"
#include <xray/particle/world.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/res_effect.h>

#include "render_particle_emitter_instance.h"
#include "stage_particles.h"
#include <xray/render/core/destroy_data_helper.h>

#include "speedtree_tree.h"
#include "speedtree_forest.h"

#include <xray/render/core/options.h>


#include <xray/render/facade/material_effects_instance_cook_data.h>

namespace xray {
namespace render {

//TODO: not here
const D3D_INPUT_ELEMENT_DESC F_L_sl[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32_FLOAT,		0, 28,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R32_UINT,		0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
};
struct vertex_colored_sl
{
	math::float3	position;
	u32				color;
	math::float3	begin_line_position;
	float			shift;
	u32				pattern;
}; 

untyped_buffer * system_renderer::create_quad_ib()
{
	const u32 quad_count = 4*1024;
	const u32 idx_count  = quad_count*2*3;

	// 	if ( device::ref().get_caps().geometry.software)
	// 	{
	// 		ASSERT( 0);
	// 		usage |= D3DUSAGE_SOFTWAREPROCESSING;
	// 	}

	u16	indices[idx_count];

	int		vertex_id = 0;
	int		idx= 0;
	for ( int i=0; i<quad_count; ++i)
	{
		indices[idx++]=u16( vertex_id+0);
		indices[idx++]=u16( vertex_id+1);
		indices[idx++]=u16( vertex_id+2);

		indices[idx++]=u16( vertex_id+3);
		indices[idx++]=u16( vertex_id+2);
		indices[idx++]=u16( vertex_id+1);

		vertex_id+=4;
	}

	
	return resource_manager::ref().create_buffer( idx_count*sizeof(u16), indices, enum_buffer_type_index, false);
}


system_renderer::system_renderer( renderer_context * renderer_context):
	m_renderer_context	(renderer_context),
	m_vertex_stream		( 1024*1024 ),
	m_index_stream		(  512*1024 ),
	m_grid_mode			(false),
	m_color_write		(true)
{
	ASSERT( m_renderer_context);

	m_renderer_context->m_quad_ib			= create_quad_ib();
	m_renderer_context->m_g_quad_uv			= resource_manager::ref().create_geometry( vertex_formats::F_TL, sizeof(vertex_formats::TL), backend::ref().vertex.buffer(), *m_renderer_context->m_quad_ib);
	m_renderer_context->m_g_quad_2uv		= resource_manager::ref().create_geometry( vertex_formats::F_TL2uv, sizeof(vertex_formats::TL2uv), backend::ref().vertex.buffer(), *m_renderer_context->m_quad_ib);
	m_renderer_context->m_g_quad_eye_ray	= resource_manager::ref().create_geometry( vertex_formats::F_Tquad, sizeof(vertex_formats::Tquad), backend::ref().vertex.buffer(), *m_renderer_context->m_quad_ib);

	m_colored_geom = resource_manager::ref().create_geometry( vertex_formats::F_L, sizeof(vertex_formats::L), m_vertex_stream.buffer(), m_index_stream.buffer());

	m_colored_geom_sl = resource_manager::ref().create_geometry( F_L_sl, sizeof(vertex_colored_sl), m_vertex_stream.buffer(), m_index_stream.buffer());

	m_selection_color				= xray::math::float4( 0.0f, 0.0f, 0.5f, 1.f );
	m_selection_rate				= 1.0f;
	m_current_selection_color		= xray::math::float4( 0.f, 0.f, 0.f, 0.f );
	m_ghost_model_color				= xray::math::float4( 0.2f, 0.2f, 0.2f, 0.2f );
	
	//state_descriptor state_desc;

	//state_desc.color_write_enable( D3D_COLOR_WRITE_ENABLE_NONE);
	//m_rotation_mode_states[0] = resource_manager::ref().create_state( state_desc);

	//state_desc.color_write_enable( D3D_COLOR_WRITE_ENABLE_ALL);
	//m_rotation_mode_states[1] = resource_manager::ref().create_state( state_desc);

	//m_rotation_mode_states[0] = resource_manager::ref().create_state( state_desc);

	//state_desc.set_depth( true, true);
	//state_desc.color_write_enable(D3D_COLOR_WRITE_ENABLE_NONE);

	//m_rotation_mode_states[1] = resource_manager::ref().create_state( state_desc);

	//state_desc.set_stencil( true, 0x00/*ref_value*/, 0xFF, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP );
	//state_desc.color_write_enable( D3D_COLOR_WRITE_ENABLE_NONE);

	//m_rotation_mode_states[2] = resource_manager::ref().create_state( state_desc);
	//m_rotation_mode_states[3] = m_rotation_mode_states[2];

	//state_desc.set_stencil( true, 0x00/*ref_value*/, 0xFF, 0xFF, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_DECR_SAT, D3D_STENCIL_OP_KEEP );
	//state_desc.color_write_enable( D3D_COLOR_WRITE_ENABLE_NONE);

	//m_rotation_mode_states[4] = resource_manager::ref().create_state( state_desc);

	//state_desc.set_stencil( true, 0x00/*ref_value*/, 0xFF, 0xFF, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_KEEP );

	//state_desc.color_write_enable( D3D_COLOR_WRITE_ENABLE_ALL);
	//state_desc.set_depth(false, true);
	//m_rotation_mode_states[5] = resource_manager::ref().create_state( state_desc);
	
	effect_manager::ref().create_effect<effect_system_colored>(&m_sh_vcolor);
	
	effect_manager::ref().create_effect<effect_system_line>(&m_sh_sl);
	m_grid_density_constant		= backend::ref().register_constant_host( "grid_density", rc_float );
	m_ui_geom					= resource_manager::ref().create_geometry( vertex_formats::F_TL, sizeof(vertex_formats::TL), m_vertex_stream.buffer(), *m_renderer_context->m_quad_ib);
	
	pvoid buffer			    = ALLOCA( 1 * Kb );
	effect_options_descriptor   desc(buffer, 1 * Kb);
	desc["ui_texture0"]		    = "ui/ui_font_arial_21_1024";
	desc["ui_texture1"]		    = "ui/ui_skull";
	
	effect_manager::ref().create_effect<effect_system_ui>(&m_sh_ui, desc);
	m_WVP_sl					= backend::ref().register_constant_host( "m_WVP_sl", rc_float );
	
	// Setup selection shader and color constant.
	
	resource_manager::ref().register_constant_binding( shader_constant_binding("selection_color",	&m_current_selection_color));
	effect_manager::ref().create_effect<effect_wireframe_colored>(&m_notexture_shader);
	
	effect_manager::ref().create_effect<effect_speedtree_selection>(&m_speedtree_selection_shader);
	
	for (u32 i=0; i<num_vertex_input_types; i++)
	{
		if (i==post_process_vertex_input_type)
			continue;
		
		byte data[Kb];
		effect_options_descriptor desc(data, Kb);
		desc["vertex_input_type"] = (enum_vertex_input_type)i;
		
		effect_manager::ref().create_effect<effect_editor_selection>(&m_editor_selection_shader[i], desc);
	}
	
	effect_manager::ref().create_effect<effect_editor_model_ghost>(&m_editor_model_ghost_shader);
	
	effect_manager::ref().create_effect<effect_particle_selection>(&m_sh_particle_selection);
	
	m_c_start_corner = backend::ref().register_constant_host( "start_corner", rc_float );

/*
	xray::resources::query_resource(
		"editor_terrain_debug", 
		resources::material_class, 
		boost::bind(&system_renderer::on_material_loaded, this, _1), 
		render::g_allocator
		);
*/
	m_cook_data_to_delete			=
		XRAY_NEW_IMPL(
			&::xray::memory::g_mt_allocator, xray::render::material_effects_instance_cook_data)(
			post_process_vertex_input_type, 
			NULL,
			false
		);
	
	resources::user_data_variant	user_data;
	user_data.set					(m_cook_data_to_delete);
	
	resources::query_resource		(
		"editor_terrain_debug",
		xray::resources::material_effects_instance_class,
		boost::bind(&system_renderer::on_material_loaded, this, _1),
		&::xray::memory::g_mt_allocator,
		&user_data
	);
}

bool system_renderer::is_effects_ready() const
{
	for (u32 i=0; i<num_vertex_input_types; i++)
	{
		if (i==post_process_vertex_input_type)
			continue;
		
		if (m_editor_selection_shader[i].c_ptr() == NULL)
			return false;
	}
	
	return 
		   m_sh_vcolor.c_ptr() != NULL
		&& m_sh_ui.c_ptr() != NULL
		&& m_sh_sl.c_ptr() != NULL
		&& m_editor_model_ghost_shader.c_ptr() != NULL
		&& m_speedtree_selection_shader.c_ptr() != NULL
		&& m_notexture_shader.c_ptr() != NULL
		&& m_sh_particle_selection.c_ptr() != NULL;
}

void system_renderer::on_material_loaded(xray::resources::queries_result& result)
{
	XRAY_UNREFERENCED_PARAMETER(result);
	
 	if (result.is_successful())
 	{
		m_terrain_debug_material = xray::static_cast_resource_ptr<material_effects_instance_ptr>(result[0].get_unmanaged_resource());
 	}
	
	XRAY_DELETE_IMPL(&::xray::memory::g_mt_allocator, m_cook_data_to_delete);
}

system_renderer::~system_renderer()
{

}
void system_renderer::set_w	(const float4x4& m)		
{ 
	m_renderer_context->set_w(m); 
}
void system_renderer::set_v	(const float4x4& m)		
{ 
	m_renderer_context->set_v(m); 
}
void system_renderer::set_p	(const float4x4& m)		
{ 
	m_renderer_context->set_p(m); 
}

const float4x4& system_renderer::get_w	()	const 	
{ 
	return m_renderer_context->get_w(); 
}
const float4x4& system_renderer::get_v	()	const 	
{ 
	return m_renderer_context->get_v(); 
}
const float4x4& system_renderer::get_p	()	const	
{ 
	return m_renderer_context->get_p(); 
}


void system_renderer::draw_lines		(
		vertex_colored const* const vertices_begin,
		vertex_colored const* const vertices_end,
		u16 const* const indices_begin,
		u16 const* const indices_end,
		bool covering_effect
	)
{
	if (!is_effects_ready())
		return;
	
	R_ASSERT_CMP( vertices_begin, <, vertices_end );
	R_ASSERT_CMP( indices_begin,  <, indices_end );

	// Prepare vertex buffer.
	render::vertex_colored* vbuffer;
	u32 voffset;
	u32 const vertices_size	=  vertices_end - vertices_begin;
	m_vertex_stream.lock( vertices_size, &vbuffer, voffset);
	memory::copy( vbuffer, vertices_size*sizeof(render::vertex_colored), vertices_begin, vertices_size*sizeof(render::vertex_colored));
	m_vertex_stream.unlock();
	
	// Prepare index buffer
	u32 ioffset;
	u32 const indices_size	=  indices_end - indices_begin;
	u16* ibuffer = (u16*) m_index_stream.lock( indices_size, ioffset);
	memory::copy( ibuffer, indices_size*sizeof(u16), indices_begin, indices_size*sizeof(u16));
	m_index_stream.unlock();
	m_colored_geom->apply();
	
	if (covering_effect)
		m_sh_vcolor->apply( effect_system_colored::cover );
	else
		m_sh_vcolor->apply ( );
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_LINELIST, indices_size, ioffset /* ? */, voffset );
}

static xray::math::float2 clip_2_screen( xray::math::float3 const& world_pixel, math::float4x4 const& wvpMatrix, u32 screen_width, u32 screen_height)
{
	xray::math::float4 result = wvpMatrix.transform(float4(world_pixel,1.0f));
	
	if( math::is_zero(result.elements[3], math::epsilon_5) )
		result.elements[3]	= math::epsilon_3;

	result = result * (1.0f / result.elements[3]);

	result.elements[0] *= 0.5f;
	result.elements[1] *= -0.5f;

	result = result + float4(0.5f,0.5f,0.0f,0.0f);

	result.elements[0] *= screen_width;
	result.elements[1] *= screen_height;

	return xray::math::float2( result.elements[0], result.elements[1]);
}

static u8 pattern_length = 8;

static float frac( float f)
{
	return f - (u32)f;
}

static u8 calc_pattern(xray::math::float2 const& begin, xray::math::float2 const& end)
{
	return static_cast_checked<u8>(frac( xray::math::max(abs(end.elements[0]-begin.elements[0]), abs(end.elements[1]-begin.elements[1])) / (float)pattern_length ) * ((float)pattern_length));
}

void system_renderer::draw_screen_lines	( xray::math::float3 const* points, u32 count, math::color const& color, float width, u32 pattern, bool use_depth, bool is_screen_space_coord)
{
	if (!is_effects_ready())
		return;
	
	ASSERT( count > 0);
	
	XRAY_UNREFERENCED_PARAMETER(width);
	
	// Vertices already in world space.
	math::float4x4 wvpMatrix = is_screen_space_coord ? float4x4().identity() : math::mul4x4( m_renderer_context->get_v(), m_renderer_context->get_p());

	u32 screen_width = xray::render::backend::ref().target_width();
	u32 screen_height = xray::render::backend::ref().target_height();
	
	PIX_EVENT( draw_screen_lines);
	u32 u32_color = color.m_value;
	u32 vertex_count = count * 2 - 2;

	// Prepare index buffer
	u32 ioffset;
	u16* ibuffer = (u16*) m_index_stream.lock( vertex_count, ioffset);

	// Prepare vertex buffer
	vertex_colored_sl* vbuffer;
	u32 voffset;
	m_vertex_stream.lock( vertex_count, &vbuffer, voffset);

	float shift = 0.0f;

	for( u32 i = 1; i < count;  ++i)
	{
		const xray::math::float3& start_point	= points[ i-1 ],
								  current_point = points[ i   ];
		
		vertex_colored_sl&		  line_begin	= *vbuffer++;
		vertex_colored_sl&		  line_end		= *vbuffer++;
		
		line_begin.position = start_point;
		line_begin.color = u32_color;
		line_begin.begin_line_position = start_point;

		line_begin.shift = line_end.shift = shift;
		line_begin.pattern = line_end.pattern = pattern;

		xray::math::float2 screen_start = clip_2_screen( start_point, wvpMatrix, screen_width, screen_height);
		xray::math::float2 screen_end	= clip_2_screen( current_point, wvpMatrix, screen_width, screen_height);

		shift = shift + calc_pattern( screen_start, screen_end);

		if ( (u32)shift >= pattern_length)
			shift = shift - pattern_length;

		line_end.position = current_point;
		line_end.color = u32_color;
		line_end.begin_line_position = start_point;

		*ibuffer++ = static_cast_checked<u16>(i*2-2);
		*ibuffer++ = static_cast_checked<u16>(i*2-1);
	}

	m_vertex_stream.unlock();
	m_index_stream.unlock();

	m_colored_geom_sl->apply();

	if( use_depth)
		m_sh_sl->apply		( effect_system_line::z_enabled);
	else
		m_sh_sl->apply		( effect_system_line::z_disabled);

	backend::ref().set_vs_constant( m_WVP_sl, transpose(wvpMatrix) );

	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_LINELIST, vertex_count, ioffset , voffset );
}

void system_renderer::draw_3D_point	( xray::math::float3 const& position, float width, math::color const& color, bool use_depth)
{
	if (!is_effects_ready())
		return;
	
	XRAY_UNREFERENCED_PARAMETER(use_depth);
	
	u32 screen_width  = xray::render::backend::ref().target_width();

	float4x4 view_matrix = m_renderer_context->get_v(),
			 proj_matrix = m_renderer_context->get_p();
	
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

	typedef fixed_vector< vertex_colored, 4 >	colored_vertices_type;
	typedef fixed_vector< u16, 6 >				colored_indices_type;
	colored_vertices_type vertices;
	colored_indices_type indices;

	// Setup vertices.
	for ( u32 i = 0; i < 4; ++i)
		vertices.push_back	( vertex_colored( quad_position[i], color ) );

	// Setup indices.
	indices.push_back(2); indices.push_back(1); indices.push_back(0);
	indices.push_back(3); indices.push_back(2); indices.push_back(0);

	draw_triangles(
		&*vertices.begin(),
		&*vertices.end(),
		&*indices.begin(),
		&*indices.end()
	);
}

void system_renderer::draw_aabb( math::aabb const& aabb, math::color const& color)
{
	if (!is_effects_ready())
		return;
	
	render::vertex_colored vertices[] = {
		vertex_colored( aabb.min, color ),
		vertex_colored( float3( aabb.min.x, aabb.min.y, aabb.max.z), color ),
		vertex_colored( float3( aabb.min.x, aabb.max.y, aabb.min.z), color ),
		vertex_colored( float3( aabb.max.x, aabb.min.y, aabb.min.z), color ),
		vertex_colored( float3( aabb.min.x, aabb.max.y, aabb.max.z), color ),
		vertex_colored( float3( aabb.max.x, aabb.min.y, aabb.max.z), color ),
		vertex_colored( float3( aabb.max.x, aabb.max.y, aabb.min.z), color ),
		vertex_colored( aabb.max, color ),
	};

	draw_lines	(
		vertices,
		vertices + array_size(vertices),
		aabb_indices,
		aabb_indices + array_size(aabb_indices)
	);
}

void system_renderer::draw_obb			( math::float4x4 const& transform, math::color const& color )
{
	if (!is_effects_ready())
		return;
	
	render::vertex_colored vertices[] = {
		vertex_colored( transform.transform_position( float3( -1.f, -1.f, -1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( -1.f, -1.f, +1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( -1.f, +1.f, -1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( +1.f, -1.f, -1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( -1.f, +1.f, +1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( +1.f, -1.f, +1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( +1.f, +1.f, -1.f ) ), color ),
		vertex_colored( transform.transform_position( float3( +1.f, +1.f, +1.f ) ), color ),
	};

	draw_lines	(
		vertices,
		vertices + array_size(vertices),
		aabb_indices,
		aabb_indices + array_size(aabb_indices)
	);
}

void system_renderer::draw_triangles	(
		vertex_colored const* const vertices_begin,
		vertex_colored const* const vertices_end,
		u16 const* const indices_begin,
		u16 const* const indices_end,
		bool covering_effect
	)
{
	if (!is_effects_ready())
		return;
	
	R_ASSERT_CMP( vertices_begin, <, vertices_end );
	R_ASSERT_CMP( indices_begin,  <, indices_end );
	
	// Prepare ill vertex buffer.
	u32 voffset;
	u32 const vertices_size	= vertices_end - vertices_begin;
	render::vertex_colored* vbuffer = (render::vertex_colored*) m_vertex_stream.lock( vertices_size, sizeof(render::vertex_colored), voffset);
	memory::copy( vbuffer, vertices_size*sizeof(render::vertex_colored), vertices_begin, vertices_size*sizeof(render::vertex_colored));
	m_vertex_stream.unlock();

	// Prepare index buffer
	u32 ioffset;
	u32 const indices_size	= indices_end - indices_begin;
	u16* ibuffer = (u16*) m_index_stream.lock( indices_size, ioffset);
	memory::copy( ibuffer, indices_size*sizeof(u16), indices_begin, indices_size*sizeof(u16));
	m_index_stream.unlock();
	
	m_colored_geom->apply	();

	if (covering_effect)
	{
		m_sh_vcolor->apply		( effect_system_colored::cover);
	}
	else
	{
		if (!m_color_write)
			m_sh_vcolor->apply		( effect_system_colored::voided);
		else 
		if( !m_grid_mode)
			m_sh_vcolor->apply		( effect_system_colored::solid);
		else
			m_sh_vcolor->apply		( effect_system_colored::stenciled);
	}
	
	backend::ref().set_ps_constant ( m_grid_density_constant, m_grid_density);
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices_size, ioffset /* ? */, voffset );
}

void system_renderer::setup_grid_render_mode	( u32 grid_density )
{
	m_grid_mode = true;
	m_grid_density = grid_density/100.f;
}

void system_renderer::remove_grid_render_mode( )
{
	m_grid_mode = false;
}

void system_renderer::setup_rotation_control_modes ( bool color_write)
{
	m_color_write = color_write;
}

void system_renderer::draw_ui_vertices( vertex_formats::TL const * vertices, u32 const & count, int prim_type, int point_type )
{
	if (!is_effects_ready())
		return;
	
	u32 v_offset;
	vertex_formats::TL* vbuffer;
	m_vertex_stream.lock( count, &vbuffer, v_offset);
	memory::copy( vbuffer, count*sizeof(vertex_formats::TL), vertices, count*sizeof(vertex_formats::TL));
	m_vertex_stream.unlock();

	m_ui_geom->apply();

	if( prim_type == 0)
	{
		if( point_type == 0)
			m_sh_ui->apply( effect_system_ui::ui_font);

		else if( point_type == 1)
			m_sh_ui->apply( effect_system_ui::ui);

		else if( point_type == 2)
			m_sh_ui->apply( effect_system_ui::ui_fill);

		else
			UNREACHABLE_CODE();

		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 3*count/2, 0, v_offset);

	}
	else if( prim_type == 1)
	{
		m_sh_ui->apply( effect_system_ui::ui_line_strip);

		backend::ref().render( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, count, v_offset);
	}
	else
	{
		UNREACHABLE_CODE();
	}

}

static void fill_surface(ref_rt surf, renderer_context* context)
{
	float t_w = float(surf->width());
	float t_h = float(surf->height());

	backend::ref().set_render_targets( &*surf, 0, 0, 0);
	backend::ref().reset_depth_stencil_target();

	u32	    color = math::color_rgba( 255u, 255u, 255u, 255u);

	float2	p0( 0,0);
	float2	p1( 1,1);

	u32		offset;

	vertex_formats::TL* pv = ( vertex_formats::TL*)backend::ref().vertex.lock( 4, sizeof(vertex_formats::TL), offset);
	pv->set( 0,	  t_h, 0, 1.0, color, p0.x, p1.y); pv++;
	pv->set( 0,	  0,   0, 1.0, color, p0.x, p0.y); pv++;
	pv->set( t_w, t_h, 0, 1.0, color, p1.x, p1.y); pv++;
	pv->set( t_w, 0,   0, 1.0, color, p1.x, p0.y); pv++;
	backend::ref().vertex.unlock();

	context->m_g_quad_uv->apply();

	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, offset);
}

void system_renderer::draw_debug_terrain( )
{
	if (!is_effects_ready())
		return;


	backend::ref().set_render_targets	( 
		&*m_renderer_context->m_targets->m_rt_position,
		&*m_renderer_context->m_targets->m_rt_normal,
		&*m_renderer_context->m_targets->m_rt_tangents,
		0
	);
	backend::ref().clear_render_targets	(
		math::color(0.0f, 0.0f, 0.0f, 1.0f),
		math::color(0.0f, 0.0f, 0.0f, 1.0f),
		math::color(0.0f, 0.0f, 0.0f, 1.0f),
		math::color(0.0f, 0.0f, 0.0f, 1.0f)
	);
	backend::ref().reset_depth_stencil_target();
	backend::ref().clear_depth_stencil( D3D_CLEAR_STENCIL, 1.0f, 0);
	
//////////////////////////////////////////////////////////////////////////
	typedef render::vector<terrain_render_model_instance_ptr>		terrain_ptr_cells;
	typedef terrain_ptr_cells::const_iterator		terrain_ptr_cells_cit;

	terrain_ptr_cells									terrain_cells;

////// Rendering terrain cells ///////////////////////////////////////////

	if (options::ref().m_enabled_draw_terrain && m_renderer_context->scene()->terrain())
	{
		m_renderer_context->scene()->select_terrain_cells( m_renderer_context->get_culling_vp(), terrain_cells);

		terrain_ptr_cells_cit it_tr = terrain_cells.begin();
		terrain_ptr_cells_cit en_tr = terrain_cells.end();

		m_renderer_context->set_w( math::float4x4().identity() );

		for( ; it_tr != en_tr; ++it_tr)
		{
			// TODO: remove this check
			if (!(*it_tr)->m_terrain_model->effect())
				continue;
			
			(*it_tr)->m_terrain_model->effect()->apply(0);
			//(*it_tr)->m_terrain_model->render_geometry().geom->apply();
			
			float3	start_corner( 0.f, 0.f, 0.f);
			start_corner = (*it_tr)->m_terrain_model->transform().transform_position( start_corner);

			float3 distance_vector = start_corner + float3( (*it_tr)->m_terrain_model->physical_size()/2, 0, -(*it_tr)->m_terrain_model->physical_size()/2);
			distance_vector -= m_renderer_context->get_view_pos();

			u32 primitive_count = 0;
			if( distance_vector.length() > 150)
			{
				m_renderer_context->scene()->terrain()->m_grid_geom_1->apply();
				primitive_count = m_renderer_context->scene()->terrain()->m_grid_geom_1->intex_buffer()->size()/(sizeof(u16));
			}
			else
			{
				m_renderer_context->scene()->terrain()->m_grid_geom_0->apply();
				primitive_count = m_renderer_context->scene()->terrain()->m_grid_geom_0->intex_buffer()->size()/(sizeof(u16));
			}
			
//			backend::ref().set_ps_constant( m_object_transparency_scale_parameter, 1.0f);
			backend::ref().set_vs_constant( m_c_start_corner, float4( start_corner, 0));
//			backend::ref().set_ps_constant( m_c_start_corner, float4( start_corner, 0));
/*			
			backend::ref().set_ps_constant(
				m_far_fog_color_and_distance,
				float4(
					m_renderer_context->scene_view()->post_process_parameters().environment_far_fog_color,
					m_renderer_context->scene_view()->post_process_parameters().environment_far_fog_distance
				)
			);
			
			backend::ref().set_ps_constant(
				m_near_fog_distance,
				m_renderer_context->scene_view()->post_process_parameters().environment_near_fog_distance
			);
			
			backend::ref().set_ps_constant(
				m_ambient_color, 
				float4(
					m_renderer_context->scene_view()->post_process_parameters().environment_ambient_color,
					0
				)
			);
*/			
			// TODO: remove skylight from sun pass, add the skylight_pass
//			backend::ref().set_ps_constant( m_c_environment_skylight_upper_color, m_renderer_context->get_scene_view()->post_process_parameters().environment_skylight_upper_color );
//			backend::ref().set_ps_constant( m_c_environment_skylight_lower_color, m_renderer_context->get_scene_view()->post_process_parameters().environment_skylight_lower_color );
//			backend::ref().set_ps_constant( m_c_environment_skylight_parameters, m_renderer_context->get_scene_view()->post_process_parameters().environment_skylight_parameters );

//			if (tech_index==0)
//				backend::ref().set_stencil_ref(all_geometry_type + terrain_geometry_type);
//
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, primitive_count/*(*it_tr)->m_terrain_model->render_geometry().primitive_count*3*/, 0, 0);
///			statistics::ref().visibility_stat_group.num_triangles.value += primitive_count / 3;
		}
	}
		
	if (m_terrain_debug_material.c_ptr() &&
		m_terrain_debug_material->get_material_effects().stage_enable[debug_post_process_render_stage] && 
		m_terrain_debug_material->get_material_effects().m_effects[debug_post_process_render_stage])
	{
		m_terrain_debug_material->get_material_effects().m_effects[debug_post_process_render_stage]->apply();
		fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
	}


	/*
	backend::ref().set_render_targets	( &*m_renderer_context->m_targets->m_rt_position, &*m_renderer_context->m_targets->m_rt_normal, &*m_renderer_context->m_targets->m_rt_color, &*m_renderer_context->m_targets->m_rt_emissive);
	backend::ref().reset_depth_stencil_target();
	backend::ref().clear_depth_stencil( D3D_CLEAR_STENCIL, 1.0f, 0);
	backend::ref().clear_render_targets	( math::color( 0, 0, 0, 0));
	
	typedef render::vector<terrain_render_model_instance_ptr>		terrain_ptr_cells;
	typedef terrain_ptr_cells::const_iterator		terrain_ptr_cells_cit;
	
	terrain_ptr_cells									terrain_cells;
	
	if( options::ref().m_enabled_draw_terrain && m_renderer_context->scene()->terrain())
	{
		m_renderer_context->scene()->select_terrain_cells( m_renderer_context->get_culling_vp(), terrain_cells);
		
		terrain_ptr_cells_cit it_tr = terrain_cells.begin();
		terrain_ptr_cells_cit en_tr = terrain_cells.end();
		
		m_renderer_context->set_w( math::float4x4().identity() );
		
		for( ; it_tr != en_tr; ++it_tr)
		{
			(*it_tr)->m_terrain_model->effect()->apply(0);
			//(*it_tr)->m_terrain_model->render_geometry().geom->apply();
			
			float3	start_corner( 0.f, 0.f, 0.f);
			start_corner = (*it_tr)->m_terrain_model->transform().transform_position( start_corner);
			
			float3 distance_vector = start_corner + float3( (*it_tr)->m_terrain_model->physical_size()/2, 0, -(*it_tr)->m_terrain_model->physical_size()/2);
			distance_vector -= m_renderer_context->get_view_pos();
			
			u32 primitive_count = 0;
			if( distance_vector.length() > 150)
			{
				m_renderer_context->scene()->terrain()->m_grid_geom_1->apply();
				primitive_count = m_renderer_context->scene()->terrain()->m_grid_geom_1->intex_buffer()->size()/(sizeof(u16));
			}
			else
			{
				m_renderer_context->scene()->terrain()->m_grid_geom_0->apply();
				primitive_count = m_renderer_context->scene()->terrain()->m_grid_geom_0->intex_buffer()->size()/(sizeof(u16));
			}
			
			backend::ref().set_vs_constant( m_c_start_corner, float4( start_corner, 0));
			backend::ref().set_ps_constant( m_c_start_corner, float4( start_corner, 0));
			
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, primitive_count, 0, 0);
		}
	}
	*/
// 	if (m_terrain_debug_material_effects.stage_enable[debug_post_process_render_stage] && m_terrain_debug_material_effects.m_effects[debug_post_process_render_stage])
// 	{
// 		m_terrain_debug_material_effects.m_effects[debug_post_process_render_stage]->apply();
// 		fill_surface(m_renderer_context->m_targets->m_rt_present, m_renderer_context);
// 	}
	
	//backend::ref().set_render_targets(&*m_renderer_context->m_targets->m_rt_present,0,0,0);
	//m_sh_copy_image->apply(effect_copy_image::copy_rewrite);
}


void system_renderer::set_model_ghost_mode( polymorph_vector_base<render_model_instance> const& /*render_models*/, bool /*value*/)
{
	if (!is_effects_ready())
		return;
	
	//for (polymorph_vector_base<render_model_instance>::iterator it=render_models.begin(); it!=render_models.end(); ++it)
	//{
	//	render_model_instance* instance = (*it);
	//	
	//	if (value)
	//	{
	//		m_render_model_to_material[instance] = instance->m_material_effects;
	//		
	//		material_effects temp_material;
	//		temp_material.m_material = instance->m_material_effects.m_material;
	//		instance->m_material_effects = temp_material;
	//		
	//		instance->m_material_effects.m_effects[forward_render_stage]	 = m_editor_model_ghost_shader;
	//		instance->m_material_effects.stage_enable[forward_render_stage] = true;
	//	}
	//	else
	//	{
	//		if (m_render_model_to_material.find(instance)!=m_render_model_to_material.end())
	//		{
	//			instance->m_material_effects = m_render_model_to_material[instance];
	//			m_render_model_to_material.erase(instance);
	//		}
	//	}
	//}
}

void system_renderer::draw_ghost_render_models	( render_surface_instances& render_models)
{
	if (!is_effects_ready())
		return;
	
	PIX_EVENT( draw_render_models_selection);
	
	float3 view_pos = m_renderer_context->get_v_inverted().c.xyz();
	
	for (render_surface_instances::iterator it=render_models.begin(); it!=render_models.end(); ++it)
	{
		render_surface_instance& instance = *(*it);
		
		float4x4 selected_transform = *instance.m_transform;
		m_renderer_context->set_w( selected_transform);
		
		float4 old_current_selection_color = m_current_selection_color;
		m_current_selection_color = m_ghost_model_color;
		
		m_editor_selection_shader[instance.m_render_surface->get_vertex_input_type()]->apply();
		instance.set_constants();
		instance.m_render_surface->m_render_geometry.geom->apply();
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance.m_render_surface->m_render_geometry.primitive_count*3, 0, 0);
		
		m_current_selection_color = old_current_selection_color;
	}
}

void system_renderer::draw_render_models_selection	( render_surface_instances& render_models)
{
	if (!is_effects_ready())
		return;
	
	PIX_EVENT( draw_render_models_selection);
	
	float3 view_pos = m_renderer_context->get_v_inverted().c.xyz();

	math::aabb draw_box = math::create_invalid_aabb();

	for (render_surface_instances::iterator it=render_models.begin(), it_end = render_models.end(); it != it_end; ++it)
	{
		render_surface_instance&	instance = *(*it);
		draw_box.modify				( math::aabb( instance.m_render_surface->m_aabbox ).modify( *instance.m_transform ) );
		
		/*
		float4x4 selected_transform = *instance.m_transform;
		//const float shift_factor = 0.001f;
		//selected_transform.c.xyz() += ( ( view_pos - (*instance->m_transform).c.xyz()) * shift_factor );
		m_renderer_context->set_w( selected_transform);
		
		//m_notexture_shader->apply();
		m_editor_selection_shader[instance.m_render_surface->get_vertex_input_type()]->apply();
		instance.set_constants();
		m_current_selection_color = m_selection_color * xray::math::pow(xray::math::abs(xray::math::cos(m_renderer_context->m_current_time * m_selection_rate)), 0.5f);
		
		instance.m_render_surface->m_render_geometry.geom->apply();
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance.m_render_surface->m_render_geometry.primitive_count*3, 0, 0);
		*/
	}

		
	m_current_selection_color		= m_selection_color * xray::math::pow(xray::math::abs(xray::math::cos(m_renderer_context->m_current_time * m_selection_rate)), 0.5f);
	draw_aabb						(
		draw_box,
		math::color(
			m_current_selection_color.x, 
			m_current_selection_color.y, 
			m_current_selection_color.z, 
			1.0f
		)
	);
}

void system_renderer::draw_system_render_models	( render_surface_instances const& render_models)
{
	if (!is_effects_ready())
		return;
	
	PIX_EVENT( draw_system_render_models);

	for (render_surface_instances::const_iterator it=render_models.begin(); it!=render_models.end(); ++it)
	{
		render_surface_instance const& instance = *(*it);
		
		m_renderer_context->set_w( *instance.m_transform);
		
		instance.m_render_surface->get_material_effects().m_effects[forward_render_stage]->apply();
		instance.m_render_surface->m_render_geometry.geom->apply();
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, instance.m_render_surface->m_render_geometry.primitive_count*3, 0, 0);
	}
}

void system_renderer::draw_particle_system_instance_selections ( render::vector<particle::particle_system_instance_ptr> const& instances)
{
	if ( instances.empty() )
		return;

	if ( !is_effects_ready() )
		return;

	if ( !m_renderer_context->scene() )
		return;
	
	if ( !m_renderer_context->scene()->particle_world() )
		return;
	
	m_current_selection_color							=	m_selection_color * 
															xray::math::pow(xray::math::abs(xray::math::cos(m_renderer_context->m_current_time * m_selection_rate)), 0.5f);
	
	
	// TODO: LODs?
	
	
	xray::particle::world* part_world					= m_renderer_context->scene()->particle_world();
	
	for (render::vector<particle::particle_system_instance_ptr>::const_iterator instance_it = instances.begin(); 
																				instance_it != instances.end(); 
																				++instance_it)
	{
		particle::render_particle_emitter_instances_type emitters( g_allocator );
		
		part_world->get_render_emitter_instances(*instance_it, emitters);
		
		for (particle::render_particle_emitter_instances_type::const_iterator it=emitters.begin(); it!=emitters.end(); ++it)
		{
			render::render_particle_emitter_instance*	instance		= static_cast< render::render_particle_emitter_instance* >( *it );
			/*math::aabb		draw_box;
			draw_box		= instance->get_aabb();
			draw_box.modify	(instance->transform());
			
			draw_aabb		(draw_box, 
				math::color(
					m_current_selection_color.x, 
					m_current_selection_color.y, 
					m_current_selection_color.z, 
					1.0f
				)
			);
			*/

			u32 const									num_particles	= instance->get_num_particles();
			
			if (!num_particles)
				continue;
			
			switch (instance->vertex_type()) {
				case xray::particle::particle_vertex_type_billboard:
					m_sh_particle_selection->apply(effect_particle_selection::billboard);
					break;
				
				case xray::particle::particle_vertex_type_billboard_subuv:
					m_sh_particle_selection->apply(effect_particle_selection::billboard_subuv);
					break;
				
				case xray::particle::particle_vertex_type_trail:
				case xray::particle::particle_vertex_type_beam:
					m_sh_particle_selection->apply(effect_particle_selection::beamtrail);
					break;
			}
			
			particle_shader_constants::ref().set(
				m_renderer_context->get_v_inverted().transform_direction(float3(0,1000,0)).normalize(),
				m_renderer_context->get_v_inverted().transform_direction(float3(1000,0,0)).normalize(),
				m_renderer_context->get_v_inverted().lines[3].xyz(),
				instance->locked_axis(),
				instance->screen_alignment()
				);
			particle_shader_constants::ref().set_time(m_renderer_context->m_current_time);
			
			m_renderer_context->set_w( instance->transform() );
			
			instance->render(m_renderer_context->get_v_inverted().lines[3].xyz(), num_particles);
		}
	}
}

void system_renderer::draw_speedtree_instance_selections (render::vector<render::speedtree_instance_ptr> const& instances)
{
	if (!is_effects_ready())
		return;
	
	if (!instances.size())
		return;
	
	m_current_selection_color							=	m_selection_color * 
															xray::math::pow(xray::math::abs(xray::math::cos(m_renderer_context->m_current_time * m_selection_rate)), 0.5f);
	
	for (render::vector<render::speedtree_instance_ptr>::const_iterator it = instances.begin(); it != instances.end(); ++it)
	{
		math::aabb			draw_box(math::create_zero_aabb());
		draw_box			= it->c_ptr()->m_speedtree_tree_ptr->m_bbox;
		draw_box.modify		(it->c_ptr()->m_transform);
		
		draw_aabb			(draw_box, 
			math::color(
				m_current_selection_color.x, 
				m_current_selection_color.y, 
				m_current_selection_color.z, 
				1.0f
			)
		);
	}
	
	/*
	speedtree_forest::tree_render_info_array_type		visible_trees;
	
	m_renderer_context->scene()->get_speedtree_forest()->get_visible_tree_components(m_renderer_context, instances, true, visible_trees);
	
	for (speedtree_forest::tree_render_info_array_type::iterator it=visible_trees.begin(); it!=visible_trees.end(); ++it)
	{
		switch (it->tree_component->get_geometry_type())
		{
		case SpeedTree::GEOMETRY_TYPE_BRANCHES:
			m_speedtree_selection_shader->apply(effect_speedtree_selection::branch);
			break;
		case SpeedTree::GEOMETRY_TYPE_FRONDS:
			m_speedtree_selection_shader->apply(effect_speedtree_selection::frond);
			break;
		case SpeedTree::GEOMETRY_TYPE_LEAF_CARDS:
			m_speedtree_selection_shader->apply(effect_speedtree_selection::leafcard);
			break;
		case SpeedTree::GEOMETRY_TYPE_LEAF_MESHES:
			m_speedtree_selection_shader->apply(effect_speedtree_selection::leafmesh);
			break;
		case SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS:
			m_speedtree_selection_shader->apply(effect_speedtree_selection::billboard);
			break;
		default:
			NODEFAULT();
		}
		
		m_renderer_context->scene()->get_speedtree_forest()->get_speedtree_wind_parameters().set	(it->tree_component->m_parent->GetWind());
		m_renderer_context->scene()->get_speedtree_forest()->get_speedtree_common_parameters().set	(m_renderer_context, it->tree_component, m_renderer_context->get_v_inverted().c.xyz());
		if (it->instance)
			m_renderer_context->set_w																(
				m_renderer_context->scene()->get_speedtree_forest()->get_instance_transform(*it->instance)
			);
		else
			m_renderer_context->set_w																(math::float4x4().identity());

		
		if (it->tree_component->get_geometry_type()==SpeedTree::GEOMETRY_TYPE_VERTICAL_BILLBOARDS)
		{
			m_renderer_context->scene()->get_speedtree_forest()->get_speedtree_billboard_parameters().set(m_renderer_context, it->tree_component);
			it->tree_component->m_render_geometry.geom->apply();
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, it->tree_component->m_render_geometry.index_count, 0, 0);
		}
		else
		{
			m_renderer_context->scene()->get_speedtree_forest()->get_speedtree_tree_parameters().set(it->tree_component, it->instance, it->instance_lod);
			it->tree_component->m_render_geometry.geom->apply();
			backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, it->lod->num_indices, it->lod->start_index, 0);
		}
	}
	*/
}

void system_renderer::setup_scene_view		( scene_view_ptr view_ptr)
{
	m_renderer_context->set_scene_view		( view_ptr);
}

void system_renderer::setup_render_output_window	( render_output_window_ptr in_output_window, viewport_type const& viewport)
{
	render_output_window* output_window = (render_output_window*)in_output_window.c_ptr();
	
	m_renderer_context->set_target_context		( &output_window->target_context() );
	backend::ref().set_render_output			( output_window->render_output() );
	backend::ref().reset_depth_stencil_target	();
	
	backend::ref().set_render_targets			( &*m_renderer_context->m_targets->m_rt_present,0, 0, 0);
	backend::ref().reset_depth_stencil_target	( );

	viewport_type res_viewport( math::float2( 0.f, 0.f ), math::float2( 1.f, 1.f ));

	R_ASSERT			( viewport.width() );
	R_ASSERT			( viewport.height() );

	res_viewport.left	= math::max( res_viewport.left,		viewport.left);
	res_viewport.right	= math::min( res_viewport.right,	viewport.right);
	res_viewport.top	= math::max( res_viewport.top,		viewport.top);
	res_viewport.bottom	= math::min( res_viewport.bottom,	viewport.bottom);

	u32 const window_width	= backend::ref().target_width( );
	u32 const window_height	= backend::ref().target_height( );
	
#pragma warning(push)
#pragma warning(disable:4244)
	D3D_VIEWPORT d3d_viewport	= { window_width * res_viewport.left, window_height * res_viewport.top, window_width * res_viewport.width(), window_height * res_viewport.height(), 0.f, 1.f};
#pragma warning(pop)
	backend::ref().set_viewport	( d3d_viewport);
}

void system_renderer::set_selection_parameters		( xray::math::float4 in_selection_color, float in_selection_rate )
{
	m_selection_color = in_selection_color;
	m_selection_rate  = in_selection_rate;
}

} // namespace render
} // namespace xray


