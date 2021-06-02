////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/world.h>
#include <xray/render/facade/particles.h>
#include <xray/render/facade/scene_view_mode.h>
#include <xray/render/facade/render_stage_types.h>
#include <xray/render/facade/ui_renderer.h>

#include <xray/os_preinclude.h>
#undef NOD3D
#pragma warning(push)
#pragma warning(disable:4995)
#include <xray/os_include.h>
#pragma warning(pop)

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"xgraphics.lib")

using xray::resources::managed_resource_ptr;

xray::render::render_allocator_type*	xray::render::g_allocator = 0;

namespace xray {
namespace render {
namespace engine {

static xray::uninitialized_reference< world > s_world;

world* create_world					( )
{
	R_ASSERT						( !s_world.initialized() );
	XRAY_CONSTRUCT_REFERENCE		( s_world, world ) ( );
	return							s_world.c_ptr( );
}

void destroy						( world*& world )
{
	R_ASSERT						( s_world.initialized() );
	R_ASSERT						( s_world.c_ptr() == world );
	XRAY_DESTROY_REFERENCE			( s_world );
	world							= 0;
}

world::world						( ) :
	m_device						( 0 ),
	m_direct3d						( 0 ),
	m_vertex_shader					( 0 ),
	m_pixel_shader					( 0 ),
	m_vertex_buffer					( 0 ),
	m_index_buffer					( 0 ),
	m_frame_id						( 0 ),
	m_started_scene					( false )
{
	create_device					( );
	create_vertex_shader			( );
	create_pixel_shader				( );
	create_vertex_buffer			( );
	create_index_buffer				( );
	create_vertex_declaration		( );

    device().SetRenderState			(D3DRS_CULLMODE, D3DCULL_NONE);
    device().SetRenderState			(D3DRS_ZENABLE, TRUE);
	device().SetRenderState			(D3DRS_FILLMODE, D3DFILL_SOLID);
}

world::~world						( )
{
	if ( m_device ) {
		m_device->Release			( );
		m_device					= 0;
	}

	if ( !m_direct3d )
		return;

	m_direct3d->Release				( );
	m_direct3d						= 0;

	m_vertex_shader->Release		( );
	m_vertex_shader					= 0;

	m_pixel_shader->Release			( );
	m_pixel_shader					= 0;

	m_vertex_buffer->Release		( );
	m_vertex_buffer					= 0;

	m_index_buffer->Release			( );
	m_index_buffer					= 0;

	m_vertex_declaration->Release	( );
	m_vertex_declaration			= 0;
}

void world::initialize				( )
{
}

void world::clear_resources			( )
{
}

void world::create_device			( )
{
	ASSERT							( !m_direct3d );
	m_direct3d						= Direct3DCreate9(D3D_SDK_VERSION);
	R_ASSERT						( m_direct3d );

    // Set up the structure used to create the D3DDevice.
    XVIDEO_MODE						video_mode;
    ZeroMemory						( &video_mode, sizeof( video_mode ) );
    XGetVideoMode					( &video_mode );
    bool const is_1280x720			= video_mode.dwDisplayHeight >= 720;

	m_world.identity				( );

	D3DXMATRIXA16					projection;
	D3DXMatrixPerspectiveFovLH		( &projection, math::pi_d4, is_1280x720 ? 1280.f/720.f : 800.f/480.f, .2f, 500.0f );
	memory::copy					( &m_projection, sizeof(m_projection), projection, sizeof( projection ) );

	D3DPRESENT_PARAMETERS params	= {
		is_1280x720 ? 1280 : 800,		// BackBufferWidth;
		is_1280x720 ? 720 : 480,		// BackBufferHeight;
		D3DFMT_A8R8G8B8,				// BackBufferFormat;
		1,								// BackBufferCount;
		D3DMULTISAMPLE_NONE,			// MultiSampleType;
		0,								// MultiSampleQuality;
		D3DSWAPEFFECT_DISCARD,			// SwapEffect;
		NULL,							// hDeviceWindow;
		FALSE,							// Windowed;
		TRUE,							// EnableAutoDepthStencil;
		D3DFMT_D24S8,					// AutoDepthStencilFormat;
		0,								// Flags;
		0,								// FullScreen_RefreshRateInHz;
		D3DPRESENT_INTERVAL_IMMEDIATE,	// FullScreen_PresentationInterval;
	};

	ASSERT							(!m_device);
	HRESULT const result			= 
		m_direct3d->CreateDevice	(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			0,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&params,
			&m_device
		);
	XRAY_UNREFERENCED_PARAMETER		( result );
	R_ASSERT						( !FAILED( result ) );
}

void world::create_vertex_shader	( )
{
	pcstr const vertex_shader_source_code = 
	" float4x4 matWVP : register(c0);              "  
	"                                              "  
	" struct VS_IN                                 "  
	"                                              "  
	" {                                            " 
	"     float4 ObjPos   : POSITION;              "  // Object space position 
	"     float4 Color    : COLOR;                 "  // Vertex color                 
	" };                                           " 
	"                                              " 
	" struct VS_OUT                                " 
	" {                                            " 
	"     float4 ProjPos  : POSITION;              "  // Projected space position 
	"     float4 Color    : COLOR;                 "  
	" };                                           "  
	"                                              "  
	" VS_OUT main( VS_IN In )                      "  
	" {                                            "  
	"     VS_OUT Out;                              "  
	"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into
	"     Out.Color = In.Color;                    "  // Projected space and 
	"     return Out;                              "  // Transfer color
	" }                                            ";

    ID3DXBuffer* shader_code		= 0;
    ID3DXBuffer* error_message		= 0;
	HRESULT const result			= 
		D3DXCompileShader(
			vertex_shader_source_code,
			strings::length( vertex_shader_source_code ),
            0, 0, "main", "vs_2_0", 0,
			&shader_code,
			&error_message, 0
		);
	XRAY_UNREFERENCED_PARAMETER		( result );
	R_ASSERT						( !FAILED( result ), "%s", error_message );

    device( ).CreateVertexShader	( ( DWORD* )shader_code->GetBufferPointer( ), &m_vertex_shader );
    shader_code->Release			( );
}

void world::create_pixel_shader		( )
{
	pcstr const pixel_shader_source_code = 
	" struct PS_IN                                 "
	" {                                            "
	"     float4 Color : COLOR;                    "  // Interpolated color from                      
	" };                                           "  // the vertex shader
	"                                              "  
	" float4 main( PS_IN In ) : COLOR              "  
	" {                                            "  
	"     return In.Color;                         "  // Output color
	" }                                            "; 

	ID3DXBuffer* shader_code		= 0;
    ID3DXBuffer* error_message		= 0;
	HRESULT const result			= 
		D3DXCompileShader(
			pixel_shader_source_code,
			strings::length( pixel_shader_source_code ),
			0, 0, "main", "ps_2_0", 0,
            &shader_code,
			&error_message, 0
		);
	XRAY_UNREFERENCED_PARAMETER		( result );
	R_ASSERT						( !FAILED( result ), "%s", error_message );

    device().CreatePixelShader		( ( DWORD* )shader_code->GetBufferPointer( ), &m_pixel_shader );
    shader_code->Release			( );
}

void world::create_vertex_buffer	( )
{
	ASSERT							( !m_vertex_buffer );
	HRESULT	const result			= 
		device().CreateVertexBuffer(
			65536*sizeof( vertex_colored ),
			0,
			D3DFVF_XYZ | D3DFVF_DIFFUSE,
			D3DPOOL_DEFAULT,
			&m_vertex_buffer,
			0
		);
	XRAY_UNREFERENCED_PARAMETER		( result );
	R_ASSERT						( !FAILED( result ) );
}

void world::create_index_buffer		( )
{
	ASSERT							( !m_index_buffer );
	HRESULT	const result			= 
		device().CreateIndexBuffer(
			65536*sizeof(u16),
			0,
			D3DFMT_INDEX16,
			D3DPOOL_DEFAULT,
			&m_index_buffer,
			0
		);
	XRAY_UNREFERENCED_PARAMETER		( result );
	R_ASSERT						( !FAILED( result ) );
}

void world::create_vertex_declaration ( )
{
    D3DVERTEXELEMENT9 vertex_elements[3] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
        D3DDECL_END()
    };
    device( ).CreateVertexDeclaration ( vertex_elements, &m_vertex_declaration );
}

void world::set_view_matrix			( xray::render::scene_view_ptr const& scene_view, xray::math::float4x4 const& view )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene_view, view );
	NOT_IMPLEMENTED( );
}

