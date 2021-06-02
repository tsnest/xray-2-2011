////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skeleton_mesh_cpu_skinning_4weights.h"
#include <xray/render/engine/model_format.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/custom_config.h>

using xray::math::float3;
using xray::math::float2;
using xray::render::skeleton_mesh_cpu_skinning_4weights;
using xray::math::float4x4;

D3D_INPUT_ELEMENT_DESC software_4weights_skinning_vertex_layout[] =
{
	{"POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",	0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 12,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",	0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 16,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"BINORMAL",0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, 20, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,	0, 24, 	D3D_INPUT_PER_VERTEX_DATA, 0},
};

skeleton_mesh_cpu_skinning_4weights::skeleton_mesh_cpu_skinning_4weights	( ) :
	m_vertices_4w		( 0 )
{
}

skeleton_mesh_cpu_skinning_4weights::~skeleton_mesh_cpu_skinning_4weights	( )
{
	FREE				( m_vertices_4w );
}

void skeleton_mesh_cpu_skinning_4weights::load				( xray::configs::binary_config_value const& properties, xray::memory::chunk_reader& r )
{
	super::load			( properties, r );

	memory::reader verts_reader		= r.open_reader( model_chunk_vertices );
    m_render_geometry.vertex_count	= verts_reader.r_u32( );

	u32 buffer_length				= sizeof(vert_boned_4w) * m_render_geometry.vertex_count;
	m_vertices_4w					= (vert_boned_4w*)MALLOC( buffer_length, "surface_4w_software" );
	verts_reader.r					( m_vertices_4w, buffer_length, buffer_length);

	memory::reader indices_reader	= r.open_reader( model_chunk_indices );
	m_render_geometry.index_count	= indices_reader.r_u32( );

	m_render_geometry.primitive_count	= m_render_geometry.index_count/3;

	untyped_buffer_ptr ib			= resource_manager::ref().create_buffer( m_render_geometry.index_count*sizeof(u16), 
																	indices_reader.pointer(), 
																	enum_buffer_type_index, 
																	false);

	ref_declaration decl	= resource_manager::ref().create_declaration( software_4weights_skinning_vertex_layout );//&*decl_code.begin(), decl_code.size());

	// The visuals are create with dynamic usage temporary.
	bool dynamic		= true;
	u32 vStride			= sizeof(vb_sw_struct);//D3DXGetDeclVertexSize					( vFormat,0);
	m_vertex_buffer		= resource_manager::ref().create_buffer	(	m_render_geometry.vertex_count*vStride, 
																	NULL, 
																	enum_buffer_type_vertex, 
																	dynamic);

	m_render_geometry.geom		= resource_manager::ref().create_geometry( &*decl, vStride, *m_vertex_buffer, *ib);
	//m_effect->init_layouts	( *decl);
}
void skeleton_mesh_cpu_skinning_4weights::update( xray::render::vector<float4x4> const& bones )
{
	skinning_sw						( bones );
}

void skeleton_mesh_cpu_skinning_4weights::skinning_sw( xray::render::vector<float4x4> const& bones )
{
	vb_sw_struct*	D = (vb_sw_struct*)m_vertex_buffer->map(D3D_MAP_WRITE_DISCARD);

	int U_Count				= m_render_geometry.vertex_count;

	vert_boned_4w*	S	= m_vertices_4w;
	vert_boned_4w*	V	= S;
	vert_boned_4w*	E	= V+U_Count;

	float3			P0,N0,T0,B0, P1,N1,T1,B1, P2,N2,T2,B2, P3,N3,T3,B3;

	xray::render::base_basis	Basis; // quant

	u32 vidx=0;
	for (; S!=E; ++vidx)
	{
		float4x4 const& M0	= transpose( bones[ S->m[0] ] );
		float4x4 const& M1	= transpose( bones[ (S->m[1]!=u16(-1)) ? S->m[1] : S->m[0] ] );
        float4x4 const& M2	= transpose( bones[ (S->m[2]!=u16(-1)) ? S->m[2] : S->m[0] ] );
        float4x4 const& M3	= transpose( bones[ (S->m[3]!=u16(-1)) ? S->m[3] : S->m[0] ] );

		P0 = M0.transform_position (S->P);		P0	*= S->w[0];
		N0 = M0.transform_direction(S->N);		N0	*= S->w[0];
		T0 = M0.transform_direction(S->T);		T0	*= S->w[0];
		B0 = M0.transform_direction(S->B);		B0	*= S->w[0];

        P1 = M1.transform_position (S->P);		P1	*= S->w[1];
        N1 = M1.transform_direction(S->N);		N1	*= S->w[1];
		T1 = M0.transform_direction(S->T);		T1	*= S->w[1];
		B1 = M0.transform_direction(S->B);		B1	*= S->w[1];

        P2 = M2.transform_position (S->P);		P2	*= S->w[2];
        N2 = M2.transform_direction(S->N);		N2	*= S->w[2];
		T2 = M0.transform_direction(S->T);		T2	*= S->w[2];
		B2 = M0.transform_direction(S->B);		B2	*= S->w[2];
		
		float w3 = 1.0f - S->w[0] - S->w[1] - S->w[2];
		P3 = M3.transform_position (S->P);		P3	*= w3;
        N3 = M3.transform_direction(S->N);		N3	*= w3;
		T3 = M0.transform_direction(S->T);		T3	*= w3;
		B3 = M0.transform_direction(S->B);		B3	*= w3;

		P0					+= P1;
		P0					+= P2;
		P0					+= P3;

		D->P				= P0;
		
		N0					+= N1;
		N0					+= N2;
		N0					+= N3;

		T0					+= T1;
		T0					+= T2;
		T0					+= T3;

		B0					+= B1;
		B0					+= B2;
		B0					+= B3;

		Basis.set			( N0 );
		D->N				= math::color_rgba( Basis.x, Basis.y, Basis.z, 127u );

		float2 const uv		= S->uv;
		std::pair<s16,u8> _24u	= s24_tc_base(uv.x);
		std::pair<s16,u8> _24v	= s24_tc_base(uv.y);

		// Tangent
		{
			Basis.set		(T0);
			D->T			= math::color_rgba( u32(Basis.x), Basis.y, Basis.z, _24u.second);
		}

		// Binormal
		{
			Basis.set		(B0);
			D->B			= math::color_rgba(u32(Basis.x), Basis.y, Basis.z, _24v.second);
		}

		// TC
		{
			D->uv			= uv;
		}

		S++;
		D++;
	}

	m_vertex_buffer->unmap	( );
}