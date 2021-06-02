////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_world_cooker.h"
#include "particle_world.h"

#ifndef MASTER_GOLD
#	include "preview_particle_system_cooker.h"
#endif // #ifndef MASTER_GOLD

namespace xray {
namespace particle {

particle_world_cooker::particle_world_cooker():
	resources::unmanaged_cook(xray::resources::particle_world_class, reuse_false, use_resource_manager_thread_id, use_resource_manager_thread_id)
{}

mutable_buffer particle_world_cooker::allocate_resource(resources::query_result_for_cook& in_query, const_buffer raw_file_data, bool file_exist)
{
	XRAY_UNREFERENCED_PARAMETERS(&in_query, file_exist);
	
	u32 bytes_to_allocate = sizeof(particle_world) + raw_file_data.size();
	
	mutable_buffer		out_buffer(UNMANAGED_ALLOC(u8, bytes_to_allocate), bytes_to_allocate);
	if ( !out_buffer )
		in_query.set_out_of_memory	(resources::unmanaged_memory, bytes_to_allocate);

	return				out_buffer;
}

void particle_world_cooker::deallocate_resource(pvoid buffer)
{
	UNMANAGED_FREE(buffer);
}

void particle_world_cooker::create_resource(resources::query_result_for_cook& in_out_query, const_buffer raw_file_data, mutable_buffer in_out_unmanaged_resource_buffer)
{	
	XRAY_UNREFERENCED_PARAMETER(raw_file_data);

	engine* engine		= 0;
	bool const result	= in_out_query.user_data()->try_get( engine );
	R_ASSERT_U			( result );
	R_ASSERT(in_out_unmanaged_resource_buffer.c_ptr());
	particle_world* new_particle_world = new(in_out_unmanaged_resource_buffer.c_ptr()) particle_world( *engine );
	in_out_unmanaged_resource_buffer  += sizeof(particle_world);
	
	//new_particle_world->m_allocator.initialize(in_out_unmanaged_resource_buffer.c_ptr(),in_out_unmanaged_resource_buffer.size(),"particle_world");
	new_particle_world->m_allocator.initialize	(
			in_out_unmanaged_resource_buffer.c_ptr(),
			math::align_down< u64 >( in_out_unmanaged_resource_buffer.size(), sizeof( xray::particle::base_particle ) ),
			"particle_world"
		);
	
	in_out_query.set_unmanaged_resource(new_particle_world, resources::nocache_memory, sizeof(particle_world));
	in_out_query.finish_query(result_success);
}	

void particle_world_cooker::destroy_resource(resources::unmanaged_resource* resource)
{	
	particle_world* pworld = static_cast_checked<particle_world*>( resource );
	pworld->~particle_world();
}

xray::uninitialized_reference<particle_world_cooker>	s_particle_world_cooker_object;

void register_particles_cooker();
void register_particles_system_cooks();

void register_particle_world_cooker()
{
	static bool cooker_registered = false;
	if (cooker_registered) return;
	
	register_particles_cooker();

	register_particles_system_cooks();

#ifndef MASTER_GOLD
	register_preview_particle_system_cooker();
#endif // #ifndef MASTER_GOLD

	XRAY_CONSTRUCT_REFERENCE(s_particle_world_cooker_object, particle_world_cooker)();
	resources::register_cook(s_particle_world_cooker_object.c_ptr());
	
	cooker_registered = true;
}

void unregister_particle_world_cooker()
{
	static bool cooker_unregistered = false;
	if (cooker_unregistered) return;
	
	//unregister_particles_system_cooks();
	//unregister_particles_cooker();

#ifndef MASTER_GOLD
	//unregister_preview_particle_system_cooker();
#endif // #ifndef MASTER_GOLD

	resources::unregister_cook(resources::particle_world_class);
	XRAY_DESTROY_REFERENCE	  (s_particle_world_cooker_object);
	
	cooker_unregistered = true;
}

} // namespace particle
} // namespace xray