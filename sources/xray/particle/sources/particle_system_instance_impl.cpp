////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_system_instance_impl.h"
#include "particle_world.h"
#include "particle_emitter.h"

namespace xray {
namespace particle {

particle_system_instance_impl::particle_system_instance_impl( particle_world& particle_world ):
	particle_system_instance( particle_world )
{
	m_pinned			 = false;
	
	m_is_playing		 = false;
	m_no_more_create	 = false;
	
	m_paused			 = false;
	m_visible			 = true;
	
	m_current_lod		 = 0;
	m_old_lod			 = 0;
	m_num_lods			 = 1;
	m_use_lods			 = true;
	
	m_lods[0].m_distance = 1;
	
	m_lods_lerp_alpha	 = 1.0f;
	m_lerped			 = false;
	
	m_ticked			 = true;
	
	m_always_looping	 = false;
	
	m_child_played		 = false;
	
	m_next				 = 0;
	
	m_transform			 = xray::math::float4x4().identity();
	
	m_particle_system_time = 0.0f;
}

particle_system_instance_impl::~particle_system_instance_impl()
{
	remove_emitter_instances();
}

void particle_system_instance_impl::remove_particles()
{
	for (u32 i=0; i<m_num_lods; i++)
	{
		emitter_instance_list_type& emitters_list = m_lods[i].m_emitter_instance_list;
		particle_emitter_instance* instance = emitters_list.front();
		while(instance)
		{
			instance->remove_particles();
			instance = emitters_list.get_next_of_object(instance);
		}
	}
}

//u32 particle_system_instance_impl::get_particle_emitter_info_count()
//{
//	return m_lods[0].m_emitter_instance_list.size();
//}

//void particle_system_instance_impl::get_particle_emitter_info(particle_emitter_info* out_info_array)
//{
//	particle_emitter_instance* emitter_instance = m_lods[0].m_emitter_instance_list.front();
//	u32 index = 0;
//	while (emitter_instance)
//	{
//		fixed_string<128> em_name;
//		em_name.assignf("emitter %d", index++);
//		
//		out_info_array->emitter_name  = em_name;//emitter_instance->m_emitter->m_emitter_name;
//		out_info_array->material_name = emitter_instance->m_material->get_material_name();
//		out_info_array->max_particles = emitter_instance->m_emitter->get_num_max_particles();
//		
//		emitter_instance = m_lods[0].m_emitter_instance_list.get_next_of_object(emitter_instance);
//		out_info_array++;
//	}
//	out_info_array = 0;
//}

void particle_system_instance_impl::set_ticked( bool ticked ) 
{
	m_ticked = ticked;
}

bool particle_system_instance_impl::get_ticked( ) const
{
	return m_ticked;
}

void particle_system_instance_impl::pin()
{
	xray::threading::interlocked_increment(m_reference_count);
	m_pinned = true;
}

void particle_system_instance_impl::unpin()
{
	xray::threading::interlocked_decrement(m_reference_count);
	m_pinned = false;
}

void particle_system_instance_impl::set_pause( bool pause )
{
	m_paused = pause;
}

void particle_system_instance_impl::set_visible( bool visible )
{
	for (u32 i=0; i<m_num_lods; i++)
	{
		particle_emitter_instance* instance = m_lods[i].m_emitter_instance_list.front();
		while(instance)
		{
			instance->set_visible(visible);
			instance = instance->m_next;
		}
	}
	m_visible = visible;
}

u32 particle_system_instance_impl::calc_num_max_particles(float time_delta)
{
	if (m_paused)
		return 0;

	u32 total_summ = 0;
	particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
	while(instance)
	{
		total_summ += instance->calc_num_max_particles(time_delta);
		instance = instance->m_next;
	}
	if (m_current_lod!=m_old_lod)
	{
		instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			total_summ += instance->calc_num_max_particles(time_delta);
			instance = instance->m_next;
		}
	}
	return total_summ;
}

u32 particle_system_instance_impl::calc_num_new_particles(float time_delta)
{
	if (m_paused)
		return 0;
	
	u32 total_summ = 0;
	particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
	while(instance)
	{
		total_summ += instance->calc_num_new_particles(time_delta, !m_no_more_create);
		instance = instance->m_next;
	}
	if (m_current_lod!=m_old_lod)
	{
		instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			total_summ += instance->calc_num_new_particles(time_delta, !m_no_more_create);
			instance = instance->m_next;
		}
	}
	return total_summ;
}

