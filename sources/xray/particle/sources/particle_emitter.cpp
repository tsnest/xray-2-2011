////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_emitter.h"
#include "particle_actions.h"

namespace xray {
namespace particle {

particle_emitter::particle_emitter()
{
	
}

void particle_emitter::set_defaults()
{
	m_delay					= 0.0f;
	m_duration				= 1.0f;
	m_duration_variance		= 0.0f;
	
	m_num_loops				= 0;
	m_max_num_particles		= 500;
	
	m_source_action			= 0;
	m_data_type_action		= 0;
	
	m_num_actions			= 0;
	
	m_world_space			= false;
	
	m_burst_entries			= 0;
	m_num_burst_entries		= 0;
	
	m_actions				= 0;
	m_num_actions			= 0;
	
	m_visibility			= true;
	
	m_event					= 0;
	
	m_particle_system		= 0;
	
	xray::memory::zero(m_material_name);
	
	m_particle_lifetime_curve.set_defaults();
	m_particle_spawn_rate_curve.set_defaults();

	m_event_index			= 0;
}

void particle_emitter::set_source_action( particle_action_source* source_act ) 
{
	m_source_action = source_act;
}

void particle_emitter::set_data_type_action( particle_action_data_type* data_type_act ) 
{
	m_data_type_action = data_type_act;
}

void particle_emitter::add_action( particle_action* action )
{
	action->m_next = 0;
	
	if (!&*m_actions)
	{
		m_actions	  = action;
		m_last_action = action;
	}
	else
	{
		m_last_action->m_next = action;
		m_last_action = action;
	}
}

void particle_emitter::remove_action( particle_action* action )
{
	particle_action* previous_i	= 0;
	particle_action* i			= m_actions;
	
	for (; i && (i != action); previous_i = i, i = i->m_next);
	
	if (i != action)
		return;
	
	if (previous_i)
		previous_i->m_next = i->m_next;
	else
		m_actions = i->m_next;
	
	if (!i->m_next)
		m_last_action = previous_i ? previous_i : m_actions;
}

u32 particle_emitter::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	u32 result = m_particle_lifetime_curve.save_binary(buffer, calc_size);
	result    += m_particle_spawn_rate_curve.save_binary(buffer, calc_size);
	
	if (calc_size)
	{
		result += sizeof(burst_entry) * m_num_burst_entries;
	}
	else
	{
		u32 size = sizeof(burst_entry) * m_num_burst_entries;
		xray::memory::copy(buffer.c_ptr(), size, &*m_burst_entries, size);
		buffer += size;
	}
	return result;
}

void particle_emitter::load_binary(xray::mutable_buffer& buffer) 
{
	m_particle_lifetime_curve.load_binary(buffer);
	m_particle_spawn_rate_curve.load_binary(buffer);
	
	m_burst_entries = (burst_entry*)buffer.c_ptr();
	buffer += sizeof(burst_entry) * m_num_burst_entries;
}

void particle_emitter::load_material(pcstr /*material_name*/)
{
//	query_resource(
//		material_name,
//		material_class,
//		boost::bind(&particle_emitter::on_material_loaded, this, _1),
//		&::xray::memory::g_mt_allocator
//		);
}

void particle_emitter::on_material_loaded(xray::resources::queries_result& /*result*/)
{
//	if (result.is_successful())
//		m_material = static_cast_checked<xray::render::material*>(result[0].get_unmanaged_resource().c_ptr());
}

} // namespace particle
} // namespace xray
