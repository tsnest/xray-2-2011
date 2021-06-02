////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/particle/api.h>
#include "particle_world_cooker.h"

namespace xray {
namespace particle {

void register_particle_world_cooker		( );
void unregister_particle_world_cooker	( );

} // namespace particle
} // namespace xray

void xray::particle::initialize	( )
{
	register_particle_world_cooker	( );
}

void xray::particle::finalize		( )
{
	unregister_particle_world_cooker( );
}