////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "preview_particle_system_instance.h"
#include "particle_world.h"
#include "particle_system.h" // because of particle_system_lod only!
#include "particle_emitter.h"

namespace xray {
namespace particle {

preview_particle_system_instance::preview_particle_system_instance	( particle_world& particle_world ):
	particle_system_instance_impl	( particle_world )
{
}

struct delete_predicate {
	inline	void	operator ( )									( particle_emitter_instance* instance ) const
	{
		MT_DELETE	( instance );
	}
}; // struct delete_predicate

preview_particle_system_instance::~preview_particle_system_instance	( )
{
	m_instances_to_delete.for_each	( delete_predicate() );
	m_instances_to_delete.clear		( );
}

bool preview_particle_system_instance::is_finished					( )
{
	// TODO: move m_always_looping to preview_particle_system_instance class
	particle_system_instance_impl::is_finished();
	return false;
}

void preview_particle_system_instance::updated_from_config			( particle_system_lod* lods, u32 num_lods )
{
	// Remove old instances. For all lods?
	for (u32 lod_index=0; lod_index<m_num_lods; lod_index++)
	{
		lod_entry& lod = m_lods[lod_index];
		particle_emitter_instance* inst = lod.m_emitter_instance_list.front();
		while(inst)
		{
			//particle_emitter_instance* to_delete = inst;
			inst = inst->m_next;
			//m_instances_to_delete.push_back(to_delete);
			//MT_DELETE(to_delete);
		}
		lod.m_emitter_instance_list.clear();
	}
	
	//m_particle_world.remove_all_child_played_particle_system_instances_if_ticked();
	
	// Create new instances. For all lods?
	m_num_lods = num_lods;
	for (u32 lod_index=0; lod_index<num_lods; lod_index++)
	{
		set_template(lod_index, &*lods[lod_index].m_parent);
		for (u32 i=0; i<lods[lod_index].m_num_emitters; i++)
		{
			particle_emitter* emitter = &lods[lod_index].m_emitters_array[i];
			
			if (emitter->m_event || !emitter->get_enable())
				continue;
			
			particle_emitter_instance* instance = m_particle_world.create_emitter_instance( *emitter );
			add_emitter_instance(lod_index, instance);
		}
	}
}

} // namespace particle
} // namespace xray
