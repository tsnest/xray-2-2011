////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/sources/platform_dx9.h>
//#include <xray/render/base/sources/platform_api_dx9.h>
// #include <xray/render/dx9/sources/visual.h>
// #include <xray/render/base/api.h>
// #include <xray/render/base/world.h>
// #include <xray/render/base/debug_renderer.h>
// #include <xray/render/base/editor_renderer.h>
// #include <xray/render/base/game_renderer.h>
#include <xray/render/base/ui_renderer.h>
// #include <xray/console_command.h>
// #include <xray/resources.h>
// #include <xray/render/common/sources/terrain.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxerr.lib")

const DWORD vertex_colored_fvf	= (D3DFVF_XYZ | D3DFVF_DIFFUSE);
const DWORD ui_vertex_fvf		= (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

//using xray::render::dx9::world;
using xray::resources::managed_resource_ptr;
using xray::memory::chunk_reader;

namespace xray {
namespace render {

command_line::key	s_render_test_scene("render_test_scene", "", "render", "renders test scene");

namespace dx9 {

render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle)
{
	return NEW( platform)( wrapper, window_handle);
}


void create_pixel_shader(pcstr text, IDirect3DPixelShader9** dest, IDirect3DDevice9* device, pcstr entrypoint);
void create_vertex_shader(pcstr text, IDirect3DVertexShader9** dest, IDirect3DDevice9* device, pcstr entrypoint);


//extern float4 g_debug_color;

platform* pw = NULL;
platform& get_w() {	return *pw;}

platform::platform					( xray::render::engine::wrapper& wrapper, HWND const window_handle ) :
// #pragma warning ( push )
// #pragma warning ( disable : 4355 )
// 	render::base_world	( wrapper, *this ),
// #pragma warning ( pop )
	m_ui_texture_font			( 0 ),
	m_ui_texture				( 0 ),
	m_ui_ps						( 0 ),
	m_ui_ps_font				( 0 ),
	m_ui_ps_fill				( 0 ),
	m_debug_vb					( 0 ),
	m_debug_ib					( 0 ),
	m_editor_vb					( 0 ),
	m_editor_ib					( 0 ),
	m_editor_selected_color		( 1.f, 1.f, 0.f, 1.f ),
	m_ps_geom					( 0 ),
	m_hw						( wrapper, window_handle ),
//	m_started_scene				( false ),
	m_render_frame_id			( 0 ),
	m_line						( 0 ),
	m_render_test_scene			( s_render_test_scene.is_set() ),
	m_res_mgr					( true )//s_render_test_scene.is_set() )
{

	m_view_matrix.identity();
	m_proj_matrix.identity();

	pw = this;
	m_world_matrix.identity		( );
	m_backend.on_device_create(hw_wrapper::get_ref().device());

	m_res_mgr.register_const_setup("selection_color", const_setup_cb(this, &platform::setup_editor_selected_color));
	//m_res_mgr.register_const_setup("m_VP", const_setup_cb(this, &platform::setup_mat_vp));
	//m_res_mgr.register_const_setup("m_WVP", const_setup_cb(this, &platform::setup_mat_wvp));
	//m_res_mgr.register_const_setup("m_WV", const_setup_cb(this, &platform::setup_mat_v));
	m_res_mgr.register_const_setup("screen_res", const_setup_cb(this, &platform::setup_screen_res));

	if ( s_render_test_scene.is_set() )
	{
		m_model_mgr.init();
	}

	m_mat_mgr.load				( );

	create_resources			( );

	R_CHK						( device().SetRenderState		(D3DRS_LIGHTING, FALSE) );
    R_CHK						( device().SetRenderState		(D3DRS_CULLMODE, D3DCULL_NONE) );
    R_CHK						( device().SetRenderState		(D3DRS_ZENABLE,  TRUE) );
	R_CHK						( device().SetRenderState		(D3DRS_FILLMODE, D3DFILL_SOLID) );
	R_CHK						( device().SetRenderState		(D3DRS_ALPHABLENDENABLE, TRUE) );

	static models_cook			models_cooker;
	resources::register_cook	(&models_cooker);

	static terrain_cell_cook	terrain_cooker;
	resources::register_cook	(&terrain_cooker);

	m_scene						= NEW(scene_render) (true);

	initialize_grid_textures	( );
}

platform::~platform					( )
{
	m_mat_mgr.unload();

	DELETE(m_scene);
	m_backend.on_device_destroy	();

	destroy_resources		( );
	release_grid_textures	( );

	if( m_line )
		m_line->Release();
}

extern D3DXMATRIXA16				view_d3d;
extern D3DXMATRIXA16				projection_d3d;

//void platform::setup_mat_vp(res_constant* c)
//{
//	m_c_mat_vp		 = c;
//	math::float4x4	result_vp;
//	result_vp		= math::mul4x4(m_view_matrix, m_proj_matrix);
//	backend::get_ref().set_c(c, result_vp);
//
//}
//
//void platform::setup_mat_wvp(res_constant* c)
//{
//	m_c_mat_wvp		 = c;
//	math::float4x4 result_wvp;
//	result_wvp		= math::mul4x4(m_world_matrix, m_view_matrix);
//	result_wvp		= math::mul4x4(result_wvp, m_proj_matrix);
//
//	backend::get_ref().set_c(c, result_wvp);
//}

void platform::clear_resources	( )
{
	m_model_mgr.clear_resources		( );
}

void platform::setup_screen_res(res_constant* c)
{
	float w = float(hw_wrapper::get_ref().get_width());
	float h = float(hw_wrapper::get_ref().get_height());
	float4	screen_res(w, h, 1/w, 1/h);
	backend::get_ref().set_c(c, screen_res);
}

//void platform::setup_mat_v(res_constant* c)
//{
//	math::float4x4 result_wv;
//	result_wv		= math::mul4x4(m_world_matrix, m_view_matrix);
//
//	backend::get_ref().set_c(c, result_wv);
//}

void platform::add_static			( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals )
{
	if( !s_render_test_scene.is_set())
		return;

	m_model_mgr.add_static( collision, geometry, visuals);
	//test								(NULL);
}

void platform::draw_frame			( )
{
	float4x4 ident;
	ident.identity			( );
	set_world_matrix		( ident );

	++m_render_frame_id;

// 	game().debug().tick		( );
// 	editor().tick			( ); 

	m_render_device.end_frame( );

	if (!m_render_device.begin_frame( ))
		return;

	m_world_matrix.identity();

	float4x4 mat = math::mul4x4(m_view_matrix, m_proj_matrix);

	if( identity(true) || m_render_test_scene )
	{
		m_scene->render	( m_view_matrix, m_proj_matrix);
		render_visuals	( );
	}
	else
	{
		backend::get_ref().invalidate();
		backend::get_ref().reset_stages();
		device().Clear( 0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	}
}

void platform::test_cooperative_level	( )
{
	m_render_device.test_corrporate_level();
}

D3DXMATRIXA16				view_d3d;
void platform::set_view_matrix		( xray::float4x4 const& view )
{
	if (m_scene)
		m_scene->set_v(view);
	m_view_matrix = m_current_view_matrix = view;
	memory::copy				( &view_d3d, sizeof(view_d3d), &view, sizeof( view ) );
	R_CHK						( device().SetTransform	( D3DTS_VIEW, &view_d3d ) );
}

D3DXMATRIXA16				projection_d3d;
void platform::set_projection_matrix	( xray::float4x4 const& projection )
{
	if (m_scene)
		m_scene->set_p(projection);
	m_proj_matrix = projection;
	memory::copy				( &projection_d3d, sizeof(projection_d3d), &projection, sizeof( projection ) );
	R_CHK						( device().SetTransform	( D3DTS_PROJECTION, &projection_d3d ) );
}

void platform::set_world_matrix	( xray::float4x4 const& w )
{
	if ( m_scene )
		m_scene->set_w(w);

	m_world_matrix				= w;
}

xray::float4x4 const& platform::get_world_matrix	( ) const
{
	return m_world_matrix;
}

xray::float4x4 const&	platform::get_view_matrix	( ) const
{
	return m_view_matrix;
}

xray::float4x4 const& platform::get_projection_matrix	( ) const
{
	return m_proj_matrix;
}


void platform::draw_debug_lines			( colored_vertices_type const& vertices, colored_indices_type const& indices )
{

	//if (!g_default_mode)
	//	return;

	ASSERT							( indices.size( ) % 2 == 0 );

	R_CHK(device( ).SetVertexShader		( 0 ));
	R_CHK(device( ).SetPixelShader		( 0 ));
	R_CHK(device( ).SetTexture			( 0, 0 ));
// 	R_CHK(device( ).SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE ));
// 	R_CHK(device( ).SetRenderState		( D3DRS_ZWRITEENABLE, TRUE ));
// 	R_CHK(device( ).SetRenderState		( D3DRS_ZFUNC, D3DCMP_LESSEQUAL ));

	u32 const vertex_count			= vertices.size( );

	render::vertex_colored	*vertices_locked = 0;
	u32 const vertices_size			= vertex_count*sizeof(render::vertex_colored);
	R_CHK							( m_debug_vb->Lock(0, vertices_size, (pvoid*)&vertices_locked, 0) );
	memory::copy					( vertices_locked, vertices_size, &*vertices.begin(), vertices_size);
	R_CHK							( m_debug_vb->Unlock() );

	u32 const index_count			= indices.size( );

	u16								*indices_locked = 0;
	u32 const indices_size			= index_count*sizeof(u16);
	R_CHK							( m_debug_ib->Lock(0, indices_size, (pvoid*)&indices_locked,0) );
	memory::copy					(indices_locked, indices_size, &*indices.begin(), indices_size );
	R_CHK							( m_debug_ib->Unlock() );

	R_CHK							( device().SetFVF( vertex_colored_fvf ) );
	R_CHK							( device().SetStreamSource( 0, m_debug_vb, 0, sizeof( render::vertex_colored ) ) );
	R_CHK							( device().SetIndices( m_debug_ib) );
	R_CHK							( 
		device().DrawIndexedPrimitive	(
			D3DPT_LINELIST,
			0,
			0,
			vertex_count,
			0,
			index_count/2
		)
	);
}

void platform::draw_debug_triangles		( colored_vertices_type const& vertices, colored_indices_type const& indices )
{
	//if (!g_default_mode)
	//	return;

	R_CHK(device( ).SetVertexShader		( 0 ));
	R_CHK(device( ).SetPixelShader		( 0 ));
// 	R_CHK(device( ).SetRenderState		( D3DRS_ZWRITEENABLE, TRUE ));
// 	R_CHK(device( ).SetRenderState		( D3DRS_ZFUNC, D3DCMP_LESSEQUAL ));
	R_ASSERT						( indices.size( ) % 3 == 0 );
	u32								vertex_buffer_size = vertices.size( )*sizeof( render::vertex_colored );

	colored_vertices_type::value_type		*vertices_locked = 0;
	R_CHK							( m_debug_vb->Lock(0, vertex_buffer_size, (pvoid*)&vertices_locked, 0) );
	memory::copy					(vertices_locked, vertex_buffer_size, &*vertices.begin(), vertex_buffer_size);
	R_CHK							( m_debug_vb->Unlock() );

	u32								index_buffer_size = indices.size( ) * sizeof( u16 );
	u16*							indices_locked = 0;
	R_CHK							( m_debug_ib->Lock(0, index_buffer_size, (pvoid*)&indices_locked,0) );
	memory::copy					(indices_locked, index_buffer_size, &*indices.begin(), indices.size( )*sizeof( u16 ) );
	R_CHK							( m_debug_ib->Unlock() );

	R_CHK							( device().SetFVF( vertex_colored_fvf ) );
	R_CHK							( device().SetStreamSource( 0, m_debug_vb, 0, sizeof(render::vertex_colored) ) );
	R_CHK							( device().SetIndices( m_debug_ib) );

	R_CHK							( 
		device().DrawIndexedPrimitive	(
			D3DPT_TRIANGLELIST,
			0,
			0,
			vertices.size( ),
			0,
			indices.size( )/3
		)
	);

}

void platform::draw_editor_lines		( colored_vertices_type const& vertices, colored_indices_type const& indices )
{
	ASSERT							( indices.size( ) % 2 == 0 );

// 	if( use_alpha )
// 	{
// 		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, TRUE) );
// 		R_CHK			( device().SetRenderState		( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
// 		R_CHK			( device().SetRenderState		( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );
// 	}
//	else
// 		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE) );


	R_CHK(device( ).SetVertexShader		( 0 ));
	R_CHK(device( ).SetPixelShader		( 0 ));
	R_CHK(device( ).SetTexture			( 0, 0 ));

	u32 const vertex_count			= vertices.size( );

	render::vertex_colored	*vertices_locked = 0;
	u32 const vertex_buffer_size	= vertex_count*sizeof(render::vertex_colored);
	R_CHK							( m_editor_vb->Lock(0, vertex_buffer_size, (pvoid*)&vertices_locked, 0) );
	memory::copy					(vertices_locked, vertex_buffer_size, &*vertices.begin(), vertex_buffer_size );
	R_CHK							( m_editor_vb->Unlock() );

	u32 const index_count			= indices.size( );

	u16								*indices_locked = 0;
	u32 const index_buffer_size		= index_count*sizeof(u16);
	R_CHK							( m_editor_ib->Lock(0, index_buffer_size, (pvoid*)&indices_locked,0) );
	memory::copy					(indices_locked, index_buffer_size, &*indices.begin(), index_buffer_size );
	R_CHK							( m_editor_ib->Unlock() );

	R_CHK							( device().SetFVF( vertex_colored_fvf ) );
	R_CHK							( device().SetStreamSource( 0, m_editor_vb, 0, sizeof( render::vertex_colored ) ) );
	R_CHK							( device().SetIndices( m_editor_ib) );
	R_CHK							( 
		device().DrawIndexedPrimitive	(
		D3DPT_LINELIST,
		0,
		0,
		vertex_count,
		0,
		index_count/2
		)
	);

//   	if( use_alpha )
//   		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE) );

}

void platform::draw_editor_triangles( colored_vertices_type const& vertices, colored_indices_type const& indices )
{
// 	if( use_alpha )
// 	{
// 		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, TRUE) );
// 		R_CHK			( device().SetRenderState		( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
// 		R_CHK			( device().SetRenderState		( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );
// 	}
// 	else
// 		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE) );


// 	R_CHK(device( ).SetVertexShader		( 0 ));
// 	R_CHK(device( ).SetPixelShader		( 0 ));
	R_ASSERT						( indices.size( ) % 3 == 0 );
	u32								vertex_buffer_size = vertices.size( )*sizeof( render::vertex_colored );

	colored_vertices_type::value_type		*vertices_locked = 0;
	R_CHK							( m_editor_vb->Lock(0, vertex_buffer_size, (pvoid*)&vertices_locked, 0) );
	memory::copy					(vertices_locked, vertex_buffer_size, &*vertices.begin(), vertex_buffer_size);
	R_CHK							( m_editor_vb->Unlock() );

	u32								index_buffer_size = indices.size( ) * sizeof( u16 );
	u16*							indices_locked = 0;
	R_CHK							( m_editor_ib->Lock(0, index_buffer_size, (pvoid*)&indices_locked,0) );
	memory::copy					(indices_locked, index_buffer_size, &*indices.begin(), indices.size( )*sizeof( u16 ) );
	R_CHK							( m_editor_ib->Unlock() );

	R_CHK							( device().SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE ) );
	R_CHK							( device().SetStreamSource( 0, m_editor_vb, 0, sizeof(colored_vertices_type::value_type) ) );
	R_CHK							( device().SetIndices( m_editor_ib) );

	R_CHK							( 
		device().DrawIndexedPrimitive	(
		D3DPT_TRIANGLELIST,
		0,
		0,
		vertices.size( ),
		0,
		indices.size( )/3
		)
	);

// 	if( use_alpha )
// 		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE) );
}

void platform::draw_ui_vertices	( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type)
{
	//if (!g_default_mode)
	//	return;
	
	R_CHK(device( ).SetVertexShader		( 0 ));
	u32 vertex_buffer_size			= count * sizeof(render::ui::vertex);
	render::ui::vertex		*vertices_locked = 0;
	R_CHK							( m_ui_vb->Lock(0, vertex_buffer_size, (pvoid*)&vertices_locked, 0) );
	memory::copy					(vertices_locked, vertex_buffer_size, vertices, vertex_buffer_size);
	R_CHK							( m_ui_vb->Unlock() );

	R_CHK							( device().SetTexture( 0, (point_type==0)?m_ui_texture_font:m_ui_texture ) );

	R_CHK							( device().SetFVF( ui_vertex_fvf ) );
	R_CHK							( device().SetStreamSource( 0, m_ui_vb, 0, sizeof(render::ui::vertex) ) );

	int dx_prim_count = 0;
	_D3DPRIMITIVETYPE dx_prim_type = D3DPT_TRIANGLELIST;
	switch (prim_type)
	{
	case 0:
		dx_prim_count	= count/2;
		dx_prim_type	= D3DPT_TRIANGLELIST;
		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, TRUE) );
		R_CHK			( device().SetRenderState		( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
		R_CHK			( device().SetRenderState		( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );
		R_CHK			( device().SetIndices			( m_ui_ib ) );
		switch(point_type)
		{
		case 0:
			R_CHK			( device().SetPixelShader		( m_ui_ps_font ) );
			break;
		case 1:
			R_CHK			( device().SetPixelShader		( m_ui_ps ) );
			break;
		case 2:
			R_CHK			( device().SetPixelShader		( m_ui_ps_fill ) );
			break;
		}
		R_CHK			( device().DrawIndexedPrimitive	( dx_prim_type, 0, 0, count, 0, dx_prim_count )	);
		break;
	case 1:
		dx_prim_count	= count-1;
		dx_prim_type	= D3DPT_LINESTRIP;
		R_CHK			( device().SetPixelShader		( m_ui_ps_font ) );
		R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE) );
		R_CHK			( device().DrawPrimitive		( dx_prim_type, 0, dx_prim_count )	);
		break;
	};

	R_CHK			( device().SetRenderState		( D3DRS_ALPHABLENDENABLE, FALSE) );
	R_CHK			( device().SetTexture( 0, 0 ) );
}

void platform::destroy_resources()
{
	m_ui_vb->Release( );
	m_ui_ib->Release( );
	m_ui_ps->Release( );
	m_ui_ps_font->Release( );
	m_ui_ps_fill->Release( );
	m_ui_texture->Release( );
	m_ui_texture_font->Release( );

	m_debug_vb->Release( );
	m_debug_ib->Release( );

	R_CHK						( m_editor_vb->Release( ) );
	R_CHK						( m_editor_ib->Release( ) );
}

void platform::create_resources()
{
	R_CHK							( 
		device().CreateVertexBuffer(
			1024*512, //512 kB
			0,
			D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1,
			D3DPOOL_MANAGED,
			&m_ui_vb,
			0
		)
	);
	R_CHK							( 
		device().CreateVertexBuffer(
			1024*512, //512 kB
			0,
			D3DFVF_XYZ | D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED,
			&m_debug_vb,
			0
		)
	);

	R_CHK							( 
		device().CreateVertexBuffer(
		1024*512, //512 kB
		0,
		D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_MANAGED,
		&m_editor_vb,
		0
		)
	);



	const u32 quad_count		= 4096; //1quad = 2triangle
	const u32 quad_idx_count	= quad_count*2*3;

	R_CHK							(
		device().CreateIndexBuffer(
			quad_idx_count*sizeof(u16),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_ui_ib,
			NULL
		)
	);
	
	u16		*indices_type		= 0;
	R_CHK(m_ui_ib->Lock(0,0,(void**)&indices_type,0));
	{
		int		Cnt = 0;
		int		ICnt= 0;
		for (int i=0; i<quad_count; i++)
		{
			indices_type[ICnt++]=u16(Cnt+0);
			indices_type[ICnt++]=u16(Cnt+1);
			indices_type[ICnt++]=u16(Cnt+2);

			indices_type[ICnt++]=u16(Cnt+3);
			indices_type[ICnt++]=u16(Cnt+2);
			indices_type[ICnt++]=u16(Cnt+1);

			Cnt+=4;
		}
	}
	R_CHK(m_ui_ib->Unlock());

	const u32 vertex_count	= 32769; // 32769 % 3 == 0
	const u32 index_count	= vertex_count*2;


	R_CHK							(
		device().CreateIndexBuffer(
			index_count*sizeof(u16),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_debug_ib,
			NULL
		)
	);

	R_CHK							(
		device().CreateIndexBuffer(
		index_count*sizeof(u16),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_editor_ib,
		NULL
		)
	);
/*	
	R_CHK( D3DXCreateTextureFromFile(	m_device, 
										"e://stk2//resources//textures//ui//ui_skull.dds", 
										&m_ui_texture ) );
	R_CHK( D3DXCreateTextureFromFile(	m_device, 
										"e://stk2//resources//textures//ui//ui_font_arial_21_1024.dds",
										&m_ui_texture_font ) );
	
*/
	resources::request	resources[]	= {
		{ "resources/textures/ui/ui_skull.dds",					xray::resources::raw_data_class },
		{ "resources/textures/ui/ui_font_arial_21_1024.dds",	xray::resources::raw_data_class },
	};

	xray::resources::query_resources	(
		resources,
		array_size(resources),
		boost::bind(&platform::on_ui_texture_loaded, this, _1),
		render::g_allocator
	);

	pcstr const ui_ps_source_code = 
"struct 	v2p									\n"
"{\n"
"half2 	tc0: 	TEXCOORD0;						\n"
"half4	c0:		COLOR0;							\n"
"};\n"
"\n"
" sampler2D s_base: register(s0);               \n"
"\n"
"half4 	main_ps_font	( v2p I )	: COLOR		\n"
"{												\n"
"	half4 r 	= tex2D	(s_base,I.tc0);			\n"
"	r.rgb		= I.c0.rgb; 					\n"
"	r.a			*= I.c0.a;						\n"
"	return 		r;								\n"
"}\n"

"half4 	main_ps_image	( v2p I )	: COLOR		\n"
"{												\n"
"	half4 r 	= tex2D	(s_base,I.tc0);			\n"
"	r			*= I.c0;						\n"
"	return 		r;								\n"
"}\n"
"half4 	main_ps_color	( v2p I )	: COLOR		\n"
"{												\n"
"	return		I.c0;							\n"
"}\n";

 	create_pixel_shader(ui_ps_source_code, &m_ui_ps_font, /*m_device*/&device(), "main_ps_font");
 	create_pixel_shader(ui_ps_source_code, &m_ui_ps, /*m_device*/&device(), "main_ps_image");
 	create_pixel_shader(ui_ps_source_code, &m_ui_ps_fill, /*m_device*/&device(), "main_ps_color");
}

void platform::on_ui_texture_loaded	( xray::resources::queries_result& data )
{
	ASSERT							( !data.empty( ) );
	ASSERT							( data.size( ) == 2 );
	
	//ui_skull.dds
	resources::pinned_ptr_const<u8> const tex_data0	( data[ 0 ].get_managed_resource( ) );
	D3DXCreateTextureFromFileInMemory(	&device(), tex_data0.c_ptr(), tex_data0.size(), &m_ui_texture ); 
	
	//ui_font_arial_21_1024.dds
	resources::pinned_ptr_const<u8> const tex_data1	( data[ 1 ].get_managed_resource( ) );
	D3DXCreateTextureFromFileInMemory(	&device(), tex_data1.c_ptr(), tex_data1.size(), &m_ui_texture_font ); 
}

void platform::clear_zbuffer		( float z_value )
{
	device().Clear(0, 0, D3DCLEAR_ZBUFFER, 0x00000000, z_value, 0 );
}

void platform::draw_screen_lines	( math::float2 const* points, u32 count, color clr, float width, u32 pattern )
{
	if( !m_line )
	{
		D3DXCreateLine( &device(), &m_line );
		ASSERT( m_line );
		
		m_line->SetPatternScale	( 1.f );
		m_line->SetWidth		( 1.f );
		m_line->SetGLLines		( true );
	}

	m_line->SetPattern	( pattern );
	m_line->SetWidth	( width );

	//m_line->Begin();
	m_line->Draw( (D3DXVECTOR2*)points, count, clr.get_d3dcolor() );
	//m_line->End();
}
void platform::draw_3D_screen_lines	( math::float3 const* points, u32 count, color clr, float width, u32 pattern, bool use_depth )
{
	(void)&use_depth;

	float2* points_2d = NEW_ARRAY(float2, count);

	float4x4 proj_mat = math::mul4x4( m_view_matrix, m_proj_matrix );
	math::rectangle<math::int2_pod> viewport = get_viewport		( );

	for ( u32 i = 0; i < count; ++i)
	{
		float4 tmp;
		tmp.set( points[i].x, points[i].y, points[i].z, 1 );
		tmp = proj_mat.transform( tmp );

		points_2d[i] = math::float2( (1+tmp.x/tmp.w)*viewport.max.x/2, (1-tmp.y/tmp.w)*viewport.max.y/2 );
	}

	draw_screen_lines(points_2d,count,clr,width,pattern);

	DELETE_ARRAY(points_2d);
}
void platform::setup_grid_render_mode		( u32 grid_density )
{
	// This sets up a specific render mode when primitives can be rendered with
	// disabled depth buffer and by a special grid stencil

	device().SetRenderState( D3DRS_STENCILENABLE, TRUE );
	device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
	device().SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	device().SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );
	device().SetRenderState( D3DRS_STENCILREF,	0xFF );
	device().SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
	device().SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
	
	device().SetRenderState( D3DRS_ZENABLE, FALSE );
	device().SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

 	device().SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
 	device().SetRenderState( D3DRS_ALPHAREF, 0x01 );
 	device().SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	device().SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	device().SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO);
	device().SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);

	if( grid_density == 25 )
		device().SetTexture( 0, m_grid_texture_25 );
	else
		device().SetTexture( 0, m_grid_texture_50 );

	device().SetSamplerState( 0, D3DSAMP_ADDRESSU, /*D3DTADDRESS_MIRROR*/D3DTADDRESS_WRAP );
	device().SetSamplerState( 0, D3DSAMP_ADDRESSV, /*D3DTADDRESS_MIRROR*/D3DTADDRESS_WRAP );
	device().SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	device().SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	device().SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	device().SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1);
	device().SetPixelShader( 0 );

	int const vert_size = 6;
	math::rectangle<math::int2_pod> viewport = platform::get_viewport		( );
	float const width	= (float)viewport.width();
	float const heihgt	= (float)viewport.height();

	float vertices_type[][vert_size] =
	{
		// x, y, z, w, tc
		{ 0.f,		0.f,	0.0f, 1.f, 0.25,				0.25f },
		{ width,	0.f,	0.0f, 1.f, width*0.5f+0.25f,	0.25f },
		{ 0.f,		heihgt,	0.0f, 1.f, 0.25,				heihgt*0.5f+0.25f },
		{ width,	heihgt,	0.0f, 1.f, width*0.5f+0.25f,	heihgt*0.5f+0.25f }
	};

	device().DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices_type, 4*vert_size);
	device().SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	device().SetRenderState( D3DRS_ALPHAREF, 0x0 );
	device().SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );

	device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
	device().SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	device().SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

	device().SetTexture			( 0, 0 );

	//device().SetRenderState( D3DRS_ZENABLE, TRUE );

	device().SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device().SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE);
	device().SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO);
}

