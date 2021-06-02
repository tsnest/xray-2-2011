////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHT_PROPS_H_INCLUDED
#define LIGHT_PROPS_H_INCLUDED

namespace xray {
namespace render {

enum light_type {
	light_type_point,
	light_type_spot,

	light_type_obb,
	light_type_capsule,
	
	light_type_parallel,
	
	light_type_sphere,
	light_type_plane_spot,
	
	num_light_types,
}; // enum light_type

struct light_props {
	float4x4		transform;
	float			attenuation_power;
	float			intensity;
	float			range;
	float			spot_umbra_angle;
	float			spot_penumbra_angle;
	float			spot_falloff;
	float			diffuse_influence_factor;
	float			specular_influence_factor;
	u32				color;
	light_type		type;
	int				lighting_model;
	
	u32				sun_shadow_map_size;
	u32				num_sun_cascades;
	
	u32				shadow_map_size;
	u32				shadow_map_size_index;

	float			local_light_z_bias;
	float			shadow_transparency;
	bool			shadow_distribution_sides[6];
	
	bool			does_cast_shadows;
	
	bool			shadower;
	
	u32					m_year;
	u32					m_month;
	u32					m_day;
	u32					m_hours;
	u32					m_minutes;
	u32					m_seconds;
	bool				m_use_auto_lacation;
	
	bool			use_with_lpv;
}; // struct light_props

} // namespace render
} // namespace xray

#endif // #ifndef LIGHT_PROPS_H_INCLUDED
