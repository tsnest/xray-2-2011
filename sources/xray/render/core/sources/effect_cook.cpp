////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/effect_cook.h>
#include <xray/render/core/effect_descriptor.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/shader_macros.h>
#include <xray/render/core/shader_defines.h>
#include <xray/render/core/effect_manager.h>

#include "include_handler.h"

namespace xray {
namespace render {

effect_cook::effect_cook(): 
	resources::unmanaged_cook(
		resources::render_effect_class, 
		reuse_true, 
		use_current_thread_id,
		use_current_thread_id
	)
{
}

void effect_cook::on_all_tasks_finished(res_effect* effect_resource, resources::query_result_for_cook* in_out_query, effect_compile_data* cook_data)
{
	in_out_query->set_unmanaged_resource		(effect_resource, resources::nocache_memory, sizeof(res_effect));
	in_out_query->finish_query					(result_success );
	DELETE										(cook_data);
}

void effect_cook::on_binary_shaders(resources::query_result_for_cook* in_out_query,
									res_effect* effect_resource, 
									effect_compile_data* compile_data, 
									resources::queries_result& data)
{
	binary_shader_sources_type sources;
	
	bool has_one_at_least = false;
	for (u32 i = 0; i < data.size(); i++)
	{
// 		// All effect's shader must be created.
// 		if (!data[i].is_successful())
// 		{
// 			in_out_query->finish_query		(result_error);
// 			DELETE							(compile_data);
// 			return;
// 		}
// 		
// 		binary_shader_source_ptr	ptr		= static_cast_resource_ptr<binary_shader_source_ptr>(data[i].get_unmanaged_resource());
// 		binary_shader_key_type		key		(ptr->shader_name.c_str(), ptr->shader_type, ptr->configuration);
// 		sources[key]						= ptr;

		// All effect's shader must be created.
		if (data[i].is_successful())
		{
			binary_shader_source_ptr	ptr		= static_cast_resource_ptr<binary_shader_source_ptr>(data[i].get_unmanaged_resource());
			binary_shader_key_type		key		(ptr->shader_name.c_str(), ptr->shader_type, ptr->configuration);
			sources[key]						= ptr;
			has_one_at_least					= true;
		}
	}
	
	if (!has_one_at_least)
	{
		in_out_query->finish_query			(result_error);
		DELETE								(compile_data);
		return;
	}

	effect_compiler	compiler				(*effect_resource, false, &sources);
	compile_data->descriptor->compile		(compiler, compile_data->config->root());
	
	in_out_query->set_unmanaged_resource	(effect_resource, resources::nocache_memory, sizeof(res_effect));
	in_out_query->finish_query				(result_success );
	
	if (compile_data->add_to_array)
	{
	 	effect_manager::ref().add_effect	(compile_data->descriptor, compile_data->config, effect_resource);	
	}
	
	DELETE									(compile_data);
}

void effect_cook::create_resource(resources::query_result_for_cook&	in_out_query, 
								  const_buffer						raw_file_data, 
								  mutable_buffer					in_out_unmanaged_resource_buffer)
{
	XRAY_UNREFERENCED_PARAMETER				(raw_file_data);
	
	effect_compile_data* compile_data		= 0;
	if (in_out_query.user_data())
		in_out_query.user_data()->try_get	(compile_data);
	
	if (!compile_data)
	{
		in_out_query.finish_query			(result_error, assert_on_fail_false);
		DELETE								(compile_data);
		return;
	}
	
	//ASSERT_CMP								(compile_data->descriptor, !=, 0);
	//ASSERT_CMP								(compile_data->config.c_ptr(), !=, 0);
	
	if (!compile_data->descriptor)
	{
		LOG_ERROR("No effect descriptor!");
		in_out_query.finish_query			(result_error);
		DELETE								(compile_data);
		return;
	}
	
	if (!compile_data->config.c_ptr())
	{
		LOG_ERROR("No effect config!");
		in_out_query.finish_query			(result_error);
		DELETE								(compile_data);
		return;
	}
	
	res_effect* effect_resource				= new(in_out_unmanaged_resource_buffer.c_ptr())res_effect;
	effect_compiler	compiler				(*effect_resource, true);
	
	compile_data->descriptor->compile		(compiler, compile_data->config->root());
	
	effect_compiler::shader_cache_info_vector const& cached_shaders_info = compiler.get_cached_shaders_info();
	
	u32 const num_shader_types				= 3;
	u32 const num_shader_info				= cached_shaders_info.size();
	u32 const num_requests					= num_shader_info * num_shader_types;
	
	resources::creation_request* requests	= (resources::creation_request*)ALLOCA(sizeof(resources::creation_request) * num_requests);
	fs_new::virtual_path_string* pathes		= (fs_new::virtual_path_string*)ALLOCA(sizeof(fs_new::virtual_path_string) * num_requests);
	fs_new::virtual_path_string* pathes_init= pathes;
	
	for (u32 i = 0; i < num_requests; i++)
		new(pathes_init++)fs_new::virtual_path_string;
	
	u32 request_index						= 0;
	for (effect_compiler::shader_cache_info_vector::const_iterator	it = cached_shaders_info.begin(); 
																	it != cached_shaders_info.end(); 
																	++it)
	{
#ifndef MASTER_GOLD

		effect_resource->used_shaders_lock.lock();
		if (it->vertex_shader_name.length())	effect_resource->used_shaders.push_back(it->vertex_shader_name);
		if (it->pixel_shader_name.length())		effect_resource->used_shaders.push_back(it->pixel_shader_name);
		if (it->geometry_shader_name.length())	effect_resource->used_shaders.push_back(it->geometry_shader_name);
		effect_resource->used_shaders_lock.unlock();

#endif // #ifndef MASTER_GOLD
		
		// Request vertex shader.
		pathes[request_index]				= it->vertex_shader_name;
		new(&requests[request_index])resources::creation_request(
			pathes[request_index].c_str(),
			const_buffer(
				(pvoid)NEW(binary_shader_cook_data)(
					it->configuration, 
					pathes[request_index], 
					enum_shader_type_vertex, 
					true,
					effect_resource
				),
				sizeof(binary_shader_cook_data)
			),
			resources::shader_binary_source_class
		);
		request_index++;
		
		// Request geometry shader.
		pathes[request_index]				= it->geometry_shader_name;
		new(&requests[request_index])resources::creation_request(
			pathes[request_index].c_str(),
			const_buffer(
				(pvoid)NEW(binary_shader_cook_data)(
					it->configuration, 
					pathes[request_index], 
					enum_shader_type_geometry, 
					true,
					effect_resource
				),
				sizeof(binary_shader_cook_data)
			),
			resources::shader_binary_source_class
		);
		request_index++;
		
		// Request pixel shader.
		pathes[request_index]				= it->pixel_shader_name;
		new(&requests[request_index])resources::creation_request(
			pathes[request_index].c_str(),
			const_buffer(
				(pvoid)NEW(binary_shader_cook_data)(
					it->configuration, 
					pathes[request_index], 
					enum_shader_type_pixel, 
					true,
					effect_resource
				),
				sizeof(binary_shader_cook_data)
			),
			resources::shader_binary_source_class
		);
		request_index++;
	}

	in_out_query.finish_query				(result_postponed);
	
	resources::query_create_resources		(
		requests, 
		num_requests, 
		boost::bind(
			&effect_cook::on_binary_shaders, 
			this, 
			&in_out_query, 
			effect_resource, 
			compile_data, 
			_1
		), 
		in_out_query.get_user_allocator(),
		NULL,
		& in_out_query,
		assert_on_fail_false
	);
}

void effect_cook::destroy_resource(resources::unmanaged_resource* resource_to_destroy)
{
	effect_manager::ref().remove_effect		(static_cast<res_effect*>(resource_to_destroy));
	resource_to_destroy->~unmanaged_resource();
}

mutable_buffer	effect_cook::allocate_resource(resources::query_result_for_cook&	in_query, 
											   const_buffer							raw_file_data, 
											   bool									file_exist)
{
 	XRAY_UNREFERENCED_PARAMETERS			 (&file_exist, &raw_file_data, &in_query);
	return									 mutable_buffer((pvoid)MALLOC(sizeof(res_effect),"res_effect"), sizeof(res_effect));
}

void effect_cook::deallocate_resource(pvoid buffer) 
{
	FREE									 (buffer);
}

} // namespace render
} // namespace xray
