////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "grass.h"
#include "material_manager.h"
#include "material_effects.h"
#include <xray/render/core/res_effect.h>
#include "renderer_context.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/resource_manager.h>

#include <xray/math_randoms_generator.h>

static xray::math::random32 randomizer(100);
static u32 const random_max = 65535;

float random_float(float min_value, float max_value)
{
	float alpha = float(randomizer.random(random_max)) / float(random_max);
	return min_value * (1 - alpha) + max_value * alpha;
}

namespace xray {
namespace render {

grass_cell_mesh::grass_cell_mesh(grass_world* g): 
	m_grass_world(g)
{
	
}

void grass_cell_mesh::render(renderer_context* context, u32 stage_index)
{
	material_effects& effects = m_grass_world->get_layer(layer_index)->layer_effects;
	
	if (effects.stage_enable[stage_index] && effects.m_effects[stage_index])
	{
		effects.m_effects[stage_index]->apply();
		context->set_w( float4x4().identity() );
		geometry->apply();
		backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, index_count, 0, 0);
	}
}

grass_cell::grass_cell(grass_world* g)
{
	position			= math::float2(0.0f, 0.0f);
	height_min_max		= math::float2(0.0f, 1.0f);
	next_visible_cell	= 0;
	m_grass_world		= g;
}

grass_cell::~grass_cell()
{
	for (render::vector<grass_cell_mesh*>::const_iterator it=meshes.begin(); it!=meshes.end(); ++it)
	{
		grass_cell_mesh* mesh = *it;
		DELETE(mesh);
	}
	for (render::vector<grass_cell_mesh*>::const_iterator it=meshes_lod_1.begin(); it!=meshes_lod_1.end(); ++it)
	{
		grass_cell_mesh* mesh = *it;
		DELETE(mesh);
	}
}

grass_world::~grass_world()
{
	for (cells_array_type::const_iterator it=m_cells.begin(); it!=m_cells.end(); it++)
	{
		grass_cell* cell = *it;
		DELETE(cell);
	}
	
	for (grass_layer_array_type::const_iterator it=m_grass_layers.begin(); it!=m_grass_layers.end(); it++)
	{
		grass_layer* layer = *it;
		DELETE(layer);
	}
}

grass_layer* grass_world::get_layer(u32 layer_index)
{
	ASSERT_CMP(layer_index, <, m_grass_layers.size());
	
	return m_grass_layers[layer_index];
}

grass_layer* grass_world::add_grass_layer()
{
	grass_layer* layer = NEW(grass_layer);
	m_grass_layers.push_back(layer);
	return layer;
}

grass_cell* grass_world::add_cell(float pos_x, float pos_y, float height_min, float height_max)
{
	grass_cell* cell		= NEW(grass_cell)(this);
	cell->height_min_max	= math::float2(height_min, height_max);
	cell->position			= math::float2(pos_x, pos_y);
	
	m_cells.push_back(cell);
	
	return cell;
}

const D3D_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,  D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,			0, 24, D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT,			0, 32, D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 40, D3D_INPUT_PER_VERTEX_DATA, 0 },
};

#pragma pack(push,1)
struct grass_billboard_vertex
{
	grass_billboard_vertex(math::float3 in_location, math::float3 const& in_offset, math::float2 const& in_rotation, math::float2 const& in_uv, math::float4 const& in_color)
	{
		location		= in_location;;
		offset			= in_offset;
		rotation		= in_rotation;;
		uv				= in_uv;
		color			= in_color;
	}
	math::float3 location;
	math::float3 offset;
	math::float2 rotation;
	math::float2 uv;
	math::float4 color;
};
#pragma pack(pop)

grass_world::grass_world()
{
	//m_view_to_local_parameter = backend::ref().register_constant_host( "view_to_local_matrix", rc_float );
}

