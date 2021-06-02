////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#if 0

#ifndef XRAY_RENDER_BASE_PLATFORM_H_INCLUDED
#define XRAY_RENDER_BASE_PLATFORM_H_INCLUDED

#include "xray/render/base/common_types.h"
#include <xray/render/base/light_props.h>
#include <xray/render/base/visual.h>
#include <xray/render/base/common_types.h>
#include <xray/render/base/particles.h>

namespace xray {

namespace render {
struct scene_base;
namespace ui {
	struct vertex;
} // namespace ui

struct scene_base;
struct scene_view_base;
struct render_output_window_base;

template < typename T >
class vector;

struct XRAY_NOVTABLE platform {
	virtual	void	clear_resources					( ) = 0;

	virtual void	create_scene					( xray::render::editor_renderer_configuration renderer_configuration, scene_base** scene_ptr /*out*/ ) = 0;
	virtual void	destroy							( scene_base* scene) = 0;

	virtual	void	create_scene_view				( scene_view_base** scene_view_ptr /*out*/ ) = 0;
	virtual void	destroy							( scene_view_base* scene_view) = 0;

	virtual void	create_render_output_window		( HWND window, render_output_window_base** output_window /*out*/) = 0;
	virtual void	destroy							( render_output_window_base* output_window) = 0;
		
