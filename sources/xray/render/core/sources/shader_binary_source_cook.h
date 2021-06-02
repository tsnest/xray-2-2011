////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SHADER_BINARY_SOURCE_COOK_H_INCLUDED
#define XRAY_RENDER_SHADER_BINARY_SOURCE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

struct shader_binary_source_cook_data
{
	fs_new::virtual_path_string		dest_path;
	pvoid				buffer;
	u32					size;
};
struct binary_shader_cook_data;
struct binary_shader_source;
class res_effect;

struct conveted_shader_loaded_data
{
	conveted_shader_loaded_data(
			resources::query_result_for_cook*	in_in_out_query,
			binary_shader_cook_data*			in_cook_data,
			binary_shader_source*				in_new_resource,
			fs_new::virtual_path_string						in_converted_shader_path):
		in_out_query(in_in_out_query),
		cook_data(in_cook_data),
		new_resource(in_new_resource),
		converted_shader_path(in_converted_shader_path)
	{}
	resources::query_result_for_cook*	in_out_query;
	binary_shader_cook_data*			cook_data;
	binary_shader_source*				new_resource;
	fs_new::virtual_path_string						converted_shader_path;
	pbyte								data_to_save;
	u32									save_data_size;
};

struct shader_binary_source_cook: public resources::unmanaged_cook
{
public:
								shader_binary_source_cook	();
	virtual						~shader_binary_source_cook	();

	virtual	mutable_buffer		allocate_resource			(resources::query_result_for_cook &	in_query, 
															 const_buffer						raw_file_data, 
															 bool								file_exist);
	
	virtual void				deallocate_resource			(pvoid								buffer);
	
	virtual void				create_resource				(resources::query_result_for_cook &	in_out_query, 
										 					 const_buffer						raw_file_data, 
															 mutable_buffer						in_out_unmanaged_resource_buffer); 
	virtual void				destroy_resource			(resources::unmanaged_resource *	resource);
	
	void						converted_shader_loaded		(conveted_shader_loaded_data* data,
															 resources::queries_result& result);
	
	void						save_binary_shader			(resources::query_result_for_cook*	in_out_query,
															 pvoid								data,
															 u32								size,
															 pcstr								shader_name);
	
	void						all_tasks_finished			(conveted_shader_loaded_data* data);
	
	void						compile_shader_task			(conveted_shader_loaded_data* data);
	
private:
	bool						is_need_recompile			(conveted_shader_loaded_data* data,
															 resources::managed_resource_ptr binary_shader_ptr) const;

private:
	tasks::task_type*			m_tasks_type;
}; // struct shader_binary_source_cook

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_SHADER_BINARY_SOURCE_COOK_H_INCLUDED