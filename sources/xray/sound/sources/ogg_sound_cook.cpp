////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_sound_cook.h"
#include "sound_world.h"
#include <xray/sound/sound_rms.h>

namespace xray {
namespace sound {

ogg_sound_cook::ogg_sound_cook( ) :
		super(resources::ogg_sound_class, reuse_false, use_resource_manager_thread_id)
{

}

ogg_sound_cook::~ogg_sound_cook( )
{
}

void ogg_sound_cook::translate_query( resources::query_result_for_cook& parent )
{
	
	fs_new::virtual_path_string req_path;

	req_path.assignf( "%s%s%s", _converted_local_path, parent.get_requested_path(), _ogg_ext );
	resources::request request_array[] = {	
		{ req_path.c_str(), resources::ogg_file_contents_class },
		{ req_path.c_str(), resources::sound_rms_class }
	};

	query_resources			(
		request_array, 
		array_size(request_array),
		boost::bind( &ogg_sound_cook::on_sub_resources_loaded, this, _1), 
		resources::unmanaged_allocator(),
		NULL,
		&parent 
	);

}

void ogg_sound_cook::on_sub_resources_loaded( resources::queries_result& data )
{
	R_ASSERT				( data.is_successful() );

	resources::query_result_for_cook* const parent	= data.get_parent_query();

	ogg_file_contents_ptr file_contents	= static_cast_resource_ptr<ogg_file_contents_ptr>(data[0].get_unmanaged_resource());
	sound_rms_ptr rms_ptr				= static_cast_resource_ptr<sound_rms_ptr>(data[1].get_managed_resource());
	ogg_sound* new_sound;
	UNMANAGED_NEW( new_sound, ogg_sound )( file_contents, rms_ptr );

	if ( !new_sound )
	{
		parent->set_out_of_memory	( resources::unmanaged_memory, sizeof( ogg_sound ) );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	parent->set_unmanaged_resource	( new_sound, resources::nocache_memory, sizeof( ogg_sound ) );
	parent->finish_query			( result_success );
}

void ogg_sound_cook::delete_resource( resources::resource_base* res)
{
	UNMANAGED_DELETE				( res );
}


} // namespace sound
} // namespace xray
