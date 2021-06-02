////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_INLINE_H_INCLUDED
#define PARTICLE_SYSTEM_INLINE_H_INCLUDED

namespace xray {
namespace particle {

template < class ConfigValueType >
inline void particle_system::load_lod_from_config( particle_system_lod& lod, ConfigValueType const& lod_config, bool mt_alloc )
{
	//clear_guid_list();
	lod.m_parent = this;
	
	u32 num_total_emitters = get_num_total_emitters(lod_config);

	add_root_to_guid_list(read_config_value<pcstr>(lod_config, "id", ""), &lod);
	
	if (lod_config.value_exists("properties"))
		lod.m_total_length = read_config_value(lod_config["properties"],"TotalLength", lod.m_total_length);
	
	if (!lod_config.value_exists("children"))
		return;
	
	ConfigValueType children_order = lod_config["children_order"];

	lod.m_num_emitters = num_total_emitters;
	
	if (!num_total_emitters)
	{
		lod.m_emitters_array = 0;
		return;
	}
	
	lod.m_emitters_array = MT_ALLOC(particle_emitter, lod.m_num_emitters);
	xray::memory::zero(lod.m_emitters_array, lod.m_num_emitters * sizeof(particle_emitter));
	
	particle_emitter* emitter_to_init = lod.m_emitters_array;
	for (u32 i=0; i<lod.m_num_emitters; i++)
	{
		new(emitter_to_init)particle_emitter;
		emitter_to_init->set_defaults();
		emitter_to_init++;
	}
	
	u32 out_emitter_index = 0;
	for ( typename ConfigValueType::const_iterator it = children_order.begin(); it != children_order.end(); ++it )
	{
		ConfigValueType _it = lod_config["children"][pcstr(*it)];
		
		enum_particle_entity_type entity_type = enum_particle_entity_type(read_config_value<u32>(_it, "type_index", particle_action_unsupported_type));
		if (entity_type==particle_entity_emitter_type)
		{
			parse_emitter( lod, &lod.m_emitters_array[out_emitter_index], _it, mt_alloc, out_emitter_index );
		}
		else if (entity_type==particle_entity_particle_group_type)
		{
			parse_group( lod, _it, mt_alloc, out_emitter_index );
		}
	}
}

template< class ConfigValueType >
inline void particle_system::load_from_config( ConfigValueType const& config, bool mt_alloc )
{
	particle_system* ps = this;
	
	u32 num_lods = 1;
	
	ps->m_num_lods = num_lods;
	
	ps->m_lods = MT_ALLOC(particle_system_lod, num_lods);
	xray::memory::zero(ps->m_lods, num_lods * sizeof(particle_system_lod));
	
	particle_system_lod* lod_to_init = ps->m_lods;
	for (u32 i=0; i<num_lods; i++)
		new(lod_to_init++)particle_system_lod;
	
	ps->m_lods[0].set_defaults();
	
	
	// TODO: Iterate all lods.
	if (config.value_exists("LOD 0"))
	{
		configs::binary_config_value const& lod_config = config["LOD 0"];
		load_lod_from_config(ps->m_lods[0], lod_config, mt_alloc);
	}
}


template< class ConfigValueType >
inline u32 particle_system::get_num_total_emitters(ConfigValueType const& lod_config)
{
	if (!lod_config.value_exists("children"))
		return 0;
	
	u32 num_emitters = 0;
	ConfigValueType children_order = lod_config["children_order"];
	
	for ( typename ConfigValueType::const_iterator it = children_order.begin(); it != children_order.end(); ++it )
	{
		ConfigValueType const& a = lod_config["children"];
		ConfigValueType const& b = a[pcstr(*it)];
		ConfigValueType const& c = b["type_index"];
		
		if (u32(c) == particle_entity_emitter_type)
			num_emitters++;
	}
	
	for ( typename ConfigValueType::const_iterator it = children_order.begin(); it != children_order.end(); ++it )
	{
		ConfigValueType _it = lod_config["children"][pcstr(*it)];
		
		enum_particle_entity_type entity_type = enum_particle_entity_type(read_config_value<u32>(_it, "type_index", particle_action_unsupported_type));
		if (entity_type==particle_entity_emitter_type)
			num_emitters += get_num_emitters_in_emitter(_it);
		else if (entity_type==particle_entity_particle_group_type)
			num_emitters += get_num_emitters_in_group(_it);
	}
	return num_emitters;
}

template< class ConfigValueType >
inline void particle_system::parse_emitter( particle_system_lod& ps_lod, particle_emitter* emitter, ConfigValueType const& emitter_config, bool mt_alloc, u32& out_emitter_index )
{
	out_emitter_index++;
	emitter->m_event = 0;
	emitter->m_particle_system = this;
	
	add_emitter_to_guid_list(read_config_value<pcstr>(emitter_config, "id", ""), emitter);
	
	if (emitter_config.value_exists("properties"))
		emitter->load(emitter_config["properties"]);
	
	if (!emitter_config.value_exists("children"))
		return;
	
	ConfigValueType const& children_order = emitter_config["children_order"];
	
	for ( typename ConfigValueType::const_iterator it  = children_order.begin();	it != children_order.end(); ++it)
	{
		particle_action* new_action = construct_action( ps_lod, emitter, emitter_config["children"][pcstr(*it)], mt_alloc, out_emitter_index );
		
		if (!new_action) continue;
		
		emitter->add_action(new_action);
		emitter->m_num_actions++;
	}
}

template< class ConfigValueType >
inline void particle_system::parse_group( particle_system_lod& ps_lod, ConfigValueType const& group_config, bool mt_alloc, u32& out_emitter_index )
{
	bool group_visibility  = read_config_value(group_config["properties"], "Enabled",  true);
	bool group_world_space = read_config_value(group_config["properties"], "WorldSpace",  false);
	u32 group_num_loops    = read_config_value<u32>(group_config["properties"], "Loops",  0);
	
	ConfigValueType children_order = group_config["children_order"];
	
	for ( typename ConfigValueType::const_iterator it = children_order.begin(); it != children_order.end(); ++it )
	{
		ConfigValueType _it = group_config["children"][pcstr(*it)];
		enum_particle_entity_type entity_type = enum_particle_entity_type(read_config_value<u32>(_it, "type_index", particle_action_unsupported_type));
		
		if (entity_type==particle_entity_emitter_type)
		{
			particle_emitter& emitter = ps_lod.m_emitters_array[out_emitter_index];
			
			emitter.m_world_space	= group_world_space;
			emitter.m_num_loops		= group_num_loops;
			emitter.m_visibility	= group_visibility;
			
			parse_emitter( ps_lod, &emitter, _it, mt_alloc, out_emitter_index );
		}
		else if (entity_type==particle_entity_particle_group_type)
		{
			parse_group(ps_lod, _it, mt_alloc, out_emitter_index);
		}
	}
}
template< class ConfigValueType >
inline u32 particle_system::get_num_emitters_in_group(ConfigValueType const& group_config)
{
	if (!group_config.value_exists("children"))
		return 0;

	u32 num_emitters = 0;
	ConfigValueType const& children_order = group_config["children_order"];

	for ( typename ConfigValueType::const_iterator it = children_order.begin(); it != children_order.end(); ++it )
	{
		if (u32(group_config["children"][pcstr(*it)]["type_index"]) == particle_entity_emitter_type)
			num_emitters++;
	}

	for ( typename ConfigValueType::const_iterator it  = children_order.begin();	it != children_order.end(); ++it)
	{
		ConfigValueType _it = group_config["children"][pcstr(*it)];
		enum_particle_entity_type entity_type = enum_particle_entity_type(read_config_value<u32>(_it, "type_index", particle_action_unsupported_type));
		if (entity_type==particle_entity_emitter_type)
			num_emitters += get_num_emitters_in_emitter(_it);
		else if (entity_type==particle_entity_particle_group_type)
			num_emitters += get_num_emitters_in_group(_it);
	}
	return num_emitters;
}

template< class ConfigValueType >
inline u32 particle_system::get_num_emitters_in_emitter(ConfigValueType const& emitter_config)
{
	if (!emitter_config.value_exists("children"))
		return 0;

	u32 num_emitters = 0;
	ConfigValueType const& children_order = emitter_config["children_order"];

	for ( typename ConfigValueType::const_iterator it  = children_order.begin();	it != children_order.end(); ++it)
	{
		ConfigValueType _it = emitter_config["children"][pcstr(*it)];
		enum_particle_entity_type entity_type = enum_particle_entity_type(read_config_value<u32>(_it, "type_index", particle_action_unsupported_type));
		if (entity_type>=particle_action_on_death_type && entity_type<=particle_action_on_play_type)
			num_emitters += get_num_total_emitters(_it);
	}
	return num_emitters;
}

template< class ConfigValueType >
inline particle_action* particle_system::construct_action(particle_system_lod& ps_lod, particle_emitter* emitter, ConfigValueType action_config, bool mt_alloc, u32& out_emitter_index)
{
	(void)&ps_lod;
	
	//u32						  id			= read_config_value<u32>(action_config, "id", 0);
	enum_particle_entity_type entity_type	= enum_particle_entity_type(read_config_value<u32>(action_config, "type_index", particle_action_unsupported_type));
	particle_action*		  particle_act  = 0;
	particle_event*			  particle_evt  = 0;
	
	switch(entity_type)
	{
		case particle_action_initial_domain_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_source);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			emitter->set_source_action( (particle_action_source*)particle_act );
			break;
		}
		case particle_action_initial_color_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_initial_color);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_initial_size_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_initial_size);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_initial_velocity_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_initial_velocity);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_initial_rotation_rate_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_initial_rotation_rate);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		
		case particle_action_random_velocity_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_random_velocity);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_initial_rotation_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_initial_rotation);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_velocity_over_lifetime_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_velocity_over_lifetime);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_acceleration_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_acceleration);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_size_over_velocity_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_size_over_velocity);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_size_over_lifetime_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_size_over_lifetime);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_kill_volume_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_kill_volume);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_color_over_lifetime_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_color_over_lifetime);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_orbit_type: 
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_orbit);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			break;
		}
		case particle_action_billboard_type: 
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_billboard);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			emitter->set_data_type_action((particle_action_data_type*)particle_act);
			break;
		}
		case particle_action_mesh_type: 
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_mesh);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			emitter->set_data_type_action((particle_action_data_type*)particle_act);
			break;
		}
		case particle_action_trail_type: 
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_trail);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			emitter->set_data_type_action((particle_action_data_type*)particle_act);
			break;
		}
		case particle_action_beam_type: 
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_beam);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			emitter->set_data_type_action((particle_action_data_type*)particle_act);
			break;
		}
		case particle_action_decal_type: 
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_act = MT_NEW(particle_action_decal);
			particle_act->set_defaults(mt_alloc);
			particle_act->load(prop_config);
			emitter->set_data_type_action((particle_action_data_type*)particle_act);
			break;
		}
		case particle_action_on_death_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_evt = MT_NEW(particle_event_on_death);
			particle_evt->set_defaults();
			particle_evt->load(prop_config);
			break;
		}
		case particle_action_on_birth_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_evt = MT_NEW(particle_event_on_birth);
			particle_evt->set_defaults();
			particle_evt->load(prop_config);
			break;
		}
		case particle_action_on_play_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_evt = MT_NEW(particle_event_on_play);
			particle_evt->set_defaults();
			particle_evt->load(prop_config);
			break;
		}
		case particle_action_on_collide_type:
		{
			ConfigValueType prop_config = action_config["properties"];
			particle_evt = MT_NEW(particle_event_on_collide);
			particle_evt->set_defaults();
			particle_evt->load(prop_config);
			break;
		}
	}
	if (particle_evt)
	{
		particle_act = particle_evt;
		if (action_config.value_exists("children"))
		{
			ConfigValueType children_order = action_config["children_order"];
			
			u32 start_emitter_index = out_emitter_index;
			
			for ( typename ConfigValueType::const_iterator it = children_order.begin(); it != children_order.end(); ++it )
			{
				ConfigValueType _it = action_config["children"][pcstr(*it)];
				
				enum_particle_entity_type child_entity_type = enum_particle_entity_type(read_config_value<u32>(_it, "type_index", particle_action_unsupported_type));
				switch(child_entity_type)
				{
					case particle_entity_emitter_type:
						parse_emitter( ps_lod, &ps_lod.m_emitters_array[out_emitter_index], _it, mt_alloc, out_emitter_index );
						break;
					case particle_entity_particle_group_type:
						parse_group( ps_lod, _it, mt_alloc, out_emitter_index );
						break;
				}

			}
			for (u32 i=start_emitter_index; i<out_emitter_index; i++)
			{
				if (!ps_lod.m_emitters_array[i].m_event)
					ps_lod.m_emitters_array[i].m_event = particle_evt;
			}
		}
	}
	
	if (particle_act)
	{
		add_action_to_guid_list(read_config_value<pcstr>(action_config, "id", ""), particle_act);
	}
	
	return particle_act;
}

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_INLINE_H_INCLUDED