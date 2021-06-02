////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_cook.h"
#include <xray/resources.h>
#include <xray/fs_utils.h>
#include <amd/compress/texture_compressor/api.h>
#include <amd/compress/ATI_Compress.h>

using namespace xray::resources;

using xray::resources::fs_iterator;

namespace xray {
namespace render {

static texture_cook_wrapper*		texture_cook_wrapper_obj	= NULL;
static texture_converter_cook*		texture_converter_obj		= NULL;
static texture_cook*				texture_cook_obj			= NULL;

void register_texture_cook										()
{
	//register cooks
	ASSERT							(texture_cook_wrapper_obj==NULL);
	ASSERT							(texture_converter_obj==NULL);
	ASSERT							(texture_cook_obj==NULL);
	texture_cook_wrapper_obj		= NEW(texture_cook_wrapper)();
	texture_converter_obj			= NEW(texture_converter_cook)();
	texture_cook_obj				= NEW(texture_cook)();
	register_cook					(texture_cook_wrapper_obj);
	register_cook					(texture_converter_obj);
	register_cook					(texture_cook_obj);
}

void unregister_texture_cook									()
{
	unregister_cook					(resources::texture_wrapper_class);
	unregister_cook					(resources::texture_converter_class);
	unregister_cook					(resources::texture_class);

	DELETE							(texture_cook_wrapper_obj);
	DELETE							(texture_converter_obj);
	DELETE							(texture_cook_obj);
}

static pvoid texture_compressor_allocate						( u32 size, bool is_addintional_buffer )
{
	XRAY_UNREFERENCED_PARAMETER		(is_addintional_buffer);
	return							MALLOC( size , "texture compressor");
}

static void texture_compressor_deallocate						( pvoid buffer )
{
	FREE							( buffer );
}

texture_cook_data_block::texture_cook_data_block				(unmanaged_resource_ptr options, managed_resource_ptr buffer, fs::path_string dest_path):
	m_options(options), m_buffer(buffer), m_dest_path(dest_path)
{}

texture_data_resource::texture_data_resource					(mutable_buffer buffer):
	m_buffer(buffer)
{
}

texture_data_resource::~texture_data_resource					()
{
	texture_compressor_deallocate((pvoid)m_buffer.c_ptr());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

texture_cook_wrapper::texture_cook_wrapper				(): 
translate_query_cook(resources::texture_wrapper_class, reuse_true, use_user_thread_id)
{

}

void				texture_cook_wrapper::translate_query	( resources::query_result & result )
{
	//prepare paths
	fs::path_string		old_path			(result.get_requested_path());
	fs::path_string		root_path;
	texture_cook_wrapper_data * data	=  MT_NEW(texture_cook_wrapper_data) ();
	//fs::path_string*	resource_path	=	MT_NEW(fs::path_string);
	pcstr				find_str		=	strstr(old_path.c_str(), "textures");

	root_path.append					(old_path.begin(), find_str+sizeof("textures")-1);
	data->resource_path.append			(find_str+sizeof("textures"), old_path.end());
	data->query_result					= &result;
	
 	resources::query_fs_iterator(
 		root_path.c_str(), 
 		boost::bind(&texture_cook_wrapper::on_fs_iterator_ready_source, this, data, _1),
		g_allocator
 	);

	resources::query_fs_iterator(
		root_path.c_str(), 
		boost::bind(&texture_cook_wrapper::on_fs_iterator_ready_converted, this, data, _1),
		g_allocator
		);
}

void texture_cook_wrapper::on_fs_iterator_ready_source			(texture_cook_wrapper_data* data, xray::resources::fs_iterator fs_it)
{
	fs::path_string source_path;
	source_path.append						("sources/");
	source_path.append						(data->resource_path.begin(), data->resource_path.end()-4);
	source_path.append						(".tga");
	fs_iterator fs_it_source			=	fs_it.find_child(source_path.c_str());

	fs::path_string source_options_path;
	source_options_path.append				("sources/");
	source_options_path.append				(data->resource_path.begin(), data->resource_path.end()-4);
	source_options_path.append				(".options");
	fs_iterator fs_it_source_options	=	fs_it.find_child(source_options_path.c_str());

	fs::path_info		source_info;
	fs::path_info		source_options_info;
	fs::path_string		source_absolute_path;
	fs::path_string		source_options_absolute_path;

	fs_it_source.get_disk_path		(source_absolute_path);
	fs_it_source.get_disk_path		(source_options_absolute_path);

	fs::get_path_info				(&source_info, source_absolute_path.c_str());
	fs::get_path_info				(&source_info, source_options_absolute_path.c_str());

	data->source_info_time			= source_info.file_last_modify_time;
	data->source_options_info_time	= source_options_info.file_last_modify_time;

	data->source_ready				= true;
	if( data->converted_ready)
		on_fs_iterators_ready( data);
}

void texture_cook_wrapper::on_fs_iterator_ready_converted		(texture_cook_wrapper_data* data, xray::resources::fs_iterator fs_it)
{
	//
	// if converted texture doesn't exist -> convert it
	//
	fs::path_string converted_path;
	converted_path.append			("converted_local/"); 
	converted_path.append			(data->resource_path.c_str());

	xray::resources::fs_iterator fs_it_converted		= fs_it.find_child(converted_path.c_str());

	if(fs_it_converted.is_end())
	{
		request_convertion				(&data->resource_path, data->query_result);
		MT_DELETE						(data);
		return;
	}

	fs::path_info		converted_info;
	fs::path_string		converted_absolute_path;

	fs_it_converted.get_disk_path	(converted_absolute_path);
	fs::get_path_info				(&converted_info, converted_absolute_path.c_str());

	data->converted_info_time = converted_info.file_last_modify_time;
	data->converted_ready = true;
	if( data->source_ready)
		on_fs_iterators_ready( data);
}

void	texture_cook_wrapper::on_fs_iterators_ready		(texture_cook_wrapper_data* data)
{
	//
	// if converted texture is old or source options younger than texture -> convert texture
	//
	if(	data->source_info_time > data->converted_info_time || data->source_options_info_time > data->converted_info_time)
	{
		request_convertion			(&data->resource_path, data->query_result);
		MT_DELETE					(data);
		return;
	}

	//
	// otherwise call texture cooker
	//
	query_resource(
		data->query_result->get_requested_path(),
		texture_class,
		boost::bind(&texture_cook_wrapper::on_texture_converted, this, _1), 
		&memory::g_mt_allocator,
		NULL,
		0,
		data->query_result
	); 
	MT_DELETE						(data);
}


void				texture_cook_wrapper::request_convertion(fs::path_string* resource_path, query_result_for_cook* parent)
{
	//prepare paths
	fs::path_string options_path;
	fs::path_string tga_path;

	options_path.append					("resources/textures/sources/");
	options_path.append					(resource_path->begin(), resource_path->end()-4);
	options_path.append					(".options");

	tga_path.append						("resources/textures/sources/");
	tga_path.append						(resource_path->begin(), resource_path->end()-4);
	tga_path.append						(".tga");

	request	arr[]	= {	
		{ options_path.c_str(), config_lua_class },
		{ tga_path.c_str(), raw_data_class },
	};
	query_resources(
		arr, 
		2, 
		boost::bind(&texture_cook_wrapper::on_raw_data_loaded, this, _1), 
		&memory::g_mt_allocator,
		NULL,
		0,
		parent
	); 
}

void				texture_cook_wrapper::on_raw_data_loaded			(resources::queries_result& result)
{
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();

	if ( result.is_failed())
	{
		parent->finish_query			(result_error);
		return;
	}
	
	texture_cook_data_block*	buf_struct		= MT_NEW(texture_cook_data_block)(
		result[0].get_unmanaged_resource(),
		result[1].get_managed_resource(),
		parent->get_requested_path()
	);
 	const_buffer			buf			((pcvoid)buf_struct, sizeof(texture_cook_data_block));

	query_create_resource	(
		"",
		buf, 
		resources::texture_converter_class,  
		boost::bind(&texture_cook_wrapper::on_texture_converted, this, _1),
		& memory::g_mt_allocator,
		NULL,
		0,
		parent);
}

void				texture_cook_wrapper::on_texture_converted			(resources::queries_result& result)
{
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();

	if ( result.is_failed() )
	{
		parent->finish_query			(result_error);
		return;
	}

	parent->set_unmanaged_resource	(result[0].get_unmanaged_resource(), result[0].get_unmanaged_resource()->memory_usage());
	parent->finish_query			(result_success);
}

void				texture_cook_wrapper::delete_resource				(resources::unmanaged_resource* resource)
{
	LOGI_INFO					("texture_cooker_new",	"texture_cook::delete_unmanaged called");
	DELETE						(resource);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

texture_cook::texture_cook										() 
: unmanaged_cook(	resources::texture_class, reuse_true, use_current_thread_id, use_user_thread_id) {}

// void				texture_cook::translate_request				(pcstr request, resources::class_id res_class, fs::path_string& new_request, resources::class_id& new_res_class) const
// {
// 	//prepare paths
// 	fs::path_string	old_path		(request);
// 	fs::path_string new_path;
// 	pcstr			find_str		= strstr(old_path.c_str(), "textures_new");
// 
// 	new_path.append					(old_path.begin(), find_str+12);
// 	new_path.append					("/converted_local");
// 	new_path.append					(find_str+12, old_path.end());
// 	new_path.append					(".dds");
// 
// 	new_request						=	new_path;
// 	new_res_class					=	res_class;
// 	LOGI_INFO						("texture_cooker_new",	"translate_request called");
// }

void				texture_cook::create_resource	(resources::query_result_for_cook &	in_out_query, 
												 			 		 const_buffer						raw_file_data, 
															 		 mutable_buffer						in_out_unmanaged_resource_buffer)
{
	XRAY_UNREFERENCED_PARAMETER		(in_out_unmanaged_resource_buffer);

	mutable_buffer dest				(ALLOC(u8, raw_file_data.size()), raw_file_data.size());
	memory::copy					(dest, raw_file_data);

	texture_data_resource * const out	= NEW(texture_data_resource)(dest);
	
	in_out_query.set_unmanaged_resource	(out, resources::memory_type_non_cacheable_resource, sizeof(texture_data_resource));

	in_out_query.finish_query		(result_success);
}

void				texture_cook::destroy_resource				(resources::unmanaged_resource* resource)
{
	LOGI_INFO						("texture_cooker_new",	"texture_cook::delete_unmanaged called");
	DELETE							(resource);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

texture_converter_cook::texture_converter_cook	() 
: unmanaged_cook(	resources::texture_converter_class, reuse_false, use_current_thread_id, use_user_thread_id) {}

void		texture_converter_cook::create_resource	(resources::query_result_for_cook &	in_out_query, 
											 					 				 const_buffer						raw_file_data, 
																 				 mutable_buffer						in_out_unmanaged_resource_buffer)
{
	XRAY_UNREFERENCED_PARAMETER		(in_out_unmanaged_resource_buffer);
	texture_data_resource * out	=	NULL;
	//if no requested file
	
	if (!in_out_query.is_success())
	{
		in_out_query.finish_query		(result_error);
		return;
	}

	texture_cook_data_block *		buf_struct = (texture_cook_data_block*)raw_file_data.c_ptr();

	resources::pinned_ptr_const<u8> pinned_buffer (buf_struct->m_buffer);

	fs::path_string					destination_path;
	in_out_query.get_parent_query()->set_request_path(buf_struct->m_dest_path.c_str());
	if ( !in_out_query.get_parent_query()->select_disk_path_from_request_path(&destination_path) )
	{
		in_out_query.finish_query	(result_error);
		return;
	}

	fs::path_string dir				= destination_path;
	dir.set_length					(dir.rfind('/'));
	
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
			pinned_buffer.c_ptr(),
			ATI_TC_FORMAT_DXT1,
			texture_compressor::compression_speed_fast,
			false
		);

	FILE* file;
	if(!fs::open_file(&file, fs::open_file_create | fs::open_file_truncate | fs::open_file_write, destination_path.c_str()))
	{
		in_out_query.finish_query	(result_error);
		return;
	}
	
	fwrite							( destination_buffer, 1, new_buffer_size, file );
	fclose							( file );

	out							=	NEW(texture_data_resource)(mutable_buffer(destination_buffer, new_buffer_size));
	
	in_out_query.set_unmanaged_resource	(out, resources::memory_type_non_cacheable_resource, sizeof(texture_data_resource));

	MT_DELETE						(buf_struct);
	in_out_query.finish_query		(result_success);
}

void			texture_converter_cook::destroy_resource		(resources::unmanaged_resource* res)
{
	LOGI_INFO						("texture_cooker_new",	"texture_cook::delete_unmanaged called");
	DELETE							(res);
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

} // namespace render
} // namespace xray