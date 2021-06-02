////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_RENDER_H_INCLUDED
#define SCENE_RENDER_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/facade/render_stage_types.h>
#include "statistics.h"
#include <xray/render/engine/base_classes.h>

namespace xray {

namespace particle {
	struct world;
} // namespace particle

namespace ui {
	struct font;
} // namespace ui

namespace render {

enum enum_render_stage_type;

//Forward declarations
class renderer_context;
class render_output_window;

class stage_scene_update;
class stage_gbuffer;
class stage_texture_mip_clamp_test;
class stage_fr_test;
class stage_pre_lighting;
class stage_shadow_direct;
class stage_sun;
class stage_sun_materials;
class stage_skybox;
class stage_combine;
class stage_lights;
class stage_screen_image;
class stage_particles;
class stage_accumulate_distortion;
class stage_apply_distortion;
class stage_forward;
class stage_postprocess;
class stage_view_mode;
class stage_debug;

class shader_constant_host;

class scene;
class scene_view;
class untyped_buffer;
class stage;

class res_texture;
class resource_intrusive_base;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class res_effect;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

typedef math::rectangle< math::float2 >	 viewport_type;

class renderer: public boost::noncopyable
{
public:
	typedef xray::fixed_vector<stage*, num_render_stages> stages_type;


				renderer						(renderer_context* renderer_context);
				~renderer						();
	void		render							(scene_ptr const& scene,
												 scene_view_ptr const& view,
												 render_output_window_ptr const& output_window,
												 viewport_type const& viewport,
												 boost::function< void ( bool ) > const& on_draw_scene,
												 bool draw_debug_terrain);
	void		present							(render_output_window_ptr output_window, viewport_type const& viewport);
	inline	u32	frame_id						() const { return m_frame_id; }

public:
	void		setup_render_output_window		(render_output_window_ptr in_output_window,
												 viewport_type const& viewport);
	void		toggle_render_stage				(enum_render_stage_type stage_type, bool toggle);
	void		draw_text						(pcstr text, xray::math::float2 const& position, xray::ui::font* const in_font, xray::math::color const& in_color);

private:
	void		execute_stages					();
	bool		is_effects_ready				() const;

	stages_type									m_stages;
	
	renderer_context*							m_renderer_context;
	
	stage_screen_image*							m_present_stage;
	stage_view_mode*							m_view_mode_stage;
	stage_debug*								m_stage_debug;
	
	timing::timer								m_timer;

	shader_constant_host*						m_gbuffer_to_screen_type;
	ref_effect									m_gbuffer_to_screen_shader;
	ref_effect									m_editor_apply_wireframe_shader;
	
	float										m_last_frame_time;
	float										m_current_time;
	u32											m_frame_id;
}; // class renderer

//typedef	xray::resources::resource_ptr < xray::render::renderer, xray::resources::unmanaged_intrusive_base > renderer_ptr;

} // namespace render 
} // namespace xray 



#endif // #ifndef SCENE_RENDER_H_INCLUDED