void world::set_projection_matrix	( xray::render::scene_view_ptr const& scene_view, xray::math::float4x4 const& projection )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene_view, projection );
	NOT_IMPLEMENTED( );
}

void world::set_world_matrix		( xray::render::scene_view_ptr const& scene_view, xray::float4x4 const& world )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene_view, world );
	NOT_IMPLEMENTED( );
}

void world::end_frame				( )
{
	if ( m_started_scene ) {
		//game().debug().tick			( );
		device().EndScene			( );
	}

	device( ).Present				( 0, 0, 0, 0 );

	++m_frame_id;

	device( ).Clear					(
		0,
		0,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB( 0, 0, 255 ),
		1.f,
		0
	);

	m_started_scene					= SUCCEEDED( device( ).BeginScene( ) );
}

void world::draw_text					(
		xray::vectora< ui::vertex >& output,
		pcstr const& text,
		xray::ui::font const& font,
		xray::float2 const& position,
		xray::math::color const& text_color,
		xray::math::color const& selection_color,
		u32 const max_line_width,
		bool const is_multiline,
		u32 const start_selection,
		u32 const end_selection
	)
{
	XRAY_UNREFERENCED_PARAMETERS( &output, &text, &font, &position, &text_color, &selection_color, max_line_width, is_multiline, start_selection, end_selection );
}