void platform::remove_grid_render_mode		( )
{
	device().SetRenderState( D3DRS_STENCILENABLE, FALSE );
	device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
	device().SetRenderState( D3DRS_ZENABLE, TRUE );
	device().SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
}

math::rectangle<math::int2_pod> platform::get_viewport		( ) const
{
	D3DVIEWPORT9 viewport;
	device().GetViewport( &viewport );
	
	return math::rectangle<math::int2_pod>( math::int2(viewport.X, viewport.Y), math::int2(viewport.Width, viewport.Height) );
}


void platform::setup_rotation_control_modes ( int step, int ref_value )
{
	if( step == 0 ) // Reset mode.
	{
		device().SetRenderState( D3DRS_STENCILENABLE, FALSE );
		device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		device().SetRenderState( D3DRS_ZENABLE, TRUE );
		device().SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
	else if( step == 1 ) // First step
	{
		device().SetRenderState( D3DRS_ZENABLE, TRUE );
		device().SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		device().SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		device().SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		device().SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}
	else if( step == 2 ) 
	{
		device().Clear(0, 0, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0 );

		device().SetRenderState( D3DRS_STENCILENABLE, TRUE );
		device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		device().SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
		device().SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );
		device().SetRenderState( D3DRS_STENCILREF,	ref_value );
		device().SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
		device().SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
		device().SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		device().SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		device().SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}
	else if ( step == 3 )
	{
		device().SetRenderState( D3DRS_STENCILREF,	ref_value );
	}
	else if ( step == 4 )
	{
		device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		device().SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECRSAT );
		device().SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		device().SetRenderState( D3DRS_STENCILREF,	ref_value );
		device().SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		device().SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		device().SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	}
	else if ( step == 5 )
	{
		device().SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
		device().SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		device().SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		device().SetRenderState( D3DRS_STENCILREF,	ref_value );
		device().SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}
}

