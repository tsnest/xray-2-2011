////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Andrew Kolomiets
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include <xray/render/engine/visual.h>
#include <xray/render/engine/effect_notexture.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/engine/model_manager.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/decl_utils.h>

#include <xray/collision/api.h>

// Temporary to support old file formats!
#include <D3D9Types.h>
#include <d3dx9mesh.h>


namespace xray {
namespace render_dx10 {

render_visual::render_visual()
:type				( 0),
flags				( 0),
m_collision_geom	( NULL)
{
	effect_notexture	b;
	m_notexture_shader = effect_manager::ref().create_effect( &b);
}

render_visual::~render_visual()
{
// 	if( m_collision_geom != NULL)
// 		collision::destroy( m_collision_geom);
}

bool b_test = false;

void render_visual::load( memory::chunk_reader& chunk)
{
	ASSERT( chunk.chunk_exists( ogf_chunk_header));
 
	if( chunk.chunk_exists( ogf_chunk_collision_v))
	{
		memory::reader reader	= chunk.open_reader( ogf_chunk_collision_v);
		u32 const vertex_count	= reader.length()/( sizeof( float3));
		float3* vertices		= ALLOC( float3, vertex_count);

		ASSERT					( vertex_count*sizeof( float3) == reader.length());
		reader.r				( vertices, reader.length(), reader.length());

		reader					= chunk.open_reader( ogf_chunk_collision_i);
		u32 const index_count	= reader.length()/( sizeof( u32));
		u32* indices			= ALLOC( u32, index_count);

		ASSERT					( index_count*sizeof( u32) == reader.length());
		ASSERT					( ( index_count % 3) == 0, "invalid index count[%d]", index_count);
		reader.r				( indices, reader.length(), reader.length());

//		m_collision_geom		= &*collision::create_triangle_mesh_geometry( xray::render::g_allocator, vertices, vertex_count, indices, index_count);

		FREE( vertices);
		FREE( indices);
	}

	memory::reader reader = chunk.open_reader( ogf_chunk_header);

	ogf_header	hdr;
	reader.r	( &hdr, sizeof( hdr), sizeof( hdr));
	
	m_sphere		= hdr.bs;
	m_aabb			= hdr.bb;

	R_ASSERT( hdr.format_version==xrOGF_FormatVersion);

	//Overwrite MRT shader by default shader
/*
	if( g_default_mode)
	{
		simple_static_effect	effect;
		m_effect = ( b_test)?
			model_manager::ref().m_test_shader : 
					effect_manager::ref().create_effect( &effect);
	}
	else*/
	if( chunk.chunk_exists( ogf_texture))
	{
		fixed_string<256>		fnT,fnS;

		memory::reader reader = chunk.open_reader( ogf_texture);

		fnT = reader.r_string();
		fnS = reader.r_string();

		utils::fix_path( fnT.get_buffer());
		//m_effect = effect_manager::ref().create_effect( fnS.c_str(), fnT.c_str());
		set_shader( fnS.c_str(), fnT.c_str());
	}
	else
	{
		if( hdr.shader_id)
		{
			m_effect = model_manager::ref().get_shader( hdr.shader_id);
			if( !m_effect)
			{
				FATAL( "Visual loading error: shader is not created");
				//LOG_WARNING( "Visual loading error: shader is not created");
			}
		}
	}
}

void render_visual::render_selected		()
{
	ref_effect orig_shader = m_effect;
	m_effect = m_notexture_shader;

	render();

	m_effect = orig_shader;
}

bool render_visual::set_shader( char const* shader, char const* texture)
{
	ref_effect new_shader = effect_manager::ref().create_effect( shader, texture);	

	ASSERT( new_shader, "Shader can't be found!");
	if( !new_shader)
		return false;

	m_effect = new_shader;
	return true;
}

simple_visual::simple_visual()
{
	m_c_L_material			= backend::ref().register_constant_host( "L_material");
	m_c_hemi_cube_pos_faces = backend::ref().register_constant_host( "hemi_cube_pos_faces");
	m_c_hemi_cube_neg_faces = backend::ref().register_constant_host( "hemi_cube_neg_faces");
	m_c_dt_params			= backend::ref().register_constant_host( "dt_params");
}

simple_visual::~simple_visual()
{

}

void simple_visual::load( memory::chunk_reader& chunk)
{
	super::load				( chunk);

	if( chunk.chunk_exists( ogf_chunk_gcontainer))
	{
// --Porting to DX10_
// 		//simple_static_effect	effect;
// 
// 		memory::reader reader	= chunk.open_reader( ogf_chunk_gcontainer);
// 		
// 		vertex_buffer_id		= reader.r_u32();
// 		vertex_base				= reader.r_u32();
// 		vertex_count			= reader.r_u32();
// 
// 		index_buffer_id			= reader.r_u32();
// 		index_base				= reader.r_u32();
// 		index_count				= reader.r_u32();
// 
// 		primitive_count			= index_count/3;
// 		geom					= model_manager::ref().create_geom( vertex_buffer_id, index_buffer_id);
	}
	else
	{
		D3DVERTEXELEMENT9 const*  vFormat = NULL;

		memory::reader reader	= chunk.open_reader( ogf_chunk_vertices);
		vFormat	= ( D3DVERTEXELEMENT9*) reader.pointer();
		u32 const dcl_len		= D3DXGetDeclLength( vFormat) + 1;

		vector<D3D_INPUT_ELEMENT_DESC> decl_code;
		decl_utils::convert_vertex_declaration( vector<D3DVERTEXELEMENT9>( vFormat,vFormat+dcl_len), decl_code);

		ref_declaration decl	= resource_manager::ref().create_declaration( &*decl_code.begin(), decl_code.size());
		reader.advance( dcl_len*sizeof( D3DVERTEXELEMENT9));

		// The visuals are create with dynamic usage temporary.
		bool dynamic		= true;

		m_mesh.vertex_count		= reader.r_u32				();
		u32 vStride			= D3DXGetDeclVertexSize		( vFormat,0);
		ref_buffer vb		= resource_manager::ref().create_buffer( m_mesh.vertex_count*vStride, reader.pointer(), enum_buffer_type_vertex, dynamic);

//--- read indices here

		reader					= chunk.open_reader( ogf_chunk_indices);
		m_mesh.index_base		= 0;
		m_mesh.index_count		= reader.r_u32();
		m_mesh.primitive_count	= m_mesh.index_count/3;

		ref_buffer ib			= resource_manager::ref().create_buffer( m_mesh.index_count*2, reader.pointer(), enum_buffer_type_index, false);

		m_mesh.geom				= resource_manager::ref().create_geometry( &*decl, vStride, &*vb, &*ib);

		m_vb					= &*vb;

		m_effect->init_layouts( *decl);
	}

}

// namespace dx9 {
// 	world& get_w();
// }

void simple_visual::render()
{
	ASSERT( m_effect);

	//dx9::get_w().set_world_matrix( float4x4().identity());
// 	if( 	dx9::get_w().m_c_mat_wvp)
// 		dx9::get_w().setup_mat_wvp( get_w().m_c_mat_wvp);


	m_effect->apply();
	m_mesh.geom->apply();
// 	dx9::get_w().set_world_matrix_tmp( float4x4().identity());
//	dx9::get_w().set_world_matrix( float4x4().identity());
//	backend::ref().set_w( float4x4().identity());

	//float mtl = m_effect->get
	backend::ref().set_constant( m_c_L_material, float4( 0.5f, 0.5f, 0.0f, ( 0.75+0.5f)/4.0f));
	backend::ref().set_constant( m_c_hemi_cube_pos_faces, float4( 0.f, 0.f, 0.f, 0.f));
	backend::ref().set_constant( m_c_hemi_cube_neg_faces, float4( 0.f, 0.f, 0.f, 0.f));
	backend::ref().set_constant( m_c_dt_params, float4( 0.f, 0.f, 0.f, 0.f));

	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_mesh.primitive_count*3, 0, 0);
}


hierrarchy_visual::~hierrarchy_visual()
{
	childs::iterator		it = m_managed_visuals.begin();
	childs::const_iterator	en = m_managed_visuals.end();
	for( ; it != en; ++it)
		XRAY_DELETE_IMPL( *::xray::render::g_allocator, *it);
}

void hierrarchy_visual::load( memory::chunk_reader& chunk)
{
	super::load				( chunk);

	if( chunk.chunk_exists( ogf_children_l)) 
	{
		memory::reader reader= chunk.open_reader( ogf_children_l);
		// From Link
		u32 cnt				= reader.r_u32();
		m_childs.resize		( cnt);
		for( u32 i=0; i<cnt; ++i)	
		{
			u32 id			= reader.r_u32();
			m_childs[i]		= model_manager::ref().get_visual( id);
			//ASSERT( m_childs[i]);
			//m_childs[i]		= ( dxRender_Visual*)::Render->getVisual( ID);
		}
	}else
	{	
		memory::chunk_reader chunk_reader	= chunk.open_chunk_reader( ogf_children);
        u32 count					= 0;

		while( chunk_reader.chunk_exists( count)) 
		{
			memory::chunk_reader child_chunk	= chunk_reader.open_chunk_reader( count);
			
			u16 type				= model_manager::get_visual_type( child_chunk);
			render_visual* V		= model_manager::create_instance( type);
			V->load					( child_chunk);
			V->flags |= enum_flag_visual_managed;
			m_childs.push_back			( V);
			m_managed_visuals.push_back	( V);
			++count;
        }
	}
	R_ASSERT( !m_childs.empty());
}

void hierrarchy_visual::render()
{
	childs_it it	= m_childs.begin();
	childs_it it_e	= m_childs.end();

	for( ; it!=it_e; ++it)
	{
		if( render_visual* v = *it) v->render();
	}
}

void hierrarchy_visual::render_selected()
{
	childs_it it	= m_childs.begin();
	childs_it it_e	= m_childs.end();

	for( ; it!=it_e; ++it)
	{
		if( render_visual* v = *it) v->render_selected();
	}
}

simple_visual_pm::~simple_visual_pm()
{
	DELETE_ARRAY( m_swi.sw);
}

void simple_visual_pm::load( memory::chunk_reader& chunk)
{
	super::load				( chunk);
	
	memory::reader reader  = chunk.open_reader( ogf_chunk_swi);
    m_swi.reserved[0]	= reader.r_u32();
    m_swi.reserved[1]	= reader.r_u32();
    m_swi.reserved[2]	= reader.r_u32();
    m_swi.reserved[3]	= reader.r_u32();
    m_swi.count			= reader.r_u32();
	ASSERT				( NULL==m_swi.sw);
    m_swi.sw			= NEW_ARRAY( slide_window, m_swi.count);
	reader.r			( m_swi.sw, m_swi.count*sizeof( slide_window), m_swi.count*sizeof( slide_window));
}


void simple_visual_pm::render()
{
	ASSERT( m_effect);

	m_effect->apply();
	m_mesh.geom->apply();
	int swi_idx = 0;
	slide_window const& sw = m_swi.sw[swi_idx];

	//get_w().setup_mat_wvp( get_w().m_c_mat_wvp);

	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, sw.num_tris*3, 0, 0);
}

