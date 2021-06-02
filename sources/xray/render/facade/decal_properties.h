////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_DECAL_PROPERTIES_H_INCLUDED
#define XRAY_RENDER_FACADE_DECAL_PROPERTIES_H_INCLUDED

namespace xray {
namespace render {

struct decal_properties
{
										decal_properties			();
										
										decal_properties			(
		math::float4x4						in_transform, // no scale
		resources::unmanaged_resource_ptr	in_material, // material_effects_instance_ptr
		math::float3						in_width_height_far_distance = float3(1.0f, 1.0f, 1.0f),
		float								in_alpha_angle = -1.0f,
		float								in_clip_angle = -1.0f,
		bool 								in_projection_on_terrain_geometry = true,
		bool 								in_projection_on_static_geometry = true,
		bool 								in_projection_on_speedtree_geometry = true,
		bool 								in_projection_on_skeleton_geometry = true,
		bool 								in_projection_on_particle_geometry = true
	);
	
	inline bool							is_project_on_all			() const;
	inline bool							is_project_on_something		() const;
	
	math::float4x4						transform; // no scale
	resources::unmanaged_resource_ptr	material; // material_effects_instance_ptr
	math::float3						width_height_far_distance;
	float								alpha_angle;
	float								clip_angle;
	bool								projection_on_terrain_geometry;
	bool								projection_on_static_geometry;
	bool								projection_on_speedtree_geometry;
	bool								projection_on_skeleton_geometry;
	bool								projection_on_particle_geometry;
}; // struct decal_properties

} // namespace render
} // namespace xray

#include "decal_properties_inline.h"

#endif // #ifndef XRAY_RENDER_FACADE_DECAL_PROPERTIES_H_INCLUDED