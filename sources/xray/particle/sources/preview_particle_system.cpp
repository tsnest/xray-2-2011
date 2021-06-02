////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef MASTER_GOLD

#include "preview_particle_system.h"
#include "particle_actions.h"
#include "particle_emitter.h"
#include "particle_world.h"
#include "particle_entity_types.h"
#include "particle_system_instance_cook.h"
#include "preview_particle_system_instance.h"

namespace xray {
namespace particle {

extern xray::uninitialized_reference<particle_system_instance_cook>	s_particle_cook_object;

static particle_action* particle_action_by_type(particle_emitter* emitter, enum_particle_entity_type type)
{
	particle_action* action = 0;
	switch (type)
	{
		case particle_action_initial_domain_type:
			{
				action = MT_NEW(particle_action_source);
				if (emitter)
					emitter->set_source_action(static_cast_checked<particle_action_source*>(action));
				break;
			}
		case particle_action_initial_color_type:			action = MT_NEW(particle_action_initial_color); break;
		case particle_action_initial_velocity_type: 		action = MT_NEW(particle_action_initial_velocity); break;
		case particle_action_initial_rotation_rate_type: 	action = MT_NEW(particle_action_initial_rotation_rate); break;
		case particle_action_initial_size_type:				action = MT_NEW(particle_action_initial_size); break;
		case particle_action_initial_rotation_type:			action = MT_NEW(particle_action_initial_rotation); break;
		case particle_action_random_velocity_type:			action = MT_NEW(particle_action_random_velocity); break;
		case particle_action_color_over_lifetime_type:		action = MT_NEW(particle_action_color_over_lifetime); break;
		case particle_action_velocity_over_lifetime_type:	action = MT_NEW(particle_action_velocity_over_lifetime); break;
		case particle_action_acceleration_type:				action = MT_NEW(particle_action_acceleration); break;
		case particle_action_rotation_over_lifetime_type:	action = MT_NEW(particle_action_rotation_over_lifetime); break;
		case particle_action_rotation_over_velocity_type:	action = MT_NEW(particle_action_rotation_over_velocity); break;
		case particle_action_size_over_lifetime_type:		action = MT_NEW(particle_action_size_over_lifetime); break;
		case particle_action_kill_volume_type:				action = MT_NEW(particle_action_kill_volume); break;
		case particle_action_size_over_velocity_type:		action = MT_NEW(particle_action_size_over_velocity); break;
		case particle_action_billboard_type:				action = MT_NEW(particle_action_billboard);	break;
		case particle_action_mesh_type:						action = MT_NEW(particle_action_mesh); break;
		case particle_action_trail_type:					action = MT_NEW(particle_action_trail);	break;
		case particle_action_beam_type:						action = MT_NEW(particle_action_beam); break;
		case particle_action_decal_type:					action = MT_NEW(particle_action_decal);	break;
		case particle_action_on_death_type:					action = MT_NEW(particle_event_on_death); break;
		case particle_action_on_birth_type:					action = MT_NEW(particle_event_on_birth); break;
		case particle_action_on_play_type:					action = MT_NEW(particle_event_on_play); break;
		case particle_action_on_collide_type:				action = MT_NEW(particle_event_on_collide);	break;
		case particle_action_orbit_type:					action = MT_NEW(particle_action_orbit);	break;
	}
	if (action)
	{
		action->set_defaults(true);
	}
	return action;
}



preview_particle_system::preview_particle_system( particle_world& particle_world ):
	m_particle_world	( particle_world )
{
	m_preview_instance = 0;
	m_guids_list.clear();
}

preview_particle_system::~preview_particle_system()
{
	//stop_preview();
	
	for (u32 i=0; i<m_num_lods; i++)
	{
		for (u32 j=0; j<m_lods[i].m_num_emitters; j++)
		{
			particle_emitter* emitter = &m_lods[i].m_emitters_array[j];
			emitter->~particle_emitter();
		}
		
		particle_emitter* pointer	= m_lods[i].m_emitters_array;
		
		if (!pointer)
			continue;
		
		particle_action* action		= pointer->m_actions;
		while (action)
		{
			particle_action* to_delete = action;
			action = action->m_next;
			to_delete->~particle_action();
			MT_DELETE(to_delete);
		}
		m_lods[i].m_emitters_array	= 0;
		pointer->m_actions		    = 0;
		MT_FREE						( pointer );
	}
	
	particle_system_lod* pointer	= m_lods;
	MT_FREE							( pointer );
	m_lods							= 0;
}

void preview_particle_system::add_emitter(u32 lod_index, pcstr emitter_id, pcstr, xray::configs::lua_config_value const& emitter_config)
{
	ASSERT_CMP					( lod_index, <, m_num_lods );
	particle_system_lod& lod	= m_lods[lod_index];
	
	particle_emitter* emitters = MT_ALLOC(particle_emitter, lod.m_num_emitters + 1);
	
	particle_emitter* init_emitters = emitters;
	for (u32 i=0; i<lod.m_num_emitters+1; i++)
	{
		new(init_emitters)particle_emitter;
		init_emitters->set_defaults();
		init_emitters->m_particle_system = this;
		init_emitters++;
	}
	
	if (lod.m_num_emitters)
	{
		xray::memory::copy(emitters, sizeof(particle_emitter) * lod.m_num_emitters, lod.m_emitters_array, sizeof(particle_emitter) * lod.m_num_emitters);
		
		for (u32 i=0; i<lod.m_num_emitters; i++)
			exchange_emitters_in_guid_list(&lod.m_emitters_array[i], &emitters[i]);
		
		particle_emitter* pointer		= &*lod.m_emitters_array;
		
//		burst_entry* burst_entry_point  = &*pointer->m_burst_entries;
//		MT_FREE						( burst_entry_point );
		
		MT_FREE						( pointer );
		lod.m_emitters_array		= 0;
	}
	
	add_emitter_to_guid_list(emitter_id, &emitters[lod.m_num_emitters]);
	//add_group_to_guid_list(group_id, &emitters[lod.m_num_emitters]);
	
	emitters[lod.m_num_emitters].load(emitter_config);
 	
	lod.m_emitters_array = emitters;
	lod.m_num_emitters++;
}

void preview_particle_system::remove_emitter(u32 lod_index, particle_emitter* emitter)
{
	ASSERT_CMP						( lod_index, <, m_num_lods );
	particle_system_lod& lod		= m_lods[lod_index];

	if (!emitter || !lod.m_num_emitters)
		return;

	if (lod.m_num_emitters - 1 == 0)
	{
		particle_emitter* pointer	= &*lod.m_emitters_array;
		MT_FREE						( pointer );
		lod.m_emitters_array		= 0;
		lod.m_num_emitters			= 0;
		return;
	}
	
	particle_emitter* emitters = MT_ALLOC(particle_emitter, lod.m_num_emitters - 1);

	u32 new_index = 0;
	for (u32 i=0; i<lod.m_num_emitters; i++)
	{
		if (emitter==&lod.m_emitters_array[i])
			continue;

		xray::memory::copy(&emitters[new_index], sizeof(particle_emitter), &lod.m_emitters_array[i], sizeof(particle_emitter));
		exchange_emitters_in_guid_list(&lod.m_emitters_array[i], &emitters[new_index]);

		new_index++;
	}

	particle_emitter* pointer	= &*lod.m_emitters_array;
	MT_FREE						( pointer );
	lod.m_emitters_array		= 0;

	lod.m_emitters_array = emitters;
	lod.m_num_emitters--;
}

void preview_particle_system::remove_action(u32, pcstr action_id)
{
	particle_action* action = name_to_action(action_id);
	particle_emitter* emitter = find_actions_emitter(action);
	
	remove_action_from_guid_list(action_id);
	
	if (!emitter)
		return;

	if (action==emitter->get_source_action())
		emitter->set_source_action(0);
	else if (action==emitter->get_data_type_action())
		emitter->set_data_type_action(0);
	
	emitter->remove_action(action);
	emitter->m_num_actions--;

	MT_DELETE(action);
}

void preview_particle_system::remove_group(u32, pcstr /*group_id*/)
{
	//guid_to_object_pair* pair = m_guids_list.front();
	//while (pair)
	//{
	//	guid_to_object_pair* next = pair->next;
	//	if (pair->guid==group_id)
	//	{
	//		//particle_emitter* emitter = (particle_emitter*)pair->object;
	//		//remove_emitter(lod_index, emitter);
	//		//remove_emitter_from_guid_list(entity_id);
	//	}
	//	pair = next;
	//}
}

particle_action* preview_particle_system::name_to_action(pcstr action_id)
{
	guid_to_object_pair* pair = m_guids_list.front();
	while(pair)
	{
		if (pair->guid==action_id && pair->type==guid_to_object_pair::is_action)
		{
			return (particle_action*)pair->object;
		}
		pair = pair->next;
	}
	return 0;
}

particle_emitter* preview_particle_system::name_to_emitter(u32, pcstr emitter_id)
{
	guid_to_object_pair* pair = m_guids_list.front();
	while(pair)
	{
		if (pair->guid==emitter_id && pair->type==guid_to_object_pair::is_emitter)
		{
			return (particle_emitter*)pair->object;
		}
		pair = pair->next;
	}
	return 0;
}
bool preview_particle_system::is_particle_root_type(pcstr entity_index)
{
	return is_object_type(guid_to_object_pair::is_root, entity_index);
}

bool preview_particle_system::is_particle_group_type(pcstr entity_index)
{
	return is_object_type(guid_to_object_pair::is_group, entity_index);
}
bool preview_particle_system::is_particle_emitter_type(pcstr entity_index)
{
	return is_object_type(guid_to_object_pair::is_emitter, entity_index);
}

bool preview_particle_system::is_particle_action_type(pcstr entity_index)
{
	return is_object_type(guid_to_object_pair::is_action, entity_index);
}

bool preview_particle_system::is_object_type(guid_to_object_pair::object_type obj_type, pcstr entity_index)
{
	guid_to_object_pair* pair = m_guids_list.front();
	while(pair)
	{
		if (pair->guid==entity_index && pair->type==obj_type)
		{
			return true;
		}
		pair = pair->next;
	}
	return false;
}

void preview_particle_system::on_group_changed(xray::configs::lua_config_value const& config, pcstr group_id)
{
	guid_to_object_pair* pair = m_guids_list.front();
	while (pair)
	{
		guid_to_object_pair* next = pair->next;
		if (pair->guid==group_id)
		{
			particle_emitter* emitter = (particle_emitter*)pair->object;
			emitter->load(config);
		}
		pair = next;
	}
}

void preview_particle_system::on_action_changed(pcstr)
{
	u32 lod_index = 0;
	particle_system_lod& lod = m_lods[lod_index];

	for (u32 i=0; i<lod.m_num_emitters; i++)
	{
		particle_emitter& emitter = lod.m_emitters_array[i];
		particle_action* action = emitter.m_actions;
		while(action)
		{
#pragma message( XRAY_TODO("Dima to IronNick: do not use dynamic_casts, use virtual function here instead") )
			particle_action_data_type* data_type_act = dynamic_cast<particle_action_data_type*>(action);
			if (data_type_act && data_type_act->get_visibility())
			{
				emitter.set_data_type_action(data_type_act);
				break;
			}
			action = action->m_next;
		}
	}
}

struct exchange_order
{
	exchange_order(): action(0) {}
	particle_action* action;
	exchange_order* next;
};


void preview_particle_system::on_exchange_actions(pcstr action_id0, pcstr action_id1)
{
	xray::intrusive_list<exchange_order,exchange_order*,&exchange_order::next> action_order;

	particle_action* act0 = name_to_action(action_id0);
	particle_action* act1 = name_to_action(action_id1);
	
	// TODO: Add exchanging action between different emitters.
	particle_emitter* emitter = find_actions_emitter(act0),
					* emitter0 = find_actions_emitter(act1);
	if (!emitter)
		return;

	ASSERT_CMP_U	( emitter, ==, emitter0 );

	particle_action* action = emitter->m_actions;
	while (action)
	{
		exchange_order* order = MT_NEW(exchange_order);
		
		if (action==act0)
			order->action = act1;
		else if (action==act1)
			order->action = act0;
		else
			order->action = action;
		
		action_order.push_back(order);
		action = action->m_next;
	}
	
	emitter->m_actions = 0;

	exchange_order* order = action_order.front();
	while (order)
	{
		exchange_order* next = order->next;
		emitter->add_action(order->action);
		MT_DELETE(order);
		order = next;
	}
}

particle_emitter* preview_particle_system::find_actions_emitter(particle_action* find_action)
{
	particle_system_lod& lod = m_lods[0];
	for (u32 i=0; i<lod.m_num_emitters; i++)
	{
		particle_emitter* emitter = &lod.m_emitters_array[i];
		particle_action* action = emitter->m_actions;
		while(action)
		{
			if (action==find_action)
			{
				return emitter;
			}
			action = action->m_next;
		}
	}
	return 0;
}

void preview_particle_system::clear_guid_list ( )
{
	guid_to_object_pair* pair = m_guids_list.front();
	while (pair)
	{
		guid_to_object_pair* next = pair->next;
		m_guids_list.erase(pair);
		MT_DELETE(pair);
		pair = next;
	}
}

void preview_particle_system::add_action_to_guid_list ( pcstr guid_str, particle_action* object)
{
	add_object_to_guid_list(guid_to_object_pair::is_action, guid_str, object);
}

void preview_particle_system::add_emitter_to_guid_list( pcstr guid_str, particle_emitter* object)
{
	add_object_to_guid_list(guid_to_object_pair::is_emitter, guid_str, object);
}

void preview_particle_system::add_group_to_guid_list  ( pcstr guid_str, particle_emitter* object)
{
	add_object_to_guid_list(guid_to_object_pair::is_group, guid_str, object);
}

void preview_particle_system::add_root_to_guid_list  ( pcstr guid_str,  particle_system_lod* lod)
{
	add_object_to_guid_list(guid_to_object_pair::is_root, guid_str, lod);
}

void preview_particle_system::add_object_to_guid_list( guid_to_object_pair::object_type obj_type, pcstr guid_str, pvoid object)
{
	guid_to_object_pair* pair = MT_NEW(guid_to_object_pair);

	pair->guid = guid_str;
	pair->next = 0;
	pair->object = object;
	pair->type = obj_type;

	m_guids_list.push_back(pair);	
}

void preview_particle_system::remove_action_from_guid_list ( pcstr id)
{
	remove_object_from_guid_list(guid_to_object_pair::is_action, id);
}

void preview_particle_system::remove_emitter_from_guid_list( pcstr id)
{
	remove_object_from_guid_list(guid_to_object_pair::is_emitter, id);
}

void preview_particle_system::remove_group_from_guid_list  ( pcstr id)
{
	remove_object_from_guid_list(guid_to_object_pair::is_group, id);
}

void preview_particle_system::remove_root_from_guid_list  ( pcstr id)
{
	remove_object_from_guid_list(guid_to_object_pair::is_root, id);
}

void preview_particle_system::remove_object_from_guid_list(guid_to_object_pair::object_type obj_type, pcstr id)
{
	guid_to_object_pair* pair = m_guids_list.front();
	while (pair)
	{
		if (pair->guid==id && pair->type==obj_type)
		{
			m_guids_list.erase(pair);
			MT_DELETE(pair);
			return;
		}
		pair = pair->next;
	}
}

void preview_particle_system::exchange_emitters_in_guid_list( particle_emitter* from, particle_emitter* to)
{
	guid_to_object_pair* pair = m_guids_list.front();
	while (pair)
	{
		if (pair->object==from && pair->type==guid_to_object_pair::is_emitter)
		{
			pair->object = to;
		}
		pair = pair->next;
	}
}

void preview_particle_system::update_from_config( configs::lua_config_value const& config_value )
{
/*
	incoming = 
	{
		command_type = "order_changed",
		id0 = (pcstr)entity_id0,
		id1 = (pcstr)entity_id1,
	}
*/

/*
	incoming = {
		command_type 	= "added",
		type		 	= enum_particle_entity_type,
		id				= Int32,
		parent_id	 	= Int32,
		initial_values	= {},
	}
	
	incoming = {
		command_type 	= "removed",
		id			 	= Int32,
	}
	
	incoming = {
		command_type 	= "changed",
		id			 	= Int32,
		changed_values	= {},
	}
*/
	configs::lua_config_value const& root_table = config_value;
	
	enum_particle_entity_type entity_type;
	
	pcstr	entity_id		     = 0, 
			parent_entity_id     = 0,
			id_exchange_with	 = 0;
	
	pcstr command_type		 = pcstr(root_table["command_type"]);
	
	if (root_table.value_exists("id"))
		entity_id				 = pcstr(root_table["id"]);
	
	if (strstr(command_type, "order_changed"))
	{
		entity_id			 = pcstr(root_table["id0"]);
		id_exchange_with	 = pcstr(root_table["id1"]);
		on_exchange_actions(entity_id, id_exchange_with);
	}
	else if (strstr(command_type, "added"))
	{	
		parent_entity_id	 = (pcstr)root_table["parent_id"];
		entity_type			 = enum_particle_entity_type((u32)root_table["type_index"]);
		
		if (entity_type == particle_entity_particle_group_type)
		{
			
		}
		else if (entity_type == particle_entity_emitter_type)
		{
			add_emitter(0, entity_id, parent_entity_id, root_table["initial_values"]);

			if (is_particle_action_type(parent_entity_id))
			{
				particle_emitter* emitter = name_to_emitter(0, entity_id);
				emitter->m_event = static_cast_checked<particle_event*>(name_to_action(parent_entity_id));
			}
		}
		else
		{
			particle_emitter* emitter = name_to_emitter(0, parent_entity_id);
			
			particle_action* action = particle_action_by_type(emitter, entity_type);
			//if (!emitter && is_particle_action_type(parent_entity_id))
			//{
			//	particle_action* evt = name_to_action(parent_entity_id);
			//	m_event
			//}
			add_action_to_guid_list(entity_id, action);
			
			if (action && emitter)
			{
				emitter->add_action(action);
				emitter->m_num_actions++;
				
				action->load(root_table["initial_values"]);
				
				on_action_changed(entity_id);
			}
			else
			{

			}
		}
	}
	else if (strstr(command_type, "removed"))
	{
		if (is_particle_group_type(entity_id))
		{
			remove_group(0, entity_id);
		}
		else if (is_particle_emitter_type(entity_id))
		{
			particle_emitter* emitter = name_to_emitter(0, entity_id);
			remove_emitter(0, emitter);
			remove_emitter_from_guid_list(entity_id);
		}
		else if (is_particle_action_type(entity_id))
		{
			remove_action(0, entity_id);
			on_action_changed(entity_id);
		}
	}
	else if (strstr(command_type, "changed"))
	{
		if (is_particle_group_type(entity_id))
		{
			on_group_changed(root_table["changed_values"], entity_id);
		}
		else if (is_particle_emitter_type(entity_id))
		{
			particle_emitter* emitter = name_to_emitter(0, entity_id);
			//root_table["changed_values"].save_as("f:/test.lua");
			emitter->load(root_table["changed_values"]);
		}
		else if (is_particle_action_type(entity_id))
		{
			particle_action* action   = name_to_action(entity_id);
			if (action)
				action->load(root_table["changed_values"]);
		}
	}
	m_preview_instance->updated_from_config(m_lods, m_num_lods);
}

void preview_particle_system::stop_preview()
{
	if (m_preview_instance)
	{
		m_preview_instance->unpin();
		m_particle_world.remove_particle_system_instance(m_preview_instance);
	}
}

void preview_particle_system::play_preview(math::float4x4 const& transform, bool always_looping)
{
	stop_preview();
	
	m_preview_instance	= MT_NEW( preview_particle_system_instance )(m_particle_world);
	m_preview_instance->pin();
	
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		m_preview_instance->set_template(lod_index, this);
		for (u32 i=0; i<m_lods[lod_index].m_num_emitters; i++)
		{
			particle_emitter* emitter = &m_lods[lod_index].m_emitters_array[i];
			
			if (emitter->m_event)
				continue;
			
			particle_emitter_instance* instance = m_particle_world.create_emitter_instance( *emitter );
			m_preview_instance->add_emitter_instance(lod_index, instance);
		}
	}
	
	m_preview_instance->set_deleter_object(&*s_particle_cook_object, threading::current_thread_id());
	
	m_preview_instance->set_creation_source(
		resource_base::creation_source_created_by_user, 
		"",
		xray::resources::memory_usage_type(xray::resources::nocache_memory, sizeof(particle_system_instance))
		);
	
	m_preview_instance->m_always_looping = always_looping;
	m_preview_instance->m_use_lods		 = false;
	
	m_preview_instance->play(transform);
}

} // namespace particle
} // namespace xray

#endif // #ifndef MASTER_GOLD
