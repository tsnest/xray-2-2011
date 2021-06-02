////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_system_instance_cook.h"
#include "particle_system_instance.h"
#include "help_functions.h"
#include "particle_system.h"
#include "particle_emitter.h"
#include "particle_world.h"
#include "particle_system_instance_impl.h"

namespace xray {
namespace particle {

particle_system_instance_cook::particle_system_instance_cook() 
	: super(resources::particle_system_instance_class, reuse_false, use_current_thread_id)
{
}

void particle_system_instance_cook::translate_query( resources::query_result_for_cook& parent )
{
	particle::world* p_world = 0;
	
	if( parent.user_data() )
		parent.user_data()->try_get(p_world);
	
	ASSERT_CMP(p_world, !=, 0);
	
	query_resource(
		parent.get_requested_path(),
		resources::particle_system_class,
		boost::bind( &particle_system_instance_cook::on_sub_resources_loaded, this, _1, p_world),
		&::xray::memory::g_mt_allocator,
		NULL,
		&parent
	);
}

void particle_system_instance_cook::on_sub_resources_loaded( resources::queries_result& data, particle::world* p_world )
{
	R_ASSERT_U(data.is_successful());
	
	particle::particle_world& part_world = *(particle::particle_world*)p_world;
	
	particle_system* part_system = static_cast_checked<particle_system*>( data[0].get_unmanaged_resource().c_ptr() );
	
	particle_system_instance_impl* created_resource = MT_NEW(particle_system_instance_impl)(*(particle::particle_world*)p_world);
	
	for (u32 lod_index=0; lod_index<part_system->m_num_lods; lod_index++)
	{
		created_resource->set_template(lod_index, part_system);
		
		for (u32 emitter_index=0; emitter_index<part_system->m_lods[lod_index].m_num_emitters; emitter_index++)
		{
			particle_emitter& emitter = part_system->m_lods[lod_index].m_emitters_array[emitter_index];
			
			if (emitter.m_event)
				continue;
			
			particle_emitter_instance* instance = part_world.create_emitter_instance( emitter, false );
			created_resource->add_emitter_instance(lod_index, instance);
		}
	}
	
	particle_system_instance_ptr created_res	= created_resource;
	
	resources::query_result_for_cook* parent	= data.get_parent_query();
	parent->set_unmanaged_resource				( created_res, resources::nocache_memory, sizeof(particle_system_instance_impl) );
	parent->finish_query						( result_success );
}

void particle_system_instance_cook::delete_resource(resources::resource_base* res)
{
	particle_system_instance* ps_instance = static_cast_checked<particle_system_instance*>(res);
	MT_DELETE(ps_instance);
}

xray::uninitialized_reference<particle_system_instance_cook>	s_particle_cook_object;

void register_particles_cooker()
{
	static bool particles_cooker_registered = false;
	if (particles_cooker_registered) return;

	XRAY_CONSTRUCT_REFERENCE	( s_particle_cook_object, particle_system_instance_cook ) ();
	resources::register_cook	( s_particle_cook_object.c_ptr() );

	particles_cooker_registered = true;
}

void unregister_particles_cooker()
{
	static bool particles_cooker_unregistered = false;
	if (particles_cooker_unregistered) return;

	resources::unregister_cook	( resources::particle_system_instance_class );
	XRAY_DESTROY_REFERENCE		( s_particle_cook_object );

	particles_cooker_unregistered = true;
}

} // namespace particle
} // namespace xray
