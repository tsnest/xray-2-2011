////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_output_window_cook.h"
#include "scene_manager.h"
#include "render_output_window.h"

namespace xray {
namespace render {

render_output_window_cook::render_output_window_cook():
	resources::unmanaged_cook(xray::resources::render_output_window_class, reuse_false, use_current_thread_id, use_current_thread_id)
{}

mutable_buffer render_output_window_cook::allocate_resource(resources::query_result_for_cook& in_query, const_buffer raw_file_data, bool file_exist)
{
 	XRAY_UNREFERENCED_PARAMETERS(&in_query, file_exist, &raw_file_data);
	return mutable_buffer(ALLOC(u8, sizeof(xray::render::render_output_window)), sizeof(xray::render::render_output_window));
}

void render_output_window_cook::deallocate_resource(pvoid buffer)
{
	FREE(buffer);
}

void render_output_window_cook::create_resource(resources::query_result_for_cook& in_out_query, const_buffer raw_file_data, mutable_buffer in_out_unmanaged_resource_buffer)
{	
	XRAY_UNREFERENCED_PARAMETERS(&in_out_unmanaged_resource_buffer, &raw_file_data);
	
	HWND hwnd = NULL;
	
	if( in_out_query.user_data() )
		in_out_query.user_data()->try_get(hwnd);
	
	ASSERT_CMP(hwnd, !=, 0);
	
	render_output_window* created_resource	= new(in_out_unmanaged_resource_buffer.c_ptr())xray::render::render_output_window(hwnd);
	scene_manager::ref().add_render_output_window(created_resource);
	
	in_out_query.set_unmanaged_resource(created_resource, resources::nocache_memory, sizeof(xray::render::render_output_window));
	in_out_query.finish_query(result_success);
}	

void render_output_window_cook::destroy_resource(resources::unmanaged_resource* resource)
{	
 	render_output_window* output_window = static_cast_checked<render_output_window*>( resource );
	
	scene_manager::ref().add_render_output_window(output_window);
	
	output_window->~render_output_window();
}

} // namespace render
} // namespace xray