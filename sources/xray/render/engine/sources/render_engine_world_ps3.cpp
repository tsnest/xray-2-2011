////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/world.h>
#include <xray/render/facade/particles.h>
#include <xray/render/facade/scene_view_mode.h>
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/render_stage_types.h>
#include <xray/render/facade/particles.h>
#include <xray/render/facade/ui_renderer.h>

using xray::math::float4x4;
using xray::math::int2_pod;
using xray::math::rectangle;

xray::render::render_allocator_type*	xray::render::g_allocator = 0;

namespace xray {
namespace render {
namespace engine {

static xray::uninitialized_reference< engine::world > s_world;

engine::world* create_world		( )
{
	R_ASSERT					( !s_world.initialized() );
	XRAY_CONSTRUCT_REFERENCE	( s_world, world ) ( );
	return						s_world.c_ptr( );
}

void destroy					( engine::world*& world )
{
	R_ASSERT					( s_world.initialized() );
	R_ASSERT					( s_world.c_ptr() == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world				= 0;
}


world::world			( ) :
	m_frame_id	( 0 )
{
}

world::~world			( )
{
}

void world::initialize	( )
{
}

void world::clear_resources					( )
{
}

void world::set_view_matrix					( scene_view_ptr const& scene_view, float4x4 const& view )
{
	m_view_matrix	= view;
}

void world::set_projection_matrix			( scene_view_ptr const& scene_view, float4x4 const& projection )
{
	m_projection_matrix	= projection;
}

void world::draw_ui_vertices				( ui::vertex const * vertices, u32 const & count, int prim_type, int point_type )
{
	XRAY_UNREFERENCED_PARAMETERS( vertices, count, prim_type, point_type );
}

void world::clear_zbuffer				( float z_value )
{
	XRAY_UNREFERENCED_PARAMETER( z_value );
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

void world::end_frame					( )
{
	++m_frame_id;
}

void world::draw_scene					( scene_ptr const& in_scene, scene_view_ptr const& view, render_output_window_ptr const& output_window, viewport_type const& viewport, boost::function< void (bool) > const& on_draw_scene)
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &view, &output_window, &viewport, &on_draw_scene);
}

u32 world::frame_id							( )
{
	return		m_frame_id;
}

void world::reload_shaders				( )
{
	
}

void world::reload_modified_textures	( )
{

}

void world::add_speedtree_instance( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, &transform, populate_forest );
}

void world::remove_speedtree_instance( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, bool populate_forest )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, populate_forest);
}

void world::update_speedtree_instance( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &v, &transform, populate_forest );
}

void world::add_model					( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, float4x4 const& transform, bool apply_transform )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v, &transform, apply_transform );
}

void world::update_model				( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, xray::math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v, &transform );
}

void world::update_system_model	( render::render_model_instance_ptr const& v, xray::math::float4x4 const& transform)
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &transform );
}

void world::remove_model				( scene_ptr const& in_scene, render::render_model_instance_ptr const& v )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v );
}

void world::set_speedtree_instance_material ( render::speedtree_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr const& mtl_ptr)
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &subsurface_name, &mtl_ptr );
}

void world::set_model_material			( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr const& mtl_ptr )
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &subsurface_name, &mtl_ptr );
}

void world::set_model_visible			( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, bool value )
{
	XRAY_UNREFERENCED_PARAMETERS	( &v, &subsurface_name, value );
}


void world::set_model_ghost_mode( render::render_model_instance_ptr const& v, bool value)
{
	XRAY_UNREFERENCED_PARAMETERS( &v, value );
}

void	world::draw_model_selections		( render::vector<render::render_model_instance_ptr> const& models)
{
	XRAY_UNREFERENCED_PARAMETER	( models );
}

void world::update_model_vertex_buffer	( render::render_model_instance_ptr const& v, vectora<buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, &fragments );
}

void world::update_model_index_buffer	( render::render_model_instance_ptr const& v, vectora<buffer_fragment> const& fragments )
{
	XRAY_UNREFERENCED_PARAMETERS( &v, &fragments );
}

void world::add_light					( scene_ptr const& in_scene, u32 id, light_props const& props )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id, &props );
}

void world::update_light				( scene_ptr const& in_scene, u32 id, light_props const& props )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id, &props );
}

