////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_EDITOR_RENDERER_H_INCLUDED
#define XRAY_RENDER_FACADE_EDITOR_RENDERER_H_INCLUDED

#include <xray/render/api.h>
#include <xray/render/engine/base_classes.h>
#include <xray/particle/world.h>
#include <xray/render/facade/common_types.h>

namespace xray {

namespace particle	{
	class particle_system;

	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_instance_ptr;

	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_ptr;
} // namespace particle

namespace render {

namespace engine {
	class world;
} // namespace engine

namespace debug {
	class renderer;
} // namespace debug

namespace ui {
	class renderer;
} // namespace debug

class scene_renderer;

class world;

typedef math::rectangle< math::float2 >	viewport_type;

class render_model_instance;
typedef	resources::resource_ptr<
	render_model_instance,
	resources::unmanaged_intrusive_base
> render_model_instance_ptr;

class speedtree_instance;
typedef	resources::resource_ptr<
	speedtree_instance,
	resources::unmanaged_intrusive_base
> speedtree_instance_ptr;

namespace editor {

typedef boost::function< void () >	on_command_finished_functor_type;

class XRAY_RENDER_API renderer : private core::noncopyable {
private:
	friend class render::world;
			renderer									( world& world, engine::world& engine_world );

public:
			~renderer									( );
		
public:
	debug::renderer&	debug							( ) const;
	ui::renderer&		ui								( ) const;
	scene_renderer&		scene							( ) const;
	
	void	set_renderer_configuration					( fs_new::virtual_path_string const& config_name );
	void	apply_render_options_changes				( pcstr args );
	void	end_frame									( );
	void	draw_scene									( scene_ptr const& scene, scene_view_ptr const& scene_view, render_output_window_ptr const& render_output_window, viewport_type const& viewport );

	void	select_model								( scene_ptr const& scene, render_model_instance_ptr const& render_model, bool is_selected );
	void	select_particle_system_instance				( scene_ptr const& scene, particle::particle_system_instance_ptr const& instance, bool is_selected );

	void	clear_zbuffer								( float z_value );
	void	setup_grid_render_mode						( u32 grid_density );
	void	remove_grid_render_mode						( );
	void	setup_rotation_control_modes				( bool color_write);
	
	void	draw_screen_lines							( scene_ptr const& scene, float2 const* points, u32 count, math::color const& color, float width, u32 pattern );
	void	draw_screen_rectangle						( scene_ptr const& scene, math::float2 const& left_top, math::float2 const& right_bottom, math::color const& color, u32 pattern );
	
	void	draw_3D_screen_line							( scene_ptr const& scene, math::float3 const& start, math::float3 const& end,  math::color const& color, float width, u32 pattern, bool use_depth = true );
	void	draw_3D_screen_lines						( scene_ptr const& scene, math::float3 const* points, u32 count, math::float4x4 const& tranform, math::color const& color, float width, u32 pattern, bool use_depth = true );
	void 	draw_3D_screen_ellipse						( scene_ptr const& scene, math::float4x4 const& matrix, math::color const& color, float width, u32 pattern, bool use_depth = true );
	void 	draw_3D_screen_rectangle					( scene_ptr const& scene, math::float4x4 const& matrix, math::color const& color, float width, u32 pattern, bool use_depth = true );
	void	draw_3D_screen_point						( scene_ptr const& scene, math::float3 const& position, float width, math::color const& color, bool use_depth = true );
	
	void	update_preview_particle_system				( scene_ptr const& scene, particle::particle_system** preview_ps, xray::configs::lua_config_value const& config_value );
	void	update_preview_particle_system				( scene_ptr const& scene, particle::particle_system** preview_ps, math::float4x4 const& transform );
	void	add_preview_particle_system					( scene_ptr const& scene, particle::particle_system** preview_ps, xray::configs::lua_config_value const& init_values, math::float4x4 const& transform = math::float4x4().identity() );
	void	restart_preview_particle_system				( scene_ptr const& scene, particle::particle_system** preview_ps );
	void	set_looping_preview_particle_system			( scene_ptr const& scene, particle::particle_system** preview_ps, bool looping );
	void	remove_preview_particle_system				( scene_ptr const& scene, particle::particle_system** preview_ps );
	void	show_preview_particle_system				( scene_ptr const& scene, particle::particle_system** preview_ps );
	void	gather_preview_particle_system_statistics	( scene_ptr const& scene, particle::particle_system** preview_ps, particle::preview_particle_emitter_info* out_info );
	u32		get_num_preview_particle_system_emitters	( scene_ptr const& scene, particle::particle_system** preview_ps) const;
	
	void	apply_material_changes						( fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value );
	
	void 	terrain_update_cell_buffer					( scene_ptr const& scene, render::render_model_instance_ptr const& v, xray::vectora<terrain_buffer_fragment> & fragments, float4x4 const& transform, on_command_finished_functor_type const& on_command_finished_functor );
	void 	terrain_update_cell_aabb					( scene_ptr const& scene, render::render_model_instance_ptr const& v, math::aabb aabb);
	void 	terrain_add_cell_texture					( scene_ptr const& scene, render::render_model_instance_ptr const& v, fs_new::virtual_path_string const & texture, u32 user_tex_id);
	void 	terrain_remove_cell_texture					( scene_ptr const& scene, render::render_model_instance_ptr const& v, u32 user_tex_id);
	void	terrain_exchange_texture					( scene_ptr const& scene, fs_new::virtual_path_string const& old_texture, fs_new::virtual_path_string const& new_texture);
	void	enable_terrain_debug_mode					( bool enable );
	
	void	set_selection_parameters					( xray::math::float4 const& selection_color, float selection_rate );
	
	void	select_speedtree_instance					( scene_ptr const& scene, render::speedtree_instance_ptr const& instance, bool is_selected );
	
private:
	void	draw_scene_impl								( scene_ptr const& scene, scene_view_ptr const& scene_view, render_output_window_ptr const& render_output_window, viewport_type const& viewport );
	void	on_draw_scene								( bool use_depth );
		
private:
	world&				m_world;
	engine::world&		m_render_engine_world;
	debug::renderer*	m_debug;
	ui::renderer*		m_ui;
	scene_renderer*		m_scene;
}; // class renderer

} // namespace editor
} // namespace render
} // namespace xray

#ifdef MASTER_GOLD
#	error class must not be used in MASTER_GOLD
#endif

#endif // #ifndef XRAY_RENDER_FACADE_EDITOR_RENDERER_H_INCLUDED