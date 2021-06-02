////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_RENDER_H_INCLUDED
#define SCENE_RENDER_H_INCLUDED

#include "render_queue.h"
#include <xray/render/common/sources/blender_light_mask.h>
#include <xray/render/common/sources/blender_light_direct.h>
#include "stage.h"

namespace xray {
namespace render {

//Forward declarations
class stage_gbuffer;
class stage_pre_lighting;
class stage_sun;
class stage_sun_materials;
class stage_skybox;
class stage_combine;
class stage_lights;
class stage_screen_image;
class scene_context;

class scene_render: public stage
{
public:

	scene_render(bool render_test_scene);
	~scene_render();

	void render	(const float4x4& view_mat, const float4x4& proj_mat);

public:
	void set_w	(const float4x4& m)	{ m_context.set_w(m); }
	void set_v	(const float4x4& m)	{ m_context.set_v(m); }
	void set_p	(const float4x4& m)	{ m_context.set_p(m); }


private:
	void create_quad_ib	();
	void create_material_texture();

	void on_test_visual_loaded(resources::queries_result& data);
	void test(pcstr args);


//private:
	public:
	scene_context	m_context;

	stage_gbuffer*			m_gstage;
	stage_pre_lighting*		m_pre_lighting_stage;
	stage_sun*				m_sun_stage;
	stage_sun_materials*	m_sun_stage_materials;
	stage_skybox*			m_skybox_stage;
	stage_combine*			m_combine_stage;
	stage_lights*			m_lighting_stage;
	stage_screen_image*		m_present_stage;

	ref_texture				m_t_envmap_0;
	ref_texture				m_t_envmap_1;
	ref_texture				m_t_material;

	ID3DDevice*				m_device;

	render_visual*			m_test_visual;
}; // class scene_render

} // namespace render 
} // namespace xray 



#endif // #ifndef SCENE_RENDER_H_INCLUDED