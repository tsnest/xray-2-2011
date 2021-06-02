////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shader_binary_source_cook.h"
#include <xray/render/core/effect_descriptor.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/shader_defines.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/shader_macros.h>
#include <xray/render/core/shader_defines.h>
#include <xray/render/core/shader_declarated_macros.h>

#include "include_handler.h"

#pragma warning( push )
#pragma warning( disable : 4995 )
#	if !USE_DX10 
#		include <d3dx11tex.h>
#		include <d3dcompiler.h>
#	else // #if !USE_DX10 
#		include <d3dx10tex.h>
#	endif // #if !USE_DX10 
#pragma warning( pop )


static xray::command_line::key s_no_cache_shaders_key( "no_cache_shaders", "", "", "disable binary shaders caching" );

namespace xray {
namespace render {

shader_binary_source_cook::shader_binary_source_cook (): 
	unmanaged_cook(	
		resources::shader_binary_source_class, 
		reuse_false, 
		use_current_thread_id,
		use_current_thread_id
	),
	m_tasks_type	( tasks::create_new_task_type("compile_shader_task", 0) )
{
}

shader_binary_source_cook::~shader_binary_source_cook ()
{
//!	tasks::delete_task_type	( m_tasks_type );
}

bool shader_binary_source_cook::is_need_recompile(conveted_shader_loaded_data* data,
												  resources::managed_resource_ptr binary_shader_ptr) const
{
	resources::pinned_ptr_const<u8>	ptr_managed		(binary_shader_ptr);
	
	u32		num_saved_infos							= ((u32*)ptr_managed.c_ptr())[0];
	pvoid	additional_data_ptr						= (pbyte)ptr_managed.c_ptr() + sizeof(u32);
	
#ifdef MASTER_GOLD
	xray::vectora<shader_file_info>	loaded_includes_info(::xray::memory::g_mt_allocator);
#else // #ifdef MASTER_GOLD
	xray::vectora<shader_file_info>	loaded_includes_info(::xray::debug::g_mt_allocator);
#endif // #ifdef MASTER_GOLD
	
	loaded_includes_info.resize						(num_saved_infos);
	xray::memory::copy(
		&loaded_includes_info[0], 
		num_saved_infos * sizeof(shader_file_info), 
		additional_data_ptr, 
		num_saved_infos * sizeof(shader_file_info)
		);
	
#ifndef MASTER_GOLD
	res_effect * const effect = data->cook_data->effect_resource;
	effect->used_shaders_lock.lock();
	for (vectora<shader_file_info>::const_iterator	it = loaded_includes_info.begin(); 
													it != loaded_includes_info.end(); 
													++it)
	{
		fs_new::virtual_path_string file_name	= it->name;
		file_name								= file_name.substr(file_name.rfind('/')+1);
		file_name.set_length					(file_name.rfind('.'));

		if (std::find(effect->used_shaders.begin(), effect->used_shaders.end(), file_name) 
				== effect->used_shaders.end() )
		{
			effect->used_shaders.push_back(file_name);
		}
	}
	effect->used_shaders_lock.unlock();
#endif // #ifdef MASTER_GOLD
	
	resource_manager::map_shader_sources const& shader_sources = resource_manager::ref().get_shader_sources();
	
 	for (render::vector<shader_file_info>::const_iterator	info_it = loaded_includes_info.begin(); 
 															info_it != loaded_includes_info.end(); 
 															++info_it)
 	{
 		resource_manager::map_shader_sources::const_iterator found_source = shader_sources.find(info_it->name);
 		
 		if (found_source == shader_sources.end() || found_source->second.time != info_it->time)
 			return true;
 	}
	return false;
}

void shader_binary_source_cook::save_binary_shader(resources::query_result_for_cook* in_out_query,
												   pvoid data,
												   u32 size,
												   pcstr shader_name)
{
	fs_new::native_path_string				physical_path;
	bool const converted_path			=	resources::convert_virtual_to_physical_path(& physical_path, shader_name, resources::sources_mount);
	R_ASSERT								(converted_path);
	in_out_query->save_generated_resource	(resources::save_generated_data::create(const_buffer(data, size), true, physical_path));
}

static pcstr shader_type_to_compile_target(enum_shader_type const type)
{
	switch(type)
	{
		case enum_shader_type_vertex:	return "vs_4_0";
		case enum_shader_type_pixel:	return "ps_4_0";
		case enum_shader_type_geometry: return "gs_4_0";
		default: NODEFAULT(return "");
	}
}

static pcstr shader_type_to_string(enum_shader_type const type)
{
	switch(type)
	{
		case enum_shader_type_vertex:	return "vertex shader";
		case enum_shader_type_pixel:	return "pixel shader";
		case enum_shader_type_geometry: return "geometry shader";
		default: NODEFAULT(return "");
	}
}

void shader_binary_source_cook::all_tasks_finished(conveted_shader_loaded_data* data)
{
	data->new_resource->configuration				= data->cook_data->configuration;
	data->new_resource->shader_name					= data->cook_data->shader_name;
	data->new_resource->shader_type					= data->cook_data->shader_type;
	
	if (data->data_to_save)
	{
		fs_new::native_path_string				physical_path;
		bool const converted_path			=	resources::convert_virtual_to_physical_path(& physical_path, data->converted_shader_path, resources::sources_mount);
		R_ASSERT								(converted_path);

		data->in_out_query->save_generated_resource	(
			resources::save_generated_data::create(
				const_buffer(data->data_to_save, data->save_data_size), 
				true,
				physical_path
			)
		);
		
#ifdef MASTER_GOLD
		MT_FREE										(data->data_to_save);
#else // #ifdef MASTER_GOLD
		DEBUG_FREE									(data->data_to_save);
#endif // #ifdef MASTER_GOLD
	}		
	
	if (data->new_resource->shader_byte_code)
	{
		data->in_out_query->set_unmanaged_resource	(data->new_resource, resources::nocache_memory, sizeof(binary_shader_source));
		data->in_out_query->finish_query			(result_success);
	}
	else
	{
		data->in_out_query->finish_query			(result_error);
	}
	
	DELETE											(data->cook_data);
	DELETE											(data);
}

void shader_binary_source_cook::compile_shader_task(conveted_shader_loaded_data* data)
{
	shader_defines_list								working_defines_list;
	shader_macros::ref().fill_shader_macro_list		(working_defines_list, data->cook_data->configuration);
	
	bool is_found_source							= false;
	resource_manager::shader_source& shader_source	= resource_manager::ref().get_shader_source_by_short_name(
		data->cook_data->shader_name.c_str(), 
		data->cook_data->shader_type,
		is_found_source
	);
	
	if (!is_found_source)
	{
		FATAL										(
			"!%s \"%s\" was not found!", 
			shader_type_to_string(data->cook_data->shader_type), 
			data->cook_data->shader_name.c_str()
		);
		data->in_out_query->finish_query			(result_error);
		return;
	}
	
	include_handler includer(/*&resource_manager::ref().m_sources, */&shader_source);
	ID3D10Blob*		shader_code						= NULL;
	ID3D10Blob*		error_buf						= NULL;
	
	shader_file_info	info;
	xray::strings::copy								(
		info.name, 
		resource_manager::ref().get_full_shader_path(data->cook_data->shader_name.c_str(), data->cook_data->shader_type).c_str()
	);
	
	info.time										= shader_source.time;
	includer.includes_info.push_back				(info);
	
	//LOG_INFO										(
	//	"%s %s compiling started...", 
	//	data->cook_data->shader_name.c_str(),
	//	shader_type_to_string(data->cook_data->shader_type)
	//);
	
	fixed_vector<D3D_SHADER_MACRO, shader_macros_max_cout> d3d_shader_macroses;
	for (shader_defines_list::const_iterator it = working_defines_list.begin(); it != working_defines_list.end(); ++it)
	{
		D3D_SHADER_MACRO	macro;
		macro.Name			= it->name.c_str();
		macro.Definition	= it->definition.c_str();
		d3d_shader_macroses.push_back(macro);
		//if (macro.Name)
		//	LOG_INFO("%s=%s", macro.Name, macro.Definition);
		//else
		//	LOG_INFO("none=none");
	}
	
	D3D_SHADER_MACRO	macro;
	macro.Name			= NULL;
	macro.Definition	= NULL;
	d3d_shader_macroses.push_back(macro);
	
	HRESULT result	= D3DXCompileFromMemory			(
		(LPCSTR)&shader_source.data[0],
		shader_source.data.size(),
		"",
		d3d_shader_macroses.empty() ? NULL : &*d3d_shader_macroses.begin(), 
		(ID3DInclude*)&includer, 
		"main",
		shader_type_to_compile_target(data->cook_data->shader_type),

#ifdef NDEBUG
		D3DCOMPILE_OPTIMIZATION_LEVEL3 |
#else
		D3D_SHADER_DEBUG | 
#endif//#ifdef NDEBUG

		D3D_SHADER_PACK_MATRIX_ROW_MAJOR, 
		0,
		0,
		&shader_code,
		&error_buf,
		0
	);
	
	//LOG_INFO										(
	//	"%s %s compiling finished.",
	//	data->cook_data->shader_name.c_str(),
	//	shader_type_to_string(data->cook_data->shader_type)
	//);
	
#ifndef MASTER_GOLD	
	if (SUCCEEDED(result))
	{
		res_effect * const effect = data->cook_data->effect_resource;
		threading::simple_lock_raii	raii(effect->used_shaders_lock);

		for (vectora<shader_file_info>::const_iterator	it = includer.includes_info.begin(); 
														it != includer.includes_info.end(); 
														++it)
		{
			fs_new::virtual_path_string file_name				= it->name;
			file_name								= file_name.substr(file_name.rfind('/')+1);
			file_name.set_length					(file_name.rfind('.'));

			if (std::find(effect->used_shaders.begin(), effect->used_shaders.end(), file_name) 
				== data->cook_data->effect_resource->used_shaders.end() )
			{
				data->cook_data->effect_resource->used_shaders.push_back(file_name);
			}
		}
	}
#endif // #ifndef MASTER_GOLD
	
	data->new_resource->error_code					= error_buf;
	if (FAILED(result) && error_buf)
	{
		pcstr const err_msg							= (LPCSTR)error_buf->GetBufferPointer();
		FATAL										(
			"!%s \"%s\" is not created. \nerrors: \n%s", 
			shader_type_to_string(data->cook_data->shader_type), 
			data->cook_data->shader_name.c_str(), 
			err_msg
		);
		data->new_resource->shader_byte_code		= 0;
	}
	if (shader_code)
	{
		u32 const num_size							= includer.includes_info.size();
		u32 const total_size						= 
			sizeof(u32) + 
			sizeof(shader_file_info) * includer.includes_info.size() + 
			shader_code->GetBufferSize();
		
#ifdef MASTER_GOLD
		pbyte shader_data							= MT_ALLOC(byte, total_size);
#else // #ifdef MASTER_GOLD
		pbyte shader_data							= DEBUG_ALLOC(byte, total_size);
#endif // #ifdef MASTER_GOLD
		
		xray::memory::copy							(shader_data, sizeof(u32), (pcvoid)&num_size, sizeof(u32));
		xray::memory::copy							(
			shader_data + sizeof(u32), 
			sizeof(shader_file_info) * includer.includes_info.size(), 
			(pcvoid)&includer.includes_info[0], 
			sizeof(shader_file_info) * includer.includes_info.size()
		);
		
		xray::memory::copy							(
			shader_data + sizeof(u32) + sizeof(shader_file_info) * includer.includes_info.size(), 
			shader_code->GetBufferSize(), 
			shader_code->GetBufferPointer(), 
			shader_code->GetBufferSize()
		);
		
		data->data_to_save							= shader_data;
		data->save_data_size						= total_size;
		
		data->new_resource->shader_byte_code		= shader_code;
	}
	else
	{
		data->data_to_save							= NULL;
		data->save_data_size						= 0;
	}
	data->in_out_query->on_task_finished			();
}

void shader_binary_source_cook::converted_shader_loaded(conveted_shader_loaded_data* data,
													   resources::queries_result& result)
{
	if (!result[0].is_successful() || s_no_cache_shaders_key.is_set() || is_need_recompile(data, result[0].get_managed_resource()))
	{
		LOG_INFO(result[0].get_requested_path());
		data->in_out_query->set_tasks_finished_callback(boost::bind(&shader_binary_source_cook::all_tasks_finished, this, data));
		data->in_out_query->set_pending_tasks_count	(1);
		
		tasks::spawn_task ( 
			boost::bind(
				&shader_binary_source_cook::compile_shader_task,
				this,
				data
			),
			m_tasks_type
		);
	}
	else
	{
		data->new_resource->shader_source			= result[0].get_managed_resource();
		
		resources::pinned_ptr_const<u8>	ptr_managed	(result[0].get_managed_resource());
		
		u32		num_saved_infos						= ((u32*)ptr_managed.c_ptr())[0];
		u32		additional_data_size				= sizeof(u32) + num_saved_infos * sizeof(shader_file_info);
		u32		binary_shader_size					= ptr_managed.size() - additional_data_size;
		pvoid	binary_shader_byte_code				= (pbyte)ptr_managed.c_ptr() + additional_data_size;
		
#if USE_DX10
		D3D10CreateBlob								(binary_shader_size, &data->new_resource->shader_byte_code);
#else // #if USE_DX10
		D3DCreateBlob								(binary_shader_size, &data->new_resource->shader_byte_code);
#endif // #if USE_DX10
		
		memory::copy								(
			data->new_resource->shader_byte_code->GetBufferPointer(), 
			binary_shader_size, 
			binary_shader_byte_code, 
			binary_shader_size
		);
		
		data->new_resource->configuration			= data->cook_data->configuration;
		data->new_resource->shader_name				= data->cook_data->shader_name;
		data->new_resource->shader_type				= data->cook_data->shader_type;
		
		data->in_out_query->set_unmanaged_resource	(data->new_resource, resources::nocache_memory, sizeof(binary_shader_source));
		data->in_out_query->finish_query			(result_success);
		
		DELETE										(data->cook_data);
		DELETE										(data);
	}
}


void   shader_binary_source_cook::create_resource (resources::query_result_for_cook&	in_out_query,
											 	   const_buffer							raw_file_data, 
												   mutable_buffer						in_out_unmanaged_resource_buffer)
{
	binary_shader_cook_data* user_data				= (binary_shader_cook_data*)raw_file_data.c_ptr();
	
	if (!user_data->shader_name.length())
	{
		in_out_query.finish_query					(result_error, in_out_query.assert_on_fail());
		return;
	}
	
	binary_shader_source* new_resource				= new(in_out_unmanaged_resource_buffer.c_ptr())binary_shader_source;
	
	fs_new::virtual_path_string converted_shader_path;
	converted_shader_path.assignf					(
		"%s/%s/%s/", 
		resource_manager::ref().get_converted_shader_path(),
		user_data->shader_name.c_str(), 
		shader_type_to_compile_target(user_data->shader_type)
	);
	
	bool is_found_source = false;
	
	resource_manager::shader_source& shd_source = 
		resource_manager::ref().get_shader_source_by_short_name(user_data->shader_name.c_str(), user_data->shader_type, is_found_source);
	
	shader_declarated_macroses_list found_macros_names;
	
	if (!is_found_source)
	{
		FATAL										(
			"!%s \"%s\" file was not found!", 
			shader_type_to_string(user_data->shader_type), 
			user_data->shader_name.c_str()
		);
		in_out_query.finish_query					(result_error);
		return;
	}
	
	found_shader_declarated_macroses((pcstr)&shd_source.data[0], found_macros_names);
	
	shader_defines_list								working_defines_list;
	shader_macros::ref().fill_shader_macro_list		(working_defines_list, user_data->configuration);
	
	bool has_defines = false;
	for (shader_defines_list::const_iterator define_it = working_defines_list.begin(); 
											 define_it != working_defines_list.end() && define_it->definition.length();
											 ++define_it)
	{

		//check_strings_valid(found_macros_names.begin(), found_macros_names.end());

		if (std::find(found_macros_names.begin(), found_macros_names.end(), define_it->name)
			!=
			found_macros_names.end())
		{
			converted_shader_path.append	(define_it->definition.c_str());
			has_defines						= true;
		}
		else
		{
			converted_shader_path.append	("_");
		}
	}
	
	if (!has_defines)
	{
	//	converted_shader_path.append				("no_configuration");
	}
	//ASSERT											(has_defines);
	
	in_out_query.finish_query						(result_postponed);
	
	resources::request	request			=	{ converted_shader_path.c_str(), resources::raw_data_class };
	resources::query_resource_params	params(
		& request, 
		NULL, 
		1, 
		boost::bind(
			&shader_binary_source_cook::converted_shader_loaded, 
			this, 
			NEW(conveted_shader_loaded_data)(
				&in_out_query, 
				user_data, 
				new_resource, 
				converted_shader_path
			), 
			_1
		),
		in_out_query.get_user_allocator(),
		NULL,
		NULL,
		NULL,
		&in_out_query
	);

	params.assert_on_fail				=	assert_on_fail_false;

	resources::query_resources				(params);
}

void shader_binary_source_cook::destroy_resource(resources::unmanaged_resource* resource_to_destroy)
{
	binary_shader_source* resource					= (binary_shader_source*)resource_to_destroy;
	
	if (resource->shader_byte_code)
		resource->shader_byte_code->Release			();
	
	if (resource->error_code)
		resource->error_code->Release				();
	
	resource->~binary_shader_source					();
}

mutable_buffer	shader_binary_source_cook::allocate_resource(resources::query_result_for_cook &	in_query, 
															 const_buffer				raw_file_data, 
															 bool						file_exist)
{
	XRAY_UNREFERENCED_PARAMETERS					(&file_exist, &raw_file_data, &in_query);
	return											xray::mutable_buffer(
		(pvoid)ALLOC(binary_shader_source, 1), 
		sizeof(binary_shader_source)
	);
}

void   shader_binary_source_cook::deallocate_resource (pvoid buffer) 
{
	FREE											(buffer);
}

} // namespace render
} // namespace xray
