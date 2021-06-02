////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_stream_cooker.h"
#include "sound_stream.h"
#include "sound_world.h"

namespace xray {
namespace sound {

sound_stream_cooker::sound_stream_cooker( ) 
:super(resources::sound_stream_class, reuse_true, use_user_thread_id)
{
}

sound_stream_cooker::~sound_stream_cooker( )
{
}

void sound_stream_cooker::translate_query( resources::query_result& parent )
{
	fs::path_string req_path;
	req_path.assignf( "%s%s%s", _converted_local_path, parent.get_requested_path(), _ogg_ext );
	resources::request request_array[] = {	
		{ req_path.c_str(), resources::ogg_raw_file },
//		{ "request_options", resources::config_lua_class },
	};
	query_resources	(
						request_array, 
						array_size(request_array), 
						boost::bind(&sound_stream_cooker::on_sub_resources_loaded, this, _1), 
						parent.get_user_allocator(), 
						NULL,
						0,
						&parent );
}

void sound_stream_cooker::on_sub_resources_loaded( resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	// [0] - raw ogg file
	// [1] - lua config - game options
	
	resources::managed_resource_ptr ogg_raw_file	= data[0].get_managed_resource();
//	configs::lua_config_ptr options	= static_cast_resource_ptr<configs::lua_config_ptr>(data[1].get_unmanaged_resource());
	
	sound_stream * created_resource			= MT_NEW(sound_stream)( ogg_raw_file );
//	created_resource->recalc_rms			( );

	resources::query_result_for_cook * parent = data.get_parent_query();
	
	parent->set_unmanaged_resource			( created_resource, resources::memory_type_non_cacheable_resource, sizeof(sound_stream) );
	parent->finish_query			( result_success );
}

void sound_stream_cooker::delete_resource( resources::unmanaged_resource* res)
{
	MT_DELETE		( res );
}


} // namespace sound
} // namespace xray
