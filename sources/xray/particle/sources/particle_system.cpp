////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_system.h"
#include "particle_actions.h"
#include "particle_system_instance_cook.h"
#include "particle_emitter.h"
#include "particle_world.h"

namespace xray {
namespace particle {

extern xray::uninitialized_reference<particle_system_instance_cook>	s_particle_cook_object;

static s32 action_classname_to_index(pcstr name)
{
		 if (strstr(name,"particle_action_initial_color"))			return 0;
	else if (strstr(name,"particle_action_initial_size"))			return 1;
	else if (strstr(name,"particle_action_initial_velocity"))		return 2;
	else if (strstr(name,"particle_action_initial_rotation"))		return 3;
	else if (strstr(name,"particle_action_velocity_over_lifetime"))	return 4;
	else if (strstr(name,"particle_action_color_over_lifetime"))	return 5;
	else if (strstr(name,"particle_action_rotation_over_lifetime"))	return 6;
	else if (strstr(name,"particle_action_rotation_over_velocity"))	return 7;
	else if (strstr(name,"particle_action_size_over_lifetime"))		return 8;
	else if (strstr(name,"particle_action_size_over_velocity"))		return 9;
	//else if (strstr(name,"particle_action_lifetime"))				return 10;
	else if (strstr(name,"particle_action_source"))					return 11;
	else if (strstr(name,"particle_action_billboard"))				return 12;
	else if (strstr(name,"particle_action_mesh"))					return 13;
	else if (strstr(name,"particle_action_trail"))					return 14;
	else if (strstr(name,"particle_action_beam"))					return 15;
	else if (strstr(name,"particle_action_decal"))					return 16;
	else if (strstr(name,"particle_event_on_play"))					return 17;
	else if (strstr(name,"particle_event_on_birth"))				return 18;
	else if (strstr(name,"particle_event_on_death"))				return 19;
	else if (strstr(name,"particle_event_on_collide"))				return 20;
	else if (strstr(name,"particle_action_random_velocity"))		return 21;
	else if (strstr(name,"particle_action_initial_rotation_rate"))	return 22;
	else if (strstr(name,"particle_action_orbit"))					return 23;
	else if (strstr(name,"particle_action_acceleration"))			return 24;
	else if (strstr(name,"particle_action_kill_volume"))			return 25;

	ASSERT(0, "Invalid particle action class name.");
	return -1;
}

static particle_action* create_action_by_index(particle_system_lod& lod, xray::mutable_buffer& buffer, particle_emitter* emitter, s32 index)
{
	(void)&lod;

	particle_action* action = 0;
	switch (index)
	{
	case 0:			
		{
			action = new(buffer.c_ptr())particle_action_initial_color;
			buffer += sizeof(particle_action_initial_color);
			break;
		}
	case 1:
		{
			action = new(buffer.c_ptr())particle_action_initial_size;
			buffer += sizeof(particle_action_initial_size);
			break;
		}
	case 2:
		{
			action = new(buffer.c_ptr())particle_action_initial_velocity;
			buffer += sizeof(particle_action_initial_velocity);
			break;
		}
	case 3:
		{
			action = new(buffer.c_ptr())particle_action_initial_rotation;
			buffer += sizeof(particle_action_initial_rotation);
			break;
		}
	case 4:
		{
			action = new(buffer.c_ptr())particle_action_velocity_over_lifetime;
			buffer += sizeof(particle_action_velocity_over_lifetime);
			break;
		}
	case 5:
		{
			action = new(buffer.c_ptr())particle_action_color_over_lifetime;
			buffer += sizeof(particle_action_color_over_lifetime);
			break;
		}
	case 6:
		{
			action = new(buffer.c_ptr())particle_action_rotation_over_lifetime;
			buffer += sizeof(particle_action_rotation_over_lifetime);
			break;
		}
	case 7:
		{
			action = new(buffer.c_ptr())particle_action_rotation_over_velocity;
			buffer += sizeof(particle_action_rotation_over_velocity);
			break;
		}
	case 8:
		{
			action = new(buffer.c_ptr())particle_action_size_over_lifetime;
			buffer += sizeof(particle_action_size_over_lifetime);
			break;
		}
	case 9:
		{
			action = new(buffer.c_ptr())particle_action_size_over_velocity;
			buffer += sizeof(particle_action_size_over_velocity);
			break;
		}
	case 10:
		{
			//action = new(buffer.c_ptr())particle_action_lifetime;
			//buffer += sizeof(particle_action_lifetime);
			break;
		}
	case 11:
		{
			action = new(buffer.c_ptr())particle_action_source;
			buffer += sizeof(particle_action_source);
			emitter->set_source_action((particle_action_source*)action);
			break;
		}
	case 12:
		{
			action = new(buffer.c_ptr())particle_action_billboard;
			buffer += sizeof(particle_action_billboard);
			emitter->set_data_type_action((particle_action_data_type*)action);
			break;
		}
	case 13:
		{
			action = new(buffer.c_ptr())particle_action_mesh;
			buffer += sizeof(particle_action_mesh);
			emitter->set_data_type_action((particle_action_data_type*)action);
			break;
		}
	case 14:
		{
			action = new(buffer.c_ptr())particle_action_trail;
			buffer += sizeof(particle_action_trail);
			emitter->set_data_type_action((particle_action_data_type*)action);
			break;
		}
	case 15:
		{
			action = new(buffer.c_ptr())particle_action_beam;
			buffer += sizeof(particle_action_beam);
			emitter->set_data_type_action((particle_action_data_type*)action);
			break;
		}
	case 16:
		{
			action = new(buffer.c_ptr())particle_action_decal;
			buffer += sizeof(particle_action_decal);
			emitter->set_data_type_action((particle_action_data_type*)action);
			break;
		}
	case 17:
		{
			action = new(buffer.c_ptr())particle_event_on_play;
			buffer += sizeof(particle_event_on_play);
			break;
		}
	case 18:
		{
			action = new(buffer.c_ptr())particle_event_on_birth;
			buffer += sizeof(particle_event_on_birth);
			break;
		}
	case 19:
		{
			action = new(buffer.c_ptr())particle_event_on_death;
			buffer += sizeof(particle_event_on_death);
			break;
		}
	case 20:
		{
			action = new(buffer.c_ptr())particle_event_on_collide;
			buffer += sizeof(particle_event_on_collide);
			break;
		}
	case 21:
		{
			action = new(buffer.c_ptr())particle_action_random_velocity;
			buffer += sizeof(particle_action_random_velocity);
			break;
		}
	case 22:
		{
			action = new(buffer.c_ptr())particle_action_initial_rotation_rate;
			buffer += sizeof(particle_action_initial_rotation_rate);
			break;
		}
	case 23:
		{
			action = new(buffer.c_ptr())particle_action_orbit;
			buffer += sizeof(particle_action_orbit);
			break;
		}
	case 24:
		{
			action = new(buffer.c_ptr())particle_action_acceleration;
			buffer += sizeof(particle_action_acceleration);
			break;
		}
	case 25:
		{
			action = new(buffer.c_ptr())particle_action_kill_volume;
			buffer += sizeof(particle_action_kill_volume);
			break;
		}
	};
	return action;
}

particle_system::~particle_system()
{
//   	for (u32 i=0; i<m_num_lods; i++)
//   	{
//   		for (u32 j=0; j<m_lods[i].m_num_emitters; j++)
//   		{
//   			particle_emitter* emitter = &m_lods[i].m_emitters_array[j];
//   			emitter->~particle_emitter();
//   		}
//   		
//   		particle_emitter* pointer	= m_lods[i].m_emitters_array;
//   		mt_free						( pointer );
//   		m_lods[i].m_emitters_array	= 0;
//   	}
//   	
//   	particle_system_lod* pointer	= m_lods;
//   	mt_free							( pointer );
//   	m_lods							= 0;
}

void particle_system::set_defaults( )
{
	m_lods = 0;
	m_num_lods = 0;
}

u32 particle_system::action_to_index(particle_system_lod& lod, particle_action* in_action)
{
	if (in_action)
	{
		u32 action_index = 0;
		for (u32 i=0; i<lod.m_num_emitters; i++)
		{
			particle_action* action = lod.m_emitters_array[i].m_actions;
			while(action)
			{
				if(action==in_action)
				{
					return (action_index+1);
				}
				action_index++;
				action = action->m_next;
			}
		}
	}

	return 0;
}

particle_action* particle_system::index_to_action(particle_system_lod& lod, u32 index)
{
	if (index==0)
		return 0;
	
	u32 action_index = 0;
	for (u32 i=0; i<lod.m_num_emitters; i++)
	{
		particle_action* action = lod.m_emitters_array[i].m_actions;
		while(action)
		{
			if(action_index==((u32)index-1))
			{
				return action;
			}
			action_index++;
			action = action->m_next;
		}
	}
	return 0;
}

void particle_system::load_lod_actions_binary( particle_system_lod& lod, xray::mutable_buffer& buffer)
{
	for (u32 e=0; e<lod.m_num_emitters; e++)
	{
		particle_emitter* emitter = &lod.m_emitters_array[e];
		
		emitter->m_actions		= 0;
		emitter->m_last_action	= 0;
		
		emitter->m_particle_system = this;
		
		for (u32 a=0; a<emitter->m_num_actions; a++)
		{
			s32 class_index;
			
			xray::memory::copy(&class_index, sizeof(class_index), buffer.c_ptr(), sizeof(class_index));
			buffer+=sizeof(class_index);
			
			particle_action* action = create_action_by_index(lod, buffer, emitter, class_index);
			
			if (action) 
			{
				action->load_binary(buffer);
				emitter->add_action(action);
			}
		}
	}
}

void particle_system::load_binary( xray::mutable_buffer& buffer )
{
	xray::memory::copy(&m_num_lods, sizeof(m_num_lods), buffer.c_ptr(), sizeof(m_num_lods));
	buffer+=sizeof(m_num_lods);
	
	// Load lods.
	m_lods = (particle_system_lod*)buffer.c_ptr();

	particle_system_lod* init_lod = m_lods;
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		new(init_lod++)(particle_system_lod);
	}
	
