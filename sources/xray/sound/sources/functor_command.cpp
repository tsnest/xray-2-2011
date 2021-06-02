////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "functor_command.h"
#include <xray/sound/world_user.h>


namespace xray {
namespace sound {

notify_receiver_command::notify_receiver_command ( functor_type const& receiver_function,
												  world_user& user,
												  u64 producer_address,
												  u64 receiver_address ) :
	m_functor			( receiver_function ),
	m_user				( user ),
	m_producer_address	( producer_address ),
	m_receiver_address	( receiver_address ) 
{
}

void notify_receiver_command::execute	( )
{
	if ( m_user.is_producer_deleted( m_producer_address ) || m_user.is_receiver_deleted( m_receiver_address ) )
		return;

	m_functor				( );
}


} // namespace sound
} // namespace xray
