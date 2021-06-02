////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#pragma managed(push)
#pragma unmanaged
#include <xray/resources.h>
#include <xray/resources_cook_classes.h>
#include <xray/resources_fs.h>
#include <xray/fs_path_string.h>
#pragma managed(pop)

#include "memory.h"
//#include <conio.h>

#pragma managed(push)
#pragma unmanaged
#include <xray/core/sources/resources_manager.h>
#include <xray/core/sources/resources_managed_allocator.h>
#pragma managed(pop)

#include "sound_object_cook.h"
#include "raw_file_property_struct.h"
#include "sound_editor.h"

#pragma managed(push)
#pragma unmanaged
#include <xray/fs_utils.h>
#include <xray/linkage_helper.h>
#pragma managed(pop)

DECLARE_LINKAGE_ID(resources_test)

using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Windows::Forms;

using xray::editor::raw_file_property_struct;
using xray::editor::sound_editor;
using xray::editor::unmanaged_string;
using xray::resources::fs_iterator;

#pragma message(XRAY_TODO("fix cooks"))
#if 0 // commented by Lain

namespace xray
{
	namespace resources
	{
		static void					make_sound_options					(fs::path_string& file_path);
		static void					encode_sound_file					(
			fs::path_string &end_file_path,
			int bits_per_sample,
			int number_of_chanels,
			int samples_per_second,
			int output_bitrate
		);

		//ogg_resource class
		sound_object_resource::sound_object_resource(pcbyte data_raw)
		{
			XRAY_UNREFERENCED_PARAMETER(data_raw);
			m_options_resource_ptr = NULL;
		}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

