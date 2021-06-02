////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_BASE_CLASSES_H_INCLUDED
#define XRAY_RENDER_ENGINE_BASE_CLASSES_H_INCLUDED

#include <xray/resources_resource_ptr.h>
#include <xray/resources_intrusive_base.h>
#include <xray/render/engine/api.h>

namespace xray {
namespace render {

class base_command;

struct base_scene : public resources::unmanaged_resource {
	inline	base_scene	( ) :
		first_command	( 0 ),
		last_command	( 0 )
	{
	}

	typedef	resources::resource_ptr <
		base_scene,
		resources::unmanaged_intrusive_base
	> scene_ptr;

	base_command*		first_command;
	base_command*		last_command;
	scene_ptr			next_scene;
}; // struct base_scene

typedef base_scene* scene_pointer;

struct base_scene_view : public resources::unmanaged_resource {
	inline	base_scene_view	( ) :
		first_command	( 0 ),
		last_command	( 0 )
	{
	}

	typedef	resources::resource_ptr <
		base_scene_view,
		resources::unmanaged_intrusive_base
	> scene_view_ptr;

	base_command*		first_command;
	base_command*		last_command;
	scene_view_ptr		next_scene_view;
}; // struct base_scene_view

typedef base_scene_view* scene_view_pointer;

struct base_output_window : public resources::unmanaged_resource { };

typedef	base_scene::scene_ptr			scene_ptr;
typedef	base_scene_view::scene_view_ptr	scene_view_ptr;

typedef	resources::resource_ptr <
	base_output_window,
	resources::unmanaged_intrusive_base
> render_output_window_ptr;

typedef base_output_window* render_output_window_pointer;

} // namespace render
} // namespace xray

template class XRAY_RENDER_ENGINE_API
	xray::resources::resource_ptr <
		xray::render::base_scene,
		xray::resources::unmanaged_intrusive_base
	>;

template class XRAY_RENDER_ENGINE_API
	xray::resources::resource_ptr <
		xray::render::base_scene_view,
		xray::resources::unmanaged_intrusive_base
	>;

template class XRAY_RENDER_ENGINE_API
	xray::resources::resource_ptr <
		xray::render::base_output_window,
		xray::resources::unmanaged_intrusive_base
	>;

#endif // #ifndef XRAY_RENDER_ENGINE_BASE_CLASSES_H_INCLUDED