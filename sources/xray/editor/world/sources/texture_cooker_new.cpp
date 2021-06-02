////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_cooker_new.h"
#include <xray/resources.h>
#include <xray/resources_cook.h>
#include <xray/fs_utils.h>
#include <amd/compress/texture_compressor/api.h>
#include <amd/compress/ATI_Compress.h>

typedef xray::resources::cook::result_enum result_enum;
using namespace xray::resources;

namespace xray {
namespace editor {


//	static void			compress_file_to_dds							(fs::path_string &end_file_path	);
	static pvoid		texture_compressor_allocate			( u32 size );
	static void			texture_compressor_deallocate		( pvoid buffer );

	texture_cooker_data_block::texture_cooker_data_block(unmanaged_resource_ptr options, resource_ptr buffer, fs::path_string dest_path):
		m_options(options), m_buffer(buffer), m_dest_path(dest_path)
	{}


		texture_data_resource::texture_data_resource(mutable_buffer buffer):
		m_buffer(buffer)
	{
		
	}

		texture_data_resource::~texture_data_resource()
	{
		texture_compressor_deallocate((pvoid)m_buffer.data());
	}

texture_cooker_new::texture_cooker_new	() 
: cook(	resources::texture_class_new, flag_process|flag_generate_if_no_file|flag_translate_request, use_cooker_thread_id) {}

void				texture_cooker_new::translate_request			(pcstr request, resources::class_id res_class, fs::path_string& new_request, resources::class_id& new_res_class) const
{
	//prepare paths
	fs::path_string	old_path		(request);
	fs::path_string new_path;
	pcstr			find_str		= strstr(old_path.c_str(), "textures_new");

	new_path.append					(old_path.begin(), find_str+12);
	new_path.append					("/converted_local");
	new_path.append					(find_str+12, old_path.end());
	new_path.append					(".dds");
	
	new_request						=	new_path;
	new_res_class					=	res_class;
	LOGI_INFO						("texture_cooker_new",	"translate_request called");
}

result_enum			texture_cooker_new::process						(resources::query_result_for_cook& query, u32& final_managed_resource_size)
{
	XRAY_UNREFERENCED_PARAMETER	( final_managed_resource_size );

	texture_data_resource * out			= NULL;
	//if no requested file
	if ( query.get_error_type() == resources::query_result::error_type_file_not_found )
	{
		//prepare paths
		fs::path_string	old_path		(query.get_requested_path());
		fs::path_string options_path;
		fs::path_string tga_path;
		pcstr			find_str		= strstr(old_path.c_str(), "converted_local");

		options_path.append					(old_path.begin(), find_str);
		options_path.append					("sources");
		options_path.append					(find_str+15, old_path.end());
		options_path.set_length				(options_path.length()-4);
		tga_path							= options_path;
		options_path.append					(".options");
		tga_path.append						(".tga");

// 		//generate files
// 		query_resource	(new_path.c_str(), 
// 			resources::config_lua_class,  
// 			resources::query_callback(this, &texture_cooker_new::on_raw_properties_loaded),
// 			& memory::g_mt_allocator,
// 			0,
// 			& query);

		request	arr[]	= {	
			{ options_path.c_str(), config_lua_class },
			{ tga_path.c_str(), raw_data_class },
		};
		query_resources					(
			arr, 
			2, 
			query_callback(this, &texture_cooker_new::on_raw_data_loaded), 
			& memory::g_mt_allocator, 
			0,
			& query); 

		return							result_postponed;
	}
	
	if (!query.is_success())
		return						result_error;

	mutable_buffer src				= query.get_file_data();
	mutable_buffer dest				(MT_ALLOC(u8, src.size()), src.size());
	memory::copy					(dest, src);
	out								= MT_NEW(texture_data_resource)(dest);
	
	out->set_deleter_object			(this, &query);
	query.set_unmanaged_resource	(out);

	return							result_reuse_unmanaged;
}

void				texture_cooker_new::on_raw_data_loaded			(resources::queries_result& result)
{
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();

	if ( result.is_failed() )
	{
		parent->finish_postponed			(result_error);
		return;
	}
	
	texture_cooker_data_block*	buf_struct		= MT_NEW(texture_cooker_data_block)(
		result[0].get_unmanaged_resource(),
		result[1].get_resource(),
		parent->get_requested_path()
	);
 	const_buffer			buf			(buf_struct, sizeof(texture_cooker_data_block));

	query_create_resource	(
		buf, 
		resources::texture_converter,  
		resources::query_callback(this, &texture_cooker_new::on_texture_converted),
		& memory::g_mt_allocator,
		0,
		parent);
}

void				texture_cooker_new::on_texture_converted		(resources::queries_result& result)
{
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();

	if ( result.is_failed() )
	{
		parent->finish_postponed			(result_error);
		return;
	}

	parent->set_unmanaged_resource(result[0].get_unmanaged_resource().get());
	parent->finish_postponed(result_reuse_unmanaged);
}

static pvoid texture_compressor_allocate	( u32 size )
{
	return							MT_MALLOC( size , "texture compressor");
}

static void texture_compressor_deallocate	( pvoid buffer )
{
	MT_FREE							( buffer );
}

// static void			compress_file_to_dds							(fs::path_string &end_file_path	)
// {
// 	//make path's
// 	fs::path_string			src_file_path;
// 	char*					sources_offset				= strstr(end_file_path.c_str(), "converted_local");
// 
// 	src_file_path.append	(end_file_path.begin(), sources_offset);
// 	src_file_path.append	("sources");
// 	src_file_path.append	(sources_offset+15, end_file_path.end());
// 
// 	src_file_path.set_length(src_file_path.length()-3);
// 	src_file_path.append	("tga");
// 
// 	//if destination directory is not exists -> make it
// 	fs::path_string dir;
// 	dir = end_file_path;
// 	dir.set_length(dir.rfind('/'));
// 	
// 	if(fs::get_path_info(NULL, dir.c_str()) == fs::path_info::type_nothing)
// 	{
// 		fs::make_dir_r(dir.c_str());
// 	}
// 
// 	STARTUPINFO				si;
// 	ZeroMemory		( &si, sizeof(si) );
// 	si.cb			= sizeof(STARTUPINFO);
// 	si.wShowWindow	= 0;
// 	si.dwFlags		= STARTF_USESHOWWINDOW;
// 
// 	PROCESS_INFORMATION		pi;
// 	ZeroMemory		( &pi, sizeof(pi) );
// 
// 	fixed_string512			cl_args;
// 	cl_args			+= "../../nvidia_compress.exe";
// 	cl_args 		+= " -fast ";
// 	cl_args			+= src_file_path.c_str();
// 	cl_args 		+= " ";
// 	cl_args			+= end_file_path.c_str();
// 	
// 
// 	if(CreateProcess(NULL,cl_args.c_str(),NULL, NULL, FALSE,0, NULL, NULL, &si, &pi))
// 		WaitForSingleObject(pi.hProcess, INFINITE);
// 	else
// 		LOG_ERROR("CreateProcess [%s] failed", cl_args.c_str());
// }

void				texture_cooker_new::delete_unmanaged			(resources::unmanaged_resource* res)
{
	LOGI_INFO						("texture_cooker_new",	"texture_cook::delete_unmanaged called");
	MT_DELETE						(res);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

texture_converter_cooker::texture_converter_cooker	() 
: cook(	resources::texture_converter, flag_process, use_cooker_thread_id) {}

result_enum		texture_converter_cooker::process					(resources::query_result_for_cook& query, u32& final_managed_resource_size)
{
	XRAY_UNREFERENCED_PARAMETER	( final_managed_resource_size );
	texture_data_resource * out		= NULL;
	//if no requested file
	
	if (!query.is_success())
		return							result_error;

	mutable_buffer data				= query.get_file_data();
	texture_cooker_data_block*		buf_struct = reinterpret_cast<texture_cooker_data_block*>((u8*)data.data());

	resources::pinned_ptr<u8> src	(buf_struct->m_buffer);
	u8 * buffer						= (u8 *)src.raw_pointer();

	fs::path_string destination_path;
	query.get_parent_query()->set_request_path(buf_struct->m_dest_path.c_str());
	CURE_ASSERT						(query.get_parent_query()->select_disk_path_from_request_path(&destination_path), return result_error);
	fs::path_string dir = destination_path;
	dir.set_length(dir.rfind('/'));
	
	if(fs::get_path_info(NULL, dir.c_str()) == fs::path_info::type_nothing)
	{
		fs::make_dir_r(dir.c_str());
	}

	u32 new_buffer_size				= 0;
	u8* destination_buffer			=
		create_texture(
			new_buffer_size,
			&texture_compressor_allocate,
			&texture_compressor_deallocate,
			buffer,
			ATI_TC_FORMAT_DXT1,
			texture_compressor::compression_speed_fast,
			true
		);

	FILE* file;
	if(!fs::open_file(&file, fs::open_file_create|fs::open_file_write|fs::open_file_truncate, destination_path.c_str()))
		return result_error;
	
	fwrite							( destination_buffer, 1, new_buffer_size, file );
	fclose							( file );

	out								= MT_NEW(texture_data_resource)(mutable_buffer(destination_buffer, new_buffer_size));
	
	out->set_deleter_object			(this, &query);
	query.set_unmanaged_resource	(out);

	MT_DELETE						(buf_struct);
	return							result_reuse_unmanaged;
}

void			texture_converter_cooker::delete_unmanaged			(resources::unmanaged_resource* res)
{
	LOGI_INFO						("texture_cooker_new",	"texture_cook::delete_unmanaged called");
	MT_DELETE						(res);
}

} // namespace editor
} // namespace xray