////////////////////////////////////////////////////////////////////////////
//	Created		: 20.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_spl_cook.h"
#include <xray/sound/spl_utils.h>
#include <xray/sound/sound_spl.h>
#include <xray/resources_fs.h>

namespace xray {
namespace sound {

using namespace resources;

sound_spl_cook::sound_spl_cook			( ) 
:super	( sound_spl_class, reuse_true, use_resource_manager_thread_id )
{
}

sound_spl_cook::~sound_spl_cook			( )
{
}

void sound_spl_cook::translate_query	( query_result_for_cook& parent )
{
	fs_new::virtual_path_string	source_path	= "resources/sounds/single/";
	source_path.append						( parent.get_requested_path( ) );

	resources::query_resource(
		source_path.c_str( ),
		resources::binary_config_class,
		boost::bind(&sound_spl_cook::on_config_loaded, this, _1, &parent ),
		resources::unmanaged_allocator( ),
		0,
		&parent
		);
}

void sound_spl_cook::delete_resource	( resource_base* res )
{
	UNMANAGED_DELETE				( res );
}

void sound_spl_cook::on_config_loaded( queries_result& data, query_result_for_cook* parent )
{
	if( !data.is_successful() )
	{
	#ifndef MASTER_GOLD
		if(data[0].get_error_type()==resources::query_result_for_user::error_type_file_not_found)
		{

			fs_new::native_path_string spl_file_name_disk_path;
			if( !resources::convert_virtual_to_physical_path( &spl_file_name_disk_path, parent->get_requested_path(), resources::sources_mount ))
			{
				parent->finish_query		( result_error );
				return;
			}
			create_default_spl_config		( spl_file_name_disk_path.c_str(), resources::unmanaged_allocator() ); 
			parent->finish_query			( result_requery );
		}
	#endif // #ifndef MASTER_GOLD

		parent->finish_query				( data[0].get_error_type() );
		return;
	}

	configs::binary_config_ptr config		= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );

	sound_spl_ptr spl						= XRAY_NEW_IMPL( resources::unmanaged_allocator( ), sound_spl) ( );
	spl->load								( config->get_root( )["spl"] );
	parent->set_unmanaged_resource			( spl.c_ptr(), resources::nocache_memory, sizeof( sound_spl ));
	parent->finish_query					( result_success );
}


} // namespace sound
} // namespace xray