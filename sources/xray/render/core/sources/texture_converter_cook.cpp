////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_converter_cook.h"
#include <xray/render/core/texture_options.h>

#ifndef MASTER_GOLD
#	include <amd/compress/texture_compressor/api.h>
#endif // #ifndef MASTER_GOLD

xray::command_line::key	g_highquality_dds("hq_dds", "", "render", "using nvidia texture converter for producing DDS texture (slow, but high quality)");

namespace xray {
namespace render {

#ifndef MASTER_GOLD

texture_converter_params::texture_converter_params	(resources::unmanaged_resource_ptr		options, 
													 resources::managed_resource_ptr		buffer, 
													 fs_new::virtual_path_string			dest_path, 
													 fs_new::virtual_path_string			old_dest_path)
:m_options_cfg	(options), 
m_buffer	(buffer), 
m_dest_path	(dest_path), 
m_old_dest_path(old_dest_path)
{}


static void* texture_compressor_allocate	(u32 size, bool is_additional_buffer)
{
	XRAY_UNREFERENCED_PARAMETER				(is_additional_buffer);
	pvoid const result						= DEBUG_MALLOC(size , "texture compressor");
//.	LOG_INFO								( "allocating buffer: 0x%08x => %d bytes", result, size );
	return									result;
}

static void texture_compressor_deallocate (pvoid buffer)
{
//.	LOG_INFO								( "deallocating buffer: 0x%08x", buffer );
	DEBUG_FREE								(buffer);
}

texture_raw_file_data::~texture_raw_file_data					()
{
	texture_compressor_deallocate			(m_buffer.c_ptr());
}

void log_function( pcstr string )
{
	LOG_INFO(string);
}

texture_converter_cook::texture_converter_cook () : managed_cook(	resources::texture_converter_class, 
																	reuse_false, 
																	use_resource_manager_thread_id,
																	flag_create_allocates_destroy_deallocates	) { }

u8*   texture_converter_cook::create_texture (u32 *				out_size,
											  bool				only_calculate_size,
											  mutable_buffer *	opt_destination_data,
											  const_buffer		raw_file_data)
{
	texture_converter_params * const buf_struct	= (texture_converter_params *)raw_file_data.c_ptr();
	resources::pinned_ptr_const<u8> pinned_buffer(buf_struct->m_buffer);

	configs::binary_config_ptr options_config_ptr	= static_cast_resource_ptr<configs::binary_config_ptr>(buf_struct->m_options_cfg);
	configs::binary_config_value const& options_config = options_config_ptr->get_root()["options"];
	
	texture_compressor::texture_dimension dim	= (texture_compressor::texture_dimension)( (u32)options_config["texture_dimension"] );
	
	pcstr texture_format				=	(pcstr)options_config["format"];
	
	texture_compressor::set_log_function(log_function);
	timing::timer t;
	t.start();

	bool hq								= g_highquality_dds.is_set() || 
		(
			options_config.value_exists("high_quality") ? 
			(0!=(int)options_config["high_quality"]) :
			false
		);
	bool generate_mip					= (0!=(int)options_config["has_mip_levels"]);
	bool is_srgb						= options_config.value_exists("srgb") ? (0!=(int)options_config["srgb"]) : false;
	
	u8* destination_buffer				= texture_compressor::create_texture(	* out_size,
															only_calculate_size,
															opt_destination_data ? (u8*)opt_destination_data->c_ptr() : 0,
															opt_destination_data ? opt_destination_data->size() : 0,
															& texture_compressor_allocate,
															& texture_compressor_deallocate,
															pinned_buffer.c_ptr(),
															texture_format,
															(hq) ? texture_compressor::compression_highquality : texture_compressor::compression_fast,
															pinned_buffer.size(),
															generate_mip,
															dim,
															is_srgb);

	if(!only_calculate_size)
		LOG_INFO("converting %s take %f sec", buf_struct->m_dest_path, t.get_elapsed_sec() );

	R_ASSERT( (*out_size != 0) && (only_calculate_size || (!only_calculate_size && (destination_buffer!=NULL))) , 
			"while converting file %s", buf_struct->m_dest_path.c_str() );

	return								destination_buffer;
}

void   texture_converter_cook::create_resource (resources::query_result_for_cook &	in_out_query, 
											 	const_buffer						raw_file_data, 
												resources::managed_resource_ptr		out_resource)
{
	resources::pinned_ptr_mutable<u8>	pinned_buffer	(pin_for_write<u8>(out_resource));

	u32 buffer_size						=	0;
	mutable_buffer	destination_buffer	=	pinned_buffer;
	u8 * result_buffer					=	create_texture(&buffer_size, false, &destination_buffer, raw_file_data);

	if ( result_buffer )
	{
		R_ASSERT_CMP						(buffer_size, <=, destination_buffer.size());
		texture_converter_params * const buf_struct	= (texture_converter_params *)raw_file_data.c_ptr();

		fs_new::native_path_string			physical_path;
		bool const converted_path		=	resources::convert_virtual_to_physical_path(& physical_path, buf_struct->m_dest_path, resources::sources_mount);
		R_ASSERT							(converted_path);

		in_out_query.save_generated_resource(resources::save_generated_data::create(out_resource, physical_path));

		in_out_query.finish_query			(result_success);
	}
	else
	{
		LOG_INFO("error.");
		in_out_query.finish_query			(result_error);
	}
}

u32	  texture_converter_cook::calculate_resource_size	( const_buffer raw_file_data, bool file_exist )
{
	XRAY_UNREFERENCED_PARAMETER				(file_exist);
	u32 buffer_size						=	0;
	create_texture							(& buffer_size, true, NULL, raw_file_data);
	R_ASSERT								(buffer_size != 0);

	return									buffer_size;
}

void	texture_converter_cook::destroy_resource	(resources::managed_resource *	dying_resource)
{
	XRAY_UNREFERENCED_PARAMETER				(dying_resource);
}

#endif // #ifndef MASTER_GOLD

} // namespace render
} // namespace xray