void world::remove_light				( scene_ptr const& in_scene, u32 id )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id);
}

void world::add_decal					( scene_ptr const& in_scene, u32 id, decal_properties const& properties )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id, &properties );
}

void world::update_decal				( scene_ptr const& in_scene, u32 id, decal_properties const& properties )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id, &properties );
}

void world::remove_decal				( scene_ptr const& in_scene, u32 id )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &id);
}

void world::terrain_add_cell			( scene_ptr const& in_scene, render::render_model_instance_ptr const& v )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v );
}

void world::terrain_remove_cell			( scene_ptr const& in_scene, render::render_model_instance_ptr const& v )
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v );
}

void world::terrain_update_cell_buffer	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, xray::vectora<terrain_buffer_fragment> const& fragments, float4x4 const& transform)
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v, &fragments, &transform );
}

void world::terrain_update_cell_aabb	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, math::aabb const & aabb)
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v, &aabb );
}

void world::update_skeleton( render::render_model_instance_ptr const& v, math::float4x4* matrices, u32 count )
{
	XRAY_UNREFERENCED_PARAMETERS	(&v, matrices, count);
}

void world::terrain_add_cell_texture	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, fs_new::virtual_path_string const & texture, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v, &texture, user_tex_id );
}

void world::terrain_remove_cell_texture	( scene_ptr const& in_scene, render::render_model_instance_ptr const& v, u32 user_tex_id)
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &v, user_tex_id );
}

void world::terrain_exchange_texture	( scene_ptr const& in_scene, fs_new::virtual_path_string const & old_texture, fs_new::virtual_path_string const & new_texture)
{
	XRAY_UNREFERENCED_PARAMETERS( &in_scene, &old_texture, &new_texture );
}

// void world::create_scene					( xray::render::editor_renderer_configuration renderer_configuration, &in_scene** scene_ptr /*out*/ )
// {
// 	XRAY_UNREFERENCED_PARAMETERS	( &renderer_configuration, scene_ptr);
// 	NOT_IMPLEMENTED();
// }
// 
// void world::destroy						( scene_ptr const& in_scene)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( &in_scene);
// 	NOT_IMPLEMENTED();
// }
// 
// void world::create_scene_view			( scene_view** scene_view_ptr /*out*/ )
// {
// 	XRAY_UNREFERENCED_PARAMETER	( scene_view_ptr);
// 	NOT_IMPLEMENTED();
// }
// 
// void world::destroy						( scene_view* scene_view)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( scene_view);
// 	NOT_IMPLEMENTED();
// }
// 
// void world::create_render_output_window	( HWND window, render_output_window** output_window /*out*/)
// {
// 	XRAY_UNREFERENCED_PARAMETERS	( window, output_window);
// 	NOT_IMPLEMENTED();
// }
// 
// void world::destroy						( render_output_window* output_window)
// {
// 	XRAY_UNREFERENCED_PARAMETER	( output_window);
// 	NOT_IMPLEMENTED();
// }

void world::setup_view_and_output			( scene_view_ptr const& view, render_output_window_ptr const& output_window, viewport_type const& viewport )
{
	XRAY_UNREFERENCED_PARAMETERS	( &view, &output_window, &viewport );
	NOT_IMPLEMENTED();
}

void world::play_particle_system			( scene_ptr const& in_scene, particle::particle_system_instance_ptr in_instance, bool use_transform, bool always_looping, math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, &in_instance, use_transform, always_looping, &transform );
	NOT_IMPLEMENTED();
}

void world::stop_particle_system			( scene_ptr const& in_scene, particle::particle_system_instance_ptr in_instance )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_instance, &in_scene);
	NOT_IMPLEMENTED();
}

void world::remove_particle_system_instance	( particle::particle_system_instance_ptr in_instance, scene_ptr const& in_scene )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_instance, &in_scene);
	NOT_IMPLEMENTED();
}

void world::update_particle_system_instance	( particle::particle_system_instance_ptr instance, scene_ptr const& in_scene, xray::math::float4x4 const& transform, bool visible, bool paused)
{
	XRAY_UNREFERENCED_PARAMETERS	( &instance, &transform, visible, paused, &in_scene);
	NOT_IMPLEMENTED();
}

#ifndef MASTER_GOLD

void world::update_preview_particle_system	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, xray::configs::lua_config_value config_value )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &config_value, &in_scene);
	NOT_IMPLEMENTED();
}

