////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_cooker.h"
#include "sound_world.h"
#include "sound_stream.h"

namespace xray {
namespace sound {

sound_cooker::sound_cooker() 
: super(resources::sound_class, reuse_false, use_user_thread_id)
{
}

void sound_cooker::translate_query( resources::query_result& parent )
{
	pcstr req_path = parent.get_requested_path();

	query_resource	(
						req_path, 
						resources::sound_stream_class,
						boost::bind( &sound_cooker::on_sub_resources_loaded, this, _1), 
						parent.get_user_allocator( ), 
						NULL,
						0,
						&parent 
					);
}

void sound_cooker::on_sub_resources_loaded( resources::queries_result& data )
{
	R_ASSERT									( data.is_successful() );
	
	sound_stream_ptr stream						= static_cast_resource_ptr<sound_stream_ptr>(data[0].get_unmanaged_resource());

	sound_object_impl* created_resource			= MT_NEW(sound_object_impl)( stream );
	created_resource->set_thread_allocator		( data.get_user_allocator( ) );

	resources::query_result_for_cook* parent	= data.get_parent_query();
	parent->set_unmanaged_resource				( created_resource, resources::memory_type_non_cacheable_resource, sizeof(sound_object_impl) );
	parent->finish_query				( result_success );
}

void sound_cooker::delete_resource( resources::unmanaged_resource* res)
{
	MT_DELETE		( res );
}

} // namespace sound
} // namespace xray
