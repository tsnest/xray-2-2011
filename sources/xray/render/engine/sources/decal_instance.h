////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_DECAL_INSTANCE_H_INCLUDED
#define XRAY_RENDER_ENGINE_DECAL_INSTANCE_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/facade/decal_properties.h>
#include "material_effects.h"
#include <xray/collision/object.h>


namespace xray {

namespace collision {
	struct space_partitioning_tree;
	class geometry;
	class object;
} // namespace collision

namespace render {

enum enum_render_stage_type;
class shader_constant_host;
class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

class renderer_context;
class untyped_buffer;
class resource_intrusive_base;
typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class decal_shader_constants_and_geometry: public quasi_singleton<decal_shader_constants_and_geometry>
{
public:
							decal_shader_constants_and_geometry	();
	void					set									(xray::render::renderer_context* context,
																 math::float4x4 const& world_to_decal_matrix,
																 math::float4x4 const& decal_tangent_to_view_space_matrix,
																 float alpha_angle,
																 float clip_angle,
																 math::float3 const& decal_width_height_far_distance,
																 math::float4x4 const& decal_transform);
	void					set_geometry						();
private:
	void					create_decal_geometry				();
	shader_constant_host*	m_world_to_decal_parameter;
	shader_constant_host*	m_eye_ray_corner_parameter;
	shader_constant_host*	m_decal_tangent_to_view_space_matrix_parameter;
	shader_constant_host*	m_decal_angle_parameters;
	untyped_buffer_ptr		m_decal_vertex_buffer;
	untyped_buffer_ptr		m_decal_index_buffer;
	ref_geometry			m_decal_geometry;
}; // class decal_shader_constants_and_geometry

struct decal_instance : public boost::noncopyable {
							decal_instance		(collision::space_partitioning_tree* tree, 
												 decal_properties const& properties, 
												 u32 const id);
							~decal_instance		();
	void					set_properties		(decal_properties const& in_properties);
	decal_properties const& get_properties		() const;
	material_effects const& get_effects			() const;
	
	u32						draw				(xray::render::renderer_context* context,
												 ref_effect opaque_geometry_mask_effect,
												 enum_render_stage_type stage_type);
	
private:
	void					render				(xray::render::renderer_context* context,
												 enum_render_stage_type stage_type);
	void					render_geometry		();
	void					set_materail_effects(resources::unmanaged_resource_ptr const& in_ptr);
	void					remove_collision	();
	friend class								scene;
	decal_properties							m_properties;
	u32											m_id;
	decal_instance*								m_next;
	collision::space_partitioning_tree* const	m_collision_tree;
	collision::geometry_instance*				m_collision_geometry;
	collision::object*							m_collision_object;
}; // class decal_instance

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_DECAL_INSTANCE_H_INCLUDED