void platform::initialize_grid_textures	( )
{
	// creating grid texture of 50 percent density 
	int const width_texture_50 = 2;
	int const height_texture_50 = 2;
	HRESULT res = device().CreateTexture( width_texture_50, height_texture_50, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_grid_texture_50, NULL );

	D3DLOCKED_RECT	locked_rect; 
	res = m_grid_texture_50->LockRect( 0, &locked_rect, NULL, 0 );
	ASSERT(res == S_OK && locked_rect.pBits);

	// Filling texture like this:
	//								+ -
	//								- +
	for( u32 i = 0; i< height_texture_50; ++i )
	{
		for( u32 j = 0; j< width_texture_50; ++j )
		{
			u32 tmp = 0xFF000000*(((i&0x01)+(j&0x01))&0x01);
			*((u32*)((char*)locked_rect.pBits + i*locked_rect.Pitch + j*sizeof(u32))) = tmp;
		}
	}

	m_grid_texture_50->UnlockRect( 0 );

	//////////////////////////////////////////////////////////////////////////
	// creating grid texture of 25 percent density 
	int const width_texture_25 = 2;
	int const height_texture_25 = 2;
	res = device().CreateTexture( width_texture_25, height_texture_25, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_grid_texture_25, NULL );

	locked_rect; 
	res = m_grid_texture_25->LockRect( 0, &locked_rect, NULL, 0 );
	ASSERT(res == S_OK && locked_rect.pBits);

	// Filling texture like this:
	//								+ -
	//								- -
	for( u32 i = 0; i< height_texture_25; ++i )
	{
		for( u32 j = 0; j< width_texture_25; ++j )
		{
			u32 tmp = 0xFF000000*((i&0x01)*(j&0x01));
			*((u32*)((char*)locked_rect.pBits + i*locked_rect.Pitch + j*sizeof(u32))) = tmp;
		}
	}

	m_grid_texture_25->UnlockRect( 0 );
}

