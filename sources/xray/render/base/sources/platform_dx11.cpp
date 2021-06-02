////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/base/sources/platform_dx11.h>
#include <xray/render/base/api.h>
#include <xray/render/base/common_types.h>
#include <xray/render/base/game_renderer.h>
#include <xray/render/base/debug_renderer.h>
#include <xray/render/base/editor_renderer.h>
#include <xray/render/base/sources/platform_api.h>

#include <xray/render/core/backend.h>
#include <xray/render/engine/scene_render.h>
#include <xray/render/engine/visual.h>
#include <xray/render/engine/terrain.h>

#	pragma comment( lib,"DXGI.lib")

// Turning on this ifdef may cause crash in startup: module is created incorrect.

#if !USE_DX10
#	pragma comment( lib,"D3D11.lib")
#	pragma comment( lib,"D3DX11.lib")
#else
#	pragma comment( lib,"D3D10.lib")
#	pragma comment( lib,"D3DX10.lib")
#endif


// #pragma comment( lib,"d3d9.lib") // For pix events
#pragma comment( lib,"d3dcompiler.lib") 

// For #include <D3D9Types.h> and #include <d3dx9mesh.h> only need to be removed.
//#pragma comment( lib,"D3dx9.lib")


namespace xray {
namespace render {

void register_texture_cook();
void unregister_texture_cook();

namespace dx10 {

render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle)
{
	return NEW( platform)( wrapper, window_handle);
}


using engine::wrapper;
using namespace render_dx10;

ref_buffer		g_quad_ib;
res_buffer * create_quad_ib();

platform::platform		( wrapper& wrapper, HWND const window_handle) :
	m_device			( &wrapper, window_handle, true),
	m_backend			( m_device.d3d_device()),
	m_scene				( create_quad_ib()),
	m_system_renderer	( m_scene, create_quad_ib()),
	m_window_handle		( window_handle),
	m_frame_id			( 0)
{
	render::register_texture_cook();

	//backend::ref().on_device_create( device::ref().d3d_context());

	static float4 selection_color ( 1.f, 1.f, 0.f, 1.f );
	m_res_mgr.register_constant_binding( constant_binding("selection_color",	&selection_color));

	//m_model_mgr.init();

	m_mat_mgr.load				( );

	static models_cook			models_cooker;
	resources::register_cook	(&models_cooker);

	static terrain_cell_cook	terrain_cooker;
	resources::register_cook	(&terrain_cooker);
}
	
platform::~platform									()
{
	g_quad_ib = NULL;
}

void platform::clear_resources						()
{
	// Not sure if we need this
	m_model_mgr.clear_resources		( );
}

xray::render::dx10::renderer& platform::extended	()
{
	return				( *this);
}

void platform::set_view_matrix						( xray::math::float4x4 const& view_matrix)
{
	m_scene.set_v( view_matrix);
}

void platform::set_projection_matrix				( xray::float4x4 const& projection)
{
	m_scene.set_p( projection);
}

void platform::draw_frame						()
{
	float4x4 ident;
	ident.identity			();
	m_scene.set_w			( ident);

	++m_frame_id;

// 	game().debug().tick		();
// 	editor().tick			(); 

	device::ref().end_frame();

	if ( !device::ref().begin_frame())
		return;

	if( identity( true))
	{
		m_scene.render	();
		render_visuals	();
	}
	else
	{
		backend::ref().reset();
		backend::ref().clear_render_targets( math::color( 0.f, 0.f, 0.f));
		backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);
	}
}

void platform::draw_debug_lines			( colored_vertices_type  const& vertices, colored_indices_type  const& indices)
{
	m_scene.set_w( float4x4().identity());
	m_system_renderer.draw_lines		( vertices, indices);
}

void platform::draw_debug_triangles		( colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	m_scene.set_w( float4x4().identity());
	m_system_renderer.draw_triangles( vertices, indices);
}

u32 platform::frame_id								()
{
	return				( m_frame_id);
}

pcstr platform::type								()
{
	return				( "dx10");
}

void platform::add_static							( 
	xray::resources::managed_resource_ptr const& collision,
	xray::resources::managed_resource_ptr const& resource,
	xray::resources::managed_resource_ptr const& visuals)
{
	XRAY_UNREFERENCED_PARAMETERS	( collision, resource, visuals);
	NOT_IMPLEMENTED();
}

void platform::draw_ui_vertices	( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type)
{
	m_system_renderer.draw_ui_vertices( (vertex_formats::TL*)vertices, count, prim_type, point_type );
}

void platform::clear_zbuffer		( float z_value)
{
	backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH /*| D3D11_CLEAR_STENCIL */, z_value, 0);
}

