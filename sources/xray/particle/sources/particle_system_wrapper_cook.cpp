////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_system_wrapper_cook.h"

#include "help_functions.h"
#include "particle_entity_types.h"
#include "particle_actions.h"
#include "particle_system.h"

#include "help_functions.h"
#include "particle_emitter.h"
#include "particle_entity_types.h"
#include "particle_system_cook.h"
#include "particle_system_wrapper_cook.h"

namespace xray {
namespace particle {

using namespace fs_new;
using namespace resources;

particle_system_wrapper_cook::particle_system_wrapper_cook() 
	: super(particle_system_class, reuse_true, use_resource_manager_thread_id)
{
}

void particle_system_wrapper_cook::translate_query( query_result_for_cook& parent )
{
	fs_new::virtual_path_string				source_path;
	source_path.assignf						("resources/particles/%s.particle", parent.get_requested_path());

	fs_new::virtual_path_string				converted_path;
	converted_path.assignf					("resources.converted/particles/%s.binary_particle", parent.get_requested_path());

	request	requests[]	=	
	{
		{ source_path.c_str(),		fs_iterator_class },
		{ converted_path.c_str(),	fs_iterator_class },
	};
	
	query_resources(
		requests, 
		boost::bind( & particle_system_wrapper_cook::on_fs_iterators_ready, this, _1),
		helper_allocator(),
		0,
		& parent
	);
}

void   particle_system_wrapper_cook::on_fs_iterators_ready	(queries_result & result)
{
	vfs::vfs_iterator const source_it		=	result[0].get_result_iterator();
	vfs::vfs_iterator const converted_it	=	result[1].get_result_iterator();

	query_result_for_cook * const parent	=	result.get_parent_query();

	fs_new::physical_path_info const source_info	=	get_physical_path_info(source_it);
	fs_new::physical_path_info const converted_info	=	get_physical_path_info(converted_it);

	if ( !converted_it ||
		  (source_info.last_time_of_write() > converted_info.last_time_of_write()) )
	{
		request_convertion						(result[0].get_requested_path(), parent);
		return;
	}

	fs_new::virtual_path_string const converted_path	=	result[1].get_requested_path();

	query_resource							(
		converted_path.c_str(), 
		particle_system_binary_class, 
		boost::bind(& particle_system_wrapper_cook::on_resource_ready, this, _1),
		helper_allocator(),
		0,
		parent
	);
}

void   particle_system_wrapper_cook::on_resource_ready	(queries_result & result)
{
	query_result_for_cook * const parent	=	result.get_parent_query();
	unmanaged_resource_ptr resource		=	result[0].get_unmanaged_resource();

	if ( resource )
		parent->set_unmanaged_resource		(resource, resource->memory_usage());

	parent->finish_query					(result[0].get_error_type(), result.assert_on_fail());
}

void particle_system_wrapper_cook::request_convertion	(pcstr resource_path, query_result_for_cook * parent)
{
	query_resource	( 
		resource_path, 
		binary_config_class, 
		boost::bind( &particle_system_wrapper_cook::on_config_loaded, this, _1), 
		helper_allocator(),
		NULL, 
		parent 
	);
}

void particle_system_wrapper_cook::on_config_loaded	(queries_result & result)
{
	query_result_for_cook * const	parent	=	result.get_parent_query();
	
	if ( !result.is_successful() )
	{
		parent->finish_query				(result_error);
		return;
	}

	fs_new::virtual_path_string				converted_path;
	converted_path.assignf					("resources.converted/particles/%s.binary_particle", parent->get_requested_path());
	fs_new::native_path_string				physical_path;
	if ( !convert_virtual_to_physical_path(& physical_path, converted_path, sources_mount) )
	{
		parent->finish_query				(result_error);
		return;
	}

	configs::binary_config_ptr config_ptr	=	static_cast_resource_ptr<configs::binary_config_ptr>(result[0].get_unmanaged_resource());
	configs::binary_config_value const& config	= config_ptr->get_root();

	particle_system							temp_resource;
	temp_resource.set_defaults				();
	temp_resource.load_from_config			(config, false);
	
	mutable_buffer							temp_buffer;
	u32 data_size 						=	temp_resource.save_binary(temp_buffer, true);

	u32 resource_size   				=	sizeof(particle_system) + data_size;
	mutable_buffer resource_buffer			(UNMANAGED_ALLOC(u8, resource_size), resource_size);
	if ( !resource_buffer )
	{
		parent->set_out_of_memory			(unmanaged_memory, resource_size);
		parent->finish_query				(result_out_of_memory);
		return;
	}

	memory::zero							(resource_buffer.c_ptr(), resource_buffer.size());
	
	mutable_buffer data_buffer			=	resource_buffer + (u32)sizeof(particle_system);
	
	{
		mutable_buffer temp_buffer		=	data_buffer;
		temp_resource.save_binary			(temp_buffer, false);
	}

	save_generated_data * const save_info	=	save_generated_data::create(data_buffer, false, physical_path, converted_path);
	parent->save_generated_resource			(save_info);

	particle_system * const out_resource	=	new (resource_buffer.c_ptr()) particle_system;
	out_resource->load_binary				(data_buffer);
	
	parent->set_unmanaged_resource			(out_resource, unmanaged_memory, resource_size);
	parent->finish_query					(result_success);
}

void particle_system_wrapper_cook::delete_resource	(resource_base * resource)
{
	UNMANAGED_DELETE						(resource);
}

uninitialized_reference<particle_system_wrapper_cook>	s_particle_system_wrapper_cook;
uninitialized_reference<particle_system_cook>			s_particle_system_cook;

void register_particles_system_cooks()
{
	static bool particles_system_cooker_registered	=	false;
	
	if ( particles_system_cooker_registered ) 
		return;
	
	XRAY_CONSTRUCT_REFERENCE				( s_particle_system_wrapper_cook, particle_system_wrapper_cook ) ( );
	XRAY_CONSTRUCT_REFERENCE				( s_particle_system_cook, particle_system_cook ) ( );

	register_cook							( s_particle_system_wrapper_cook.c_ptr() );
	register_cook							( s_particle_system_cook.c_ptr() );
	
	particles_system_cooker_registered	=	true;
}

void unregister_particles_system_cooks()
{
	static bool particles_system_cooker_unregistered	=	false;
	
	if ( particles_system_cooker_unregistered ) 
		return;
	
	unregister_cook							( particle_system_binary_class );
	unregister_cook							( particle_system_class );

	XRAY_DESTROY_REFERENCE					( s_particle_system_cook );
	XRAY_DESTROY_REFERENCE					( s_particle_system_wrapper_cook );
	
	particles_system_cooker_unregistered	=	true;
}

} // namespace particle
} // namespace xray