void platform::release_grid_textures	( )
{
	m_grid_texture_50->Release( );
	m_grid_texture_25->Release( );
}

void platform::setup_editor_selected_color(res_constant* c)
{
	backend::get_ref().set_c(c, m_editor_selected_color);
}

void create_pixel_shader(pcstr text, IDirect3DPixelShader9** dest, IDirect3DDevice9* device, pcstr entrypoint)
{
	ID3DXBuffer* shader_code		= 0;
    ID3DXBuffer* error_message		= 0;
	HRESULT hr = D3DXCompileShader				(	text,
										xray::strings::length( text ),
										0, 0, entrypoint, "ps_3_0", D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR,
										&shader_code,
										&error_message, 0);

	if( FAILED( hr))
	{
		LOG_ERROR("! entrypoint: %s", entrypoint);
		LOG_ERROR("! hr: %d", hr);
		LOG_ERROR("! PS: %s", text);
		if (error_message)
		{
			pcstr err_msg = (LPCSTR)error_message->GetBufferPointer();
			LOG_ERROR("!error: %s", err_msg);
		}
		else
			LOG_ERROR("error_message: NULL");

		LOG_ERROR ("!Shader is not created");
		return;
	}

	R_CHK(hr);
	device->CreatePixelShader		( ( DWORD* )shader_code->GetBufferPointer( ), dest );

	safe_release(shader_code);
	safe_release(error_message);
}

