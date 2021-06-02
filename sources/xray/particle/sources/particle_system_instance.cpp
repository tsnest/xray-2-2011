////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_system_instance.h"
#include "particle_world.h"

namespace xray {
namespace particle {

particle_system_instance::particle_system_instance( particle_world& particle_world ):
	m_particle_world	( particle_world )
{
}

void particle_system_instance::play()
{
	m_particle_world.play(this, xray::math::float4x4().identity(), false, false);
}

void particle_system_instance::play(xray::math::float4x4 const& transform)
{
	m_particle_world.play(this, transform, true, false);
}

void particle_system_instance::set_transform(xray::math::float4x4 const& transform)
{
	m_particle_world.set_transform_to_particle_instance(this, transform);
}

void particle_system_instance::stop(float time)
{
	m_particle_world.stop(this, time);
}

} // namespace particle
} // namespace xray