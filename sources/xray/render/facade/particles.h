////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PARTICLE_PARTICLES_BASE_H_INCLUDED
#define XRAY_PARTICLE_PARTICLES_BASE_H_INCLUDED

namespace xray {
namespace particle {

enum enum_particle_render_mode
{
	normal_particle_render_mode,
	dots_particle_render_mode,
	size_particle_render_mode,
}; // enum enum_particle_render_mode

struct particle_emitter_info
{
	fixed_string<64> emitter_name;
	fixed_string<64> material_name;
	u32				 max_particles;
}; // struct particle_emitter_info

} // namespace particle
} // namespace xray

#endif // #ifndef XRAY_PARTICLE_PARTICLES_BASE_H_INCLUDED