////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "sound_collection_cook.h"
#include "sound_collection.h"
#include "sound_world.h"

namespace xray {
namespace sound {

pcstr collection_extention								= ".sound_collection";
struct sound_collection_cook_user_data
{
	configs::binary_config_value const*					val;
	configs::binary_config_ptr							cfg_ptr;
};

sound_collection_cook::sound_collection_cook			( sound_world& world ) :
	super												(
		resources::sound_collection_class,
		reuse_true,
		use_resource_manager_thread_id
	),
	m_world												( world )
{
}

sound_collection_cook::~sound_collection_cook			( )
{
}

void sound_collection_cook::translate_query				( resources::query_result_for_cook& parent )
{
	resources::user_data_variant* ud					= parent.user_data();
	if( ud )
	{
		sound_collection_cook_user_data					data;
		ud->try_get										( data );
		parent.clear_user_data							( );
		request_items									( data.cfg_ptr, *data.val, &parent );
	}
	else
	{
		fs_new::virtual_path_string config_path;
		config_path.assignf("%s%s%s", "resources/sounds/collections/", parent.get_requested_path(), collection_extention);
		query_resource								(
			config_path.c_str(), 
			resources::binary_config_class,
			boost::bind( &sound_collection_cook::collection_config_loaded, this, _1 ),
			resources::unmanaged_allocator(),
			0,
			&parent 
		);
	}
}

void sound_collection_cook::collection_config_loaded ( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query();
	if ( !data.is_successful() )
	{
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_ptr config_ptr				= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );
	
	CURE_ASSERT	( config_ptr->get_root().value_exists( "collection" ), { parent->finish_query( result_error ); return; } );

	configs::binary_config_value const& collection		= config_ptr->get_root()["collection"];
	 
	request_items										( config_ptr, collection, parent );
}

void sound_collection_cook::request_items				( configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection, resources::query_result_for_cook* const parent )
{
	if( !( collection.value_exists( "sound_items" ) ) )
	{
		sound_collection* new_collection				= create_collection(collection);
		if ( !new_collection )
		{
			parent->set_out_of_memory					(
				resources::unmanaged_memory, sizeof( sound_collection )
			);
			parent->finish_query						( result_out_of_memory );
			return;
		}

		parent->set_unmanaged_resource					( new_collection, resources::nocache_memory, sizeof( sound_collection ) );
		parent->finish_query							( result_success );
		return;
	}

	configs::binary_config_value const& sounds			= collection["sound_items"];
	configs::binary_config_value::const_iterator it		= sounds.begin( );
	u32 requests_count									= sounds.size( );

	buffer_vector< resources::request > requests						( ALLOCA( sizeof( resources::request ) * requests_count ), requests_count );
	buffer_vector< resources::user_data_variant > usr_data				( ALLOCA( sizeof( resources::user_data_variant ) * requests_count ), requests_count );
	buffer_vector< resources::user_data_variant const* > usr_data_ptrs	( ALLOCA( sizeof( resources::user_data_variant const* ) * requests_count ), requests_count );
	buffer_vector< resources::creation_request > creation_requests		( ALLOCA( sizeof( resources::creation_request ) * requests_count ), requests_count );

	for ( ; it != sounds.end(); ++it )
	{
		configs::binary_config_value const& sound_value	= *it;
		resources::request								request;
		u32	class_id									= sound_value["resource_type"];
		request.id										= (resources::class_id_enum)class_id;

		if( request.id == resources::sound_collection_class && sound_value.value_exists("collection") )
		{
			request.path								= "integrated_collection";

			sound_collection_cook_user_data				data;
			data.val									= &(sound_value["collection"]);
			data.cfg_ptr								= config_ptr;
			resources::user_data_variant ud;
			ud.set										( data ); 
			usr_data.push_back							( ud );
			usr_data_ptrs.push_back						( &*usr_data.rbegin( ) );
			creation_requests.push_back					( resources::creation_request( "", 0, request.id ) );
		}
		else
		{
			request.path								= sound_value["filename"];
			usr_data_ptrs.push_back						( NULL );
			creation_requests.push_back					( resources::creation_request( "", 0, resources::unknown_data_class ) );
		}
		
		requests.push_back								( request );
	}

	xray::resources::query_resource_params params		(
		requests.begin( ), 
		creation_requests.begin( ), 
		requests.size( ), 
		boost::bind( &sound_collection_cook::sub_sounds_loaded, this, _1, config_ptr, boost::cref(collection) ),  
		resources::unmanaged_allocator( ), 
		NULL, 
		NULL, 
		usr_data_ptrs.begin( ), 
		parent
	);

	query_resources										( params );
}

void sound_collection_cook::sub_sounds_loaded ( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection )
{
	XRAY_UNREFERENCED_PARAMETER						(config_ptr);
	resources::query_result_for_cook* const	parent	= data.get_parent_query();
	if ( !data.is_successful() )
	{
		parent->finish_query						( result_error );
		return;
	}

	sound_collection* new_collection = create_collection(collection);
	if ( !new_collection )
	{
		parent->set_out_of_memory					(
			resources::unmanaged_memory, sizeof( sound_collection )
		);
		parent->finish_query						( result_out_of_memory );
		return;
	}
	
	for ( u32 i = 0, n = data.size(); i < n; ++i )
	{
		sound_emitter_ptr emitter					= static_cast_resource_ptr<sound_emitter_ptr>( data[i].get_unmanaged_resource() );
		new_collection->add_sound					( emitter );
	}

	parent->set_unmanaged_resource					( new_collection, resources::nocache_memory, sizeof( sound_collection ) );
	parent->finish_query							( result_success );
}

sound_collection* sound_collection_cook::create_collection	( configs::binary_config_value const& collection )
{
	pcstr collection_type								= collection["type"];
	collection_playing_types type						= strings::equal( collection_type, "random" ) ?
														collection_playing_type_random : collection_playing_type_sequential;
	bool can_repeat_successively						= collection["dont_repeat_sound_successively"];
	u16 cyclic_repeating_index							= collection["cyclic_repeat_from_sound"];
	
	u32 const sound_collection_size						= sizeof( sound_collection );
	u32 sounds_buffer_size								= 0;
	if( collection.value_exists( "sound_items" ) )
		sounds_buffer_size								= sizeof( sound_emitter_ptr ) * collection["sound_items"].size();

	char* buffer										= UNMANAGED_ALLOC( char, sound_collection_size + sounds_buffer_size );
	sound_collection* new_collection					= new( buffer )sound_collection(
															type,
															can_repeat_successively,
															cyclic_repeating_index,
															buffer + sound_collection_size,
															collection["sound_items"].size(),
															m_world.get_last_time()
														);
		
	return new_collection;
}

void sound_collection_cook::delete_resource			( resources::resource_base* res )
{
	res->~resource_base								( );
	UNMANAGED_FREE									( res );
}

} // namespace sound
} // namespace xray