////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_source_cooker.h"
#include "ogg_utils.h"
#include <xray/resources_fs.h>

namespace xray {
namespace sound {

pcstr _converted_local_path	= "resources.converted/sounds/";
pcstr _sound_sources_path	= "resources/sounds/";
pcstr _wav_ext				= ".wav";
pcstr _ogg_ext				= ".ogg";
pcstr _options_ext			= ".options";

void change_substring(fs::path_string& src_and_dest, pcstr what, pcstr to)
{
	fs::path_string	result;
	fs::path_string::size_type pos	= src_and_dest.find( what );
	u32 what_len					= strings::length( what );

	result.assign		(src_and_dest.begin(), src_and_dest.begin()+pos); // head
	result.append		( to ); // body
	result.append		( src_and_dest.begin()+pos+what_len, src_and_dest.end() ); // tail
	src_and_dest		= result;
}

ogg_source_cooker::ogg_source_cooker( )
:super( resources::ogg_raw_file, reuse_true, use_cooker_thread_id )
{
}

void ogg_source_cooker::translate_query( resources::query_result& parent )
{
	fs::path_string	ogg_path		= parent.get_requested_path();
	fs::path_string	wav_path		= ogg_path;
	change_substring				( wav_path, _converted_local_path, _sound_sources_path );
	change_substring				( wav_path, _ogg_ext, _wav_ext );

	fs::path_string	options_path	= wav_path;
	change_substring				( options_path, _wav_ext, _options_ext );
	
	// compare source and converted resource file age
	fs::path_info			source_file_info;
	fs::path_info			options_file_info;
	fs::path_info			ogg_file_info;
	fs::path_string	path;
	fs::path_info::type_enum res_source;
	fs::path_info::type_enum res_options;
	fs::path_info::type_enum res_ogg;

	res_source = resources::get_path_info_by_logical_path(	&source_file_info, 
												wav_path.c_str(), 
												g_allocator);

	res_options = resources::get_path_info_by_logical_path(	&options_file_info, 
												options_path.c_str(), 
												g_allocator);

	res_ogg = resources::get_path_info_by_logical_path(	&ogg_file_info, 
												ogg_path.c_str(), 
												g_allocator);


	R_ASSERT( res_source==fs::path_info::type_file );
	R_ASSERT( res_options==fs::path_info::type_file );
	u32 max_sources = math::max( source_file_info.file_last_modify_time, options_file_info.file_last_modify_time );
	if(res_ogg==fs::path_info::type_file && max_sources < ogg_file_info.file_last_modify_time)
	{ //use converted entry
		resources::query_resource(
			ogg_path.c_str(),
			resources::raw_data_class,
			boost::bind(&ogg_source_cooker::on_ogg_file_loaded, this, _1, &parent),
			&memory::g_mt_allocator
			);
	}else
	{ // convert needed, query for sound options
		resources::query_resource(
			options_path.c_str(),
			resources::config_lua_class,
			boost::bind(&ogg_source_cooker::on_source_options_loaded, this, _1, &parent),
			&memory::g_mt_allocator
			);
	}
}

void ogg_source_cooker::on_ogg_file_loaded( resources::queries_result& data, 
											resources::query_result_for_cook* parent_query )
{
	R_ASSERT							( data.is_successful() );
	parent_query->set_managed_resource	( data[0].get_managed_resource() );
	parent_query->finish_query			( result_success );
}

// options for encoding arrived
void ogg_source_cooker::on_source_options_loaded(	resources::queries_result& data, 
													resources::query_result_for_cook* parent_query )
{
	R_ASSERT							( data.is_successful() );
	resources::query_result_for_user const& options_result	= data[0];
	configs::lua_config_ptr options_ptr	= static_cast_resource_ptr<configs::lua_config_ptr>( options_result.get_unmanaged_resource() );

	fs::path_string input_file_name		= options_result.get_requested_path();
	change_substring					( input_file_name, _options_ext, _wav_ext );

	fs::path_string output_file_name	= input_file_name;
	change_substring					( output_file_name, _sound_sources_path, _converted_local_path );
	change_substring					( output_file_name, _wav_ext,_ogg_ext );

	configs::lua_config_value const& config_root	= options_ptr->get_root();
	configs::lua_config_value t						= config_root["options"]; 

	ogg_utils::encode_sound_file(
							input_file_name.c_str(),
							output_file_name.c_str(),
							t["bits_per_sample"],
							t["number_of_chanels"],
							t["samples_per_second"],
							t["output_bitrate"]
						);


	parent_query->finish_query		 ( result_requery );
}

void ogg_source_cooker::delete_resource( resources::unmanaged_resource* resource )
{
	XRAY_UNREFERENCED_PARAMETERS( resource );
	NOT_IMPLEMENTED				( );
}

} // namespace sound
} // namespace xray