tree_visual::tree_visual		()
{
	m_c_L_material			= backend::ref().register_constant_host( "L_material");	
}

tree_visual::~tree_visual()
{}

void tree_visual::load( memory::chunk_reader& r)
{
	XRAY_UNREFERENCED_PARAMETER(r);
	NOT_IMPLEMENTED();
	// --Porting to DX10_
// 	super::load			( r);
// 
// 	// read vertices
// 	memory::reader reader	= r.open_reader( ogf_chunk_gcontainer);
// 	// verts
// 	vertex_buffer_id	= reader.r_u32();
// 	vertex_base			= reader.r_u32();
// 	vertex_count		= reader.r_u32();
// 
// 	index_buffer_id		= reader.r_u32();
// 	index_base			= reader.r_u32();
// 	index_count			= reader.r_u32();
// 	primitive_count		= index_count/3;
// 	geom				= resource_manager::ref().create_geometry( vertex_buffer_id, index_buffer_id);
// 
// 	//// load tree-def
// 	reader				= r.open_reader( ogf_chunk_tree_def);
// 	reader.r			( &m_transform,	sizeof( float4x4),	sizeof( float4x4));
// 		//reader.r			( &c_scale,	sizeof( c_scale));	
// 		//c_scale.rgb.mul	( .5f);	
// 		//c_scale.hemi*=.5f;	
// 		//c_scale.sun	*=.5f;
// 
// 		//reader.r			( &c_bias,	sizeof( c_bias));	
// 		//c_bias.rgb.mul	( .5f);	
// 		//c_bias.hemi	*=.5f;	
// 		//c_bias.sun	*=.5f;

}

void tree_visual::render()
{
//	float4x4	bk = get_w().get_world_matrix();

	// --Porting to DX10_
	//dx9::get_w().set_world_matrix( m_transform);
	
	//if( get_w().m_c_mat_wvp)
	//	get_w().setup_mat_wvp( get_w().m_c_mat_wvp);
	
	geom->apply				();

	backend::ref().set_constant( m_c_L_material, float4(0.5f, 0.5f, 0.0f, ( 0.75+0.5f)/4.0f));

	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, primitive_count*3, 0, 0);

//	get_w().set_world_matrix( bk);
//	get_w().setup_mat_vp( get_w().m_c_mat_vp);
}

} // namespace render 
} // namespace xray