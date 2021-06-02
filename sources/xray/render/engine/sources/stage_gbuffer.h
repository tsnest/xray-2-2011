////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_GBUFFER_H_INCLUDED
#define STAGE_GBUFFER_H_INCLUDED

#include "stage.h"
#include "render_model.h"

namespace xray {
namespace render {

class res_state;
class resource_intrusive_base;
typedef intrusive_ptr<res_state, resource_intrusive_base, threading::single_threading_policy>	ref_state;

class shader_constant_host;

class stage_gbuffer: public stage
{
public:
			stage_gbuffer			(renderer_context* context, bool is_pre_pass);
			~stage_gbuffer			();
	
	// Executes Geometry buffer stage.
	// rt_position, rt_normal, rt_color: G buffer render targets
	// zb: Z buffer
	// Notes: The function clears all render targets and Z buffer.
	virtual void execute			();

private:
	render_surface_instances		m_dynamic_visuals;
	
	ref_state						m_state;
	shader_constant_host*			m_sun_direction;
	shader_constant_host*			m_c_start_corner;
	shader_constant_host*			m_object_transparency_scale_parameter;
	shader_constant_host*			m_far_fog_color_and_distance;
	shader_constant_host*			m_near_fog_distance;
	shader_constant_host*			m_ambient_color;
	shader_constant_host*			m_c_environment_skylight_upper_color;
	shader_constant_host*			m_c_environment_skylight_lower_color;
	shader_constant_host*			m_c_environment_skylight_parameters;
	
	shader_constant_host*			m_c_gs_test_constant;
	
	bool							m_is_pre_pass;
}; // class stage_gbuffer

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_GBUFFER_H_INCLUDED