		ogg_sound_cook_wrapper::ogg_sound_cook_wrapper					() 
: cook(	ogg_sound_wrapper_class, flag_process|flag_generate_if_no_file|flag_translate_query, threading::current_thread_id()/*use_cooker_thread_id*/) {}

void				ogg_sound_cook_wrapper::translate_query				( resources::query_result& result )
{
	//prepare paths
	fs::path_string		old_path			(result.get_requested_path());
	fs::path_string		root_path;
	fs::path_string*	resource_path	=	MT_NEW(fs::path_string);
	pcstr				find_str		=	strstr(old_path.c_str(), "sounds");

	root_path.append					(old_path.begin(), find_str+6);
	resource_path->append				(find_str+23, old_path.end());
	
 	resources::query_fs_iterator(
 		root_path.c_str(), 
 		boost::bind(&ogg_sound_cook_wrapper::on_fs_iterator_ready, this, resource_path, &result, _1),
		result.get_user_allocator()
 	);
}

void				ogg_sound_cook_wrapper::on_fs_iterator_ready		(fs::path_string* resource_path, query_result_for_cook* parent, xray::resources::fs_iterator fs_it)
{
	//
	// if converted ogg isn't exists -> convert it and options
	//
	fs::path_string					converted_path;
	converted_path.append			("converted_local/"); 
	converted_path.append			(resource_path->c_str());
	converted_path.append			(".ogg");
	fs_iterator fs_it_converted	=	fs_it.find_child(converted_path.c_str());

	if(fs_it_converted.is_end())
	{
		request_convertion			(resource_path, parent);
		MT_DELETE						(resource_path);
		return;
	}

	//
	// if converted ogg is old or source options younger than converted ogg -> convert ogg and options
	//
	fs::path_string							source_path;
	source_path.append						("sources/");
	source_path.append						(resource_path->c_str());
	source_path.append						(".wav");
	fs_iterator fs_it_source			=	fs_it.find_child(source_path.c_str());

	fs::path_string							source_options_path;
	source_options_path.append				("sources/");
	source_options_path.append				(resource_path->c_str());
	source_options_path.append				(".options");
	fs_iterator fs_it_source_options	=	fs_it.find_child(source_options_path.c_str());

	fs::path_info		source_info;
	fs::path_info		converted_info;
	fs::path_info		source_options_info;
	fs::path_string		source_absolute_path;
	fs::path_string		converted_absolute_path;
	fs::path_string		source_options_absolute_path;

	fs_it_source.get_disk_path				(source_absolute_path);
	fs_it_converted.get_disk_path			(converted_absolute_path);
	fs_it_source_options.get_disk_path		(source_options_absolute_path);
	fs::get_path_info						(&source_info, source_absolute_path.c_str());
	fs::get_path_info						(&converted_info, converted_absolute_path.c_str());
	fs::get_path_info						(&source_options_info, source_options_absolute_path.c_str());

	if(	source_info.file_last_modify_time > converted_info.file_last_modify_time ||
		source_options_info.file_last_modify_time > converted_info.file_last_modify_time)
	{
		request_convertion			(resource_path, parent);
		MT_DELETE						(resource_path);
		return;
	}

	//
	// if converted options isn't exists -> convert it
	//
	
	fs::path_string			converted_options_path;
	converted_options_path.append	("converted_local/"); 
	converted_options_path.append	(resource_path->c_str());
	converted_options_path.append	(".options");

	xray::resources::fs_iterator fs_it_converted_options	= fs_it.find_child(converted_options_path.c_str());

	if(fs_it_converted_options.is_end())
	{
		request_convertion_options	(resource_path, parent);
		MT_DELETE						(resource_path);
		return;
	}

	//
	// if converted options is old -> convert it
	//

	fs::path_info		converted_options_info;
	fs::path_string		converted_options_absolute_path;
	
	fs_it_converted_options.get_disk_path	(converted_options_absolute_path);
	fs::get_path_info						(&converted_options_info, converted_options_absolute_path.c_str());
	
	if( converted_options_info.file_last_modify_time < converted_info.file_last_modify_time)
	{
		request_convertion_options	(resource_path, parent);
		MT_DELETE						(resource_path);
		return;
	}

	//
	// otherwise request sound
	//

	query_resource(
  		parent->get_requested_path(),
  		ogg_sound_class,
  		boost::bind(&ogg_sound_cook_wrapper::sound_loaded, this, _1), 
  		&memory::g_mt_allocator,
  		0,
  		parent
  	); 

	MT_DELETE						(resource_path);
}

void				ogg_sound_cook_wrapper::request_convertion			(fs::path_string* resource_path, query_result_for_cook* parent)
{
	//prepare paths
	fs::path_string ogg_path;
	fs::path_string tga_path;

	ogg_path.append						("resources/sounds/converted_local/");
	ogg_path.append						(resource_path->c_str());
	ogg_path.append						(".ogg");

	query_resource(
		ogg_path.c_str(), 
		ogg_converter_class, 
		boost::bind(&ogg_sound_cook_wrapper::on_sound_converted, this, false, _1), 
		&memory::g_mt_allocator,
		0,
		parent
	); 
}

void				ogg_sound_cook_wrapper::request_convertion_options	(fs::path_string* resource_path, query_result_for_cook* parent)
{
	//prepare paths
	fs::path_string ogg_path;
	fs::path_string tga_path;

	ogg_path.append						("resources/sounds/converted_local/");
	ogg_path.append						(resource_path->c_str());
	ogg_path.append						(".options");

	query_resource(
		ogg_path.c_str(), 
		ogg_options_converter_class, 
		boost::bind(&ogg_sound_cook_wrapper::on_sound_converted, this, true, _1), 
		&memory::g_mt_allocator,
		0,
		parent
	); 
}

void				ogg_sound_cook_wrapper::on_sound_converted			(bool is_options_complete, resources::queries_result& result)
{
	resources::query_result_for_cook * const	parent	=	result.get_parent_query();

	if(is_options_complete)
	{
	 	query_resource(
	  		parent->get_requested_path(),
	  		ogg_sound_class,
	  		boost::bind(&ogg_sound_cook_wrapper::sound_loaded, this, _1), 
	  		&memory::g_mt_allocator,
	  		0,
	  		parent
	  	); 
	}
	else
	{
		fs::path_string		old_path		(result[0].get_requested_path());
		fs::path_string		resource_path;
		pcstr				find_str	=	strstr(old_path.c_str(), "converted_local");

		resource_path.append				(find_str+16, old_path.end()-4);

		request_convertion_options			(&resource_path, parent);
	}
}

void				ogg_sound_cook_wrapper::sound_loaded				(queries_result& result)
{
	query_result_for_cook * const		parent		= result.get_parent_query();

	if(result.is_failed())
	{
		parent->finish_translated_query		(result_error);
		return;
	}

	parent->set_unmanaged_resource		(result[0].get_unmanaged_resource().get());
	parent->finish_translated_query		(result_dont_reuse);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//ogg_sound_cook class
ogg_sound_cook::ogg_sound_cook											() 
	: cook(	ogg_sound_class, cook::flag_translate_query, use_cooker_thread_id) {}

void				ogg_sound_cook::translate_query						(query_result& parent)
{
	pstr request_ogg				= 0;
	STR_JOINA						(
		request_ogg,
		parent.get_requested_path(),
		".ogg"
	);

	pstr request_options			= 0;
	STR_JOINA						(
		request_options,
		parent.get_requested_path(),
		".options"
	);

	request arr[] = {	
		{ request_ogg, ogg_class },
		{ request_options, config_lua_class },
	};
	query_resources					(arr, 
									 2, 
									 boost::bind(&ogg_sound_cook::on_sub_resources_loaded, this, _1), 
									 parent.get_user_allocator(), 
									 0,
									 &parent); 
}

void				ogg_sound_cook::on_sub_resources_loaded				(queries_result & result)
{
	query_result_for_cook * const	parent	=	result.get_parent_query();

	unmanaged_resource_ptr ogg_res;
	if ( result[0].is_success() && result[1].is_success() )
	{
		ogg_res						=	result[0].get_unmanaged_resource();
		static_cast_checked<sound_object_resource*>( ogg_res.get()) -> m_options_resource_ptr =
			(result[1].get_unmanaged_resource());
	}
	else
	{
		parent->finish_translated_query	(result_error);
		return;
	}

	parent->set_unmanaged_resource		(ogg_res.get());
	parent->finish_translated_query		(result_reuse_unmanaged);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

					ogg_cook::ogg_cook() : cook(ogg_class, flag_process|flag_generate_if_no_file) {}

cook::result_enum	ogg_cook::process									(query_result_for_cook& query, u32& final_managed_resource_size)
{
	XRAY_UNREFERENCED_PARAMETER	( final_managed_resource_size );

	sound_object_resource * out	=	NULL;
		
	if (!query.is_success())
		return						result_error;

	mutable_buffer data		=		query.get_file_data();
	pcbyte data_raw			=		(pcbyte)data.data();
	out						=		MT_NEW(sound_object_resource)(data_raw);
	
	query.set_unmanaged_resource	(out);

	return							result_reuse_unmanaged;
}

void				ogg_cook::delete_unmanaged							(unmanaged_resource* res)
{
	MT_DELETE						(res);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void					encode_sound_file	(
	fs::path_string &end_file_path,
	int bits_per_sample,
	int number_of_chanels,
	int samples_per_second,
	int output_bitrate							);

ogg_converter::ogg_converter() : cook(ogg_converter_class, flag_process|flag_generate_if_no_file) {}

cook::result_enum	ogg_converter::process(query_result_for_cook& query, u32& final_managed_resource_size)
{
	XRAY_UNREFERENCED_PARAMETER	( final_managed_resource_size );

	//prepare paths
	fs::path_string	dest_path		(query.get_requested_path());
	fs::path_string source_options_path;
	pcstr			find_str		= strstr(dest_path.c_str(), "converted_local");

	source_options_path.append					(dest_path.begin(), find_str);
	source_options_path.append					("sources");
	source_options_path.append					(find_str+15, dest_path.end()-4);
	source_options_path.append					(".options");

	fs::path_string					disk_path;
	CURE_ASSERT						(query.select_disk_path_from_request_path(& disk_path),
									return result_error);

	query_resource	(
		source_options_path.c_str(), 
		config_lua_class,  
		boost::bind(&ogg_converter::on_raw_properties_loaded, this, _1),
		& memory::g_mt_allocator,
		0,
		& query);

	return							result_postponed;
}

void				ogg_converter::on_raw_properties_loaded	(queries_result& result)
{
	query_result_for_cook * const		parent		= result.get_parent_query();

	if ( result.is_failed() )
	{
		parent->finish_postponed			(result_error);
		return;
	}

	configs::lua_config_ptr				config		= static_cast_intrusive_ptr<configs::lua_config_ptr>(result[0].get_unmanaged_resource());

	configs::lua_config_value const&	config_root = config->get_root();
	configs::lua_config_value			value		= config_root["options"]; 

	fs::path_string						disk_path;
	CURE_ASSERT							(parent->select_disk_path_from_request_path(&disk_path), parent->finish_postponed(result_error));

	encode_sound_file(
		disk_path,
		value["bits_per_sample"],
		value["number_of_chanels"],
		value["samples_per_second"],
		value["output_bitrate"]
	);
	
	parent->finish_postponed			(result_dont_reuse);
}

static void					encode_sound_file(
	fs::path_string &end_file_path,
	int bits_per_sample,
	int number_of_chanels,
	int samples_per_second,
	int output_bitrate)
{
	//make path's
	fs::path_string			src_file_path;
	char*					sources_offset				= strstr(end_file_path.c_str(), "converted_local");

	src_file_path.append	(end_file_path.begin(), sources_offset);
	src_file_path.append	("sources");
	src_file_path.append	(sources_offset+15, end_file_path.end());
	src_file_path.set_length(src_file_path.length()-4);
	src_file_path.append	(".wav");

	//if destination directory is not exists -> make it
	fs::path_string dir;
	dir = end_file_path;
	dir.set_length(dir.rfind('/'));
	
	if(fs::get_path_info(NULL, dir.c_str()) == fs::path_info::type_nothing)
	{
		fs::make_dir_r(dir.c_str());
	}

	STARTUPINFO				si;
	ZeroMemory		( &si, sizeof(si) );
	si.cb			= sizeof(STARTUPINFO);
	si.wShowWindow	= 0;
	si.dwFlags		= STARTF_USESHOWWINDOW;

	PROCESS_INFORMATION		pi;
	ZeroMemory		( &pi, sizeof(pi) );

	char buf[12];
	fixed_string512			cl_args;
	cl_args			+= "oggenc.exe";
	cl_args 		+= " -B ";
	_itoa_s(bits_per_sample, buf, 10);
	cl_args			+= buf;
	cl_args 		+= " -C ";
	_itoa_s(number_of_chanels, buf, 10);
	cl_args			+= buf;
	cl_args 		+= " -R ";
	_itoa_s(samples_per_second, buf, 10);
	cl_args			+= buf;
	cl_args 		+= " -b ";
	_itoa_s(output_bitrate, buf, 10);
	cl_args			+= buf;
	cl_args 		+= " -o \"";
	cl_args			+= end_file_path.c_str();
	cl_args 		+= "\" \"";
	cl_args			+= src_file_path.c_str();
	cl_args			+= "\"";

	if(CreateProcess(NULL,cl_args.c_str(),NULL, NULL, FALSE,0, NULL, NULL, &si, &pi))
		WaitForSingleObject(pi.hProcess, INFINITE);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void					make_sound_options(fs::path_string& end_file_path);

					ogg_options_converter::ogg_options_converter() : cook(ogg_options_converter_class, flag_process|flag_generate_if_no_file) {}

cook::result_enum	ogg_options_converter::process(query_result_for_cook& query, u32& final_managed_resource_size)
{
	XRAY_UNREFERENCED_PARAMETER	( final_managed_resource_size );

	//if no requested file
	fs::path_string	disk_path;
		
	if(query.select_disk_path_from_request_path(& disk_path))
		make_sound_options				(disk_path);

	return							result_dont_reuse;
}

static void					make_sound_options(fs::path_string& end_file_path)
{
	//make path's
	fs::path_string			file_path;

	file_path				= end_file_path;
	file_path.set_length	(file_path.length()-8);
	file_path.append		(".ogg");

	STARTUPINFO				si;
	ZeroMemory		( &si, sizeof(si) );
	si.cb			= sizeof(STARTUPINFO);
	si.wShowWindow	= 0;
	si.dwFlags		= STARTF_USESHOWWINDOW;

	PROCESS_INFORMATION		pi;
	ZeroMemory		( &pi, sizeof(pi) );

	fixed_string512			cl_args;
	cl_args			+= "xray_rms_generator-debug.exe";
	cl_args 		+= " -o=\"";
	cl_args 		+= end_file_path.c_str();
	cl_args 		+= "\" -i=\"";
	cl_args 		+= file_path.c_str();
	cl_args 		+= "\"";

	if(CreateProcess(NULL, cl_args.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		WaitForSingleObject(pi.hProcess, INFINITE);
}

}//namespace resources
}//namespace xray

#endif // #if 0 // commented by Lain