void create_vertex_shader(pcstr text, IDirect3DVertexShader9** dest, IDirect3DDevice9* device, pcstr entrypoint)
{
	ID3DXBuffer* shader_code		= 0;
    ID3DXBuffer* error_message		= 0;
	HRESULT hr = D3DXCompileShader				(	text,
										xray::strings::length( text ),
										0, 0, entrypoint, "vs_3_0", 0/*D3DXSHADER_PACKMATRIX_ROWMAJOR*/,
										&shader_code,
										&error_message, 0);
	if( FAILED( hr))
	{
		LOG_ERROR("! entrypoint: %s", entrypoint);
		LOG_ERROR("! hr: %d", hr);
		LOG_ERROR("! VS: %s", text);
		if (error_message)
		{
			pcstr err_msg = (LPCSTR)error_message->GetBufferPointer();
			LOG_ERROR("!error: %s", err_msg);
		}
		else
			LOG_ERROR("error_message: NULL");

		LOG_ERROR ("!Shader is not created");
		return;
	}
    device->CreateVertexShader		( ( DWORD* )shader_code->GetBufferPointer( ), dest );

	safe_release(shader_code);
	safe_release(error_message);
}

//void platform::test	( pcstr args )
//{
//	resources::request	resources[]	= {
//		{ "resources/barrel_close_1.ogf",					xray::resources::raw_data_class },
////		{ "resources/stalker_dolg_1.ogf",			xray::resources::raw_data_class },
//	};
//
//	xray::resources::query_resources	(
//		resources,
//		array_size(resources),
//		resources::query_callback(
//			this,
//			&platform::on_test_visual_loaded
//		),
//		render::g_allocator
//	);
//}

