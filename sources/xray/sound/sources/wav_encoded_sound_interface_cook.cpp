////////////////////////////////////////////////////////////////////////////
//	Created		: 13.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "wav_encoded_sound_interface_cook.h"
#include "wav_encoded_sound_interface.h"

namespace xray {
namespace sound {

//pcstr _converted_local_path		= "resources.converted/sounds/";

using namespace resources;

wav_encoded_sound_interface_cook::wav_encoded_sound_interface_cook 	( ) :
	super( wav_encoded_sound_interface_class, reuse_true, use_resource_manager_thread_id )
{
}

wav_encoded_sound_interface_cook::~wav_encoded_sound_interface_cook ( )
{

}

void wav_encoded_sound_interface_cook::translate_query	( query_result_for_cook& parent )
{
	fs_new::virtual_path_string req_path;
	req_path.assignf( "%s%s%s", "resources/sounds/single/", parent.get_requested_path(), ".wav" );

	resources::query_resource(
		req_path.c_str( ),
		raw_data_class,
		boost::bind(&wav_encoded_sound_interface_cook::on_file_loaded, this, _1 ),
		parent.get_user_allocator( ),
		0,
		&parent
	);
}

void wav_encoded_sound_interface_cook::delete_resource	( resource_base* res )
{
	UNMANAGED_DELETE		( res );
}

void wav_encoded_sound_interface_cook::on_file_loaded	( queries_result& data )
{
	query_result_for_cook* const parent	= data.get_parent_query( );

	if ( !data.is_successful( ) )
	{
		parent->finish_query					( result_error );
		return;
	}

	resources::managed_resource_ptr wav_raw_file	= data[0].get_managed_resource( );

	wav_encoded_sound_interface* encoded_sound;
	UNMANAGED_NEW( encoded_sound, wav_encoded_sound_interface )( wav_raw_file );

	if ( !encoded_sound )
	{
		parent->set_out_of_memory	( unmanaged_memory, sizeof( wav_encoded_sound_interface ) );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	parent->set_unmanaged_resource	( encoded_sound, resources::nocache_memory, sizeof( wav_encoded_sound_interface ) );
	parent->finish_query			( result_success );
}

} // namespace sound
} // namespace xray
