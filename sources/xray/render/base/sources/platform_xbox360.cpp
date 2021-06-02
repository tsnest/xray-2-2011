////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform_xbox360.h"
#include <xray/render/base/api.h>
#include <xray/render/base/debug_renderer.h>
#include <xray/render/base/ui_renderer.h>
#include <xray/render/base/game_renderer.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"xgraphics.lib")

using xray::resources::managed_resource_ptr;
using xray::render::engine::wrapper;
using xray::render::visual_ptr;



namespace xray {
namespace render {
namespace xbox360 {

render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle)
{
	UNREFERENCED_PARAMETER( window_handle );

	return NEW( platform)( wrapper);
}

platform::platform		( wrapper& wrapper ) :
	m_vertex_shader					( 0 ),
	m_pixel_shader					( 0 ),
	m_vertex_buffer					( 0 ),
	m_index_buffer					( 0 ),
	m_frame_id						( 0 ),
	m_started_scene					( false )
{
	XRAY_UNREFERENCED_PARAMETER( wrapper);
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

platform::~platform						( )
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

void platform::clear_resources			( )
{
}

void platform::create_device			( )
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

void platform::create_vertex_shader	( )
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

void platform::create_pixel_shader		( )
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

void platform::create_vertex_buffer	( )
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

void platform::create_index_buffer		( )
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

void platform::create_vertex_declaration ( )
{
    D3DVERTEXELEMENT9 vertex_elements[3] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 },
        D3DDECL_END()
    };
    device( ).CreateVertexDeclaration ( vertex_elements, &m_vertex_declaration );
}

void platform::set_view_matrix			( xray::math::float4x4 const& view )
{
	m_view							= view;
	m_transform						= mul4x4( m_world, mul4x4( m_view, m_projection ) );
}

void platform::set_projection_matrix	( xray::math::float4x4 const& projection )
{
	m_projection					= projection;
	m_transform						= mul4x4( m_world, mul4x4( m_view, m_projection ) );
}

void platform::set_world_matrix		( xray::float4x4 const& world )
{
	m_world							= world;
	m_transform						= mul4x4( m_world, mul4x4( m_view, m_projection ) );
}

xray::float4x4 const& platform::get_world_matrix( ) const
{
	return							m_world;
}

xray::float4x4 const& platform::get_view_matrix			( ) const
{
	return							m_view;
}

xray::float4x4 const& platform::get_projection_matrix		( ) const
{
	return							m_projection;
}

xray::math::rectangle<xray::math::int2_pod> platform::get_viewport	( ) const
{
	D3DVIEWPORT9 viewport;
	device().GetViewport( &viewport );
	
	return math::rectangle<math::int2_pod>( math::int2(viewport.X, viewport.Y), math::int2(viewport.Width, viewport.Height) );
}

void platform::draw_frame				( )
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

void platform::setup_parameters		( )
{
	device( ).SetVertexShader		( m_vertex_shader);
	device( ).SetPixelShader		( m_pixel_shader );
	device( ).SetVertexShaderConstantF ( 0, (float*)&m_transform, 4 );
	device( ).SetVertexDeclaration	( m_vertex_declaration );
}

void platform::draw_debug_lines		( xray::render::vector< xray::render::vertex_colored > const& vertices, xray::render::vector< u16 > const& indices )
{
	ASSERT							( indices.size( ) % 2 == 0 );

	setup_parameters				( );

	u32 const vertex_count			= vertices.size( );

	colored_vertices_type::value_type		*vertices_locked = 0;
	HRESULT result					= m_vertex_buffer->Lock( 0, vertex_count*sizeof( colored_vertices_type::value_type ), (pvoid*)&vertices_locked, 0);
	ASSERT							( !FAILED( result ) );

	memory::copy					( vertices_locked, vertex_count*sizeof( colored_vertices_type::value_type ), &*vertices.begin( ), vertex_count*sizeof( colored_vertices_type::value_type ) );

	result							= m_vertex_buffer->Unlock( );
	ASSERT							( !FAILED( result ) );

	u32 const index_count			= indices.size( );
	colored_indices_type::value_type*	indices_locked = 0;
	result							= m_index_buffer->Lock( 0, index_count*sizeof( colored_indices_type::value_type ), ( pvoid* )&indices_locked, 0 );
	ASSERT							( !FAILED( result ) );

	memory::copy					( indices_locked, index_count*sizeof( colored_indices_type::value_type ), &*indices.begin( ), index_count*sizeof( colored_indices_type::value_type ) );

	result							= m_index_buffer->Unlock( );
	ASSERT							( !FAILED( result ) );

	m_device->SetStreamSource		( 0, m_vertex_buffer, 0, sizeof( colored_vertices_type::value_type ) );
	m_device->SetIndices			( m_index_buffer );
	m_device->DrawIndexedPrimitive	(
		D3DPT_LINELIST,
		0,
		0,
		vertex_count,
		0,
		index_count/2
	);
}

void platform::draw_debug_triangles	( xray::render::vector< xray::render::vertex_colored > const& vertices, xray::render::vector< u16 > const& indices )
{
	R_ASSERT						( vertices.size( ) % 3 == 0 );

	setup_parameters				( );

	u32 const vertex_buffer_size	= vertices.size( )*sizeof( vertex_colored );
	colored_vertices_type::value_type		*vertices_locked = 0;
	HRESULT result					= m_vertex_buffer->Lock( 0, vertex_buffer_size, ( pvoid* )&vertices_locked, 0 );
	ASSERT							( !FAILED( result ) );

	memory::copy					( vertices_locked, vertex_buffer_size, &*vertices.begin( ), vertex_buffer_size );

	result							= m_vertex_buffer->Unlock( );
	ASSERT							( !FAILED( result ) );

	u32 const index_buffer_size		= vertices.size( ) * sizeof( u16 );
	u16*							indices_locked = 0;
	result							= m_index_buffer->Lock( 0, index_buffer_size, (pvoid*)&indices_locked,0 );
	ASSERT							( !FAILED( result ) );

	memory::copy					( indices_locked, index_buffer_size, &*indices.begin( ), indices.size( )*sizeof( u16 ) );

	result							= m_index_buffer->Unlock();
	ASSERT							(!FAILED(result));

	m_device->SetStreamSource		( 0, m_vertex_buffer, 0, sizeof(colored_vertices_type::value_type) );
	m_device->SetIndices			( m_index_buffer);
	m_device->DrawIndexedPrimitive	(
		D3DPT_TRIANGLELIST,
		0,
		0,
		vertices.size( ),
		0,
		indices.size( )/3
	);
}

void platform::add_static				( xray::resources::managed_resource_ptr const& collision, xray::resources::managed_resource_ptr const& geometry, xray::resources::managed_resource_ptr const& visuals )
{
	XRAY_UNREFERENCED_PARAMETERS	( collision, geometry, visuals );
}

void platform::draw_static				( )
{
}

void platform::draw_ui_vertices		( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type )
{
	XRAY_UNREFERENCED_PARAMETERS	( vertices, count, prim_type, point_type );
}

void platform::add_visual				( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected, bool system_object, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS	( id, v, transform, selected, system_object, beditor );
}

void platform::update_visual			( u32 id, xray::math::float4x4 const& transform, bool selected )
{
	XRAY_UNREFERENCED_PARAMETERS	( id, transform, selected );
}

void platform::remove_visual			( u32 id )
{
	XRAY_UNREFERENCED_PARAMETERS	( id );
}

void platform::render_visuals			( )
{
}

void platform::update_visual_vertex_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS	( v, fragments);
}

