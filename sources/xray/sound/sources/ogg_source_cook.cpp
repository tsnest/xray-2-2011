////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_source_cook.h"
#include "ogg_utils.h"
#include <xray/sound/wav_utils.h>
#include <xray/resources_fs.h>

namespace xray {
namespace sound {

pcstr _converted_local_path		= "resources.converted/sounds/single/";
pcstr _sound_sources_path		= "resources/sounds/single/";
pcstr _high_quality				= ".high";
pcstr _medium_quality			= ".medium";
pcstr _low_quality				= ".low";
pcstr _high_quality_with_ext	= ".high.ogg";
pcstr _medium_quality_with_ext	= ".medium.ogg";
pcstr _low_quality_with_ext		= ".low.ogg";
pcstr _wav_ext					= ".wav";
pcstr _ogg_ext					= ".ogg";
pcstr _options_ext				= ".raw_options";

ogg_source_cook::ogg_source_cook( ) 
:super( resources::ogg_raw_file, reuse_true, use_current_thread_id )
{
}

void ogg_source_cook::translate_query( resources::query_result_for_cook& parent )
{
	fs_new::virtual_path_string	ogg_path	= parent.get_requested_path();

#ifdef MASTER_GOLD
	resources::query_resource(
		ogg_path.c_str(),
		resources::raw_data_class,
		boost::bind(&ogg_source_cook::on_ogg_file_loaded, this, _1, &parent),
		&memory::g_mt_allocator,
		NULL,
		& parent
	);
#else // #ifdef MASTER_GOLD

	fs_new::virtual_path_string	wav_path	= ogg_path;

	wav_path.replace				( _converted_local_path, _sound_sources_path );
	wav_path.replace				( _ogg_ext, _wav_ext );

	buffer_string::size_type end	= wav_path.find( _high_quality );
	if ( buffer_string::size_type(-1) != end )
		wav_path.replace			( _high_quality, "" );

	end	= wav_path.find( _medium_quality );
	if ( buffer_string::size_type(-1) != end )
		wav_path.replace			( _medium_quality, "" );

	end	= wav_path.find( _low_quality );
	if ( buffer_string::size_type(-1) != end )
		wav_path.replace			( _low_quality, "" );
	
	fs_new::virtual_path_string	options_path	= wav_path;
	options_path.replace			( _wav_ext, _options_ext );
	

	resources::request r[] ={
		{ wav_path.c_str(),		resources::fs_iterator_class },
		{ options_path.c_str(), resources::fs_iterator_class },
		{ ogg_path.c_str(),		resources::fs_iterator_class },
	
	};
	resources::query_resources(
		r,
		boost::bind( &ogg_source_cook::on_fs_iterators_ready,this, _1, &parent),
		&memory::g_mt_allocator,
		NULL,
		&parent
		);

#endif // #ifdef MASTER_GOLD
}

void ogg_source_cook::on_ogg_file_loaded( resources::queries_result& data, 
											resources::query_result_for_cook* parent )
{
	R_ASSERT						( data.is_successful() );
	parent->set_managed_resource	( data[0].get_managed_resource() );
	parent->finish_query			( result_success );
}

#ifndef MASTER_GOLD
void ogg_source_cook::on_fs_iterators_ready( resources::queries_result& data, 
											resources::query_result_for_cook* parent )
{
	R_ASSERT						( data.is_successful() );
	// compare source and converted resource file age
	vfs::vfs_iterator it_wav		= data[0].get_result_iterator();
	vfs::vfs_iterator it_options	= data[1].get_result_iterator();
	vfs::vfs_iterator it_ogg		= data[2].get_result_iterator();

	fs_new::physical_path_info		source_file_info;
	fs_new::physical_path_info		options_file_info;
	fs_new::physical_path_info		ogg_file_info;

	source_file_info			=	resources::get_physical_path_info( it_wav );
	if( !source_file_info.is_file() )
	{
		parent->finish_query( resources::query_result_for_user::error_type_file_not_found );
		return;
	}

	options_file_info			=	resources::get_physical_path_info( it_options );
	
	if( !options_file_info.is_file() )
	{
		fs_new::native_path_string wav_file_disk_path = it_wav.get_physical_path();
		LOG_DEBUG	( "create default config for %s", wav_file_disk_path.c_str() );

		fs_new::synchronous_device_interface const& device	= get_synchronous_device();

		wav_utils::create_default_config		( wav_file_disk_path.c_str(), device );

		parent->finish_query					( result_requery );
		return;
	}
		
	ogg_file_info				= resources::get_physical_path_info( it_ogg );

	
	u32 sources_modification_time = math::max( source_file_info.last_time_of_write(), options_file_info.last_time_of_write() );
	
	if( ogg_file_info.is_file() && sources_modification_time < ogg_file_info.last_time_of_write() )
	{ 
		//use converted entry
		resources::query_resource(
			it_ogg.get_virtual_path().c_str(),
			resources::raw_data_class,
			boost::bind( &ogg_source_cook::on_ogg_file_loaded, this, _1, parent ),
			&memory::g_mt_allocator,
			NULL,
			parent
		);
	}
	else
	{ 
		// convert needed, query for sound options
		resources::query_resource(
			it_options.get_virtual_path().c_str(),
			resources::lua_config_class,
			boost::bind( &ogg_source_cook::on_source_options_loaded, this, _1, parent ),
			&memory::g_mt_allocator,
			NULL,
			parent
		);
	}
}


// options for encoding arrived
void ogg_source_cook::on_source_options_loaded( resources::queries_result& data, 
												  resources::query_result_for_cook* parent )
{
	R_ASSERT							( data.is_successful( ) );
	resources::query_result_for_user const& options_result	= data[0];
	configs::lua_config_ptr options_ptr	= static_cast_resource_ptr<configs::lua_config_ptr>( options_result.get_unmanaged_resource() );

	fs_new::virtual_path_string input_file_name		= options_result.get_requested_path( );
	input_file_name.replace				( _options_ext, _wav_ext );

	configs::lua_config_value const& config_root	= options_ptr->get_root();
	configs::lua_config_value t						= config_root["options"]; 

	fs_new::virtual_path_string output_file_name	= parent->get_requested_path( );


	// high quality for default values
	configs::lua_config_value quality_conv_options = t["high_quality"];

	buffer_string::size_type end	= output_file_name.find( _medium_quality );
	if ( end != buffer_string::size_type(-1) )
	{
		quality_conv_options	= t["medium_quality"];
	}
	else if (( end = output_file_name.find( _low_quality )) != buffer_string::size_type(-1) )
	{
		quality_conv_options	= t["low_quality"];
	}

	u32 output_samples_per_second	= quality_conv_options["sample_rate"];
	u32 output_bitrate				= quality_conv_options["bit_rate"];


	fs_new::native_path_string input_file_name_disk_path;
	if ( !resources::convert_virtual_to_physical_path( &input_file_name_disk_path, input_file_name.c_str( ), resources::sources_mount))
	{
		parent->finish_query(result_error);
		return;
	}

	fs_new::native_path_string output_file_name_disk_path;
	if ( !resources::convert_virtual_to_physical_path (&output_file_name_disk_path, output_file_name.c_str( ), resources::sources_mount))
	{
		parent->finish_query(result_error);
		return;
	}

	fs_new::synchronous_device_interface const& device	=	get_synchronous_device();
	ogg_utils::encode_sound_file(
							input_file_name_disk_path.c_str(),
							output_file_name_disk_path.c_str(),
							device,
							t["bits_per_sample"],
							t["channels_number"],
							output_samples_per_second,
							output_bitrate
						);

#pragma message(XRAY_TODO("Lain 2 Andy: seems we need to check if conversion was OK, not to loop querying..."))

	parent->finish_query		 ( result_requery );
}
#endif // #ifdef MASTER_GOLD

void ogg_source_cook::delete_resource( resources::resource_base* resource )
{
	XRAY_UNREFERENCED_PARAMETERS( resource );
	NOT_IMPLEMENTED				( );
}

} // namespace sound
} // namespace xray
