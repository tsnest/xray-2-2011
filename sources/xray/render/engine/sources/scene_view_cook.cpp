////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_view_cook.h"
#include "scene_manager.h"
#include "scene_view.h"

namespace xray {
namespace render {

scene_view_cook::scene_view_cook():
	resources::unmanaged_cook(xray::resources::scene_view_class, reuse_false, use_current_thread_id, use_current_thread_id)
{}

mutable_buffer scene_view_cook::allocate_resource(resources::query_result_for_cook& in_query, const_buffer raw_file_data, bool file_exist)
{
 	XRAY_UNREFERENCED_PARAMETERS(&in_query, file_exist, &raw_file_data);
	return mutable_buffer(ALLOC(u8, sizeof(xray::render::scene_view)), sizeof(xray::render::scene_view));
}

void scene_view_cook::deallocate_resource(pvoid buffer)
{
	FREE(buffer);
}

void scene_view_cook::create_resource(resources::query_result_for_cook& in_out_query, const_buffer raw_file_data, mutable_buffer in_out_unmanaged_resource_buffer)
{	
	XRAY_UNREFERENCED_PARAMETERS(&in_out_unmanaged_resource_buffer, &raw_file_data);
	
	scene_view* created_resource	= new(in_out_unmanaged_resource_buffer.c_ptr())xray::render::scene_view();
	scene_manager::ref().add_scene_view(created_resource);
	
	in_out_query.set_unmanaged_resource(created_resource, resources::nocache_memory, sizeof(xray::render::scene_view));
	in_out_query.finish_query(result_success);
}	

void scene_view_cook::destroy_resource(resources::unmanaged_resource* resource)
{	
 	scene_view* scn_view = static_cast_checked<scene_view*>( resource );
	
	scene_manager::ref().remove_scene_view(scn_view);
	
	scn_view->~scene_view();
}

} // namespace render
} // namespace xray