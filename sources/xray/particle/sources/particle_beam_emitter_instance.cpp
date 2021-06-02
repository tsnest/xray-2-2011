////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_beam_emitter_instance.h"
#include "particle_world.h"
#include "particle_actions.h"
#include "particle_emitter.h"
#include <xray/particle/engine.h>

namespace xray {
namespace particle {

particle_beam_emitter_instance::particle_beam_emitter_instance(
		particle_world& particle_world,
		particle_emitter& emitter,
		engine& engine,
		bool const is_child_emitter_instance
	) :
	particle_emitter_instance( particle_world, emitter, engine, is_child_emitter_instance ),
	m_beam_particles_allocated(false),
	m_particle_beam_index(0.0f),
	m_beams_source_position(float3(0.0f,0.0f,0.0f)),
	m_beams_target_position(float3(50.0f, 50.0f, 50.0f)),
	m_beams_end_position(m_beams_target_position),
	m_beams_direction( normalize(m_beams_target_position - m_beams_source_position) ),
	m_move_from_source_to_target(0.0f),
	m_curve_line(0),
	m_num_curves(0),
	m_beam_pose_index(0.0f),
	m_beam_lifetime(0.0f),
	m_random_offsets(0),
	m_particle_action_beam(0)
{
	m_max_num_particles			= m_beamtrail_parameters->num_beams * m_emitter.m_max_num_particles;
	m_current_max_num_particles	= m_max_num_particles;

	alloc_dynamic_data			( 1, num_base_noise_positions );
	
	if ( m_beamtrail_parameters->num_beams != m_num_curves ) {
		free_dynamic_data		( );
		alloc_dynamic_data		( m_beamtrail_parameters->num_beams, num_base_noise_positions );
	}
	
	if ( m_data_type_action )
		m_particle_action_beam	= static_cast_checked< particle_action_beam* >( m_data_type_action );
	
	generate_offsets			( );
}

void particle_beam_emitter_instance::create_render_particle_emitter_instance	( engine& engine )
{
	R_ASSERT							( !m_render_instance );
	m_render_instance					=
		engine.create_render_emitter_instance(
			*this,
			m_particle_list,
			m_billboard_parameters,
			m_beamtrail_parameters,
			m_billboard_parameters ? m_billboard_parameters->locked_axis : particle_locked_axis_x,
			m_billboard_parameters ? m_billboard_parameters->screen_alignment : particle_screen_alignment_rectangle,
			m_world_space ? math::float4x4().identity() : m_transform,
			m_instance_color
		);
}

void particle_beam_emitter_instance::alloc_dynamic_data(u32 num_curves, u32 num_points)
{
	m_curve_line = MT_ALLOC(curve_line_float3, num_curves);
	m_num_curves = num_curves;
	
	for (u32 i=0; i<num_curves; i++) 
	{
		new(&m_curve_line[i])curve_line_float3;	
		m_curve_line[i].reserve(num_points);
	}
	
	m_random_offsets = MT_ALLOC(offsets_array_type, num_curves);
	for (u32 i=0; i<num_curves; i++) 
	{
		new(&m_random_offsets[i])curve_line_float3;	
	}
}

void particle_beam_emitter_instance::free_dynamic_data()
{
	for (u32 i=0; i<m_num_curves; i++) 
	{
		m_curve_line[i].~curve_line_float3();
		m_random_offsets[i].~offsets_array_type();
	}
	
	MT_FREE(m_curve_line);
	MT_FREE(m_random_offsets);
}

particle_beam_emitter_instance::~particle_beam_emitter_instance()
{
	free_dynamic_data();
}

void particle_beam_emitter_instance::apply_noise(u32 beam_index, float3 const& up_vector, float3 const& right_vector, float /*noise_power*/, u32 /*step*/, bool /*first_fill*/, base_particle* P, u32 num_particles)
{
	if (num_particles<2)
		return;

	for (u32 i=0; i<num_base_noise_positions; i++)
	{
		float alpha = float(i)/float(num_base_noise_positions-1);
		
		float3 p  = linear_interpolation(m_beams_source_position, m_beams_end_position, alpha);
		
		if (i!=0 && i!=num_base_noise_positions-1)
		{
			p += m_particle_action_beam->m_noise * linear_interpolation(-up_vector, up_vector, m_random_offsets[beam_index][i].x);
			p += m_particle_action_beam->m_noise * linear_interpolation(-right_vector, right_vector, m_random_offsets[beam_index][i].y);			
		}
		
		m_curve_line[beam_index].set_point(i, p, float3(0.0f,0.0f,0.0f), float3(0.0f,0.0f,0.0f), alpha);
		m_curve_line[beam_index].set_interp_type(i, curve_interp_type);
	}
	m_curve_line[beam_index].recalculate_ranges();
	m_curve_line[beam_index].calc_tangents();
	
	for (u32 i=0; i<num_particles; i++)
	{
		float alpha = float(i) / float(num_particles-1);
		
		P->position = m_curve_line[beam_index].evaluate(alpha, m_beams_source_position);
		
		P = m_particle_list.get_next_of_object(P);
	}

}

void particle_beam_emitter_instance::generate_offsets()
{
	for (u32 beam_index=0; beam_index<m_beamtrail_parameters->num_beams; beam_index++)
		for (u32 i=0; i<num_base_noise_positions; i++)
			m_random_offsets[beam_index][i] = float3(random_float(0.0f, 1.0f), random_float(0.0f, 1.0f), random_float(0.0f, 1.0f));
}

void particle_beam_emitter_instance::set_particles_positions(bool gen_new)
{
	if (m_num_live_particles<2)
		return;
	
	if (gen_new)
		generate_offsets();
	
	float3 beams_vector = m_beams_direction;
	
	float3 temp_vector  = float3(beams_vector.z*m_random_offsets[0][0].x, beams_vector.x*m_random_offsets[0][0].y, beams_vector.y*m_random_offsets[0][0].z).normalize();
	
	float3 up_vector	= xray::math::cross_product(beams_vector, temp_vector);
	float3 right_vector = xray::math::cross_product(beams_vector, up_vector);
	
	u32 const num_particle_per_beam = m_num_live_particles / m_beamtrail_parameters->num_beams;
	
	for (u32 beam_index=0; beam_index<m_beamtrail_parameters->num_beams; beam_index++)
	{
		base_particle* P = m_particle_list.front();
		u32 particle_index = 0;
		while (P)
		{
			if (particle_index == beam_index * num_particle_per_beam)
				break;
			particle_index++;
			P = m_particle_list.get_next_of_object(P);
		}
		apply_noise(beam_index, up_vector, right_vector, 1.0f, 1, true, P, num_particle_per_beam);
	}
}

void particle_beam_emitter_instance::tick(float time_delta, bool /*create_new_particles*/, float alpha )
{
	m_instance_color.w	= alpha;
	
	m_aabbox.zero();

	if (m_num_live_particles==0)
		m_beam_particles_allocated = false;
	
	m_emitter_time += time_delta;// * m_emitter_time_numerator;
	
	if (m_particle_action_beam->m_speed > xray::math::epsilon_5)
	{
		m_move_from_source_to_target += time_delta * m_particle_action_beam->m_speed;
		
		math::clamp(m_move_from_source_to_target, 0.0f, 1.0f);
		
		m_beams_end_position	= linear_interpolation(m_beams_source_position, m_beams_target_position, m_move_from_source_to_target);
	}
	else
	{
		m_beams_end_position	= m_beams_target_position;
	}
	
	m_beams_direction		= (m_beams_end_position - m_beams_source_position).normalize_safe(m_beams_target_position - m_beams_source_position);
	
	// Hack! Find source action.
	particle_action* modifier = m_emitter.m_actions;
	while(modifier)
	{
#pragma message( XRAY_TODO("Dima to IronNick: do not use dynamic_casts, use virtual function here instead") )
		particle_action_source* source_action = dynamic_cast<particle_action_source*>(modifier);
		if (source_action)
		{
			m_beams_source_position = source_action->get_transform().c.xyz() + m_transform.c.xyz();
			break;
		}
		modifier = modifier->m_next;
	}
	
	base_particle* P = m_particle_list.front();
	while(P)
	{
		P->size			 = P->start_size;
		P->old_position  = P->position;
		
		particle_action* modifier = m_emitter.m_actions;
		while(modifier)
		{
			if (modifier->get_visibility())
				modifier->update(this,P,time_delta);
			modifier = modifier->m_next;
		}
		
		P->lifetime		= P->lifetime + /*P->lifetime_numerator * */time_delta;
		
// 		if (math::is_similar(P->lifetime_numerator, 0.0f, math::epsilon_3))
// 			P->lifetime += time_delta;
		
		P->position		= P->old_position;
		
		if (P->is_dead())
		{
			process_event(event_on_death,P->position);
			
			m_particle_list.erase(P);
			base_particle* to_del = P;
			P = P->next;
			m_particle_world.deallocate_particle(to_del);
			m_num_live_particles--;
		}
		else
		{
			m_aabbox.modify(P->position);
			P = P->next;
		}
	}
	
	if (!m_emitter.m_world_space)
	{
		m_aabbox.modify(m_transform);
	}
	
	P = m_particle_list.front();
	if (P)
	{
		float frequency = m_particle_action_beam->m_frequency + 1;
		
		if ( P->get_linear_lifetime() * frequency > m_particle_beam_index)
		{
			set_particles_positions(true);
		}
		m_particle_beam_index = float(xray::math::floor(P->get_linear_lifetime() * frequency)) + 1.0f;
	}
	set_particles_positions(false);
}

u32 particle_beam_emitter_instance::calc_num_new_particles(float time_delta, bool /*allow_to_create_new*/)
{
	if (m_beam_particles_allocated)
	{
		m_num_particles_to_create = 0;
		return m_num_particles_to_create;
	}
	
	m_beam_particles_allocated = true;
	
	m_num_particles_to_create   = m_max_num_particles;
	
	for (u32 i=0; i<m_num_particles_to_create; i++)
	{
		if (m_num_live_particles+1 > m_max_num_particles)
			break;
		
		base_particle* new_particle = m_particle_world.allocate_particle();
		
		if (!new_particle)
			continue;
		
		m_particle_list.push_back(new_particle);
		
		new_particle->duration = m_emitter.m_particle_lifetime_curve.evaluate(m_emitter_time, 1.0f, linear_time_type, new_particle->seed());
		
		particle_action* modifier = m_emitter.m_actions;
		while(modifier)
		{
			if (modifier->get_visibility())
				modifier->init(this,new_particle,time_delta);
			modifier = modifier->m_next;
		}
		
		process_event(event_on_birth,new_particle->position);
		
		m_num_live_particles++;
		m_num_created_particles++;
	}
	m_num_particles_to_create = 0;
	
	set_particles_positions(true);
	
	return m_num_live_particles;
}

u32  particle_beam_emitter_instance::calc_num_max_particles(float /*time_delta*/)
{
	return m_emitter.m_max_num_particles;
}

void particle_beam_emitter_instance::shrink_particles(float /*time_delta*/, float /*limit_over_total*/, u32 /*num_need_particles */)
{
	// TODO!
}

bool particle_beam_emitter_instance::is_finished() const
{
	// TODO!
	return particle_emitter_instance::is_finished();
}

u32 particle_beam_emitter_instance::get_num_sheets( ) const
{
	return m_beamtrail_parameters->num_sheets;
}

} // namespace particle
} // namespace xray
