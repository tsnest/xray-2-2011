////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PREVIEW_PARTICLE_SYSTEM_INSTANCE_H_INCLUDED
#define PREVIEW_PARTICLE_SYSTEM_INSTANCE_H_INCLUDED

#ifndef MASTER_GOLD

#include "particle_system_instance_impl.h"

namespace xray {
namespace particle {

class particle_world;
struct particle_system_lod;

class preview_particle_system_instance: public particle_system_instance_impl
{
public:
			preview_particle_system_instance	( particle_world& particle_world );
	virtual ~preview_particle_system_instance	( );
			void updated_from_config			( particle_system_lod* lods, u32 num_lods );
	virtual bool is_finished					( );

private:
	typedef intrusive_list<
		particle_emitter_instance,
		particle_emitter_instance*,
		&particle_emitter_instance::m_next
	> particle_emitter_instances_type;

private:
	particle_emitter_instances_type				m_instances_to_delete;
}; // class preview_particle_system_instance

} // namespace particle
} // namespace xray

#else // #ifndef MASTER_GOLD
#	error do not include this header in MASTER_GOLD configurations!
#endif // #ifndef MASTER_GOLD

#endif // #ifndef PREVIEW_PARTICLE_SYSTEM_INSTANCE_H_INCLUDED