	buffer += sizeof(particle_system_lod) * m_num_lods;
	
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		particle_system_lod& lod = m_lods[lod_index];
		
		// Load emitters.
		lod.m_emitters_array = (particle_emitter*)buffer.c_ptr();
		for (u32 emitter_index=0; emitter_index<lod.m_num_emitters; emitter_index++)
		{
			particle_emitter* emitter = new(buffer.c_ptr())particle_emitter;
			(void)&emitter;
			buffer += sizeof(particle_emitter);
		}
	}
	
	// Load emitters binary data.
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		particle_system_lod& lod = m_lods[lod_index];
		
		for (u32 emitter_index=0; emitter_index<lod.m_num_emitters; emitter_index++)
		{
			particle_emitter& emitter = lod.m_emitters_array[emitter_index];
			emitter.load_binary(buffer);
		}
	}
	
	// Load actions.
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
		load_lod_actions_binary(m_lods[lod_index], buffer);
	
	// Link ptrs.
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		particle_system_lod& lod = m_lods[lod_index];
		for (u32 emitter_index=0; emitter_index<lod.m_num_emitters; emitter_index++)
		{
			particle_emitter* emitter = &lod.m_emitters_array[emitter_index];
			emitter->m_event = (particle_event*)index_to_action(lod, emitter->m_event_index);
		}
	}
}

