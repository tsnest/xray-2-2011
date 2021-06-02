////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skeleton_mesh_gpu_skinning_4weights.h"
#include <xray/render/engine/model_format.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/custom_config.h>

using xray::math::float3;
using xray::math::float2;
using xray::render::skeleton_mesh_gpu_skinning_4weights;
using xray::math::float4x4;

D3D_INPUT_ELEMENT_DESC hardware_4weights_skinning_vertex_layout[] =
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BLENDINDICES",0, DXGI_FORMAT_R8G8B8A8_UINT,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BLENDWEIGHT",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 28,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",		0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 32,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BINORMAL",	0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 36, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 40, 	D3D_INPUT_PER_VERTEX_DATA, 0},
};

skeleton_mesh_gpu_skinning_4weights::skeleton_mesh_gpu_skinning_4weights	( )
{
}

skeleton_mesh_gpu_skinning_4weights::~skeleton_mesh_gpu_skinning_4weights	( )
{
}


void skeleton_mesh_gpu_skinning_4weights::load				( xray::configs::binary_config_value const& properties, xray::memory::chunk_reader& r )
{
	super::load						( properties, r );

	memory::reader verts_reader		= r.open_reader( model_chunk_vertices );
    m_render_geometry.vertex_count	= verts_reader.r_u32( );

	//u32 const buffer_length			= sizeof(vert_boned_4w) * m_render_geometry.vertex_count;
	//vert_boned_4w* const raw_vertices = static_cast< vert_boned_4w* >( ALLOCA( buffer_length ) );
	//verts_reader.r					( raw_vertices, buffer_length, buffer_length );

	memory::reader indices_reader	= r.open_reader( model_chunk_indices );
	m_render_geometry.index_count	= indices_reader.r_u32( );

	m_render_geometry.primitive_count	= m_render_geometry.index_count/3;

	untyped_buffer_ptr ib			=
		resource_manager::ref().create_buffer(
			m_render_geometry.index_count*sizeof(u16), 
			indices_reader.pointer(), 
			enum_buffer_type_index, 
			false
		);

	ref_declaration const& declaration	= resource_manager::ref().create_declaration( hardware_4weights_skinning_vertex_layout );

	void* ptr = (void*)verts_reader.pointer();
	hardware_4weights_skinning_vertex* const prepared_vertices = static_cast< hardware_4weights_skinning_vertex* >( ptr );

	//u32 prepared_vertices_buff_size		= sizeof(hardware_4weights_skinning_vertex_layout)*m_render_geometry.vertex_count;
	//hardware_4weights_skinning_vertex* const prepared_vertices = static_cast< hardware_4weights_skinning_vertex* >( ALLOCA( prepared_vertices_buff_size ) );
	//
	//vert_boned_4w* i				= raw_vertices;
	//vert_boned_4w* const e			= raw_vertices + m_render_geometry.vertex_count;
	//hardware_4weights_skinning_vertex* j	= prepared_vertices;
	//for ( ; i != e; ++i, ++j ) {
	//	(*j).P				= (*i).P;

	//	xray::render::base_basis basis;
	//	basis.set			( (*i).N );
	//	(*j).N				= math::color_rgba( basis.x, basis.y, basis.z, 127u );


	//	(*j).uv				= (*i).uv;
	//	std::pair<s16,u8> _24u	= s24_tc_base((*i).uv.x);
	//	basis.set			( (*i).T );
	//	(*j).T				= math::color_rgba( (u32)basis.x, basis.y, basis.z, _24u.second);

	//	std::pair<s16,u8> _24v	= s24_tc_base((*i).uv.y);
	//	basis.set			( (*i).B );
	//	(*j).B				= math::color_rgba( (u32)basis.x, basis.y, basis.z, _24v.second );

	//	(*j).weights[0]		= (*i).w[0];
	//	(*j).weights[1]		= (*i).w[1];
	//	(*j).weights[2]		= (*i).w[2];

	//	u8 const indices[]	= {
	//		(u8)(*i).m[0],
	//		(u8)((*i).m[1] == u16(-1) ? (*i).m[0] : (*i).m[1]),
	//		(u8)((*i).m[2] == u16(-1) ? (*i).m[0] : (*i).m[2]),
	//		(u8)((*i).m[3] == u16(-1) ? (*i).m[0] : (*i).m[3]),
	//	};

	//	(*j).indices[0]		= indices[0];
	//	(*j).indices[1]		= indices[1];
	//	(*j).indices[2]		= indices[2];
	//	(*j).indices[3]		= indices[3];
	//}

	bool const dynamic		= true;
	u32 const stride		= sizeof( hardware_4weights_skinning_vertex );
	m_vertex_buffer			=
		resource_manager::ref().create_buffer	(
			m_render_geometry.vertex_count * stride, 
			prepared_vertices, 
			enum_buffer_type_vertex, 
			dynamic
		);

	m_render_geometry.geom	= resource_manager::ref().create_geometry( &*declaration, stride, *m_vertex_buffer, *ib);
}

void skeleton_mesh_gpu_skinning_4weights::update( xray::render::vector<float4x4> const& bones )
{
	XRAY_UNREFERENCED_PARAMETER	( bones );
}