class test_blender: public blender
{
public:
		void compile(blender_compiler& compiler, const blender_compilation_options& options)
		{
			XRAY_UNREFERENCED_PARAMETER	( options );
			shader_defines_list	defines;
			make_defines(defines);

			compiler.begin_technique(/*LEVEL*/);
			compiler.begin_pass("test", "test", defines)
					.set_z_test(TRUE)
					.set_alpha_blend(FALSE)
				.end_pass()
			.end_technique();
		}

private:
};

//void platform::on_test_visual_loaded	( xray::resources::queries_result& data )
//{
//	ASSERT							( !data.empty( ) );
//	
//	pcbyte const ogf_data			= data[ 0 ].get_managed_resource( )->pin( );
//	u32 const ogf_data_size			= data[ 0 ].get_managed_resource( )->get_size( );
//
//
//	memory::chunk_reader		chunk_reader(ogf_data, ogf_data_size, chunk_reader::chunk_type_sequential);
//	
//	u16 type					= model_manager::get_visual_type(chunk_reader);
//	m_test_visual				= model_manager::create_instance(type);
//	m_test_visual->load			(chunk_reader);
//
//	data[ 0 ].get_managed_resource( )->unpin( ogf_data );
//
//	test_blender				test_visual_blender;
//	m_test_visual_shader		= shader_manager::get_ref().create_shader(&test_visual_blender);
//
//	xray::resources::query_resources	(
//		resources,
//		array_size(resources),
//		resources::query_callback(
//			this,
//			&platform::on_test_visual_loaded
//		),
//		render::g_allocator
//	);
//}
//
//void platform::on_test_visual_loaded	( xray::resources::queries_result& data )
//{
//	ASSERT							( !data.empty( ) );
//	
//	pcbyte const ogf_data			= data[ 0 ].get_managed_resource( )->pin( );
//	u32 const ogf_data_size			= data[ 0 ].get_managed_resource( )->get_size( );
//
//
//	memory::chunk_reader		chunk_reader(ogf_data, ogf_data_size, chunk_reader::chunk_type_sequential);
//	
//	u16 type					= model_manager::get_visual_type(chunk_reader);
//	m_test_visual				= model_manager::create_instance(type);
//	m_test_visual->load			(chunk_reader);
//
//	data[ 0 ].get_managed_resource( )->unpin( ogf_data );
//
//	struct test_blender: public blender
//	{
//		void compile(blender_compiler& compiler, const blender_compilation_options& options)
//		{
//			compiler.begin_technique(/*LEVEL*/);
//			compiler.begin_pass("test", "test")
//					.set_z_test(TRUE)
//					.set_alpha_blend(FALSE)
//				.end_pass()
//			.end_technique();
//		}
//	}test_visual_blender;
//
//	m_test_visual_shader		= shader_manager::get_ref().create_shader(&test_visual_blender);
//}