u32 particle_system::save_lod_actions_binary( particle_system_lod& lod, xray::mutable_buffer& buffer, bool cals_size )
{
	u32 save_size = 0;
	for (u32 e=0; e<lod.m_num_emitters; e++)
	{
		particle_emitter* emitter = &lod.m_emitters_array[e];

		particle_action* action = emitter->m_actions;
		u32 index = 0;
		while(action)
		{
			pcstr n = action->get_class_name();
			s32 class_index = action_classname_to_index(n);
			u32 class_size = action->get_class_size();
			
			if (cals_size)
				save_size+=sizeof(class_index);
			else
			{
				xray::memory::copy(buffer.c_ptr(), sizeof(class_index), &class_index, sizeof(class_index));
				buffer+=sizeof(class_index);
			}
			
			if (cals_size)
				save_size+=class_size;
			else
			{
				xray::memory::copy(buffer.c_ptr(), class_size, action, class_size);
				buffer+=class_size;
			}
			
			save_size += action->save_binary(buffer, cals_size);
			
			index++;
			action = action->m_next;
		}
	}
	return save_size;
}

u32 particle_system::save_binary( xray::mutable_buffer& buffer, bool cals_size )
{
	u32 save_size = 0;
	
	if (cals_size)
		save_size+=sizeof(m_num_lods);
	else
	{
		xray::memory::copy(buffer.c_ptr(), sizeof(m_num_lods), &m_num_lods, sizeof(m_num_lods));
		buffer+=sizeof(m_num_lods);
	}
	
	// Save lods.
	if (cals_size)
		save_size+=sizeof(particle_system_lod) * m_num_lods;
	else
	{
		xray::memory::copy(buffer.c_ptr(), sizeof(particle_system_lod) * m_num_lods, m_lods, sizeof(particle_system_lod) * m_num_lods);
		buffer+=sizeof(particle_system_lod) * m_num_lods;
	}
	
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		particle_system_lod& lod = m_lods[lod_index];
		
		// Save emitters.
		if (cals_size)
		{
			for (u32 emitter_index=0; emitter_index<lod.m_num_emitters; emitter_index++)
			{
				save_size += sizeof(particle_emitter);// + sizeof(burst_entry) * lod.m_emitters_array[lod_index].m_num_burst_entries;
			}
		}
		else
		{
			for (u32 emitter_index=0; emitter_index<lod.m_num_emitters; emitter_index++)
			{
				particle_emitter& emitter = lod.m_emitters_array[emitter_index];
				
				emitter.m_event_index = (u16)action_to_index(lod, emitter.m_event);
				
				xray::memory::copy(buffer.c_ptr(), sizeof(particle_emitter), &emitter, sizeof(particle_emitter));
				
				buffer+=sizeof(particle_emitter);
			}
		}
	}
	
	// Save emitters binary data.
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		particle_system_lod& lod = m_lods[lod_index];
		
		for (u32 emitter_index=0; emitter_index<lod.m_num_emitters; emitter_index++)
			save_size+=lod.m_emitters_array[emitter_index].save_binary(buffer, cals_size);
	}
	
	// Save actions.
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
		save_size += save_lod_actions_binary(m_lods[lod_index], buffer, cals_size);
	
	return save_size;
}

} // namespace particle
} // namespace xray