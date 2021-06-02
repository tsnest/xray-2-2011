////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform_ps3.h"

using xray::render::engine::wrapper;
using xray::math::float4x4;
using xray::math::int2_pod;
using xray::math::rectangle;

namespace xray {
namespace render {
namespace ps3 {

render::platform * create_render_platform( xray::render::engine::wrapper& wrapper, HWND window_handle)
{
	XRAY_UNREFERENCED_PARAMETER( window_handle );

	return NEW(platform)( wrapper );
}

platform::platform							( engine::wrapper& engine ) :
	m_frame_id	( 0 )
{
	XRAY_UNREFERENCED_PARAMETER( engine );
}

platform::~platform							( )
{
}

void platform::clear_resources					( )
{
}

void platform::set_view_matrix					( float4x4 const& view )
{
	m_view_matrix	= view;
}

void platform::set_projection_matrix			( float4x4 const& projection )
{
	m_projection_matrix	= projection;
}

float4x4 const& platform::get_view_matrix		( ) const
{
	return		m_view_matrix;
}

float4x4 const& platform::get_projection_matrix( ) const
{
	return		m_projection_matrix;
}

rectangle<int2_pod> platform::get_viewport		( ) const
{
	return		m_viewport;
}

void platform::draw_debug_lines				( vector< vertex_colored > const& vertices, vector< u16 > const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( &vertices, &indices );
}

void platform::draw_debug_triangles			( vector< vertex_colored > const& vertices, vector< u16 > const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( &vertices, &indices );
}

void platform::draw_ui_vertices				( ui::vertex const * vertices, u32 const & count, int prim_type, int point_type )
{
	XRAY_UNREFERENCED_PARAMETERS( vertices, count, prim_type, point_type );
}

void platform::clear_zbuffer					( float z_value )
{
	XRAY_UNREFERENCED_PARAMETER( z_value );
}

void platform::draw_frame					( )
{
	++m_frame_id;
}

u32 platform::frame_id							( )
{
	return		m_frame_id;
}

void platform::add_static					( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals )
{
	XRAY_UNREFERENCED_PARAMETERS( &collision, &geometry, &visuals );
}

void platform::draw_static					( )
{
}

void platform::add_visual					( u32 id, visual_ptr v, float4x4 const& transform, bool selected, bool system_object, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS( &id, &v, &transform, selected, system_object, beditor );
}

void platform::update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected )
{
	XRAY_UNREFERENCED_PARAMETERS( &id, &transform, selected );
}

void platform::remove_visual				( u32 id )
{
	XRAY_UNREFERENCED_PARAMETER	( id );
}

void platform::update_visual_vertex_buffer	( xray::render::visual_ptr v, vectora<buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, &fragments );
}

void platform::update_visual_index_buffer	( xray::render::visual_ptr v, vectora<buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, &fragments );
}

void platform::change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, shader, texture );
}

void platform::add_light					( u32 id, light_props const& props, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS( &id, &props, beditor );
}

void platform::update_light				( u32 id, light_props const& props, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS( &id, &props, beditor );
}

void platform::remove_light				( u32 id, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS( &id, beditor);
}

void platform::render_visuals				( )
{
}

void platform::terrain_add_cell			( visual_ptr v, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, beditor );
}

void platform::terrain_remove_cell			( visual_ptr v, bool beditor )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, beditor );
}

void platform::terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> const& fragments, float4x4 const& transform)
{
	XRAY_UNREFERENCED_PARAMETERS( &v, &fragments, &transform );
}

void platform::terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS( &v, &texture, user_tex_id );
}

void platform::terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS( &v, user_tex_id );
}

void platform::terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture)
{
	XRAY_UNREFERENCED_PARAMETERS( &old_texture, &new_texture );
}

void platform::set_editor_render_mode			( bool beditor, bool bgame )
{
	XRAY_UNREFERENCED_PARAMETERS( beditor, bgame );
	UNREACHABLE_CODE	( );
}

} // namespace ps3
} // namespace render
} // namespace xray