void platform::add_visual( u32 id, xray::render::visual_ptr v, float4x4 const& transform, bool selected, bool system_object, bool beditor )
{	
	m_model_mgr.add_editor_visual( id, &(*v), transform, selected, system_object, beditor );

// 	backend::get_ref().invalidate();
// 	if( !selected )
// 	{
// 		set_world_matrix	(transform);
// 		v->render			();
// 	}
// 	else
// 	{
// 		float4x4 selected_transform = transform;
// 		const float shift_factor = 0.001f;
// 		selected_transform.c.xyz() += ( (math::invert4x3( m_view_matrix ).c.xyz() - transform.c.xyz()) * shift_factor );
// 		set_world_matrix	(selected_transform);
// 		v->render_selected	();
// 	}
}

void platform::update_visual( u32 id, xray::math::float4x4 const& transform, bool selected )
{
	m_model_mgr.update_editor_visual( id, transform, selected );
}

void platform::remove_visual( u32 id )
{
	m_model_mgr.remove_editor_visual( id );
}

void platform::render_visuals			( )
{
	if( !model_manager::get_ref().get_draw_editor())
		return;

 	model_manager::Editor_Visuals const& visuals = m_model_mgr.get_editor_visuals( );

	model_manager::Editor_Visuals::const_iterator	it = visuals.begin();
	model_manager::Editor_Visuals::const_iterator	end = visuals.end();

	for( ; it != end; ++it )
	{
		if( !it->system_object )
			continue;

 		backend::get_ref().invalidate();
		set_world_matrix	(it->transform);
		it->visual->render			();
	}

 	it = visuals.begin();
	hw_wrapper::get_ref().device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	float3 view_pos = math::invert4x3( m_view_matrix ).c.xyz();
	for( ; it != end; ++it )
	{
	 	backend::get_ref().invalidate();
		if( !it->selected )
			continue;

 		float4x4 selected_transform = it->transform;
 		const float shift_factor = 0.001f;
 		selected_transform.c.xyz() += ( ( view_pos - it->transform.c.xyz()) * shift_factor );
		set_world_matrix	(selected_transform);
		it->visual->render_selected	();
	}
	hw_wrapper::get_ref().device()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void platform::update_visual_vertex_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments )
{
	render_visual* r_visual = static_cast_checked<render_visual*>(&(*v));

	if( r_visual->type == mt_normal)
	{
		simple_visual* s_visual = static_cast_checked<simple_visual*>(r_visual);
		m_model_mgr.update_vb( s_visual->vertex_buffer_id, fragments);
	}
	else
	{
//   For testing only. Will be removed
// 		if( r_visual->type == mt_hierrarchy)
// 		{
// 			hierrarchy_visual* h_visual = static_cast_checked<hierrarchy_visual*>(r_visual);
// 			if( h_visual->m_childs[0]->type == mt_normal)
// 			{
// 				simple_visual* s_visual = static_cast_checked<simple_visual*>(h_visual->m_childs[0]);
// 				m_model_mgr.update_vb( s_visual->vertex_buffer_id, offset, size, buffer );
// 			}
// 		}
// 		else
		{
			ASSERT(false);
		}
	}
}

