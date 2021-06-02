////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "configs_lua_config_cook.h"
#include <xray/resources_fs.h>

namespace xray {
namespace core {
namespace configs {

mutable_buffer   lua_config_cook::allocate_resource	(resources::query_result_for_cook &	in_query, 
													 const_buffer						raw_file_data, 
													 bool								file_exist)
{
	XRAY_UNREFERENCED_PARAMETERS			(& in_query, & raw_file_data, & file_exist);
	return									mutable_buffer (lua_allocate( sizeof(xray::configs::lua_config) ), 
															sizeof(xray::configs::lua_config));
}

void   lua_config_cook::destroy_resource	(resources::unmanaged_resource * resource)
{
	destroy_lua_config						(resource);
}

void   lua_config_cook::deallocate_resource	(pvoid buffer)
{
	lua_deallocate							(buffer);
}

void   lua_config_cook::create_resource (resources::query_result_for_cook &	in_out_query, 
				 						 const_buffer						raw_file_data,
				 						 mutable_buffer						in_out_unmanaged_resource_buffer)
{
	xray::memory::reader reader		((u8 const*)raw_file_data.c_ptr(), raw_file_data.size());

#if 1
	pstr path						= 0;
	STR_DUPLICATEA					( path, in_out_query.get_requested_path() );
	char const resources_string[]	= "resources/";
	if ( strings::starts_with(path, resources_string) )
		path						+= sizeof(resources_string) - 1;
#else // #if 0
	pcstr const path				= in_out_query.get_requested_path();
#endif // #if 0

	fs_new::native_path_string	disk_path;
	
	if ( !in_out_query.creation_data_from_user() )
	{
		if ( !resources::convert_virtual_to_physical_path(& disk_path, in_out_query.get_requested_path(), resources::sources_mount) )	
		{
			in_out_query.finish_query		(result_error);
			NOT_IMPLEMENTED(return);
		}
	}
	else
	{
		disk_path = path;
	}


	xray::configs::lua_config *	out_config	=	create_lua_config_inplace( in_out_unmanaged_resource_buffer, disk_path.c_str(), reader);
	
	if ( !out_config ) {
		in_out_query.set_error_type	( resources::query_result_for_user::error_type_invalid_file_content );
		in_out_query.finish_query	( result_error );
		return;
	}

	#pragma message(XRAY_TODO("Keep in mind: resource is not really allocated from unmanaged arena, but is said to be. Fix cooker to use unmanaged allocator"))
	in_out_query.set_unmanaged_resource	(out_config, 
										 resources::unmanaged_memory, 
										 in_out_unmanaged_resource_buffer.size());

	in_out_query.finish_query		(result_success);
}

} // namespace configs
} // namespace core
} // namespace xray
