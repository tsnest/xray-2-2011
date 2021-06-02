////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "spl_cook.h"
#include <xray/memory_stream.h>
#include <xray/sound/spl_utils.h>
#include <xray/resources_fs.h>

namespace xray {
namespace sound {

using namespace resources;

//static pcstr resources_converted_string	=	"resources.converted/sounds/";
static pcstr resources					=	"resources/sounds/single/";

spl_cook::spl_cook	( ) :
	super	(
		spl_class,
		reuse_true,
		use_resource_manager_thread_id
	)
{
}

spl_cook::~spl_cook	( )
{
}

void spl_cook::translate_query	( query_result_for_cook & parent )
{
	fs_new::virtual_path_string	source_path		=	resources;
	source_path.append					( parent.get_requested_path( ) );

#ifndef MASTER_GOLD

	fs_new::physical_path_info const spl_file_info	=	resources::get_physical_path_info(source_path, resources::sources_mount);
															
	if ( !spl_file_info.is_file() )
	{

		// creating default spl confug
		fs_new::native_path_string spl_file_name_disk_path;
		if ( !resources::convert_virtual_to_physical_path( &spl_file_name_disk_path, source_path, resources::sources_mount) )
		{
			parent.finish_query( result_error );
			return;
		}

		//configs::lua_config_ptr source_config_ptr	= create_default_spl_config	( spl_file_name_disk_path.c_str() );
		create_default_spl_config	( spl_file_name_disk_path.c_str(), resources::unmanaged_allocator( ) );
	}
#endif // #ifndef MASTER_GOLD

	resources::query_resource(
		source_path.c_str( ),
		resources::binary_config_class,
		boost::bind(&spl_cook::on_lua_config_loaded, this, _1 ),
		resources::unmanaged_allocator( ),
		0,
		&parent
		);
}



void spl_cook::on_lua_config_loaded( resources::queries_result& data )
{
	resources::query_result_for_cook* parent_query	= data.get_parent_query();
	if( !data.is_successful() )
	{
		parent_query->finish_query					( result_error );
		return;
	}

	configs::binary_config_ptr resource_config_ptr	= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );

	parent_query->set_unmanaged_resource			(resource_config_ptr.c_ptr(), resources::nocache_memory, sizeof( configs::binary_config ));
	parent_query->finish_query						(result_success);
}

void spl_cook::delete_resource	( resource_base* res )
{
	UNMANAGED_DELETE				( res );
}

} // namespace sound
} // namespace xray