void world::draw_scene					( scene_ptr const& in_scene, scene_view_ptr const& view, render_output_window_ptr const& output_window, viewport_type const& viewport, boost::function< void ( bool ) > const& on_draw_scene )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &view, &output_window, &viewport, &on_draw_scene );
}

void world::setup_parameters		( )
{
	device( ).SetVertexShader		( m_vertex_shader);
	device( ).SetPixelShader		( m_pixel_shader );
	device( ).SetVertexShaderConstantF ( 0, (float*)&m_transform, 4 );
	device( ).SetVertexDeclaration	( m_vertex_declaration );
}

//void world::draw_debug_lines		( xray::render::vector< xray::render::vertex_colored > const& vertices, xray::render::vector< u16 > const& indices )
//{
//	ASSERT							( indices.size( ) % 2 == 0 );
//
//	setup_parameters				( );
//
//	u32 const vertex_count			= vertices.size( );
//
//	colored_vertices_type::value_type		*vertices_locked = 0;
//	HRESULT result					= m_vertex_buffer->Lock( 0, vertex_count*sizeof( colored_vertices_type::value_type ), (pvoid*)&vertices_locked, 0);
//	ASSERT							( !FAILED( result ) );
//
//	memory::copy					( vertices_locked, vertex_count*sizeof( colored_vertices_type::value_type ), &*vertices.begin( ), vertex_count*sizeof( colored_vertices_type::value_type ) );
//
//	result							= m_vertex_buffer->Unlock( );
//	ASSERT							( !FAILED( result ) );
//
//	u32 const index_count			= indices.size( );
//	colored_indices_type::value_type*	indices_locked = 0;
//	result							= m_index_buffer->Lock( 0, index_count*sizeof( colored_indices_type::value_type ), ( pvoid* )&indices_locked, 0 );
//	ASSERT							( !FAILED( result ) );
//
//	memory::copy					( indices_locked, index_count*sizeof( colored_indices_type::value_type ), &*indices.begin( ), index_count*sizeof( colored_indices_type::value_type ) );
//
//	result							= m_index_buffer->Unlock( );
//	ASSERT							( !FAILED( result ) );
//
//	m_device->SetStreamSource		( 0, m_vertex_buffer, 0, sizeof( colored_vertices_type::value_type ) );
//	m_device->SetIndices			( m_index_buffer );
//	m_device->DrawIndexedPrimitive	(
//		D3DPT_LINELIST,
//		0,
//		0,
//		vertex_count,
//		0,
//		index_count/2
//	);
//}
//
//void world::draw_debug_triangles	( xray::render::vector< xray::render::vertex_colored > const& vertices, xray::render::vector< u16 > const& indices )
//{
//	R_ASSERT						( vertices.size( ) % 3 == 0 );
//
//	setup_parameters				( );
//
//	u32 const vertex_buffer_size	= vertices.size( )*sizeof( vertex_colored );
//	colored_vertices_type::value_type		*vertices_locked = 0;
//	HRESULT result					= m_vertex_buffer->Lock( 0, vertex_buffer_size, ( pvoid* )&vertices_locked, 0 );
//	ASSERT							( !FAILED( result ) );
//
//	memory::copy					( vertices_locked, vertex_buffer_size, &*vertices.begin( ), vertex_buffer_size );
//
//	result							= m_vertex_buffer->Unlock( );
//	ASSERT							( !FAILED( result ) );
//
//	u32 const index_buffer_size		= vertices.size( ) * sizeof( u16 );
//	u16*							indices_locked = 0;
//	result							= m_index_buffer->Lock( 0, index_buffer_size, (pvoid*)&indices_locked,0 );
//	ASSERT							( !FAILED( result ) );
//
//	memory::copy					( indices_locked, index_buffer_size, &*indices.begin( ), indices.size( )*sizeof( u16 ) );
//
//	result							= m_index_buffer->Unlock();
//	ASSERT							(!FAILED(result));
//
//	m_device->SetStreamSource		( 0, m_vertex_buffer, 0, sizeof(colored_vertices_type::value_type) );
//	m_device->SetIndices			( m_index_buffer);
//	m_device->DrawIndexedPrimitive	(
//		D3DPT_TRIANGLELIST,
//		0,
//		0,
//		vertices.size( ),
//		0,
//		indices.size( )/3
//	);
//}