void platform::draw_editor_lines		( colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	m_scene.set_w( float4x4().identity());
	m_system_renderer.draw_lines( vertices, indices);
}

void platform::draw_editor_triangles	( colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	m_scene.set_w( float4x4().identity());
	m_system_renderer.draw_triangles( vertices, indices);
}

void platform::draw_screen_lines	( xray::math::float2 const* points, u32 count, color clr, float width, u32 pattern)
{
	xray::math::float3* points_3d = NEW_ARRAY(xray::math::float3,count);

	// normalize coordinates
	for ( u32 i = 0; i < count; ++i)
		points_3d[i].set(
		points[i].elements[0]/(float)xray::render_dx10::device::ref().get_width()*2.0f-1.0f,
		-((points[i].elements[1]/(float)xray::render_dx10::device::ref().get_height())*2.0f-1.0f),
		0);

	m_system_renderer.draw_screen_lines( points_3d, count, clr, width, pattern, false);

	DELETE_ARRAY(points_3d);
}
void platform::draw_3D_screen_lines	( xray::math::float3 const* points, u32 count, color clr, float width, u32 pattern, bool use_depth)
{
	m_system_renderer.draw_screen_lines( points, count, clr, width, pattern, use_depth);
}

void platform::setup_grid_render_mode	( u32 grid_density)
{
	XRAY_UNREFERENCED_PARAMETER	( grid_density);
	m_system_renderer.setup_grid_render_mode( grid_density);
	
}
void platform::remove_grid_render_mode	()
{
	m_system_renderer.remove_grid_render_mode();
}
xray::math::float4x4 const&	platform::get_view_matrix			() const
{
	return m_scene.get_v();
}

xray::math::float4x4 const&	platform::get_projection_matrix	() const
{
	return m_scene.get_p();
}
xray::math::rectangle<xray::math::int2_pod> platform::get_viewport	() const
{
	D3D_VIEWPORT viewport;
	backend::ref().get_viewport( viewport);
	
#pragma warning(push)
#pragma warning(disable:4244)

	return xray::math::rectangle<xray::math::int2_pod>( xray::math::int2( viewport.TopLeftX, viewport.TopLeftY), xray::math::int2( viewport.Width, viewport.Height));

#pragma warning(pop)
}
void platform::test_cooperative_level()
{
	//nothing to be done for dx10
	//test_cooperative_level();
}

void platform::add_visual	( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected, bool system_object, bool beditor)
{
	m_model_mgr.add_editor_visual( id, &(*v), transform, selected, system_object, beditor );
}
void platform::update_visual	( u32 id, xray::math::float4x4 const& transform, bool selected)
{
	m_model_mgr.update_editor_visual( id, transform, selected );
}

