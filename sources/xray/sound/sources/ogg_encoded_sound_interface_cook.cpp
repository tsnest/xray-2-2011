////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_encoded_sound_interface_cook.h"
#include "ogg_encoded_sound_interface.h"

namespace xray {
namespace sound {

ogg_encoded_sound_interface_cook::ogg_encoded_sound_interface_cook 	( ) :
	super( resources::ogg_encoded_sound_interface_class, reuse_true, use_resource_manager_thread_id )
{

}

ogg_encoded_sound_interface_cook::~ogg_encoded_sound_interface_cook 	( )
{

}

void ogg_encoded_sound_interface_cook::translate_query		( resources::query_result_for_cook& parent )
{
	fs_new::virtual_path_string req_path;
	req_path.assignf( "%s%s%s", _converted_local_path, parent.get_requested_path(), _ogg_ext );

	resources::class_id_enum type		= resources::ogg_raw_file;

	resources::request request_array[] = {	
		{ req_path.c_str( ), type }
	};
	query_resources	(
						request_array, 
						array_size(request_array), 
						boost::bind(&ogg_encoded_sound_interface_cook::on_sub_resources_loaded, this, _1), 
						parent.get_user_allocator(), 
						NULL,
						&parent );

}

void ogg_encoded_sound_interface_cook::delete_resource		( resources::resource_base* res )
{
	UNMANAGED_DELETE				( res );
}

void ogg_encoded_sound_interface_cook::on_sub_resources_loaded	( resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	resources::query_result_for_cook* const parent	= data.get_parent_query();

	resources::managed_resource_ptr ogg_raw_file	= data[0].get_managed_resource();

	ogg_encoded_sound_interface* encoded_sound;
	UNMANAGED_NEW( encoded_sound, ogg_encoded_sound_interface )( ogg_raw_file );

	if ( !encoded_sound )
	{
		parent->set_out_of_memory	( resources::unmanaged_memory, sizeof( ogg_encoded_sound_interface ) );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	parent->set_unmanaged_resource	( encoded_sound, resources::nocache_memory, sizeof( ogg_encoded_sound_interface ) );
	parent->finish_query			( result_success );
}

} // namespace sound
} // namespace xray