void world::draw_ui_vertices		( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type )
{
	XRAY_UNREFERENCED_PARAMETERS	( vertices, count, prim_type, point_type );
	NOT_IMPLEMENTED();
}

void world::add_model				( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, xray::math::float4x4 const& transform, bool apply_transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, transform, apply_transform );
	NOT_IMPLEMENTED();
}

void world::add_speedtree_instance( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest)
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, transform, populate_forest );
	NOT_IMPLEMENTED();
}

void engine::world::remove_speedtree_instance( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, bool populate_forest)
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, populate_forest);
	NOT_IMPLEMENTED();
}

void world::update_speedtree_instance( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest)
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, transform, populate_forest );
	NOT_IMPLEMENTED();
}

void world::update_model			( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, xray::math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, transform );
	NOT_IMPLEMENTED();
}

void world::update_system_model	( render::render_model_instance_ptr const& v, xray::math::float4x4 const& transform)
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, transform );
	NOT_IMPLEMENTED();
}


void world::remove_model			( scene_ptr const& in_scene, render::render_model_instance_ptr const& v )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v );
	NOT_IMPLEMENTED();
}

void world::set_speedtree_instance_material ( render::speedtree_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr mtl_ptr)
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &subsurface_name, &mtl_ptr );
	NOT_IMPLEMENTED();
}

void world::set_model_material( render::render_model_instance_ptr v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr mtl_ptr)
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &subsurface_name, &mtl_ptr );
	NOT_IMPLEMENTED();
}

void world::set_model_visible( render::render_model_instance_ptr v, fs_new::virtual_path_string const& subsurface_name, bool value )
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &subsurface_name, value );
	NOT_IMPLEMENTED();
}

