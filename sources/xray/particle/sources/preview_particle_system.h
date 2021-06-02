////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PREVIEW_PARTICLE_SYSTEM_H_INCLUDED
#define PREVIEW_PARTICLE_SYSTEM_H_INCLUDED

#ifndef MASTER_GOLD

#include "particle_system.h"

namespace xray {
namespace particle {

class preview_particle_system_instance;

typedef fixed_string<64> guid_type;

struct guid_to_object_pair
{
	enum object_type
	{
		is_action  = 0,
		is_emitter,
		is_group,
		is_root,
	};
	
	guid_type	guid;
	pvoid		object;
	object_type	type;
	
	guid_to_object_pair*	 next;
}; // struct guid_to_object_pair

class preview_particle_system : public particle_system {
	typedef xray::intrusive_list<guid_to_object_pair,guid_to_object_pair*,&guid_to_object_pair::next> guids_list_type;

public:
	preview_particle_system				(particle_world& particle_world);
	virtual ~preview_particle_system	();
	
	void play_preview					(math::float4x4 const& transform, bool always_looping);
	void stop_preview					();
	
	void update_from_config				(configs::lua_config_value const& config_value);

public:
	void add_emitter					(u32 lod_index, pcstr emitter_id, pcstr group_id, xray::configs::lua_config_value const& emitter_config);
	void remove_emitter					(u32 lod_index, particle_emitter* emitter);
	void remove_group					(u32 lod_index, pcstr group_id);
	void remove_action					(u32 lod_index, pcstr action_id);
	bool is_particle_root_type			(pcstr entity_index);
	bool is_particle_group_type			(pcstr entity_index);
	bool is_particle_emitter_type		(pcstr entity_index);
	bool is_particle_action_type		(pcstr entity_index);

	bool is_object_type					(guid_to_object_pair::object_type obj_type, pcstr entity_index);

	void on_group_changed				(xray::configs::lua_config_value const& config, pcstr group_id);
	void on_action_changed				(pcstr action_id);
	particle_emitter* name_to_emitter	(u32 lod_index, pcstr emitter_id);
	particle_action* name_to_action		(pcstr action_id);

	void on_exchange_actions			(pcstr action_id0, pcstr action_id1);
	particle_emitter* find_actions_emitter(particle_action* action);

	preview_particle_system_instance*	get_preview_instance() const { return m_preview_instance; }

	virtual void clear_guid_list				();
	virtual void add_action_to_guid_list		(pcstr guid_str, particle_action* object);
	virtual void add_emitter_to_guid_list		(pcstr guid_str, particle_emitter* object);
	virtual void add_group_to_guid_list			(pcstr guid_str, particle_emitter* object);
	virtual void add_root_to_guid_list			(pcstr guid_str,  particle_system_lod* lod);
	virtual void exchange_emitters_in_guid_list (particle_emitter* from, particle_emitter* to);

	void remove_action_from_guid_list (pcstr id);
	void remove_emitter_from_guid_list(pcstr id);
	void remove_group_from_guid_list  (pcstr id);
	void remove_root_from_guid_list   (pcstr id);

	void add_object_to_guid_list				(guid_to_object_pair::object_type obj_type, pcstr guid_str, pvoid object);
	void remove_object_from_guid_list			(guid_to_object_pair::object_type obj_type, pcstr id);

private:
	friend class preview_particle_system_cooker;

protected:
	guids_list_type						m_guids_list;
	preview_particle_system_instance*	m_preview_instance;
	particle_world&						m_particle_world;
	
}; // class preview_particle_system

} // namespace particle
} // namespace xray

#else // #ifndef MASTER_GOLD
#	error do not include this header in MASTER_GOLD configurations!
#endif // #ifndef MASTER_GOLD

#endif // #ifndef PREVIEW_PARTICLE_SYSTEM_H_INCLUDED