u32 particle_system_instance_impl::remove_dead_particles()
{
	u32 total_summ = 0;
	particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
	while(instance)
	{
		total_summ += instance->remove_dead_particles();
		instance = instance->m_next;
	}
	if (m_current_lod!=m_old_lod)
	{
		instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			total_summ += instance->remove_dead_particles();
			instance = instance->m_next;
		}
	}
	return total_summ;
}

void particle_system_instance_impl::remove_overflowing_particles()
{
	particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
	while(instance)
	{
		instance->remove_overflowing_particles();
		instance = instance->m_next;
	}
	if (m_current_lod!=m_old_lod)
	{
		instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			instance->remove_overflowing_particles();
			instance = instance->m_next;
		}
	}
}

void particle_system_instance_impl::shrink_particles(float time_delta, float limit_over_total, u32 num_need_particles)
{
	particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
	while(instance)
	{
		instance->shrink_particles(time_delta, limit_over_total, num_need_particles);
		instance = instance->m_next;
	}
	if (m_current_lod!=m_old_lod)
	{
		particle_emitter_instance* instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			instance->shrink_particles(time_delta, limit_over_total, num_need_particles);
			instance = instance->m_next;
		}
	}
}

void particle_system_instance_impl::apply_lod( u32 lod_index )
{
	ASSERT_CMP(lod_index, <, u32(max_supported_lods));
	
	// TODO: try variant with changing of creation rate, not only alpha.
	
	
  	m_old_lod			= m_current_lod;
	m_current_lod		= lod_index;
	m_lods_lerp_alpha	= 1.0f - m_lods_lerp_alpha;
	m_lerped			= false;
}

void particle_system_instance_impl::check_lods( xray::math::float3 const& view_location )
{
	if (!m_use_lods)
		return;
	
	ASSERT_CMP(m_num_lods, >=, 1);
	
	float distance = float3(view_location - m_transform.lines[3].xyz()).squared_length();
	
	// Lods are already sorted by distance.
	for (s32 i=m_num_lods-1; i>=0; i--)
	{
		lod_entry& lod		= m_lods[i];
		float activate_dist = lod.m_distance * lod.m_distance;
		
		if (distance>=activate_dist)
		{
			if (u32(i)!=m_current_lod)
				apply_lod(i);
			break;
		}
	}
}

void particle_system_instance_impl::remove_emitter_instances()
{
	for (u32 i=0; i<m_num_lods; i++)
	{
		particle_emitter_instance* instance = m_lods[i].m_emitter_instance_list.front();
		while(instance)
		{
			particle_emitter_instance* to_del = instance;
			instance = m_lods[i].m_emitter_instance_list.get_next_of_object(instance);
			MT_DELETE(to_del);
		}
		m_lods[i].m_emitter_instance_list.clear();
	}
}

