////////////////////////////////////////////////////////////////////////////
//	Created		: 21.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_SCENE_COOK_H_INCLUDED
#define XRAY_RENDER_SCENE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

class scene;

class scene_cook : public resources::unmanaged_cook
{
public:
	scene_cook										();
	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook& in_query, const_buffer raw_file_data, bool file_exist);
	virtual void			deallocate_resource		(pvoid buffer);
	virtual void			create_resource			(resources::query_result_for_cook& in_out_query, const_buffer raw_file_data, mutable_buffer in_out_unmanaged_resource_buffer);
	virtual void			destroy_resource		(resources::unmanaged_resource* resource);
	
private:
			void			on_particle_world_created(resources::queries_result& result, xray::render::scene* created_resource, resources::query_result_for_cook* in_out_query);
}; // class scene_cook

void register_scene_cook();
void unregister_scene_cook();

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_SCENE_COOK_H_INCLUDED