	virtual	void	set_view_matrix					( float4x4 const& view ) = 0;
	virtual	void	set_projection_matrix			( float4x4 const& view ) = 0;
	virtual	float4x4 const&	get_view_matrix			( ) const = 0;
	virtual	float4x4 const&	get_projection_matrix	( ) const = 0;
	virtual	math::rectangle<math::int2> get_viewport( ) const = 0;
	virtual	void	draw_debug_lines				( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;
	virtual	void	draw_debug_triangles			( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;

	virtual	void	draw_ui_vertices				( ui::vertex const * vertices, u32 const & count, int prim_type, int point_type )	  = 0;
	virtual	void	clear_zbuffer					( float z_value )	  = 0;

	virtual	void	draw_scene						( scene_base * scene, scene_view_base *view, render_output_window_base * output_window, math::rectangle<math::uint2> const * viewport) = 0;
	virtual	void	present							( render_output_window_base * output_window) = 0;
	virtual	void	resize_output_window			( render_output_window_base ** output_window_base) =0;
	virtual	void	draw_frame						( float time) = 0;

	virtual	u32		frame_id						( ) = 0;

	virtual void	reload_shaders					( ) = 0;

	virtual	void	add_model						( scene_base* scene_base, render::render_model_instance_base_ptr v, float4x4 const& transform, bool apply_transform = true) = 0;
	virtual	void	update_model					( scene_base* scene_base, render::render_model_instance_base_ptr v, xray::math::float4x4 const& transform) = 0;
	virtual	void	remove_model					( scene_base* scene_base, render::render_model_instance_base_ptr v) = 0;

	virtual	void	update_system_model				( render::render_model_instance_base_ptr v, xray::math::float4x4 const& transform) = 0;

	virtual	void	set_model_ghost_mode			( render::render_model_instance_base_ptr v, bool value) = 0;
		
	virtual void	draw_model_selections					 ( render::vector<render::render_model_instance_base_ptr> const& models) = 0;
	virtual void	draw_system_models						 ( render::vector<render::render_model_instance_base_ptr> const& models) = 0;
	virtual void	draw_particle_system_instance_selections ( render::vector<particle::particle_system_instance_ptr> const& instances) = 0;
	
	virtual void	draw_terrain_debug				( ) = 0;
	
	virtual void	set_selection_parameters		( xray::math::float4 selection_color, float selection_rate ) = 0;
	
	virtual	void	update_model_vertex_buffer		( render::render_model_instance_base_ptr v, vectora<buffer_fragment> const& fragments ) = 0;
	virtual	void	update_model_index_buffer		( render::render_model_instance_base_ptr v, vectora<buffer_fragment> const& fragments ) = 0;
//	virtual	void	change_model_shader				( render::render_model_instance_base_ptr v, char const* shader, char const* texture ) = 0;
	virtual	void	add_light						( scene_base * scene_base, u32 id, light_props const& props) = 0;
	virtual	void	update_light					( scene_base * scene_base, u32 id, light_props const& props) = 0;
	virtual	void	remove_light					( scene_base * scene_base,	u32 id) = 0;

	virtual	void	update_skeleton					( render::render_model_instance_base_ptr v, math::float4x4* matrices, u32 count ) = 0;

	virtual	void	terrain_add_cell				( scene_base * scene_base, render::render_model_instance_base_ptr v) = 0;
	virtual	void	terrain_remove_cell				( scene_base * scene_base, render::render_model_instance_base_ptr v) = 0;
	virtual	void 	terrain_update_cell_buffer		( scene_base * scene_base, render::render_model_instance_base_ptr v, xray::vectora<terrain_buffer_fragment> const& fragments, float4x4 const& transform) = 0;
	virtual	void 	terrain_update_cell_aabb		( scene_base * scene_base, render::render_model_instance_base_ptr v, math::aabb const & aabb) = 0;
	virtual	void 	terrain_add_cell_texture		( scene_base * scene_base, render::render_model_instance_base_ptr v, texture_string const & texture, u32 user_tex_id) = 0;
	virtual	void 	terrain_remove_cell_texture		( scene_base * scene_base, render::render_model_instance_base_ptr v, u32 user_tex_id) = 0;
	virtual	void	terrain_exchange_texture		( scene_base * scene_base, texture_string const& old_texture, texture_string const& new_texture) = 0;

	// Editor specific functions
	virtual	void	setup_view_and_output			( scene_view_base *view, render_output_window_base * output_window, math::rectangle<math::uint2> const * viewport) = 0;
	virtual	void	draw_editor_lines				( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;
	virtual	void	draw_editor_triangles			( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;
	virtual	void	draw_screen_lines				( float2 const* points, u32 count, math::color clr, float width, u32 pattern ) = 0;
	virtual	void	draw_3D_screen_lines			( float3 const* points, u32 count, math::color clr, float width, u32 pattern, bool use_depth ) = 0;
	virtual	void	draw_3D_screen_point			( float3 const& position, math::color clr, float width, bool use_depth ) = 0;
	virtual	void	setup_grid_render_mode			( u32 grid_density ) = 0;
	virtual	void	remove_grid_render_mode			( ) = 0;
	virtual	void	setup_rotation_control_modes	(  bool color_write) {XRAY_UNREFERENCED_PARAMETERS( color_write);};
	
	virtual void	play_particle_system			( u32* instance_initialized, scene_base* scene_base, particle::particle_system_ptr ps, particle::particle_system_instance_ptr* out_instance, bool use_transform, bool always_looping, xray::math::float4x4 const& transform ) = 0;
	virtual void	stop_particle_system			( particle::particle_system_instance_ptr in_instance, scene_base* scene_base ) = 0;
	virtual void	remove_particle_system_instance	( particle::particle_system_instance_ptr in_instance, scene_base* scene_base ) = 0;
	virtual void	update_particle_system_instance	( particle::particle_system_instance_ptr instance, scene_base* scene_base, xray::math::float4x4 const& transform, bool visible = true, bool paused = false, bool selected = false  ) = 0;
	
#ifndef MASTER_GOLD
	virtual void	update_preview_particle_system	( xray::particle::particle_system_base** preview_ps, scene_base* scene_base, xray::configs::lua_config_value config_value ) = 0;
	virtual void	update_preview_particle_system_transform	( xray::particle::particle_system_base** preview_ps, scene_base* scene_base, math::float4x4 const& transform ) = 0;
	virtual void	add_preview_particle_system		( xray::particle::particle_system_base** preview_ps, scene_base* scene_base, xray::configs::lua_config_value init_values, math::float4x4 const& transform = math::float4x4().identity() ) = 0;
	virtual void	remove_preview_particle_system	( xray::particle::particle_system_base** preview_ps, scene_base* scene_base ) = 0;
	virtual void	show_preview_particle_system	( xray::particle::particle_system_base** preview_ps, scene_base* scene_base ) = 0;
#endif // #ifndef MASTER_GOLD

//protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( platform )
}; // class platform

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_PLATFORM_H_INCLUDED

#endif