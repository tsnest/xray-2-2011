////////////////////////////////////////////////////////////////////////////
//	Created		: 07.06.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_cooker.h"
#include "object_solid_visual.h"
#include "object_light.h"
#include "object_track.h"
#include "object_environment.h"
#include "object_sky.h"
#include "object_decal.h"
#include "object_collision_geometry.h"
#include "object_proximity_trigger.h"
#include "object_scene.h"
#include "object_values_storage.h"
#include "object_composite.h"
#include "object_volumetric_sound.h"
#include "object_wire.h"
#include "game_camera.h"
#include "game.h" 

namespace stalker2 {

object_cooker::object_cooker( game& game)
:super	( resources::game_object_class, reuse_true, use_current_thread_id, use_current_thread_id ),
m_game	( game )
{}

mutable_buffer object_cooker::allocate_resource( resources::query_result_for_cook&	in_query, 
												 const_buffer						raw_file_data, 
												 bool								file_exist )
{
	XRAY_UNREFERENCED_PARAMETERS	( file_exist, &in_query );
	configs::binary_config_value* t_object = (configs::binary_config_value*)(raw_file_data.c_ptr());
	pcstr type						= (*t_object)["game_object_type"];

	u32 object_size = 0;
	if( strings::equal("solid_visual", type))
	{
		object_size = sizeof(object_solid_visual);
	}else 
	if( strings::equal("dynamic_visual", type))
	{
		object_size = sizeof(object_dynamic_visual);
	}else 
	if( strings::equal("skeleton_visual", type))
	{
		object_size = sizeof(object_skeleton_visual);
	}else 
	if( strings::equal("light", type))
	{
		object_size = sizeof(object_light);
	}else
	if( strings::equal("environment", type))
	{
		object_size = sizeof(object_environment);
	}else
	if( strings::equal("sky", type))
	{
		object_size = sizeof(object_sky);
	}else
	if( strings::equal("decal", type))
	{
		object_size = sizeof(object_decal);
	}else
	if( strings::equal("particle", type))
	{
		object_size = sizeof(object_particle_visual);
	}else
	if( strings::equal("speedtree", type))
	{
		object_size = sizeof(object_speedtree_visual);
	}else
	if( strings::equal("track", type))
	{
		object_size = sizeof(object_track);
	}else
	if( strings::equal("camera", type))
	{
		object_size = sizeof(game_camera);
	}else
	if( strings::equal("timer", type))
	{
		object_size = sizeof(object_timer);
	}else
	if( strings::equal("collision_geometry", type))
	{
		object_size = sizeof(object_collision_geometry);
	}else
	if( strings::equal("proximity_trigger", type))
	{
		object_size = sizeof(object_proximity_trigger);
	}else
	if( strings::equal("values_storage", type))
	{
		object_size = sizeof(object_values_storage);
	}else
	if( strings::equal("composite", type))
	{
		object_size = sizeof(object_composite);
	}else
	if ( strings::equal("volumetric_sound", type))
	{
		object_size = sizeof(object_volumetric_sound);
	}else
	if( strings::equal("wire_set", type))
	{
		object_size = sizeof(object_wire);
	}else
	{
		NOT_IMPLEMENTED				( );
	}
	

	pvoid data				= MALLOC( object_size, type );
	mutable_buffer	result	( data, object_size );
	return result;

}

void object_cooker::deallocate_resource( pvoid buffer )
{
	FREE		( buffer );
}

void object_cooker::create_resource( resources::query_result_for_cook& in_out_query, 
							 	   const_buffer raw_file_data, 
								   mutable_buffer in_out_unmanaged_resource_buffer )
{
	configs::binary_config_value* t_object	= (configs::binary_config_value*)(raw_file_data.c_ptr());
	pcstr type								= (*t_object)["game_object_type"];

	game_object_*	resource				= NULL;

	if( strings::equal("solid_visual", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_solid_visual( m_game.get_game_world() );
	}else 
	if( strings::equal("dynamic_visual", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_dynamic_visual( m_game.get_game_world() );
	}else 
	if( strings::equal("skeleton_visual", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_skeleton_visual( m_game.get_game_world() );
	}else 
	if( strings::equal("environment", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_environment( m_game.get_game_world() );
	}else 
	if( strings::equal("sky", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_sky( m_game.get_game_world() );
	}else 
	if( strings::equal("decal", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_decal( m_game.get_game_world() );
	}else 
	if( strings::equal("light", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_light( m_game.get_game_world() );
	}else
	if( strings::equal("particle", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_particle_visual( m_game.get_game_world() );
	}else
	if( strings::equal("speedtree", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_speedtree_visual( m_game.get_game_world() );
	}else
	if( strings::equal("track", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_track( m_game.get_game_world() );
	}else
	if( strings::equal("camera", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) game_camera( m_game.get_game_world() );
	}else
	if( strings::equal("timer", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_timer( m_game.get_game_world() );
	}else
	if( strings::equal("collision_geometry", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_collision_geometry( m_game.get_game_world() );
	}else
	if( strings::equal("proximity_trigger", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_proximity_trigger( m_game.get_game_world() );
	}else
	if( strings::equal("values_storage", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_values_storage( m_game.get_game_world() );
	}else	
	if( strings::equal("composite", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_composite( m_game.get_game_world() );
	}else
	if( strings::equal("volumetric_sound", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_volumetric_sound( m_game.get_game_world() );
	}else	
	if( strings::equal("wire_set", type))
	{
		resource = new (in_out_unmanaged_resource_buffer.c_ptr()) object_wire( m_game.get_game_world() );
	}else	
	{
		NOT_IMPLEMENTED				( );
	}

	resource->load(	*t_object );

	in_out_query.set_unmanaged_resource( resource, resources::nocache_memory, in_out_unmanaged_resource_buffer.size() );
	
	in_out_query.finish_query		( result_success );
}

void object_cooker::destroy_resource( resources::unmanaged_resource* resource )
{
	resource->~unmanaged_resource();
}

object_scene_cooker::object_scene_cooker( game& game )
:super	( resources::game_object_scene_class, reuse_true, use_current_thread_id, use_current_thread_id ),
m_game	( game )
{}

void object_scene_cooker::translate_query( resources::query_result_for_cook& parent )
{
	
	configs::binary_config_value* t_object = (configs::binary_config_value*)(parent.creation_data_from_user().c_ptr());

	configs::binary_config::const_iterator it	= (*t_object)["jobs"].begin();
	configs::binary_config::const_iterator it_e = (*t_object)["jobs"].end();
	u32 max_count = (*t_object)["jobs"].size();

	resources::request* requests = (resources::request*)ALLOCA(max_count* sizeof(resources::request));

	int count =0;
	for(; it!=it_e; ++it)
	{
		pcstr resource_name		= (*it)["job_resource"];
		if(strings::equal(resource_name, "camera_director") || strings::equal(resource_name, "timing") )
			continue;

		resources::class_id_enum clsid = resources::unknown_data_class;

		if ( strings::equal( (*it)["job_type"] , "npc" ))
			clsid = resources::human_npc_class;
		else
			clsid = resources::game_object_class;

		requests[count].id		= clsid;
		requests[count].path	= resource_name;
		++count;
	}

	if ( count == 0 )
	{
		on_scene_creation( &parent );
		return;
	}

	resources::query_resources( requests, 
								count, 
								boost::bind( &object_scene_cooker::on_subresources_loaded, this, _1, &parent ),
								parent.get_user_allocator(),
								0,
								& parent );
}

void object_scene_cooker::on_subresources_loaded( resources::queries_result& data, resources::query_result_for_cook* parent )
{
	R_ASSERT_U			( data.is_successful() );

	on_scene_creation	( parent );
}

void object_scene_cooker::on_scene_creation	( resources::query_result_for_cook* parent )
{
	configs::binary_config_value* t_object	= (configs::binary_config_value*)(parent->creation_data_from_user().c_ptr());

	object_scene* resource					= NEW(object_scene)(m_game.get_game_world());
	resource->load							( *t_object );
	parent->set_unmanaged_resource			( resource, resources::nocache_memory, sizeof(object_scene) );
	parent->finish_query					( result_success );
}

void object_scene_cooker::delete_resource( resources::resource_base* resource )
{
	DELETE ( resource );
}


} // namespace stalker2