void platform::update_visual_index_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments )
{
	render_visual* r_visual = static_cast_checked<render_visual*>(&(*v));

	if( r_visual->type == mt_normal)
	{
		simple_visual* s_visual = static_cast_checked<simple_visual*>(r_visual);
		m_model_mgr.update_ib( s_visual->index_buffer_id, fragments );
	}
	else
	{
		ASSERT(false);
	}
}

void platform::change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture )
{
	render_visual* r_visual = static_cast_checked<render_visual*>(&(*v));
	r_visual->set_shader(shader, texture);
}

void platform::add_light					( u32 id, light_props const& props, bool beditor )
{
	lights_db::get_ref().add_light( id, props, beditor);
}

void platform::update_light				( u32 id, light_props const& props, bool beditor )
{
	lights_db::get_ref().update_light( id, props, beditor);
}

void platform::remove_light				( u32 id, bool beditor )
{
	lights_db::get_ref().remove_light( id, beditor);
}
 
void platform::terrain_add_cell			( visual_ptr v, bool beditor )
{
	terrain::get_ref().add_cell( v, beditor );
}

void platform::terrain_remove_cell			( visual_ptr v, bool beditor )
{
	terrain::get_ref().remove_cell( v, beditor );
}

void platform::terrain_update_cell_buffer	( visual_ptr v, xray::vectora<xray::render::buffer_fragment_NEW> const& fragments, float4x4 const& transform)
{
	terrain::get_ref().update_cell_buffer( v, fragments, transform);
}

void platform::terrain_add_cell_texture	(  visual_ptr v, texture_string const & texture, u32 user_tex_id)
{
	terrain::get_ref().add_cell_texture( v, texture, user_tex_id);
}

void platform::terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id)
{
	terrain::get_ref().remove_cell_texture( v, user_tex_id);
}

void platform::terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture)
{
	terrain::get_ref().exchange_texture( old_texture, new_texture);
}

void platform::set_editor_render_mode		( bool draw_editor, bool draw_game )
{
	model_manager::get_ref().set_draw_editor	( draw_editor);
	model_manager::get_ref().set_draw_game		( draw_game);
}

} // namespace dx9
} // namespace render 
} // namespace xray 

