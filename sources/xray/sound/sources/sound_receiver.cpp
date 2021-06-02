////////////////////////////////////////////////////////////////////////////
//	Created		: 04.05.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_receiver.h>
#include <xray/linkage_helper.h>
#include <xray/sound/sound_object_commands.h>
#include <xray/sound/world_user.h>
#include "sound_scene.h"

//DECLARE_LINKAGE_ID(sound_receiver_linkage_id);

namespace xray {
namespace sound {

sound_receiver::sound_receiver			( ) : m_position( 0 ) { }

sound_receiver::~sound_receiver			( )	{ }

void sound_receiver::set_position		( float3 const& position )
{
	R_ASSERT					( m_position );
	m_position->set				( position );
}

void sound_receiver::on_register_receiver_order_created	( atomic_half3* position )
{
	R_ASSERT					( position );
	m_position					= position;
}

} // namespace sound
} // namespace xray
