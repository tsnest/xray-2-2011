////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "composite_sound_cook.h"
#include <xray/sound/sound_emitter.h>
#include "sound_world.h"
#include "composite_sound.h"

namespace xray {
namespace sound {

pcstr composite_extention				= ".composite_sound";

composite_sound_cook::composite_sound_cook( sound_world& world ) :
	super										(
		resources::composite_sound_class,
		reuse_true,
		use_resource_manager_thread_id
	),
	m_world( world )
{
}

composite_sound_cook::~composite_sound_cook	( )
{
}

void composite_sound_cook::translate_query	( resources::query_result_for_cook& parent )
{
	fs_new::virtual_path_string config_path;
	config_path.assignf("%s%s%s", "resources/sounds/composite/", parent.get_requested_path(), composite_extention);
	query_resource							(
		config_path.c_str(), 
		resources::binary_config_class,
		boost::bind( &composite_sound_cook::on_sub_resources_loaded, this, _1 ),
		xray::resources::unmanaged_allocator(),
		0,
		&parent 
	);
}

void composite_sound_cook::on_sub_resources_loaded	( resources::queries_result& data )
{
	R_ASSERT_U											( data.is_successful() );
	resources::query_result_for_cook* const parent		= data.get_parent_query();
	configs::binary_config_ptr config_ptr				= static_cast_resource_ptr<configs::binary_config_ptr>( data[0].get_unmanaged_resource() );

	CURE_ASSERT	( config_ptr->get_root().value_exists( "composite_sound" ), { parent->finish_query( result_error ); return; } );
	
	configs::binary_config_value const& composite		= (*config_ptr)["composite_sound"];
	if( !( composite.value_exists( "sound_items" ) ) )
	{
		composite_sound* new_composite_sound			= create_sound(composite);
		if ( !new_composite_sound )
		{
			parent->set_out_of_memory	(
				resources::unmanaged_memory, sizeof( composite_sound )
			);
			parent->finish_query		( result_out_of_memory );
			return;
		}
	}

	if ( composite.value_exists( "sound_items" ) )
	{
		configs::binary_config_value const& sounds			= composite["sound_items"];
		configs::binary_config_value::const_iterator it		= sounds.begin();
		buffer_vector< resources::request >	requests		( ALLOCA( sizeof( resources::request ) * sounds.size() ), sounds.size() );
		for ( ; it != sounds.end(); ++it )
		{
			configs::binary_config_value const& sound_value	= *it;
			resources::request								request;
			request.path									= sound_value["filename"];
			u32	class_id									= sound_value["resource_type"];
			request.id										= (resources::class_id_enum)class_id;
			requests.push_back								( request );
		}

		resources::query_resources	(
			requests.begin(), 
			requests.size(), 
			boost::bind( &composite_sound_cook::on_sounds_loaded, this, _1, config_ptr, boost::cref( sounds ) ), 
			resources::unmanaged_allocator(),
			0,
			parent
		);
	}
	else
	{
		composite_sound* new_composite_sound			= create_sound(composite);
		if ( !new_composite_sound )
		{
			parent->set_out_of_memory	( resources::unmanaged_memory, sizeof( composite_sound ) );
			parent->finish_query		( result_out_of_memory );
			return;
		}

		parent->set_unmanaged_resource	( new_composite_sound, resources::nocache_memory, sizeof( composite_sound ) );
		parent->finish_query			( result_success );
	}
}

void composite_sound_cook::on_sounds_loaded	( resources::queries_result& data, configs::binary_config_ptr config_ptr, configs::binary_config_value const& sounds )
{
	R_ASSERT( data.is_successful() );
	resources::query_result_for_cook* parent		= data.get_parent_query();
	configs::binary_config_value const& composite	= (*config_ptr)["composite_sound"];
	composite_sound* new_composite_sound			= create_sound(composite);
	if ( !new_composite_sound )
	{
		parent->set_out_of_memory	( resources::unmanaged_memory, sizeof( composite_sound ) );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	configs::binary_config_value::const_iterator it		= sounds.begin();
	for ( u32 i = 0; i < data.size(); ++i )
	{
		sound_emitter_ptr emitter		= static_cast_resource_ptr<sound_emitter_ptr>( data[i].get_unmanaged_resource() );
		pcstr file_name					= data[i].get_requested_path( );
		configs::binary_config_value const& sound_value	= *it;
		R_ASSERT						( strcmp( file_name, sound_value["filename"] ) == 0 );
		std::pair< u32, u32 >		offsets;
		fixed_string<16> type		= "random";
		if ( type == sound_value["offset_type"] )
		{
			offsets.first				= sound_value["playing_offset_min"];
			offsets.second				= sound_value["playing_offset_max"];
		}
		else
		{
			offsets.first				= sound_value["playing_offset"];
			offsets.second				= sound_value["playing_offset"];
		}

		new_composite_sound->add_sound	( emitter, offsets );
		++it;
	}

	parent->set_unmanaged_resource	( new_composite_sound, resources::nocache_memory, sizeof( composite_sound ) );
	parent->finish_query			( result_success );
}

composite_sound* composite_sound_cook::create_sound		( configs::binary_config_value const& composite )
{
	u32 const composite_sound_size						= sizeof( composite_sound );
	u32 sounds_buffer_size								= 0;
	if( composite.value_exists( "sound_items" ) )
		sounds_buffer_size								= sizeof( composite_sound::sounds_type ) * composite["sound_items"].size();

	char* buffer										= UNMANAGED_ALLOC( char, composite_sound_size + sounds_buffer_size );
	composite_sound* new_composite_sound				= new( buffer )composite_sound ( 
																		buffer + composite_sound_size,
																		sounds_buffer_size,
																		m_world.get_last_time());
	return new_composite_sound;
}


void composite_sound_cook::delete_resource	( resources::resource_base* res )
{
	res->~resource_base						( );
	UNMANAGED_FREE							( res );
}

} // namespace sound
} // namespace xray