void world::update_preview_particle_system_transform	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &transform, &in_scene);
	NOT_IMPLEMENTED();
}

void world::add_preview_particle_system		( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, xray::configs::lua_config_value init_values, math::float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &init_values, &transform, &in_scene);
	NOT_IMPLEMENTED();
}

void world::remove_preview_particle_system	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &in_scene);
	NOT_IMPLEMENTED();
}

void world::restart_preview_particle_system	( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps );
	NOT_IMPLEMENTED();
}

void world::show_preview_particle_system		( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene  )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, &in_scene );
	NOT_IMPLEMENTED();
}

void world::set_looping_preview_particle_system		( xray::particle::particle_system** preview_ps, scene_ptr const& in_scene, bool looping )
{
	XRAY_UNREFERENCED_PARAMETERS	( preview_ps, looping );
	NOT_IMPLEMENTED();
}

void world::draw_terrain_debug				( )
{
	
}

u32 world::get_num_preview_particle_system_emitters		( xray::particle::particle_system** in_ps_out_ptr, scene_ptr const& in_scene ) const
{
	XRAY_UNREFERENCED_PARAMETERS	( in_ps_out_ptr, &in_scene );
	return							0;
}

void world::gather_preview_particle_system_statistics	( xray::particle::particle_system** in_ps_out_ptr, scene_ptr const& in_scene, xray::particle::preview_particle_emitter_info* out_info )
{
	XRAY_UNREFERENCED_PARAMETERS	( in_ps_out_ptr, out_info, &in_scene);
	NOT_IMPLEMENTED();
}

void world::draw_render_statistics	( xray::ui::world* ui_world )
{
	XRAY_UNREFERENCED_PARAMETER( ui_world );
	NOT_IMPLEMENTED();
}

void world::apply_material_changes( fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value )
{
	XRAY_UNREFERENCED_PARAMETERS(&material_name, &config_value);
}

#endif // #ifndef MASTER_GOLD

void world::set_selection_parameters			( xray::math::float4 const& selection_color, float const selection_rate )
{
	XRAY_UNREFERENCED_PARAMETERS	( &selection_color, selection_rate );
}

void world::set_slomo( xray::render::scene_ptr const& in_scene, float time_multiplier )
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_scene, time_multiplier );
}

void world::set_sky_material				( scene_ptr const& scene, resources::unmanaged_resource_ptr mtl_ptr)
{
	XRAY_UNREFERENCED_PARAMETERS	( &scene, &mtl_ptr );
}

void world::toggle_render_stage				( enum_render_stage_type stage_type, bool toggle )
{
	XRAY_UNREFERENCED_PARAMETERS	( &stage_type, &toggle );
}

void world::setup_rotation_control_modes ( int step )
{
	XRAY_UNREFERENCED_PARAMETERS( step );
	NOT_IMPLEMENTED();
}

void world::set_view_mode					( scene_view_ptr const& view_ptr, scene_view_mode view_mode )
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, view_mode );
	NOT_IMPLEMENTED();
}

void world::set_particles_render_mode		( scene_view_ptr const& view_ptr, xray::particle::enum_particle_render_mode render_mode )
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, render_mode );
	NOT_IMPLEMENTED();
}

void world::enable_post_process				( scene_view_ptr const& view_ptr, bool enable )
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, enable );
	NOT_IMPLEMENTED();
}

void world::set_post_process( scene_view_ptr const& view_ptr, resources::unmanaged_resource_ptr const& post_process_resource)
{
	XRAY_UNREFERENCED_PARAMETERS( &view_ptr, &post_process_resource );
	NOT_IMPLEMENTED();
}

particle::world& world::particle_world	( scene_ptr const& scene )
{
	XRAY_UNREFERENCED_PARAMETER( scene );
	NOT_IMPLEMENTED( return *xray::identity((particle::world*)0) );
}

math::uint2 world::window_client_size	( render::render_output_window_ptr const& render_output_window )
{
	XRAY_UNREFERENCED_PARAMETER	( render_output_window );
	return	math::uint2( 1024, 768 );
}

void world::draw_lines					( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, &vertices, &indices );
}

void world::draw_triangles				( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, &vertices, &indices );
}

} // namespace engine
} // namespace render
} // namespace xray