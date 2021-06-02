////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_EFFECT_COOK_H_INCLUDED
#define XRAY_RENDER_EFFECT_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include <xray/render/core/shader_configuration.h>

namespace xray {
namespace render {

class res_effect;
enum enum_shader_type;

struct compile_shader_info
{
	fs_new::virtual_path_string		shader_name;
	shader_configuration			configuration;
	enum_shader_type				shader_type;
	bool							is_need_compile;
}; // struct compile_shader_info

struct effect_compile_data;

class effect_cook: public resources::unmanaged_cook
{
public:
								effect_cook					();
	virtual	mutable_buffer		allocate_resource			(resources::query_result_for_cook &	in_query, 
															 const_buffer						raw_file_data, 
															 bool								file_exist);
	virtual void				deallocate_resource			(pvoid								buffer);
	virtual void				create_resource				(resources::query_result_for_cook &	in_out_query, 
										 					 const_buffer						raw_file_data, 
															 mutable_buffer						in_out_unmanaged_resource_buffer); 
	virtual void				destroy_resource			(resources::unmanaged_resource *	resource);
			void				on_all_tasks_finished		(res_effect* effect_resource, 
															 resources::query_result_for_cook* in_out_query,
															 effect_compile_data* cook_data);
#if 0			
			void				on_converted_shaders_loaded	(res_effect* effect_resource, 
															 compile_shader_info* shader_info,
															 effect_compile_data* cook_data,
															 resources::query_result_for_cook* in_out_query,
															 u32 num_requests,
															 resources::queries_result& data);
#endif // #if 0			
			void				on_binary_shaders			(resources::query_result_for_cook* in_out_query,
															 res_effect* effect_resource, 
															 effect_compile_data* compile_data, 
															 resources::queries_result& data);
}; // struct effect_cook

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_EFFECT_COOK_H_INCLUDED