////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_DECAL_PROPERTIES_INLINE_H_INCLUDED
#define XRAY_RENDER_FACADE_DECAL_PROPERTIES_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline decal_properties::decal_properties()
{
	transform							= xray::math::float4x4().identity();
	material							= NULL;
	width_height_far_distance			= 0.5f * float3(1.0f, 1.0f, 1.0f);
	alpha_angle							= -1.0f;
	clip_angle							= -1.0f;
	projection_on_terrain_geometry		= true;
	projection_on_static_geometry		= true;
	projection_on_speedtree_geometry	= true;
	projection_on_skeleton_geometry		= true;
	projection_on_particle_geometry		= true;
}

inline decal_properties::decal_properties(
			math::float4x4						in_transform, // no scale
			resources::unmanaged_resource_ptr	in_material, // material_effects_instance_ptr
			math::float3						in_width_height_far_distance,
			float								in_alpha_angle,
			float								in_clip_angle,
			bool 								in_projection_on_terrain_geometry,
			bool 								in_projection_on_static_geometry,
			bool 								in_projection_on_speedtree_geometry,
			bool 								in_projection_on_skeleton_geometry,
			bool 								in_projection_on_particle_geometry
	):
		transform						(in_transform),
		width_height_far_distance		(in_width_height_far_distance),
		material						(in_material),
		alpha_angle						(in_alpha_angle),
		clip_angle						(in_clip_angle),
		projection_on_terrain_geometry	(in_projection_on_terrain_geometry),
		projection_on_static_geometry	(in_projection_on_static_geometry),
		projection_on_speedtree_geometry(in_projection_on_speedtree_geometry),
		projection_on_skeleton_geometry	(in_projection_on_skeleton_geometry),
		projection_on_particle_geometry	(in_projection_on_particle_geometry)
{
	// TODO: remove it
	transform.set_scale				(xray::math::float3(1.0f, 1.0f, 1.0f));
	width_height_far_distance		*= 0.5f;
}
	
inline bool decal_properties::is_project_on_all() const
{
	return		projection_on_terrain_geometry
			&&	projection_on_static_geometry
			&&	projection_on_speedtree_geometry
			&&  projection_on_skeleton_geometry 
			&&  projection_on_particle_geometry;
}

inline bool decal_properties::is_project_on_something() const
{
	return		projection_on_terrain_geometry
			||	projection_on_static_geometry
			||	projection_on_speedtree_geometry
			||  projection_on_skeleton_geometry 
			||  projection_on_particle_geometry;
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_DECAL_PROPERTIES_INLINE_H_INCLUDED