void world::update_model_vertex_buffer	( render::render_model_instance_ptr const& v, xray::vectora<xray::render::buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, fragments);
	NOT_IMPLEMENTED();
}

void	world::set_model_ghost_mode	( render::render_model_instance_ptr const& v, bool value)
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, value );
	NOT_IMPLEMENTED();
}

void world::update_model_index_buffer	( render::render_model_instance_ptr const& v, xray::vectora<xray::render::buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS	( v, fragments );
	NOT_IMPLEMENTED();
}

void world::clear_zbuffer		( float z_value )
{
	device().Clear(0, 0, D3DCLEAR_ZBUFFER, 0x00000000, z_value, 0 );
}

void world::setup_grid_render_mode	( u32 grid_density )
{
	XRAY_UNREFERENCED_PARAMETERS( grid_density );
	NOT_IMPLEMENTED();
}

void world::remove_grid_render_mode	( )
{
	NOT_IMPLEMENTED();
}

void world::setup_rotation_control_modes ( int step )
{
	XRAY_UNREFERENCED_PARAMETERS( step );
	NOT_IMPLEMENTED();
}


//void world::change_model_shader		( xray::render::visual_ptr v, char const* shader, char const* texture )
//{
//	XRAY_UNREFERENCED_PARAMETERS( v, shader, texture );
//	NOT_IMPLEMENTED();
//}

void world::reload_shaders				( )
{
	
}

void world::reload_modified_textures	( )
{

}

void world::add_light					( scene_ptr const& in_scene, u32 id, xray::render::light_props const& props )
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, id, &props);
	NOT_IMPLEMENTED();
}

void world::update_light				( scene_ptr const& in_scene, u32 id, xray::render::light_props const& props )
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, id, &props);
	NOT_IMPLEMENTED();
}

void world::remove_light				( scene_ptr const& in_scene, u32 id )
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, id);
	NOT_IMPLEMENTED();
}

void world::add_decal					( scene_ptr const& in_scene, u32 id, xray::render::decal_properties const& properties )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id, &properties );
	NOT_IMPLEMENTED();
}

void world::update_decal				( scene_ptr const& in_scene, u32 id, xray::render::decal_properties const& properties )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id, &properties );
	NOT_IMPLEMENTED();
}

void world::remove_decal				( scene_ptr const& in_scene, u32 id )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id);
	NOT_IMPLEMENTED();
}

void world::terrain_add_cell			( scene_ptr const& in_scene, render::render_model_instance_ptr const& v)
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, v);
	NOT_IMPLEMENTED();
}

void world::update_skeleton( render::render_model_instance_ptr const& v, math::float4x4* matrices, u32 count )
{
	XRAY_UNREFERENCED_PARAMETERS	(v, matrices, count);
	NOT_IMPLEMENTED();
}

void world::terrain_remove_cell			( scene_ptr const& in_scene, render::render_model_instance_ptr const& v)
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, v);
	NOT_IMPLEMENTED();
}

void world::terrain_update_cell_buffer	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, xray::vectora<xray::render::terrain_buffer_fragment> const& fragments, xray::math::float4x4 const& transform)
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, v,fragments,transform);
	NOT_IMPLEMENTED();
}

void world::terrain_update_cell_aabb	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, math::aabb const & aabb)
{
	XRAY_UNREFERENCED_PARAMETERS(&in_scene, &v, aabb );
	NOT_IMPLEMENTED();
}

void world::terrain_add_cell_texture	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, fs_new::virtual_path_string const & texture, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, v,texture,user_tex_id);
	NOT_IMPLEMENTED();
}

void world::terrain_remove_cell_texture	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, v,user_tex_id);
	NOT_IMPLEMENTED();
}

void world::terrain_exchange_texture	( scene_ptr const& in_scene, fs_new::virtual_path_string const & old_texture, fs_new::virtual_path_string const & new_texture)
{
	XRAY_UNREFERENCED_PARAMETERS	(&in_scene, old_texture,new_texture);
	NOT_IMPLEMENTED();
}