grass_cell_mesh* grass_world::create_base_grass_mesh(s32 layer_index, render::vector<base_grass_mesh_desc> in_desc)
{
	ref_declaration decl = resource_manager::ref().create_declaration( layout );
	
	grass_cell_mesh* mesh = NEW(grass_cell_mesh)(this);
	mesh->layer_index	  = layer_index;
	
	render::vector<grass_billboard_vertex>	vertices;
	render::vector<u16>						indices;
	
	u32 index = 0;
	for (render::vector<base_grass_mesh_desc>::iterator it=in_desc.begin(); it!=in_desc.end(); ++it)
	{
		float3 offset_x = math::float3(it->size.x, 0.0f, 0.0f);
		float3 offset_y = math::float3(0.0f, it->size.y, 0.0f);
		
		float4 clr		= float4( random_float(0.0f,1.0f), random_float(0.0f,1.0f), random_float(0.0f,1.0f), 1.0f);
		
		float2 rotation	= float2(math::sin(it->rotation), math::cos(it->rotation));
		
		float4 const& uv	= it->uv;
		
		vertices.push_back( grass_billboard_vertex(it->position, -offset_x,				rotation, math::float2(uv.x,uv.w), clr) ); // 01
		vertices.push_back( grass_billboard_vertex(it->position,  offset_x,				rotation, math::float2(uv.z,uv.w), clr) ); // 11
		vertices.push_back( grass_billboard_vertex(it->position,  offset_x + offset_y,	rotation, math::float2(uv.z,uv.y), clr) ); // 10
		vertices.push_back( grass_billboard_vertex(it->position, -offset_x + offset_y,	rotation, math::float2(uv.x,uv.y), clr) ); // 00
		
		u16 i_index = u16(index) * 4;
		
		indices.push_back(i_index + 0);
		indices.push_back(i_index + 1);
		indices.push_back(i_index + 2);
		
		indices.push_back(i_index + 0);
		indices.push_back(i_index + 2);
		indices.push_back(i_index + 3);
		
		index++;
	}
	
	mesh->index_count				= indices.size();
	mesh->primitive_count			= mesh->index_count/3;
	untyped_buffer_ptr ib			= resource_manager::ref().create_buffer( mesh->index_count*sizeof(u16), 
		&indices[0],
		enum_buffer_type_index, 
		false
		);
	
	mesh->vertex_count				= vertices.size();
	bool dynamic					= true;
	u32 stride						= sizeof(grass_billboard_vertex);
	untyped_buffer_ptr vb			= resource_manager::ref().create_buffer	(	mesh->vertex_count * stride, 
		&vertices[0],
		enum_buffer_type_vertex, 
		dynamic
		);
	
	mesh->geometry					= resource_manager::ref().create_geometry( &*decl, stride, *vb, *ib);
	
	return mesh;
}

grass_cell_mesh* grass_world::create_base_grassbillboard_mesh(s32 layer_index, render::vector<base_grass_mesh_desc> in_desc)
{
	ref_declaration decl = resource_manager::ref().create_declaration( layout );
	
	grass_cell_mesh* mesh = NEW(grass_cell_mesh)(this);
	mesh->layer_index	  = layer_index;
	
	render::vector<grass_billboard_vertex>	vertices;
	render::vector<u16>						indices;
	
	u32 index = 0;
	for (render::vector<base_grass_mesh_desc>::iterator it=in_desc.begin(); it!=in_desc.end(); ++it)
	{
		float3 size = math::float3(it->size.x, it->size.y, 0.0f);
		
		float4 clr		= float4( random_float(0.0f,1.0f), random_float(0.0f,1.0f), random_float(0.0f,1.0f), 1.0f);
		
		float4 const& uv	= it->uv;
		
		vertices.push_back( grass_billboard_vertex(it->position, size, float2(0.0f, 1.0f), math::float2(uv.x,uv.w), clr) ); // 01
		vertices.push_back( grass_billboard_vertex(it->position, size, float2(1.0f, 1.0f), math::float2(uv.z,uv.w), clr) ); // 11
		vertices.push_back( grass_billboard_vertex(it->position, size, float2(1.0f, 0.0f), math::float2(uv.z,uv.y), clr) ); // 10
		vertices.push_back( grass_billboard_vertex(it->position, size, float2(0.0f, 0.0f), math::float2(uv.x,uv.y), clr) ); // 00
		
		u16 i_index = u16(index) * 4;
		
		indices.push_back(i_index + 0);
		indices.push_back(i_index + 1);
		indices.push_back(i_index + 2);
		
		indices.push_back(i_index + 0);
		indices.push_back(i_index + 2);
		indices.push_back(i_index + 3);
		
		index++;
	}
	
	mesh->index_count				= indices.size();
	mesh->primitive_count			= mesh->index_count/3;
	untyped_buffer_ptr ib			= resource_manager::ref().create_buffer( mesh->index_count*sizeof(u16), 
		&indices[0],
		enum_buffer_type_index, 
		false
		);
	
	mesh->vertex_count				= vertices.size();
	bool dynamic					= true;
	u32 stride						= sizeof(grass_billboard_vertex);
	untyped_buffer_ptr vb			= resource_manager::ref().create_buffer	(	mesh->vertex_count * stride, 
		&vertices[0],
		enum_buffer_type_vertex, 
		dynamic
		);
	
	mesh->geometry					= resource_manager::ref().create_geometry( &*decl, stride, *vb, *ib);
	
	return mesh;
}

