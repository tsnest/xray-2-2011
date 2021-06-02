////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2008
//	Author		: Konstantin Slipchenko
//	Description : gen_contact_base
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "generate_contact_base.h"

void	generate_contact_base::change_order	( )
{
	reverted = !reverted;
}

//#ifdef DEBUG
//#	include "island.h"
//#	include "scheduler.h"
//#	include "physics_world.h"
//
//render::debug::renderer& generate_contact_base::renderer( )	
//{
//	return isl.get_scheduler().world().renderer();
//}
//
//#endif // #ifdef DEBUG