void world::setup_view_and_output		( scene_view_ptr const& view, render_output_window_ptr const& output_window, viewport_type const& viewport)
{
	XRAY_UNREFERENCED_PARAMETERS	( &view, &output_window, viewport);
	NOT_IMPLEMENTED();
}

void engine::world::play_particle_system			( scene_ptr const& in_scene, particle::particle_system_instance_ptr in_instance, bool use_transform, bool always_looping, math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, in_instance, use_transform, always_looping, &transform );
	NOT_IMPLEMENTED();
}

void engine::world::stop_particle_system			( scene_ptr const& in_scene, particle::particle_system_instance_ptr in_instance )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_instance, &in_scene);
	NOT_IMPLEMENTED();
}


void world::remove_particle_system_instance	( particle::particle_system_instance_ptr in_instance, scene_ptr const& in_scene )
{
	XRAY_UNREFERENCED_PARAMETERS	( in_instance, &in_scene);
	NOT_IMPLEMENTED();
}

void world::update_particle_system_instance	( xray::particle::particle_system_instance_ptr instance, xray::render::scene_ptr const& in_scene, xray::math::float4x4 const& transform, bool visible, bool paused)
{
	XRAY_UNREFERENCED_PARAMETERS	( &instance, &transform, visible, paused, &in_scene);
	NOT_IMPLEMENTED();
}

#ifndef MASTER_GOLD

void world::update_preview_particle_system	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, xray::configs::lua_config_value config_value )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, config_value, &in_scene);
	NOT_IMPLEMENTED();
}

void world::update_preview_particle_system_transform	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, transform, &in_scene);
	NOT_IMPLEMENTED();
}

void world::add_preview_particle_system		( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, xray::configs::lua_config_value init_values, math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, init_values, transform, &in_scene);
	NOT_IMPLEMENTED();
}

void world::remove_preview_particle_system	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &in_scene);
	NOT_IMPLEMENTED();
}

void world::restart_preview_particle_system	( xray::particle::particle_system** preview_ps, scene_ptr const& scene )
{
	NOT_IMPLEMENTED	( XRAY_UNREFERENCED_PARAMETERS( preview_ps, &scene ) );
}

void world::set_looping_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene, bool looping )
{
	NOT_IMPLEMENTED	( XRAY_UNREFERENCED_PARAMETERS( preview_ps, &scene, looping ) );
}

void world::show_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &scene );
	NOT_IMPLEMENTED();
}

void world::draw_terrain_debug( )
{
	NOT_IMPLEMENTED();
}

u32 world::get_num_preview_particle_system_emitters		( xray::particle::particle_system** in_ps_out_ptr, scene_ptr const& in_scene ) const
{
	XRAY_UNREFERENCED_PARAMETERS		( in_ps_out_ptr, &in_scene );
	NOT_IMPLEMENTED( return 0 );
}

void world::gather_preview_particle_system_statistics	( xray::particle::particle_system** in_ps_out_ptr, scene_ptr const& in_scene, xray::particle::preview_particle_emitter_info* out_info )
{
	XRAY_UNREFERENCED_PARAMETERS	( in_ps_out_ptr, out_info, &in_scene);
	NOT_IMPLEMENTED();
}

void world::draw_3D_screen_lines		( xray::render::scene_ptr const& scene, math::float3 const* points, u32 count, math::color const& clr, float width, u32 pattern, bool use_depth )
{
	NOT_IMPLEMENTED	( XRAY_UNREFERENCED_PARAMETERS( &scene, points, count, clr, width, pattern, use_depth ) );
}

void world::draw_3D_screen_point		( xray::render::scene_ptr const& scene, float3 const& position, math::color clr, float width, bool use_depth )
{
	NOT_IMPLEMENTED	( XRAY_UNREFERENCED_PARAMETERS( &scene, position, clr, width, use_depth ) );
}

void world::draw_screen_lines			( xray::render::scene_ptr const& scene, xray::math::float2 const* points, u32 count, xray::math::color const& clr, float width, u32 pattern )
{
	NOT_IMPLEMENTED	( XRAY_UNREFERENCED_PARAMETERS( &scene, points, count, clr, width, pattern ) );
}

