////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_model_user.h"
#include <xray/render/core/decl_utils.h>
#include <xray/render/core/utils.h>
#include <xray/render/engine/model_format.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/untyped_buffer.h>
#include "material.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>

#pragma warning( push )
#pragma warning( disable : 4995 )
#include <d3dx9mesh.h>
#pragma warning( pop )

namespace xray {
namespace render {

void user_render_surface::material_ready( resources::queries_result& data, render::material_effects_instance_cook_data* cook_data, pstr material_name)
{
	DELETE						( cook_data );
	
	if (data.is_successful())
		set_material_effects	( static_cast_resource_ptr<material_effects_instance_ptr>(data[0].get_unmanaged_resource()), material_name);
	
	FREE						( material_name );
}

void user_render_model_instance::assign_surface( user_render_surface* surface )
{ 
	m_surface							= surface;
	m_surface_instance.m_render_surface = surface;
	m_surface_instance.m_transform		= &m_transform;
	m_surface_instance.m_parent			= this;
	m_surface_instance.m_visible		= true;

} 

void user_render_model_instance::get_surfaces( render_surface_instances& dest, bool visible_only )
{
	XRAY_UNREFERENCED_PARAMETER	( visible_only );
	dest.push_back				( &m_surface_instance );
};



const D3D_INPUT_ELEMENT_DESC layout_editable[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D_INPUT_PER_VERTEX_DATA, 0 }, // pos.xyz
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D_INPUT_PER_VERTEX_DATA, 0 },
};
struct vertex_editable
{
	float3 position;
	float2 uv;
};

void user_render_surface_editable::load_from_chunk_data( memory::chunk_reader& chunk )
{
	m_vertex_input_type			= render::null_vertex_input_type;
	m_aabbox.min				= float3(-10.0f, -10.0f, -10.0f);
	m_aabbox.max				= float3(10.0f, 10.0f, 10.0f);
	memory::reader reader		= chunk.open_reader( model_texture);
	
	fixed_string<256> material_name	= reader.r_string();
	
	reader						= chunk.open_reader( model_chunk_vertices);


	// The visuals are create with dynamic usage temporary.
	bool dynamic		= true;

	m_render_geometry.vertex_count		= reader.r_u32				();
	u32 stride							= sizeof(vertex_editable);
	untyped_buffer_ptr vb				= resource_manager::ref().create_buffer( 
		m_render_geometry.vertex_count*stride, 
		reader.pointer(), 
		enum_buffer_type_vertex, 
		dynamic);

//--- read indices here

	reader								= chunk.open_reader( model_chunk_indices);
	m_render_geometry.index_count		= reader.r_u32();
	m_render_geometry.primitive_count	= m_render_geometry.index_count/3;

	untyped_buffer_ptr ib				= resource_manager::ref().create_buffer( m_render_geometry.index_count*2, reader.pointer(), enum_buffer_type_index, false);

	ref_declaration decl				= resource_manager::ref().create_declaration( layout_editable );
	m_render_geometry.geom				= resource_manager::ref().create_geometry( &*decl, stride, *vb, *ib);

	m_vb								= &*vb;

	render::material_effects_instance_cook_data* cook_data = 
		NEW(render::material_effects_instance_cook_data)(render::null_vertex_input_type, NULL, false);
	
	resources::user_data_variant		user_data;
	user_data.set						(cook_data);
	
	pstr c_material_name				= ALLOC(char, 256);
	xray::memory::zero					(c_material_name, 256);
	xray::strings::copy					(c_material_name, 256, material_name.c_str());
	
	resources::query_resource			(
		material_name.c_str(), 
		resources::material_effects_instance_class, 
		boost::bind(
			&user_render_surface_editable::material_ready, 
			this, 
			_1,
			cook_data,
			c_material_name
		),
		g_allocator,
		&user_data
	);
}

const D3D_INPUT_ELEMENT_DESC layout_wire[] =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,  D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 24, D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "BINORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 36, D3D_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 48, D3D_INPUT_PER_VERTEX_DATA, 0 },
};

struct vertex_wire
{
	float3 position;
	float3 normal;
	float3 tangent;
	float3 binormal;
	float2 uv;
};

