////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_SCENE_RENDERER_IMPL_H_INCLUDED
#define XRAY_RENDER_FACADE_SCENE_RENDERER_IMPL_H_INCLUDED

#include <xray/render/api.h>
#include <xray/render/engine/base_classes.h>
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/particles.h>
#include <xray/render/facade/light_props.h>
#include <xray/render/facade/scene_view_mode.h>
#include <xray/render/facade/render_stage_types.h>

namespace xray {

namespace ui {
	struct world;
} // namespace ui

namespace particle	{
	typedef	resources::resource_ptr<
		resources::unmanaged_resource,
		resources::unmanaged_intrusive_base
	> particle_system_instance_ptr;
} // namespace particle

namespace render {

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

struct decal_properties;

namespace editor {
	class renderer;
} // namespace editor

namespace game {
	class renderer;
} // namespace game

namespace engine { class world; }

class one_way_render_channel;

class XRAY_RENDER_API scene_renderer : private core::noncopyable {
private:
	friend class editor::renderer;
	friend class game::renderer;
					scene_renderer					(
						one_way_render_channel&	channel,
						memory::base_allocator&	allocator,
						render::engine::world& render_engine_world,
						math::frustum* frustum_listener = 0
					);

public:
	
	void			reload_shaders					( );
	void			reload_modified_textures		( );

	math::uint2		window_client_size				( render::render_output_window_ptr const& render_output_window );

	void			set_view_matrix					( render::scene_view_ptr const& scene_view, math::float4x4 const& view );
#ifndef MASTER_GOLD
	void			set_view_matrix_only			( render::scene_view_ptr const& scene_view, math::float4x4 const& view );
#endif // #ifndef MASTER_GOLD
	void			set_projection_matrix			( render::scene_view_ptr const& scene_view, math::float4x4 const& projection );

	// models
	void			add_model						( scene_ptr const& scene, render_model_instance_ptr const& render_model, math::float4x4 const& model_to_world );
	void			update_model					( scene_ptr const& scene, render_model_instance_ptr const& render_model, math::float4x4 const& model_to_world );
	void			remove_model					( scene_ptr const& scene, render_model_instance_ptr const& render_model );
	void			update_model_vertex_buffer		( render::render_model_instance_ptr const& object, vectora<buffer_fragment> const& fragments );
	// WTF: model instance doesn't need to know scene it is used in? 
	void			update_skeleton					( render::render_model_instance_ptr const& v, math::float4x4 const* matrices, u32 count );
	void			set_model_ghost_mode			( render::render_model_instance_ptr const& v, bool b );
	void			set_speedtree_instance_material ( render::speedtree_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr const& mtl_ptr);
	void			set_model_material				( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, resources::unmanaged_resource_ptr const& mtl_ptr);
	void			set_model_visible				( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, bool value );
	
	// speedtree
	void			add_speedtree_instance			( scene_ptr const& scene, render::speedtree_instance_ptr const& v, xray::math::float4x4 const& transform, bool populate_forest );
	void			remove_speedtree_instance		( scene_ptr const& scene, render::speedtree_instance_ptr const& v, bool populate_forest );
	void			update_speedtree_instance		( scene_ptr const& scene, render::speedtree_instance_ptr const& v, xray::math::float4x4 const& transform, bool populate_forest );
	void			populate_speedtree_forest		( scene_ptr const& scene );
	
	// particles
	void			play_particle_system			( scene_ptr const& scene, particle::particle_system_instance_ptr const& in_instance, math::float4x4 const& transform );
	void			stop_particle_system			( scene_ptr const& scene, particle::particle_system_instance_ptr const& in_instance );
	void			remove_particle_system_instance ( scene_ptr const& scene, particle::particle_system_instance_ptr const& in_instance );
	void			update_particle_system_instance	( scene_ptr const& scene, particle::particle_system_instance_ptr const& instance, math::float4x4 const& transform, bool visible = true, bool paused = false);
	void			set_particles_render_mode		( scene_view_ptr const& scene_view, particle::enum_particle_render_mode render_mode );
	void			set_slomo						( scene_ptr const& scene, float time_multiplier );

	// lights
	void			add_light						( scene_ptr const& scene, u32 id, render::light_props const& props);
	void			update_light					( scene_ptr const& scene, u32 id, render::light_props const& props);
	void			remove_light					( scene_ptr const& scene, u32 id );

	// decals
	void			add_decal						( scene_ptr const& scene, u32 id, render::decal_properties const& properties);
	void			update_decal					( scene_ptr const& scene, u32 id, render::decal_properties const& properties);
	void			remove_decal					( scene_ptr const& scene, u32 id );

	// terrain
	void			terrain_add_cell				( scene_ptr const& scene, render::render_model_instance_ptr const& v );
	void			terrain_remove_cell				( scene_ptr const& scene, render::render_model_instance_ptr const& v );

	// post process
	void			set_post_process				( scene_view_ptr const& scene_view, resources::unmanaged_resource_ptr const& post_process_resource );
	void			enable_post_process				( scene_view_ptr const& scene_view, bool enable );

	// debug
	void			set_view_mode					( scene_view_ptr const& scene_view, scene_view_mode view_mode );
	void			toggle_render_stage				( enum_render_stage_type stage_type, bool toggle );
	
	particle::world& particle_world					( scene_ptr const& scene );
	
	// sky
	void			set_sky_material				( scene_ptr const& scene, xray::resources::unmanaged_resource_ptr const& mtl_ptr);
	
#ifndef MASTER_GOLD
	void			draw_render_statistics			( xray::ui::world& ui_world );
#endif // #ifndef MASTER_GOLD

private:
	void			draw_present_impl				( );

private:
	engine::world&			m_render_engine_world;
	one_way_render_channel&	m_channel;
	memory::base_allocator&	m_allocator;
	math::frustum* const	m_frustum_listener;
	float4x4				m_view;
	float4x4				m_projection;
}; // class scene_renderer

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_SCENE_RENDERER_IMPL_H_INCLUDED