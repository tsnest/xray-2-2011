////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collection_cook.h"
#include "animation_collection.h"

namespace xray {
namespace animation {

pcstr collection_extention								= ".anim_collection";
struct animation_collection_cook_user_data
{
	configs::binary_config_value const*					val;
	configs::binary_config_ptr							cfg_ptr;
};

animation_collection_cook::animation_collection_cook	( ) :
	super												(
		resources::animation_collection_class,
		reuse_true,
		use_resource_manager_thread_id
	)
{
	register_cook( this );
}

animation_collection_cook::~animation_collection_cook ( )
{
}

void animation_collection_cook::translate_query ( resources::query_result_for_cook& parent )
{
	resources::user_data_variant* user_data					= parent.user_data( );
	if( user_data )
	{
		animation_collection_cook_user_data				data;
		user_data->try_get								( data );
		parent.clear_user_data							( );
		request_items									( data.cfg_ptr, *data.val, &parent );
	}
	else
	{
		fs::path_string config_path;
		config_path.assignf("%s%s%s", "resources/animations/collections/", parent.get_requested_path( ), collection_extention );
		query_resource								(
			config_path.c_str( ), 
			resources::binary_config_class,
			boost::bind( &animation_collection_cook::collection_config_loaded, this, _1 ),
			resources::unmanaged_allocator( ),
			0,
			&parent 
		);
	}
}

void animation_collection_cook::collection_config_loaded ( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query( );
	if ( !data.is_successful( ) )
	{
		parent->finish_query							( result_error );
		return;
	}
	
	configs::binary_config_ptr config_ptr				= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource( ) );
	
	CURE_ASSERT	( config_ptr->get_root().value_exists( "collection" ), { parent->finish_query( result_error ); return; } );

	configs::binary_config_value const& collection		= (*config_ptr)["collection"];
	R_ASSERT( collection.value_exists( "collection_type" ) );
	 
	request_items										( config_ptr, collection, parent );
}

void animation_collection_cook::request_items ( configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection_value, resources::query_result_for_cook* const parent )
{
	if( !( collection_value.value_exists( "animation_items" ) ) )
	{
		parent->finish_query							( result_error );
		return;
	}

	R_ASSERT											( collection_value.value_exists("collection_type") );

	configs::binary_config_value const& animations		= collection_value["animation_items"];
	configs::binary_config_value::const_iterator it		= animations.begin( );
	u32 requests_count									= animations.size( );

	buffer_vector< resources::request > requests						( ALLOCA( sizeof( resources::request ) * requests_count ), requests_count );
	buffer_vector< resources::user_data_variant > user_data				( ALLOCA( sizeof( resources::user_data_variant ) * requests_count ), requests_count );
	buffer_vector< resources::user_data_variant const* > user_data_ptrs	( ALLOCA( sizeof( resources::user_data_variant const* ) * requests_count ), requests_count );
	buffer_vector< resources::creation_request > creation_requests		( ALLOCA( sizeof( resources::creation_request ) * requests_count ), requests_count );

	for ( ; it != animations.end( ); ++it )
	{
		configs::binary_config_value const& animation_value	= *it;

		resources::request								request;
		u32	class_id									= animation_value["type"];
		request.id										= (resources::class_id_enum)class_id;
		
		if( request.id == resources::animation_collection_class && animation_value.value_exists( "name" ) )
		{
			request.path									= "integrated_collection";

			animation_collection_cook_user_data				data;
			data.val										= &animation_value;
			data.cfg_ptr									= config_ptr;

			resources::user_data_variant ud;
			ud.set											( data ); 
			user_data.push_back								( ud );
			user_data_ptrs.push_back						( &*user_data.rbegin( ) );

			creation_requests.push_back						( resources::creation_request( "", 0, request.id ) );
		}
		else
		{
			request.path									= animation_value["path"];
			user_data_ptrs.push_back						( NULL );
			creation_requests.push_back						( resources::creation_request( "", 0, resources::unknown_data_class ) );
		}

		requests.push_back								( request );
	}

	R_ASSERT											( collection_value.value_exists("collection_type") );

	xray::resources::query_resource_params params		(
		requests.begin( ), 
		creation_requests.begin( ), 
		requests_count, 
		boost::bind( &animation_collection_cook::sub_animations_loaded, this, _1, config_ptr, boost::cref(collection_value) ),  
		resources::unmanaged_allocator( ), 
		NULL, 
		NULL, 
		user_data_ptrs.begin( ), 
		parent
	);

	query_resources										( params );
}

void animation_collection_cook::sub_animations_loaded ( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& collection_value )
{
	XRAY_UNREFERENCED_PARAMETER						( config_ptr );
	resources::query_result_for_cook* const	parent	= data.get_parent_query( );
	if ( !data.is_successful( ) )
	{
		parent->finish_query						( result_error );
		return;
	}

	R_ASSERT										( collection_value.value_exists("collection_type") );

	animation_collection* collection				= new_collection( collection_value );
	if ( !collection )
	{
		parent->set_out_of_memory					(
			resources::unmanaged_memory, sizeof( animation_collection )
		);
		parent->finish_query						( result_out_of_memory );
		return;
	}
	
	for ( u32 i = 0, n = data.size( ); i < n; ++i )
	{
		animation_expression_emitter_ptr emitter	= static_cast_resource_ptr<animation_expression_emitter_ptr>( data[i].get_unmanaged_resource( ) );
		collection->add_animation					( emitter );
	}

	parent->set_unmanaged_resource					( collection, resources::nocache_memory, sizeof( animation_collection ) );
	parent->finish_query							( result_success );
}

animation_collection* animation_collection_cook::new_collection ( configs::binary_config_value const& collection )
{
	pcstr collection_type								= collection["collection_type"];
	collection_playing_types type						= strings::equal( collection_type, "random" ) ?
														collection_playing_type_random : collection_playing_type_sequential;
	bool can_repeat_successively						= collection["is_dont_repeat_previous"];
	bool cyclic_repeating_index							= collection["is_cyclic_repeat"];
	
	u32 const animation_collection_size						= sizeof( animation_collection );
	u32 animations_buffer_size								= 0;
	if( collection.value_exists( "animation_items" ) )
		animations_buffer_size								= sizeof( animation_expression_emitter_ptr ) * collection["animation_items"].size( );

	char* buffer										= UNMANAGED_ALLOC( char, animation_collection_size + animations_buffer_size );
	animation_collection* new_collection					= new( buffer )animation_collection(
															type,
															can_repeat_successively,
															cyclic_repeating_index,
															buffer + animation_collection_size,
															collection["animation_items"].size( ),
															NULL //m_world.get_last_time()
														);
		
	return new_collection;
}

void animation_collection_cook::delete_resource ( resources::resource_base* res )
{
	res->~resource_base								( );
	UNMANAGED_FREE									( res );
}

} // namespace animation
} // namespace xray