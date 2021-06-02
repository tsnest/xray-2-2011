////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_PLATFORM_DX11_H_INCLUDED
#define XRAY_RENDER_ENGINE_PLATFORM_DX11_H_INCLUDED

#include <xray/render/engine/base_classes.h>
#include <xray/render/core/memory.h>

namespace xray {

namespace vfs {
	struct vfs_notification;
} // namespace fs

namespace ui {
	struct world;
	struct font;
}
namespace particle	{
	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_instance_ptr;

	class particle_system;
	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_ptr;

	struct preview_particle_emitter_info;
	enum enum_particle_render_mode;
	struct world;
} // namespace particle

namespace render {

class renderer;
//typedef	xray::resources::resource_ptr < xray::render::renderer, xray::resources::unmanaged_intrusive_base > renderer_ptr;

enum scene_view_mode;
struct vertex_colored;
class scene;
class scene_view;
struct light_props;
struct decal_properties;
struct editor_renderer_configuration;
struct buffer_fragment;
struct terrain_buffer_fragment;

enum enum_render_stage_type;

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

namespace ui {
	struct vertex;
} // namespace ui

class render_output_window;

typedef math::rectangle< math::float2 >	viewport_type;
typedef vectora< vertex_colored >		debug_vertices_type;
typedef vectora< u16 >					debug_indices_type;

namespace engine {

class world : private boost::noncopyable {
public:
	typedef render::vector< render::vertex_colored >	colored_vertices_type;
	typedef render::vector< u16 >						colored_indices_type;
	typedef ui::vertex const *							ui_vertices_type;
	
	xray::render::renderer*								m_renderer;
	
	void set_renderer_configuration						(fs_new::virtual_path_string const& config_name, bool async_effects );
	void apply_render_options_changes					( );
	
private:
	void reset_renderer									( bool async_effects );
	void on_renderer_config_loaded						( bool async_effects, resources::queries_result& data );
	void on_console_commands_config_loaded				( bool load_renderer_options, resources::queries_result& data );
	void load_console_commands_config_query				( pcstr cfg_name, bool load_renderer_options );

public:
							world						( );
							~world						( );
			void			initialize					( );

			void			clear_resources				( );

			void 			draw_lines					( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices );
			void 			draw_triangles				( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices );
	static	void			draw_text					(
								xray::vectora< ui::vertex >& output,
								pcstr const& text,
								xray::ui::font const& font,
								xray::float2 const& position,
								xray::math::color const& text_color,
								xray::math::color const& selection_color,
								u32 max_line_width,
								bool is_multiline,
								u32 start_selection,
								u32 end_selection
							);

public:
			void			set_view_matrix				( scene_view_ptr const& scene_view, float4x4 const& view_matrix );
#ifndef MASTER_GOLD
			void			set_view_matrix_only		( scene_view_ptr const& scene_view, float4x4 const& view_matrix );
#endif // #ifndef MASTER_GOLD
			void			set_projection_matrix		( scene_view_ptr const& scene_view, float4x4 const& projection_matrix );

			math::uint2		window_client_size			( render::render_output_window_ptr const& render_output_window );

			void			draw_debug_lines			( colored_vertices_type const& vertices, colored_indices_type const& indices );
			void			draw_debug_triangles		( colored_vertices_type const& vertices, colored_indices_type const& indices );
			void			draw_editor_lines			( colored_vertices_type const& vertices, colored_indices_type const& indices );
			void			draw_editor_triangles		( colored_vertices_type const& vertices, colored_indices_type const& indices );
			void			setup_grid_render_mode		( u32 grid_density );
			void			remove_grid_render_mode		( );
			void			setup_rotation_control_modes( bool color_write );	
			void			draw_scene					( scene_ptr const& scene, scene_view_ptr const& view, render_output_window_ptr const& output_window, viewport_type const& viewport, boost::function< void ( bool ) > const& on_draw_scene);
			
			void			draw_text					( pcstr text, xray::math::float2 const& position, xray::ui::font* const in_font, xray::math::color const& in_color );
			
			void			end_frame					( );
			u32				frame_id					( );

			void			reload_shaders				( );
			void			reload_modified_textures	( );
			void			add_model					( scene_ptr const& scene, render::render_model_instance_ptr const& v, float4x4 const& transform, bool apply_transform = true);
			void			update_model				( scene_ptr const& scene, render::render_model_instance_ptr const& v, math::float4x4 const& transform);
			void			remove_model				( scene_ptr const& scene, render::render_model_instance_ptr const& v);
			
			void			add_speedtree_instance		( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest );
			void			remove_speedtree_instance	( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, bool populate_forest );
			void			update_speedtree_instance	( scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest );
			void			populate_speedtree_forest	( scene_ptr const& in_scene );
			
			void			set_speedtree_instance_material ( render::speedtree_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr mtl_ptr);
			
			void			set_model_material			( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr mtl_ptr);
			void			set_model_visible			( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, bool value );

			void			update_system_model			( render::render_model_instance_ptr const& v, math::float4x4 const& transform);

			void			set_model_ghost_mode		( render::render_model_instance_ptr const& v, bool value);
			
			void			draw_terrain_debug			( );

			void			set_selection_parameters	( math::float4 const& selection_color, float selection_rate );
	
