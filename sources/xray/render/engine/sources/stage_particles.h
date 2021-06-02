////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_PARTICLES_H_INCLUDED
#define STAGE_PARTICLES_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include "stage.h"

namespace xray {

namespace particle {
	//struct world;
	struct base_particle;
	class particle_emitter_instance;
	class particle_action_billboard;
	enum enum_particle_locked_axis;
	enum enum_particle_screen_alignment;
} // namespace particle

namespace render {

class shader_constant_host;

class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

class particle_shader_constants: public quasi_singleton<particle_shader_constants>
{
public:
	particle_shader_constants();
	void set_time(float time);
	void set(xray::math::float3 const up_vector, xray::math::float3 const right_vector, xray::math::float3 const view_location, xray::particle::enum_particle_locked_axis locked_axis, xray::particle::enum_particle_screen_alignment screen_alignment);
private:
	shader_constant_host*	m_right_view_vector;
	shader_constant_host*	m_up_view_vector;
	shader_constant_host*	m_use_align_by_dir;
	shader_constant_host*	m_view_location;
	shader_constant_host*	m_current_time;
	shader_constant_host*  m_use_fixed_axis;
	shader_constant_host*  m_rotation_fixed_axis;
	shader_constant_host*  m_locked_no_ratate_axis_index;
}; // class particle_shader_constants

class stage_particles: public stage
{
public:
	stage_particles(renderer_context* context);
	~stage_particles();
	
	virtual void execute ();
	
private:
	
	bool			m_particles_initialized;
	
	ref_effect		m_sh_particle_sprite;
	ref_effect		m_sh_particle_beamtrail;
	
	ref_geometry	m_g_particle_sprite;
	ref_geometry	m_g_subuv_particle_sprite;
	ref_geometry	m_g_particle_beamtrail;
	
//	vertex_buffer	m_vertex_stream;
//	index_buffer	m_index_stream;
}; // class stage_particles

} // namespace render
} // namespace xray


#endif // #ifndef STAGE_PARTICLES_H_INCLUDED