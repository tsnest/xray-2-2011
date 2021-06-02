////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef MASTER_GOLD

#include "preview_particle_system_cooker.h"
#include "preview_particle_system.h"

namespace xray {
namespace particle {

preview_particle_system_cooker::preview_particle_system_cooker() 
	: super(resources::preview_particle_system_class, reuse_true, use_any_thread_id)
{
}

void preview_particle_system_cooker::translate_query( resources::query_result_for_cook& parent )
{
	// remove "_preview"
	fs_new::virtual_path_string path = parent.get_requested_path();
	path.set_length( path.rfind('_') );
	
	fixed_string<260> new_name	= "resources/particles/";
	new_name					+= path.c_str();
	new_name					+= ".particle";
	
	query_resource( new_name.c_str(), 
		resources::lua_config_class, 
		boost::bind( &preview_particle_system_cooker::on_lua_config_loaded, this, _1), 
		parent.get_user_allocator(),
		NULL, 
		&parent 
		);
}

void preview_particle_system_cooker::on_lua_config_loaded(resources::queries_result& result)
{
	(void)&result;
#if 0
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();
	
	if (!result.is_successful())
	{
		parent->finish_query(result_error);
		return;
	}
	
	configs::lua_config_ptr config_ptr			= static_cast_checked<configs::lua_config *>( result[0].get_unmanaged_resource().c_ptr() );
	configs::lua_config_valueconst & config	= config_ptr->get_root();
	preview_particle_system* created_resource	= MT_NEW(preview_particle_system);
	
	created_resource->load_from_config			( config, true );
	created_resource->pin();

	parent->set_unmanaged_resource( created_resource, resources::nocache_memory, sizeof(preview_particle_system) );
	parent->finish_query( result_success );
#endif // #if 0
}

void preview_particle_system_cooker::delete_resource(resources::resource_base* res)
{
	preview_particle_system* ps = static_cast_checked<preview_particle_system*>(res);
	MT_DELETE(ps);
}


} // namespace particle
} // namespace xray

#endif // #ifndef MASTER_GOLD