void platform::update_visual_index_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS	( v, fragments );
}

void platform::clear_zbuffer		( float z_value )
{
	device().Clear(0, 0, D3DCLEAR_ZBUFFER, 0x00000000, z_value, 0 );
}

void platform::draw_editor_lines		( colored_vertices_type const& vertices, colored_indices_type const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( vertices, indices );
	UNREACHABLE_CODE();
}

void platform::draw_3D_screen_lines		( math::float3 const* points, u32 count, math::color clr, float width, u32 pattern, bool use_depth )
{
	XRAY_UNREFERENCED_PARAMETERS( points, count, clr, width, pattern, use_depth );
	UNREACHABLE_CODE();
}

void platform::draw_editor_triangles	( colored_vertices_type const& vertices, colored_indices_type const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( vertices, indices );
	UNREACHABLE_CODE();
}

void platform::draw_screen_lines		( xray::math::float2 const* points, u32 count, xray::math::color clr, float width, u32 pattern )
{
	XRAY_UNREFERENCED_PARAMETERS( points, count, clr, width, pattern );
	UNREACHABLE_CODE();
}

void platform::setup_grid_render_mode	( u32 grid_density )
{
	XRAY_UNREFERENCED_PARAMETERS( grid_density );
	UNREACHABLE_CODE();
}

void platform::remove_grid_render_mode	( )
{
	UNREACHABLE_CODE();
}

void platform::setup_rotation_control_modes ( int step, int ref_value )
{
	XRAY_UNREFERENCED_PARAMETERS( step, ref_value );
	UNREACHABLE_CODE();
}

void platform::test_cooperative_level	( )
{
}

void platform::change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture )
{
	XRAY_UNREFERENCED_PARAMETERS( v, shader, texture );
	UNREACHABLE_CODE();
}

void platform::add_light					( u32 id, xray::render::light_props const& props, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS	(id, props, beditor);
	UNREACHABLE_CODE();
}

void platform::update_light				( u32 id, xray::render::light_props const& props, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS	(id, props, beditor);
	UNREACHABLE_CODE();
}

void platform::remove_light				( u32 id, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS	(id, beditor);
	UNREACHABLE_CODE();
}

void platform::terrain_add_cell			( visual_ptr v, bool beditor)
{
	XRAY_UNREFERENCED_PARAMETERS	(v, beditor);
	UNREACHABLE_CODE();
}

void platform::terrain_remove_cell			( visual_ptr v, bool beditor)
{
	XRAY_UNREFERENCED_PARAMETERS	(v, beditor);
	UNREACHABLE_CODE();
}

void platform::terrain_update_cell_buffer	( visual_ptr v, xray::vectora<xray::render::buffer_fragment_NEW> const& fragments, xray::math::float4x4 const& transform)
{
	XRAY_UNREFERENCED_PARAMETERS	(v,fragments,transform);
	UNREACHABLE_CODE();
}

void platform::terrain_add_cell_texture	( visual_ptr v, xray::render::texture_string const & texture, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS	(v,texture,user_tex_id);
	UNREACHABLE_CODE();
}

void platform::terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS	(v,user_tex_id);
	UNREACHABLE_CODE();
}

void platform::terrain_exchange_texture	( xray::render::texture_string const & old_texture, xray::render::texture_string const & new_texture)
{
	XRAY_UNREFERENCED_PARAMETERS	(old_texture,new_texture);
	UNREACHABLE_CODE();
}

void platform::set_editor_render_mode		( bool draw_editor, bool draw_game )
{
	XRAY_UNREFERENCED_PARAMETERS	(draw_editor,draw_game);
	UNREACHABLE_CODE();
}

} // namespace xbox360
} // namespace render
} // namespace xray