			void			update_model_vertex_buffer	( render::render_model_instance_ptr const& v, vectora<render::buffer_fragment> const& fragments );
			void			update_model_index_buffer	( render::render_model_instance_ptr const& v, vectora<render::buffer_fragment> const& fragments );
//			void			change_model_shader			( render::render_model_instance_ptr const& v, char const* shader, char const* texture );
			void			add_light					( scene_ptr const& scene, u32 id, render::light_props const& props);
			void			update_light				( scene_ptr const& scene, u32 id, render::light_props const& props);
			void			remove_light				( scene_ptr const& scene, u32 id);

			void			add_decal					( scene_ptr const& scene, u32 id, render::decal_properties const& properties );
			void			update_decal				( scene_ptr const& scene, u32 id, render::decal_properties const& properties );
			void			remove_decal				( scene_ptr const& scene, u32 id );

			void			update_skeleton				( render::render_model_instance_ptr const& v, math::float4x4* matrices, u32 count );

			void			terrain_add_cell			( scene_ptr const& scene, render::render_model_instance_ptr const& v);
			void			terrain_remove_cell			( scene_ptr const& scene, render::render_model_instance_ptr const& v);
			void 			terrain_update_cell_buffer	( scene_ptr const& scene, render::render_model_instance_ptr const& v, vectora<terrain_buffer_fragment> const& fragments, float4x4 const& transform);
			void 			terrain_update_cell_aabb	( scene_ptr const& scene, render::render_model_instance_ptr const& v, math::aabb const & aabb);
			void 			terrain_add_cell_texture	( scene_ptr const& scene, render::render_model_instance_ptr const& v, fs_new::virtual_path_string const & texture, u32 user_tex_id);
			void 			terrain_remove_cell_texture	( scene_ptr const& scene, render::render_model_instance_ptr const& v, u32 user_tex_id);
			void			terrain_exchange_texture	( scene_ptr const& scene, fs_new::virtual_path_string const & old_texture, fs_new::virtual_path_string const & new_texture);

			void			setup_view_and_output		( scene_view_ptr const& view, render_output_window_ptr const& output_window, viewport_type const& viewport);
			void			draw_ui_vertices			( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type );
			void			clear_zbuffer				( float z_value );
			void			draw_screen_lines			( scene_ptr const& scene, float2 const* points, u32 count, math::color const& color, float width, u32 pattern );
			void			draw_3D_screen_lines		( scene_ptr const& scene, float3 const* points, u32 count, math::color const& color, float width, u32 pattern, bool use_depth );
			void			draw_3D_screen_point		( scene_ptr const& scene, float3 const& position, math::color color, float width, bool use_depth );

			void		play_particle_system			( scene_ptr const& scene, particle::particle_system_instance_ptr in_instance, bool use_transform, bool always_looping, math::float4x4 const& transform );
			void		stop_particle_system			( scene_ptr const& scene, particle::particle_system_instance_ptr in_instance );
			void		remove_particle_system_instance	( particle::particle_system_instance_ptr in_instance, scene_ptr const& scene );
			void		update_particle_system_instance	( particle::particle_system_instance_ptr instance, scene_ptr const& scene, math::float4x4 const& transform, bool visible = true, bool paused = false);

#ifndef MASTER_GOLD
			void		update_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene, configs::lua_config_value config_value );
			void update_preview_particle_system_transform ( particle::particle_system** preview_ps, scene_ptr const& scene, math::float4x4 const& transform );
			void		add_preview_particle_system		( particle::particle_system** preview_ps, scene_ptr const& scene, configs::lua_config_value init_values, math::float4x4 const& transform = math::float4x4().identity() );
			void		remove_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene );
			void		restart_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene );
			void		set_looping_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene, bool looping );
			void		show_preview_particle_system	( particle::particle_system** preview_ps, scene_ptr const& scene );
			void		gather_preview_particle_system_statistics	( particle::particle_system** preview_ps, scene_ptr const& scene, particle::preview_particle_emitter_info* out_info );
			u32			get_num_preview_particle_system_emitters	( particle::particle_system** in_ps_out_ptr, scene_ptr const& scene) const;
			void		select_particle_system_instance	( scene_ptr const& scene, particle::particle_system_instance_ptr const& instance, bool selected );
			void		select_speedtree_instance		( scene_ptr const& scene, render::speedtree_instance_ptr const& instance, bool is_selected );
			void		select_model					( scene_ptr const& scene, render_model_instance_ptr const& v, bool is_selected );
			void		enable_terrain_debug_mode		( bool is_enabled );
			void		draw_render_statistics			( xray::ui::world* ui_world );
			
			void		apply_material_changes			( fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value );
			
#endif // #ifndef MASTER_GOLD

			void		set_slomo						( scene_ptr const& scene, float time_multiplier );

			void		set_view_mode					( scene_view_ptr view_ptr, scene_view_mode view_mode );
			void		set_particles_render_mode		( scene_view_ptr view_ptr, xray::particle::enum_particle_render_mode render_mode );
			void		enable_post_process				( scene_view_ptr view_ptr, bool enable );
			void		set_post_process				( scene_view_ptr view_ptr, resources::unmanaged_resource_ptr post_process_resource);
			
			void		toggle_render_stage				( enum_render_stage_type stage_type, bool toggle );

	particle::world&	particle_world					( scene_ptr const& scene );
			void		set_sky_material				( scene_ptr const& scene, resources::unmanaged_resource_ptr mtl_ptr);
// world methods
public:
			pcstr		type							( );

private:
	u32					m_frame_id;
	bool				m_initialized;
	bool				m_enable_terrain_debug_mode;
}; // class render_engine_world

} // namespace engine
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_PLATFORM_DX11_H_INCLUDED