bool particle_system_instance_impl::is_finished()
{
	if (m_no_more_create)
	{
		particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
		while(instance)
		{
			if (instance->m_num_live_particles!=0)
				return false;
			instance = instance->m_next;
		}
		return true;
	}
	
	for (u32 i=0; i<m_num_lods; i++)
	{
		particle_emitter_instance* instance = m_lods[i].m_emitter_instance_list.front();
		while(instance)
		{
			// If some lod set as always repeating we never kill other lods.
			if (instance->m_emitter.get_num_loops()==0)
				return false;
			instance = instance->m_next;
		}
	}
	
	emitter_instance_list_type& emitter_instances_list = m_lods[m_current_lod].m_emitter_instance_list;
	
	particle_emitter_instance* instance = emitter_instances_list.front();
	
	if (!instance)
		return false;
	
	while(instance)
	{
		bool finished = instance->is_finished();
		if (!finished)
		{
			
		}
		else if (m_always_looping)
		{
			instance->m_current_loop	= 0; 
			instance->m_delayed			= true;
			instance->m_delay_time		= 0.0f;
			instance->m_waiting_for_end = false;
			instance->m_emitter_time    = 0.0f;
			instance->recalc_duration	();
		}
		else
		{
			instance->remove_particles();
		}
		
		if (finished && instance->m_is_child_emitter_instance)
		{
			instance->remove_particles();
			emitter_instances_list.erase(instance);
		}
		instance = emitter_instances_list.get_next_of_object(instance);
	}
	
	if (m_always_looping)
		return false;
	
	return true;
}

void particle_system_instance_impl::set_template(u32 lod_index, particle_system_ptr templ)
{
	ASSERT_CMP(lod_index, <, u32(max_supported_lods));
	
	//m_template = templ;
	m_lods[lod_index].m_template = templ;
}

void particle_system_instance_impl::set_transform(xray::math::float4x4 const& transform)
{
	m_transform = transform;
	for (u32 i=0; i<m_num_lods; i++)
	{
		particle_emitter_instance* instance = m_lods[i].m_emitter_instance_list.front();
		while(instance)
		{
			instance->set_transform(transform);
			instance = instance->m_next;
		}
	}
}

void particle_system_instance_impl::add_emitter_instance( u32 lod_index, particle_emitter_instance* new_instance )
{
	ASSERT_CMP(lod_index, <, u32(max_supported_lods));
	
	// TODO: move it to constructor
	new_instance->m_particle_system_instance = this;
	
	m_lods[lod_index].m_emitter_instance_list.push_back(new_instance);
	
	//new_instance->process_event(particle_event::on_play,m_transform.lines[3].xyz());
}

void particle_system_instance_impl::play_impl()
{
	m_is_playing = true;
	//m_particle_world.add_particle_system_instance(this);
}

void particle_system_instance_impl::play_impl(xray::math::float4x4 const& transform)
{
	set_transform(transform);
	play_impl();
}

void particle_system_instance_impl::stop_impl(float time)
{
	(void)&time;
	// TODO: если time==0 то удалять пс немедленно.
	// time - время доигрывания пс.
	// если time больше ее времени жизни, то пс просто доиграет до конца и удалится.
	m_is_playing = false;
	m_no_more_create = true;
}

void particle_system_instance_impl::process_lods_lerping( float time_delta )
{
	// Tick old lod.
	if (m_lods_lerp_alpha<1.0f)
	{
		// TODO: take max particle lifetime from emitter, use it as scaler = 1.0f / lifetime mb.
		m_lods_lerp_alpha += time_delta * (1.0f / 2.0f) * 0.5f;
		xray::math::clamp(m_lods_lerp_alpha,0.0f,1.0f);
		
		particle_emitter_instance* instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			instance->tick(time_delta, !m_no_more_create, 1.0f - m_lods_lerp_alpha);
			instance = instance->m_next;
		}
	}
	else // Clear old lod.
	{
		m_lerped = true;
		
		particle_emitter_instance* instance = m_lods[m_old_lod].m_emitter_instance_list.front();
		while(instance)
		{
			instance->remove_particles();
			instance = instance->m_next;
		}
	}
}

bool particle_system_instance_impl::tick(float time_delta )
{
	if (m_paused)
		return false;
	
	m_particle_system_time += time_delta;
	
	if (!m_lerped)
		process_lods_lerping(time_delta);
	
	// Tick current lod.
	particle_emitter_instance* instance = m_lods[m_current_lod].m_emitter_instance_list.front();
	while(instance)
	{
		instance->tick( time_delta, !m_no_more_create, m_lerped ? 1.0f : (m_lods_lerp_alpha) );
		instance = instance->m_next;
	}
	
	return is_finished();
}

} // namespace particle
} // namespace xray