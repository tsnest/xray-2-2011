////////////////////////////////////////////////////////////////////////////
//	Created		: 02.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_scene_cook.h"
#include "sound_scene.h"
#include "sound_world.h"

namespace xray {
namespace sound {

sound_scene_cook::sound_scene_cook	( sound_world& world ) :
	resources::unmanaged_cook	( xray::resources::sound_scene_class, reuse_false, use_current_thread_id, use_current_thread_id ),
	m_sound_world				( world ) 
{
}

mutable_buffer sound_scene_cook::allocate_resource	(	resources::query_result_for_cook& in_query,
														const_buffer raw_file_data,
														bool file_exist
													)
{
 	XRAY_UNREFERENCED_PARAMETERS	( &in_query, file_exist, &raw_file_data );
	return mutable_buffer			( ALLOC( u8, sizeof( sound_scene )), sizeof( sound_scene ));
}

void sound_scene_cook::deallocate_resource	( pvoid buffer )
{
	FREE							( buffer );
}

void sound_scene_cook::create_resource	(	resources::query_result_for_cook& in_out_query,
											const_buffer raw_file_data,
											mutable_buffer in_out_unmanaged_resource_buffer
											)
{
	XRAY_UNREFERENCED_PARAMETER			( raw_file_data );

	static u32 id						= 0;

	sound_scene* created_scene			= new	( in_out_unmanaged_resource_buffer.c_ptr( ) )
												sound_scene( m_sound_world.create_submix_voice( ), id++ );

//	m_sound_world.add_sound_scene		( created_scene );

	in_out_query.set_unmanaged_resource	( created_scene, resources::nocache_memory, sizeof ( sound_scene ) );
	in_out_query.finish_query			( result_success );
}

void sound_scene_cook::destroy_resource	( resources::unmanaged_resource* resource )
{
	sound_scene* scene					= static_cast_checked< sound_scene* >( resource );

//	m_sound_world.remove_sound_scene	( scene );
	m_sound_world.free_submix_voice		( scene->get_submix_voice( ) );

	scene->~sound_scene					( );
}


} // namespace sound
} // namespace xray
