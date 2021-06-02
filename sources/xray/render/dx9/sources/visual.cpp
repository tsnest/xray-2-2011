////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "visual.h"
//#include "dx9_world.h"
#include <xray/render/common/sources/blender_notexture.h>
#include <xray/render/dx9/sources/scene_context.h>
#include <xray/render/dx9/sources/backend.h>
#include <xray/render/common/sources/shader_manager.h>
#include <xray/render/dx9/sources/model_manager.h>


namespace xray {
namespace render {

render_visual::render_visual()
:type				(0),
flags				(0),
m_collision_geom	(NULL)
{
	blender_notexture	b;
	m_notexture_shader = shader_manager::get_ref().create_shader(&b);
}

render_visual::~render_visual()
{
	if( m_collision_geom != NULL )
		collision::destroy(m_collision_geom);
}

bool b_test = false;

void render_visual::load(memory::chunk_reader& chunk)
{
	ASSERT(chunk.chunk_exists(ogf_chunk_header));
 
	if(chunk.chunk_exists(ogf_chunk_collision_v))
	{
		memory::reader reader	= chunk.open_reader(ogf_chunk_collision_v);
		u32 const vertex_count	= reader.length()/(sizeof(float3));
		float3* vertices		= ALLOC(float3, vertex_count);

		ASSERT					( vertex_count*sizeof(float3) == reader.length() );
		reader.r				( vertices, reader.length(), reader.length() );

		reader					= chunk.open_reader(ogf_chunk_collision_i);
		u32 const index_count	= reader.length()/(sizeof(u32));
		u32* indices			= ALLOC(u32, index_count);

		ASSERT					( index_count*sizeof(u32) == reader.length() );
		ASSERT					( (index_count % 3) == 0, "invalid index count[%d]", index_count );
		reader.r				( indices, reader.length(), reader.length() );

		m_collision_geom		= &*collision::create_triangle_mesh_geometry( xray::render::g_allocator, vertices, vertex_count, indices, index_count );

		FREE( vertices );
		FREE( indices );
	}

	memory::reader reader = chunk.open_reader( ogf_chunk_header );

	ogf_header	hdr;
	reader.r	(&hdr, sizeof(hdr), sizeof(hdr));
	
	m_sphere	= hdr.bs;
	m_aabb		= hdr.bb;

	R_ASSERT(hdr.format_version==xrOGF_FormatVersion);

	//Overwrite MRT shader by default shader
	if (FALSE)
	{
		simple_static_blender	blender;
		m_shader = (b_test)?
			model_manager::get_ref().m_test_shader : 
					shader_manager::get_ref().create_shader(&blender);
	}
	else if(chunk.chunk_exists( ogf_texture ))
	{
		fixed_string<256>		fnT,fnS;

		memory::reader reader = chunk.open_reader( ogf_texture );

		fnT = reader.r_string();
		fnS = reader.r_string();

		utils::fix_path(fnT.get_buffer());
		//m_shader = shader_manager::get_ref().create_shader(fnS.c_str(), fnT.c_str());
		set_shader(fnS.c_str(), fnT.c_str());
	}
	else
	{
		if (hdr.shader_id)
		{
			m_shader = model_manager::get_ref().get_shader(hdr.shader_id);
			if (!m_shader)
			{
				FATAL("Visual loading error: shader is not created");
				//LOG_WARNING("Visual loading error: shader is not created");
			}
		}
	}
}

void render_visual::render_selected		()
{
	ref_shader orig_shader = m_shader;
	m_shader = m_notexture_shader;

	render();

	m_shader = orig_shader;
}

bool render_visual::set_shader( char const* shader, char const* texture)
{
	ref_shader new_shader = shader_manager::get_ref().create_shader( shader, texture);	

	ASSERT( new_shader, "Shader can't be found!");
	if( !new_shader)
		return false;

	m_shader = new_shader;
	return true;
}

simple_visual::~simple_visual()
{}

void simple_visual::load(memory::chunk_reader& chunk)
{
	super::load				(chunk);

	if(chunk.chunk_exists( ogf_chunk_gcontainer ))
	{
		simple_static_blender	blender;

		memory::reader reader	= chunk.open_reader( ogf_chunk_gcontainer );
		
		vertex_buffer_id		= reader.r_u32();
		vertex_base				= reader.r_u32();
		vertex_count			= reader.r_u32();

		index_buffer_id			= reader.r_u32();
		index_base				= reader.r_u32();
		index_count				= reader.r_u32();

		primitive_count			= index_count/3;
		geom					= model_manager::get_ref().create_geom(vertex_buffer_id, index_buffer_id);
	}
	else
	{
		u32		dwUsage			= D3DUSAGE_WRITEONLY;
//		D3DVERTEXELEMENT9 dcl	[MAX_FVF_DECL_SIZE];
		D3DVERTEXELEMENT9 const*  vFormat = NULL;

		memory::reader reader	= chunk.open_reader(ogf_chunk_vertices);
		vertex_base				= 0;
// 		u32 fvf					= reader.r_u32				();
// 		R_CHK					(D3DXDeclaratorFromFVF(fvf, dcl));
//		vFormat					= dcl;

		vFormat	= ( D3DVERTEXELEMENT9* ) reader.pointer( );
		u32 const dcl_len				= D3DXGetDeclLength( vFormat ) + 1;
		ref_declaration decl_ptr = resource_manager::get_ref().create_decl(vFormat);
		reader.advance( dcl_len*sizeof( D3DVERTEXELEMENT9 ) );


		vertex_count			= reader.r_u32				();
		//u32 vStride				= D3DXGetFVFVertexSize		(fvf);
		u32 vStride				= D3DXGetDeclVertexSize	(vFormat,0);
		

		vertex_buffer_id		= model_manager::get_ref().create_vb(vertex_count*vStride, dwUsage, reader.pointer());

//--- read indices here

		reader					= chunk.open_reader(ogf_chunk_indices);
		index_base				= 0;
		index_count				= reader.r_u32();
		primitive_count			= index_count/3;

		dwUsage					= 0;	// indices are read in model-wallmarks code
		index_buffer_id			= model_manager::get_ref().create_ib(index_count*2, dwUsage, reader.pointer());

		geom					= model_manager::get_ref().create_geom(vertex_buffer_id, index_buffer_id, decl_ptr);
	}

}

namespace dx9 {
	world& get_w();
}

void simple_visual::render()
{
	ASSERT(m_shader);

	//dx9::get_w().set_world_matrix( float4x4().identity() );
// 	if(	dx9::get_w().m_c_mat_wvp)
// 		dx9::get_w().setup_mat_wvp(get_w().m_c_mat_wvp);


	m_shader->apply();
	geom->apply();
// 	dx9::get_w().set_world_matrix_tmp( float4x4().identity() );
//	dx9::get_w().set_world_matrix( float4x4().identity() );
//	backend::get_ref().set_w( float4x4().identity() );

	//float mtl = m_shader->get
	backend::get_ref().set_c("L_material", 0.5f, 0.5f, 0.0f, (0.75+0.5f)/4.0f);

	backend::get_ref().render(
		D3DPT_TRIANGLELIST,
		vertex_base,
		0,
		vertex_count,
		index_base,
		primitive_count);
}


hierrarchy_visual::~hierrarchy_visual()
{
	childs::iterator		it = m_managed_visuals.begin();
	childs::const_iterator	en = m_managed_visuals.end();
	for( ; it != en; ++it )
		XRAY_DELETE_IMPL( *::xray::render::g_allocator, *it );
}

void hierrarchy_visual::load(memory::chunk_reader& chunk)
{
	super::load				(chunk);

	if(chunk.chunk_exists(ogf_children_l)) 
	{
		memory::reader reader= chunk.open_reader(ogf_children_l);
		// From Link
		u32 cnt				= reader.r_u32();
		m_childs.resize		(cnt);
		for(u32 i=0; i<cnt; ++i)	
		{
			u32 id			= reader.r_u32();
			m_childs[i]		= model_manager::get_ref().get_visual(id);
			//ASSERT(m_childs[i]);
			//m_childs[i]		= (dxRender_Visual*)::Render->getVisual(ID);
		}
	}else
	{	
		memory::chunk_reader chunk_reader	= chunk.open_chunk_reader(ogf_children);
        u32 count					= 0;

		while(chunk_reader.chunk_exists(count)) 
		{
			memory::chunk_reader child_chunk	= chunk_reader.open_chunk_reader(count);
			
			u16 type				= model_manager::get_visual_type(child_chunk);
			render_visual* V		= model_manager::create_instance(type);
			V->load					(child_chunk);
			V->flags |= enum_flag_visual_managed;
			m_childs.push_back			(V);
			m_managed_visuals.push_back	(V);
			++count;
        }
	}
	R_ASSERT(!m_childs.empty());
}

void hierrarchy_visual::render( )
{
	childs_it it	= m_childs.begin();
	childs_it it_e	= m_childs.end();

	for( ; it!=it_e; ++it)
	{
		if(render_visual* v = *it) v->render();
	}
}

void hierrarchy_visual::render_selected( )
{
	childs_it it	= m_childs.begin();
	childs_it it_e	= m_childs.end();

	for( ; it!=it_e; ++it)
	{
		if(render_visual* v = *it) v->render_selected();
	}
}

simple_visual_pm::~simple_visual_pm()
{
	DELETE_ARRAY(m_swi.sw);
}

void simple_visual_pm::load(memory::chunk_reader& chunk)
{
	super::load				(chunk);
	
	memory::reader reader  = chunk.open_reader(ogf_chunk_swi);
    m_swi.reserved[0]	= reader.r_u32();
    m_swi.reserved[1]	= reader.r_u32();
    m_swi.reserved[2]	= reader.r_u32();
    m_swi.reserved[3]	= reader.r_u32();
    m_swi.count			= reader.r_u32();
	ASSERT				(NULL==m_swi.sw);
    m_swi.sw			= NEW_ARRAY(slide_window, m_swi.count);
	reader.r			(m_swi.sw, m_swi.count*sizeof(slide_window), m_swi.count*sizeof(slide_window));
}


void simple_visual_pm::render()
{
	ASSERT(m_shader);

	m_shader->apply();
	geom->apply();
	int swi_idx = 0;
	slide_window const& sw = m_swi.sw[swi_idx];

	//get_w().setup_mat_wvp(get_w().m_c_mat_wvp);

	backend::get_ref().render(
		D3DPT_TRIANGLELIST,
		vertex_base,
		0,
		sw.num_verts,
		index_base+sw.offset,
		sw.num_tris);
}

tree_visual::~tree_visual()
{}

void tree_visual::load(memory::chunk_reader& r)
{
	super::load			(r);

	// read vertices
	memory::reader reader	= r.open_reader(ogf_chunk_gcontainer);
	// verts
	vertex_buffer_id	= reader.r_u32();
	vertex_base			= reader.r_u32();
	vertex_count		= reader.r_u32();

	index_buffer_id		= reader.r_u32();
	index_base			= reader.r_u32();
	index_count			= reader.r_u32();
	primitive_count		= index_count/3;
	geom				= model_manager::get_ref().create_geom(vertex_buffer_id, index_buffer_id);

	//// load tree-def
	reader				= r.open_reader(ogf_chunk_tree_def);
	reader.r			(&m_transform,	sizeof(float4x4),	sizeof(float4x4));
		//reader.r			(&c_scale,	sizeof(c_scale));	
		//c_scale.rgb.mul	(.5f);	
		//c_scale.hemi*=.5f;	
		//c_scale.sun	*=.5f;

		//reader.r			(&c_bias,	sizeof(c_bias));	
		//c_bias.rgb.mul	(.5f);	
		//c_bias.hemi	*=.5f;	
		//c_bias.sun	*=.5f;

}

void tree_visual::render()
{
//	float4x4	bk = get_w().get_world_matrix();
	//dx9::get_w().set_world_matrix( m_transform );
	
	//if(get_w().m_c_mat_wvp)
	//	get_w().setup_mat_wvp(get_w().m_c_mat_wvp);
	
	geom->apply				();

	backend::get_ref().set_c("L_material", 0.5f, 0.5f, 0.0f, (0.75+0.5f)/4.0f);

	backend::get_ref().render(
		D3DPT_TRIANGLELIST,
		vertex_base,
		0,
		vertex_count,
		index_base,
		primitive_count);

//	get_w().set_world_matrix( bk );
//	get_w().setup_mat_vp(get_w().m_c_mat_vp);
}

} // namespace render 
} // namespace xray