void engine::world::draw_render_statistics	( xray::ui::world* ui_world )
{
	XRAY_UNREFERENCED_PARAMETER( ui_world );
	NOT_IMPLEMENTED();
}

void engine::world::apply_material_changes( fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value )
{
	XRAY_UNREFERENCED_PARAMETERS(&material_name, &config_value);
}

#endif // #ifndef MASTER_GOLD

void	world::set_selection_parameters		( xray::math::float4 const& selection_color, float selection_rate )
{
	XRAY_UNREFERENCED_PARAMETERS	( selection_color, selection_rate);
	NOT_IMPLEMENTED();
}

void world::set_slomo						( xray::render::scene_ptr const& in_scene, float time_multiplier )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, time_multiplier );
	NOT_IMPLEMENTED();
}

void world::set_sky_material				( xray::render::scene_ptr const& scene, xray::resources::unmanaged_resource_ptr mtl_ptr)
{
	XRAY_UNREFERENCED_PARAMETERS	( &scene, &mtl_ptr );
	NOT_IMPLEMENTED();
}

void engine::world::toggle_render_stage		( enum_render_stage_type stage_type, bool toggle )
{
	XRAY_UNREFERENCED_PARAMETERS	( stage_type, toggle );
}

void engine::world::set_view_mode	( scene_view_ptr view_ptr, scene_view_mode view_mode )
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, view_mode );
	NOT_IMPLEMENTED();	
}

void engine::world::set_particles_render_mode		( scene_view_ptr view_ptr, xray::particle::enum_particle_render_mode render_mode )
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, render_mode );
	NOT_IMPLEMENTED();
}

void engine::world::enable_post_process				( scene_view_ptr view_ptr, bool enable )
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, enable );
	NOT_IMPLEMENTED();
}

void engine::world::set_post_process( scene_view_ptr view_ptr, resources::unmanaged_resource_ptr post_process_resource)
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, &post_process_resource );
	NOT_IMPLEMENTED();
}

particle::world& engine::world::particle_world	( scene_ptr const& scene )
{
	XRAY_UNREFERENCED_PARAMETER( scene );
	NOT_IMPLEMENTED( return *(particle::world*)0 );
}

math::uint2 engine::world::window_client_size	( render::render_output_window_ptr const& render_output_window )
{
	XRAY_UNREFERENCED_PARAMETER	( render_output_window );
	return	math::uint2( 1024, 768 );
}

void engine::world::draw_lines					( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, vertices, indices );
}

void engine::world::draw_triangles				( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, vertices, indices );
}

void engine::world::set_renderer_configuration			( fs_new::virtual_path_string const& config_name, bool async_effects )
{
	XRAY_UNREFERENCED_PARAMETERS( &config_name, async_effects );
	NOT_IMPLEMENTED				( );
}

void engine::world::on_renderer_config_loaded			( bool async_effects, resources::queries_result& data )
{
	XRAY_UNREFERENCED_PARAMETERS( &async_effects, &data );
	NOT_IMPLEMENTED				( );
}

void engine::world::on_console_commands_config_loaded	( bool load_renderer_options, resources::queries_result& data )
{
	XRAY_UNREFERENCED_PARAMETERS( &load_renderer_options, &data );
	NOT_IMPLEMENTED				( );
}

void engine::world::load_console_commands_config_query	( pcstr config_name, bool load_renderer_options )
{
	XRAY_UNREFERENCED_PARAMETERS( &config_name, load_renderer_options );
	NOT_IMPLEMENTED				( );
}

void engine::world::apply_render_options_changes		( )
{
	NOT_IMPLEMENTED				( );
}

void engine::world::reset_renderer						( bool async_effects )
{
	XRAY_UNREFERENCED_PARAMETERS( async_effects );
	NOT_IMPLEMENTED				( );
}

} // namespace engine
} // namespace render
} // namespace xray