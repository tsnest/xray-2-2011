////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_instance_proxy_order.h"
#include <xray/sound/world_user.h>
#include "sound_world.h"

namespace xray {
namespace sound {

sound_instance_proxy_order::sound_instance_proxy_order	(
				world_user& user,
				sound_instance_proxy_internal& proxy,
				functor_type const& functor_order
			) :
		m_world_user_base	( user ),
		m_proxy			( proxy ),
		m_functor		( functor_order )
{
}

void	sound_instance_proxy_order::execute	( )
{
	return m_world_user_base.get_sound_world( )->try_process_order( this );
}


destroy_sound_instance_proxy_order::destroy_sound_instance_proxy_order	( world_user& user, sound_instance_proxy_internal& proxy, sound_scene_ptr sound_scene ) :
	m_user					( user ),
	m_proxy					( proxy ),
	m_state					( internal_state_initialize_destruction ),
	m_sound_scene			( sound_scene )
{
}

destroy_sound_instance_proxy_order::~destroy_sound_instance_proxy_order( )
{
}

void	destroy_sound_instance_proxy_order::execute	( )
{
	return m_user.get_sound_world( )->process_destroy_sound_instance_proxy ( this );
}


} // namespace sound
} // namespace xray