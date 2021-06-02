////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PARTICLE_BASE_PARTICLE_INLINE_H_INCLUDED
#define XRAY_PARTICLE_BASE_PARTICLE_INLINE_H_INCLUDED

namespace xray {
namespace particle {

inline void base_particle::kill					()
{
	lifetime		= 1000.0f;
	duration		= 100.0f;
}

inline float base_particle::get_duration		() const 
{
	if (duration>math::epsilon_3)
	{
		return duration;
	}
	return 1.0f;
}

inline float base_particle::get_linear_lifetime	() const
{
	if (duration>math::epsilon_3)
	{
		return lifetime / duration;
	}
	else
	{
		return lifetime;
	}
}

inline float base_particle::get_lifetime		() const
{
	return lifetime;
}

inline u32 base_particle::seed					() const
{
	return m_seed;
}

inline	bool base_particle::is_dead				() const 
{ 
	if (duration>math::epsilon_3)
	{
		return lifetime > duration;
	}
	else
	{
		return false;
	}
}

inline void base_particle::set_defaults			()
{
	color					= xray::math::float4(1.0f,1.0f,1.0f,1.0f);
	target_color_y_position = 0.5f;
	lifetime				= 0.0f;
	duration				= 0.0f;
	next					= 0;
	position				= xray::math::float3(0.0f,0.0f,0.0f);
	spawn_position			= xray::math::float3(0.0f,0.0f,0.0f);
	old_position			= xray::math::float3(0.0f,0.0f,0.0f);
	rotation				= 0.0f;
	rotation_rate			= xray::math::float3(0.0f,0.0f,0.0f);
	start_rotation_rate		= xray::math::float3(0.0f,0.0f,0.0f);
	rotationY				= 0.0f;
	rotationZ				= 0.0f;
	size					= xray::math::float3(1.0f,1.0f,1.0f);
	start_size				= xray::math::float3(1.0f,1.0f,1.0f);
	start_velocity			= xray::math::float3(0.0f,0.0f,0.0f);
	velocity				= xray::math::float3(0.0f,0.0f,0.0f);
	next					= 0;
	subimage_index			= 0.0f;
	next_subimage_index		= 0.0f;
	prev_offset_position	= xray::math::float3(0.0f,0.0f,0.0f);
	m_seed					= 0;
}

} // namespace particle
} // namespace xray

#endif // #ifndef XRAY_PARTICLE_BASE_PARTICLE_INLINE_H_INCLUDED