void user_render_surface_wire::load_from_chunk_data( memory::chunk_reader& chunk )
{
	m_vertex_input_type					= render::wires_vertex_input_type;
	
	memory::reader reader				= chunk.open_reader( model_texture);
	
	fixed_string<256> material_name		= reader.r_string();
	
	float const wire_width				= reader.r_float();
	
	reader								= chunk.open_reader( model_chunk_vertices);
	
	m_aabbox.zero						();
	
	u32 const s_num_vertices			= reader.r_u32();
	u32 const num_vertices				= s_num_vertices * 3;
	u32 const num_indices				= (s_num_vertices - 1) * 6 * 3;
	
	m_render_geometry.vertex_count		= num_vertices;
	u32 stride							= sizeof(vertex_wire);
	
	float3*			s_vertices			= (float3*)ALLOCA(sizeof(float3) * s_num_vertices);
	vertex_wire*	vertices			= (vertex_wire*)ALLOCA(sizeof(vertex_wire) * num_vertices);
	u16*			indices				= (u16*)ALLOCA(sizeof(u16) * num_indices);
	
	vertex_wire*	b_vertices			= vertices;
	u16*			b_indices			= indices;
	
	for (u32 vertex_index = 0; vertex_index < s_num_vertices; vertex_index++)
		s_vertices[vertex_index]		= reader.r_float3();
	
	ASSERT_CMP							(s_num_vertices, !=, 1);
	
	float u_coord						= 0.0f;
	
	for (u32 vertex_index = 0; vertex_index < s_num_vertices; vertex_index++)
	{
		float3 const source_pos			= s_vertices[vertex_index];
		
		float3 direction				= float3(0.0f, 0.0f, 0.0f);
		float3 to_next					= float3(0.0f, 0.0f, 0.0f);
		
		if (vertex_index + 1 == s_num_vertices)
			to_next						= s_vertices[vertex_index] - s_vertices[vertex_index - 1];
		else
			to_next						= s_vertices[vertex_index + 1] - s_vertices[vertex_index];
		
		direction						= math::normalize_safe(to_next, float3(0.0f, 0.0f, 0.0f));
		
		float3 const help_vector		= float3(0.0f, 1.0f, 0.0f);
		
		float3 const left_vector		= math::normalize(math::cross_product(direction, help_vector));
		float3 const up_vector			= math::normalize(math::cross_product(left_vector, direction));
		
		vertex_wire& A0					= vertices[vertex_index * 3 + 0];
		vertex_wire& A1					= vertices[vertex_index * 3 + 1];
		vertex_wire& A2					= vertices[vertex_index * 3 + 2];
		
		A0.position						= source_pos + 0.5f * wire_width * math::normalize_safe( up_vector, float3(0.0f, 0.0f, 0.0f));
		A1.position						= source_pos + 0.5f * wire_width * math::normalize_safe( left_vector - up_vector, float3(0.0f, 0.0f, 0.0f));
		A2.position						= source_pos + 0.5f * wire_width * math::normalize_safe(-left_vector - up_vector, float3(0.0f, 0.0f, 0.0f));
		
		m_aabbox.modify					(A0.position);
		m_aabbox.modify					(A1.position);
		m_aabbox.modify					(A2.position);
		
		A0.normal						= math::normalize(A0.position - source_pos);
		A1.normal						= math::normalize(A1.position - source_pos);
		A2.normal						= math::normalize(A2.position - source_pos);
		
		A0.tangent						= math::normalize(math::cross_product(A0.normal, direction));
		A1.tangent						= math::normalize(math::cross_product(A1.normal, direction));
		A2.tangent						= math::normalize(math::cross_product(A2.normal, direction));
		
		A0.binormal						= direction;
		A1.binormal						= direction;
		A2.binormal						= direction;
		
		A0.uv							= float2(u_coord, 0.0f);
		A1.uv							= float2(u_coord, 0.5f);
		A2.uv							= float2(u_coord, 0.1f);
		
		u_coord							+= math::length(to_next);
	}
	
	u16 const faces_indices[]			= {0, 1, 3, 1, 4, 3,   
										   0, 5, 2, 0, 3, 5,  
										   2, 4, 1, 2, 5, 4};
	
	for (u32 vertex_index = 0; vertex_index < s_num_vertices - 1; vertex_index++)
		for (u32 face_index = 0; face_index < array_size(faces_indices); face_index++)
			*indices++						= static_cast_checked<u16>(vertex_index * 3) + faces_indices[face_index];
	
	untyped_buffer_ptr vb				= resource_manager::ref().create_buffer( 
		num_vertices * stride, 
		b_vertices, 
		enum_buffer_type_vertex, 
		true
	);
	
	m_render_geometry.index_count		= num_indices;
	m_render_geometry.primitive_count	= num_indices / 3;
	
	untyped_buffer_ptr ib				= resource_manager::ref().create_buffer( sizeof(u16) * num_indices, b_indices, enum_buffer_type_index, false);
	
	ref_declaration decl				= resource_manager::ref().create_declaration( layout_wire );
	m_render_geometry.geom				= resource_manager::ref().create_geometry( &*decl, stride, *vb, *ib);
	
	render::material_effects_instance_cook_data* cook_data = 
		NEW(render::material_effects_instance_cook_data)(m_vertex_input_type, NULL, false);
	
	resources::user_data_variant		user_data;
	user_data.set						(cook_data);
	
	pstr c_material_name				= ALLOC(char, 256);
	xray::memory::zero					(c_material_name, 256);
	xray::strings::copy					(c_material_name, 256, material_name.c_str());
	
	resources::query_resource			(
		material_name.c_str(), 
		resources::material_effects_instance_class, 
		boost::bind(
			&user_render_surface_wire::material_ready, 
			this, 
			_1,
			cook_data,
			c_material_name
		),
		g_allocator,
		&user_data
	);
}

} // namespace render 
} // namespace xray 