void grass_world::render(renderer_context* context, u32 stage_index) const
{
	//backend::ref().set_vs_constant(m_view_to_local_parameter, transpose(context->get_v_inverted()) );
	grass_cell* cell = get_visible_cells(context->get_culling_vp());
	while (cell)
	{
		float3 camera_postion_xz	= context->get_view_pos();
		camera_postion_xz.y			= 0.0f;
		bool lod0 = math::length(camera_postion_xz - float3(cell->position.x, 0.0f, cell->position.y)) < float(grass_cell::cell_size_xy) * 2.0f;
		
		if (lod0)
		{
			for (render::vector<grass_cell_mesh*>::const_iterator it=cell->meshes.begin(); it!=cell->meshes.end(); ++it)
				(*it)->render(context, stage_index);
		}
		else
		{
			for (render::vector<grass_cell_mesh*>::const_iterator it=cell->meshes_lod_1.begin(); it!=cell->meshes_lod_1.end(); ++it)
				(*it)->render(context, stage_index);
		}
		
		cell = cell->next_visible_cell;
	}
}

grass_cell* grass_world::get_visible_cells(math::float4x4 const& view_proj_matrix) const
{
	math::frustum view_frustum(view_proj_matrix);
	
	grass_cell* cell		= 0;
	for (render::vector<grass_cell*>::const_iterator it=m_cells.begin(); it!=m_cells.end(); it++)
	{
		math::aabb bound_box(math::create_aabb_min_max(
			float3((*it)->position.x, (*it)->height_min_max.x, (*it)->position.y),
			float3((*it)->position.x + float(grass_cell::cell_size_xy), (*it)->height_min_max.y, (*it)->position.y + float(grass_cell::cell_size_xy))
			));
		
		if (view_frustum.test_inexact(bound_box)==math::intersection_outside)
			continue;
		
		if (cell==0)
		{
			cell = *it;
		}
		else
		{
			grass_cell* next		= cell;
			cell					= *it;
			cell->next_visible_cell = next;
		}
	}
	return cell;
}

void grass_world::initialize_test_grass()
{
	resources::query_resource(
		"grass",
		xray::resources::material_class, 
		boost::bind(&grass_world::test_grass_data_loaded, this, _1), 
		render::g_allocator
		);
}

void grass_world::test_grass_data_loaded(xray::resources::queries_result& result)
{
	if (!result.is_successful())
		return;
	
	material_ptr loaded_material = static_cast_checked<material*>(result[0].get_unmanaged_resource().c_ptr());
	
	// Layer
	grass_layer* layer									= add_grass_layer();
	//material_manager::ref().initialize_material_effects (layer->layer_effects, loaded_material, grassmesh_vertex_input_type, true);

	layer												= add_grass_layer();
	//material_manager::ref().initialize_material_effects	(layer->layer_effects, loaded_material, grassbillboard_vertex_input_type, true);
	
	for (float cell_x=-200.0f; cell_x<200.0f; cell_x+=float(grass_cell::cell_size_xy))
	{
		for (float cell_y=-200.0f; cell_y<200.0f; cell_y+=float(grass_cell::cell_size_xy))
		{
			// Cell
			grass_cell* cell = add_cell(cell_x, cell_y, 0.0f, 1.0f);
			
			// Mesh
			render::vector<base_grass_mesh_desc> mesh_desc;
			render::vector<base_grass_mesh_desc> mesh_desc_lod_1;
			for (u32 i=0; i<1000; i++)
			{
				float3 position = float3(random_float(cell_x, cell_x + grass_cell::cell_size_xy), 0.0f, random_float(cell_y, cell_y + grass_cell::cell_size_xy));
				float2 size		= math::float2(1.0f, random_float(1.0f, 1.5f));
				
				u32	up_down		= u32(random_float(0.5, 0.75f));
				
				for (u32 k=0; k<3; k++)
				{
					base_grass_mesh_desc	mesh;
					mesh.position			= position;
					mesh.rotation			= float(k) * math::pi / 3.0f;
					mesh.size				= size;
					
					if (up_down==0)
						mesh.uv				= math::float4(0.0f, 0.0f, 1.0f, 0.5f);
					else
						mesh.uv				= math::float4(0.0f, 0.5f, 1.0f, 1.0f);
					
					mesh_desc.push_back		(mesh);
					
					if (k==0)
						mesh_desc_lod_1.push_back(mesh);
				}
			}
			cell->meshes.push_back( create_base_grass_mesh(0, mesh_desc) );
			cell->meshes_lod_1.push_back( create_base_grassbillboard_mesh(1, mesh_desc_lod_1) );
		}
	}

}

} // namespace render
} // namespace xray