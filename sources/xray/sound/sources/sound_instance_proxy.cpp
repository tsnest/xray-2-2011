////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_instance_proxy.h>
#include <xray/sound/sound_receiver.h>
#include <xray/sound/sound_instance_emitter.h>
#include <xray/sound/sound_emitter.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/sound/world_user.h>
#include "sound_world.h"
#include "sound_instance_proxy_order.h"
#include "sound_scene.h"

namespace xray {
namespace sound {

static u32 s_sound_instance_proxy_id		= 0;
	//sound::play_once_and_forget	(
	//	sound_instance_proxy_ptr& proxy,
	//	play_params
	//)

sound_instance_proxy::sound_instance_proxy	( ) :
	positional_unmanaged_resource			( 3 ), 
	m_is_playing							( false ),
	m_reference_count						( 0 ),
	m_callback_pending						( false ),
	m_is_playing_once						( false ),
	m_id									( s_sound_instance_proxy_id++ ),
	m_is_producing_paused					( false ),
	m_is_propagating_paused					( false ),
	m_callback								( 0 )
{
}

sound_instance_proxy_ptr sound_instance_proxy::deserialize	(	
	boost::function < void ( void ) >& fn,
	memory::reader* r,
	sound_scene_ptr& scene,
	world_user& user,
	sound_emitter const& emt,
	callback_type const& callback )
{
	XRAY_UNREFERENCED_PARAMETERS		( &fn, &r, &scene, &user, &emt, &callback );
 //	world_user& specific_user	= static_cast_checked< world_user& >( user );
	//sound_scene& scn			= static_cast_checked< sound_scene& >( *scene.c_ptr() );

	//u64 address					= r->r_u64( );
	//sound_propagator_emitter const* propagator_emitter	= emt.get_sound_propagator_emitter( address );

	//sound_instance_proxy_ptr pr = scn.create_sound_instance_proxy( emt, *propagator_emitter, specific_user );
	//static_cast_checked< sound_instance_proxy_internal* >( pr.c_ptr() )->deserialize	( fn, r, callback );
	////pr->deserialize				( fn, r, callback );
	//return pr;
	return sound_instance_proxy_ptr( 0 );
}

} // namespace sound
} // namespace xray
