////////////////////////////////////////////////////////////////////////////
//	Created		: 02.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "sound_player_cook.h"
#include <xray/ai/sound_collection_types.h>
#include <xray/ai/npc.h>
#include <xray/sound/world.h>
#include <xray/sound/sound_emitter.h>
#include "ai_sound_player.h"
#include "human_npc.h"

namespace stalker2 {

sound_player_cook::sound_player_cook( sound::sound_scene_ptr& scene, sound::world* world, resources::class_id_enum class_id ) :
	translate_query_cook			( class_id, reuse_false, use_current_thread_id ),
	m_scene							( scene ),
	m_world							( world ),
	m_dbg_input_world				( 0 )
{
}

sound_player_cook::sound_player_cook(	sound::sound_scene_ptr& scene,
										sound::world* sound_world,
										input::world* input_world,
										resources::class_id_enum class_id ) :
	translate_query_cook			( class_id, reuse_false, use_current_thread_id ),
	m_scene							( scene ),
	m_world							( sound_world ),
	m_dbg_input_world				( input_world )
{
}

void sound_player_cook::translate_query	( resources::query_result_for_cook& parent )
{
	resources::query_resource	(
		parent.get_requested_path(),
		resources::binary_config_class,
		boost::bind( &sound_player_cook::on_config_loaded, this, _1 ),
		g_allocator,
		0,
		&parent
	);
}

void sound_player_cook::delete_resource	( resources::resource_base* resource )
{
	resource->~resource_base			( );
	XRAY_FREE_IMPL						( g_allocator, resource );
}

void sound_player_cook::on_config_loaded				( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT										( data.is_successful(), "couldn't retrieve sound collection options" );
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_ptr config					= static_cast_resource_ptr<configs::binary_config_ptr const>( data[0].get_unmanaged_resource() );
	configs::binary_config_value const& sounds_value	= (*config)["sounds"];
	
	configs::binary_config_value::const_iterator it		= sounds_value.begin();
	configs::binary_config_value::const_iterator it_end	= sounds_value.end();
		
	buffer_vector< resources::request >	requests		( ALLOCA( sizeof( resources::request ) * sounds_value.size() ), sounds_value.size() );
	
//	resources::user_data_variant user_data;
//	user_data.set										( resources::wav_encoded_sound_interface_class ); 
//	buffer_vector< resources::user_data_variant const* > sound_interface_params ( ALLOCA( sizeof( resources::user_data_variant* ) * sounds_value.size() ), sounds_value.size() );

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& sound_value	= *it;
		resources::request								request;
		request.path									= sound_value["filename"];
		request.id										= resources::sound_collection_class;
		requests.push_back								( request );
		//sound_interface_params.push_back				( &user_data );
	}

	query_resources	(
		&requests.front(), 
		requests.size(), 
		boost::bind( &sound_player_cook::on_sounds_loaded, this, _1, config ), 
		g_allocator,
		0, //sound_interface_params.begin( ),
		parent
	);
}
	
void sound_player_cook::on_sounds_loaded	( resources::queries_result& data, configs::binary_config_ptr config  )	
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		R_ASSERT										( data.is_successful(), "couldn't load sounds" );
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_value const& sounds_value	= (*config)["sounds"];
	
	sound::world_user* world_user					= 0;
	switch ( get_class_id() )
	{
		case resources::sound_player_logic_class:
			world_user									= &m_world->get_logic_world_user();
			break;

#ifndef MASTER_GOLD
		case resources::sound_player_editor_class:
			world_user									= &m_world->get_editor_world_user();
			break;
#endif // #ifndef MASTER_GOLD

		default:
			NODEFAULT									( );
	}
	R_ASSERT											( world_user );


	human_npc const* npc								= 0;

	resources::query_result_for_cook* const brain_unit_query	= parent->get_parent_query	( );
	if ( brain_unit_query && brain_unit_query->get_class_id( ) == resources::brain_unit_class )
	{
		resources::user_data_variant* user_data			= brain_unit_query->user_data( );
		ai::brain_unit_cook_params params;
		if ( user_data && user_data->try_get( params ) )
		{
			npc											= static_cast_checked<human_npc const* const>( params.npc );
		}
	}

	u32 const ai_sound_player_buffer_size				= sizeof( ai_sound_player );
	u32 const sounds_size								= sizeof( ai_sound_player::sounds_collection_type ) * sounds_value.count;
	u8* sound_player_buffer								= static_cast<u8*>( XRAY_MALLOC_IMPL( g_allocator, ai_sound_player_buffer_size + sounds_size, "ai_sound_player" ) );

	ai_sound_player* player;
	if ( m_dbg_input_world )
		player			= new( sound_player_buffer )ai_sound_player( m_scene, sounds_value.count, *world_user, m_dbg_input_world, npc, npc );
	else
		player			= new( sound_player_buffer )ai_sound_player( m_scene, sounds_value.count, *world_user, npc, npc );

	ai_sound_player::sounds_collection_type* const begin = pointer_cast<ai_sound_player::sounds_collection_type*>( sound_player_buffer + ai_sound_player_buffer_size );
	ai_sound_player::sounds_collection_type* const end	= begin + sounds_value.count;

	configs::binary_config_value::const_iterator it		= sounds_value.begin( );
	configs::binary_config_value::const_iterator it_end	= sounds_value.end( );

	for ( ai_sound_player::sounds_collection_type* i = begin; ( it != it_end ) && ( i != end ); ++it, ++i )
	{
		configs::binary_config_value const& sound_value	= *it;
		u32 type										= sound_value["collection_type"];
		new( i )ai_sound_player::sounds_collection_type	(
			player,
			(ai::sound_collection_types)type,
			static_cast_resource_ptr< sound::sound_emitter_ptr >( data[std::distance( begin, i )].get_unmanaged_resource() ),
			sound_value["priority"]
		);
	}
	
	parent->set_unmanaged_resource						( player, resources::nocache_memory, sizeof( ai_sound_player ) + sounds_size );
	parent->finish_query								( result_success );
}

} // namespace stalker2
