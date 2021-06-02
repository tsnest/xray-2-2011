////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_producer.h>
#include <xray/sound/world_user.h>
#include "sound_world.h"

namespace xray {
namespace sound {

sound_producer::sound_producer		( ) : 
	m_world_user	( 0 )
{}

sound_producer::~sound_producer		( )
{
	if ( m_world_user && !m_world_user->get_sound_world( )->is_destroying( ) )
	{
		LOG_DEBUG				( "~sound_producer" );
		m_world_user->mark_producer_as_deleted( (u64)this );
		
		functor_order* order	= XRAY_NEW_IMPL( m_world_user->get_allocator( ), functor_order )
		( 
			boost::bind( &world_user::on_producer_deleted, m_world_user, (u64)this )
		);

		m_world_user->add_order	( order );
	}
}

void sound_producer::on_play	( world_user& user ) const
{
	m_world_user		= &user;
}

} // namespace sound
} // namespace xray
