////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_MODEL_WRAPPER_H_INCLUDED
#define RENDER_MODEL_WRAPPER_H_INCLUDED

#pragma managed( push, off )
#include <xray/render/facade/model.h>
#pragma managed( pop )

namespace xray {
namespace model_editor {

ref class render_model_wrapper
{
public:
											render_model_wrapper	( );
											~render_model_wrapper	( );
	render::render_model_instance_ptr		get_render_model_		( );
	collision::geometry_ptr					get_collision_geometry	( );
	void									remove_from_scene		( render::scene_renderer& scene_renderer, render::scene_ptr const& scene_ptr );
	void									add_to_scene			( render::scene_renderer& scene_renderer, render::scene_ptr const& scene_ptr, math::float4x4 const& m );
	void									move					( render::scene_renderer& scene_renderer, render::scene_ptr const& scene_ptr, math::float4x4 const& m );
	void									set_resource			( resources::unmanaged_resource_ptr model, resources::unmanaged_resource_ptr collision );
	void									reset					( );
	bool									empty					( );
private:
	render::static_model_ptr*				m_static_model;
	render::skeleton_model_ptr*				m_skeleton_model;
	collision::geometry_ptr*				m_hq_collision;
	int										mode;
};

} // namespace model_editor
} // namespace xray

#endif // #ifndef RENDER_MODEL_WRAPPER_H_INCLUDED