void platform::remove_visual	( u32 id)
{
	m_model_mgr.remove_editor_visual( id );
}
void platform::render_visuals	()
{
	if( !model_manager::ref().get_draw_editor())
		return;

	model_manager::Editor_Visuals const& visuals = m_model_mgr.get_editor_visuals( );

	model_manager::Editor_Visuals::const_iterator	it = visuals.begin();
	model_manager::Editor_Visuals::const_iterator	end = visuals.end();

	for( ; it != end; ++it )
	{
		if( !it->system_object )
			continue;

		backend::ref().reset();
		m_scene.set_w( it->transform);
		it->visual->render			();
	}

	it = visuals.begin();

	float3 view_pos = math::invert4x3( m_scene.get_v() ).c.xyz();
	for( ; it != end; ++it )
	{
		backend::ref().reset();
		if( !it->selected )
			continue;

		float4x4 selected_transform = it->transform;
		const float shift_factor = 0.001f;
		selected_transform.c.xyz() += ( ( view_pos - it->transform.c.xyz()) * shift_factor );
		m_scene.set_w( selected_transform);
		it->visual->render_selected	();
	}
}
void platform::update_visual_vertex_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments)
{

	render_visual* r_visual = static_cast_checked<render_visual*>(&(*v));

	if( r_visual->type == mt_normal)
	{
		simple_visual* s_visual = static_cast_checked<simple_visual*>(r_visual);


		ASSERT( fragments.size() > 0 );

		u8* lock_data	= (u8*)s_visual->m_vb->map( D3D_MAP_WRITE_DISCARD);

		// Assume that the fragments are sorted.
//		u32 lock_start	= fragments[0].start;
//		u32 lock_size	= (fragments[fragments.size()-1].start+fragments[fragments.size()-1].size)-lock_start;
//		ASSERT( lock_size > 0 );

		vectora<buffer_fragment>::const_iterator	it	= fragments.begin();
		vectora<buffer_fragment>::const_iterator	end	= fragments.end();

		for( ; it != end; ++it)
			memory::copy		( lock_data + it->start, it->size, it->buffer, it->size );

		s_visual->m_vb->unmap();
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

void platform::update_visual_index_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments)
{
	XRAY_UNREFERENCED_PARAMETERS	( v,fragments);
	NOT_IMPLEMENTED();
}

void platform::change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture)
{
	render_visual* r_visual = static_cast_checked<render_visual*>(&(*v));
	r_visual->set_shader(shader, texture);
}

void platform::add_light					( u32 id, xray::render::light_props const& props, bool beditor)
{
	lights_db::ref().add_light( id, props, beditor);
}

void platform::update_light				( u32 id, xray::render::light_props const& props, bool beditor)
{
	lights_db::ref().update_light( id, props, beditor);
}

void platform::remove_light				( u32 id, bool beditor)
{
	lights_db::ref().remove_light( id, beditor);
}

void platform::terrain_add_cell			( visual_ptr v, bool beditor )
{
	terrain::ref().add_cell( v, beditor );
}

void platform::terrain_remove_cell			( visual_ptr v, bool beditor )
{
	terrain::ref().remove_cell( v, beditor );
}

void platform::terrain_update_cell_buffer	( visual_ptr v, xray::vectora<xray::render::buffer_fragment_NEW> const& fragments, float4x4 const& transform)
{
	terrain::ref().update_cell_buffer( v, fragments, transform);
}

void platform::terrain_add_cell_texture	(  visual_ptr v, texture_string const & texture, u32 user_tex_id)
{
	terrain::ref().add_cell_texture( v, texture, user_tex_id);
}

void platform::terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id)
{
	terrain::ref().remove_cell_texture( v, user_tex_id);
}

void platform::terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture)
{
	terrain::ref().exchange_texture( old_texture, new_texture);
}

void platform::set_editor_render_mode		( bool draw_editor, bool draw_game )
{
	model_manager::ref().set_draw_editor	( draw_editor);
	model_manager::ref().set_draw_game		( draw_game);
}

void platform::setup_rotation_control_modes ( bool color_write )
{
	m_system_renderer.setup_rotation_control_modes( color_write);
}

res_buffer * create_quad_ib()
{
	if( g_quad_ib)
		return &*g_quad_ib;

	const u32 quad_count = 4*1024;
	const u32 idx_count  = quad_count*2*3;

	// 	if ( device::ref().get_caps().geometry.software)
	// 	{
	// 		ASSERT( 0);
	// 		usage |= D3DUSAGE_SOFTWAREPROCESSING;
	// 	}

	u16	indices[idx_count];

	int		vertex_id = 0;
	int		idx= 0;
	for ( int i=0; i<quad_count; ++i)
	{
		indices[idx++]=u16( vertex_id+0);
		indices[idx++]=u16( vertex_id+1);
		indices[idx++]=u16( vertex_id+2);

		indices[idx++]=u16( vertex_id+3);
		indices[idx++]=u16( vertex_id+2);
		indices[idx++]=u16( vertex_id+1);

		vertex_id+=4;
	}

	g_quad_ib = resource_manager::ref().create_buffer( idx_count*sizeof(u16), indices, enum_buffer_type_index, false);
	return &*g_quad_ib;
}


} // namespace dx10
} // namespace render
} // namespace xray


