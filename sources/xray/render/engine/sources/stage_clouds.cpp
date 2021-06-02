////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/pix_event_wrapper.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/render_target.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/untyped_buffer.h>
#include <xray/render/core/res_effect.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/core/options.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_descriptor.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/destroy_data_helper.h>
#include "statistics.h"
#include "scene_view.h"
#include "effect_copy_image.h"
#include "material.h"
#include "shared_names.h"
#include "lights_db.h"
#include "light.h"
#include "vertex_formats.h"
#include "stage_clouds.h"
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include "scene.h"
#include "render_model.h"

#include "effect_clouds.h"

namespace xray {
namespace render {

const D3D_INPUT_ELEMENT_DESC sliced_cube_vertex_layout[] = 
{
	{"POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0}, // grid normalized x, y, z
};

clouds_sliced_cube_geometry::clouds_sliced_cube_geometry(u32 const in_num_cells):
	m_slices(in_num_cells),
	m_stride(sizeof(vertex_type))
{
	u32 const		num_vertices				=	m_slices * 4;
	u32 const		num_indices					=	m_slices * 6;
	
	vertex_type*	vertices					=	(vertex_type*)ALLOCA(m_stride * num_vertices);
	u16*			indices						=	(u16*)ALLOCA(sizeof(u16) * num_indices);
	
	vertex_type*	vertices_begin				=	vertices;
	u16*			indices_begin				=	indices;
	
	for (u32 slice_index = 0; slice_index < m_slices; slice_index++)
	{
		float const slice_z						=	float(slice_index) / float(m_slices);
		
		(new(vertices)vertex_type)->position	=	10.0f * float4(0.0f, 0.0f, slice_z, float(slice_index)); vertices++;
		(new(vertices)vertex_type)->position	=	10.0f * float4(1.0f, 0.0f, slice_z, float(slice_index)); vertices++;
		(new(vertices)vertex_type)->position	=	10.0f * float4(1.0f, 1.0f, slice_z, float(slice_index)); vertices++;
		(new(vertices)vertex_type)->position	=	10.0f * float4(0.0f, 1.0f, slice_z, float(slice_index)); vertices++;
		
		u16 const offset						=	static_cast<u16>(slice_index * 4);
		
		// clockwise
		*(indices++)							=	0 + offset;
		*(indices++)							=	1 + offset;
		*(indices++)							=	2 + offset;
		*(indices++)							=	0 + offset;
		*(indices++)							=	2 + offset;
		*(indices++)							=	3 + offset;
	}
	
	m_vertext_declaration						=	resource_manager::ref().create_declaration(sliced_cube_vertex_layout);
	m_vertex_buffer								=	resource_manager::ref().create_buffer(
														num_vertices * m_stride, 
														vertices_begin, 
														enum_buffer_type_vertex, 
														false
													);
	m_index_buffer								=	resource_manager::ref().create_buffer(
														num_indices * sizeof(u16), 
														indices_begin, 
														enum_buffer_type_index,  
														false
													);
}

void clouds_sliced_cube_geometry::draw()
{
	backend::ref().set_declaration				(m_vertext_declaration.c_ptr());
	backend::ref().set_vb						(m_vertex_buffer.c_ptr(), m_stride);
	backend::ref().set_ib						(m_index_buffer.c_ptr(), 0);	
	backend::ref().render_indexed				(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_slices * 6, 0, 0);
}





stage_clouds::stage_clouds(renderer_context* context):
	stage(context),
	m_clouds_sliced_cube_geometry(32)
{	
	u32 const size								=	32;
	
	for (u32 i = 0; i < 2; i++)
	{
		m_3d_clouds_density_rt[i]				=	resource_manager::ref().create_volume_render_target(
														NULL, 
														size, 
														size, 
														size, 
														DXGI_FORMAT_R8G8B8A8_UNORM, 
														enum_rt_usage_render_target, 
														D3D_USAGE_DEFAULT
													);
		m_3d_clouds_density_texture[i]			=	m_3d_clouds_density_rt[i]->get_texture();
	}
	m_3d_lockable_texture						=	resource_manager::ref().create_texture3d(
														NULL, 
														size, 
														size, 
														size, 
														0, 
														DXGI_FORMAT_R8G8B8A8_UNORM, 
														D3D_USAGE_DYNAMIC, 
														1
													);
	
	union data_type
	{
		struct
		{
			u8	r;
			u8	g;
			u8	b;
			u8	a;
		};
		u32 data;
	}; // union data_type
	
	u32 row_pitch								=	0;
	
	u32* data = (u32*)m_3d_lockable_texture->map3D(D3D_MAP_WRITE_DISCARD, 0, row_pitch);
	
	if (data)
	{
		for (u32 z = 0; z < size; z++)
		{
			for (u32 y = 0; y < size; y++)
			{
				for (u32 x = 0; x < size; x++)
				{
					u32& value_ref				=	*(data + x + y * size + z * size * size);
					value_ref					=	math::color(float(x) / 32.0f, float(y) / 32.0f, float(z) / 32.0f, 1.0f).m_value;
				}
			}
		}
	}
	else
	{
		ASSERT(0);
	}
	
	m_3d_lockable_texture->unmap3D				(0);
	
	effect_manager::ref().create_effect<effect_clouds>(&m_clouds_effect);
	
	m_enabled									=	options::ref().m_enabled_clouds_stage;
}

bool stage_clouds::is_effects_ready() const
{
	return m_clouds_effect.c_ptr() != NULL;
}

stage_clouds::~stage_clouds()
{
	
}

void stage_clouds::execute()
{
	PIX_EVENT(stage_clouds);
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
//	backend::ref().set_render_targets			(&*m_context->m_targets->m_rt_generic_0, NULL, NULL, NULL);
//	backend::ref().reset_depth_stencil_target	();
//	
//	m_clouds_effect->apply						();
//	backend::ref().set_ps_texture				("t_clouds", &*m_3d_lockable_texture);
//	m_clouds_sliced_cube_geometry.draw			();
//	
//	backend::ref().reset_render_targets			();
}

} // namespace render
} // namespace xray
