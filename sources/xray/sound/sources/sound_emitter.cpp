////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/sound_emitter.h>
#include <xray/sound/world_user.h>
#include "sound_world.h"
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(sound_emitter_linkage_id);

namespace xray {
namespace sound {

sound_emitter::sound_emitter	( )
{
	m_old_address		= 0;
}


sound_instance_proxy_ptr sound_emitter::emit( sound_scene_ptr& scene, world_user& user )
{
	world_user& specific_user		= static_cast_checked< world_user& >( user );
	sound_scene& scn				= static_cast_checked< sound_scene& >( *scene.c_ptr( ) );

	sound_instance_proxy_ptr result	= scn.create_sound_instance_proxy
									(
										this,
										*get_sound_propagator_emitter( ),
										specific_user
									);

	return							result;
}

sound_instance_proxy_ptr sound_emitter::emit_point_sound( sound_scene_ptr& scene, world_user& user )
{
	world_user& specific_user		= static_cast_checked< world_user& >( user );
	sound_scene& scn				= static_cast_checked< sound_scene& >( *scene.c_ptr( ) );

	sound_instance_proxy_ptr proxy	= scn.create_sound_instance_proxy
									(
										this,
										*get_sound_propagator_emitter( ),
										specific_user
									);

	return							proxy;
}

sound_instance_proxy_ptr sound_emitter::emit_spot_sound	(	sound_scene_ptr& scene,
															world_user& user,
															sound_cone_type cone_type
														)
{
	world_user& specific_user		= static_cast_checked< world_user& >( user );
	sound_scene& scn				= static_cast_checked< sound_scene& >( *scene.c_ptr( ) );

	sound_instance_proxy_ptr proxy	= scn.create_sound_instance_proxy
									(
										this,
										*get_sound_propagator_emitter( ),
										specific_user,
										cone_type
									);

	return							proxy;
}

sound_instance_proxy_ptr sound_emitter::emit_volumetric_sound	(	sound_scene_ptr& scene,
																	world_user& user,
																	collision::geometry_instance& non_shared_instance,
																	float position_to_stereo_radius
																)
{
	world_user& specific_user		= static_cast_checked< world_user& >( user );
	sound_scene& scn				= static_cast_checked< sound_scene& >( *scene.c_ptr( ) );

	sound_instance_proxy_ptr proxy	= scn.create_sound_instance_proxy
									(
										this,
										*get_sound_propagator_emitter( ),
										specific_user,
										non_shared_instance,
										position_to_stereo_radius
									);

	return							proxy;
}

void sound_emitter::emit_and_play_once	(
											sound_scene_ptr& scene,
											world_user& user,
											float3 const& position,
											sound_producer const* producer,
											sound_receiver const* ignorable_receiver
										)
{
	world_user& specific_user			= static_cast_checked< world_user& >( user );
	sound_scene& scn					= static_cast_checked< sound_scene& >( *scene.c_ptr( ) );

	sound_instance_proxy_ptr result		= scn.create_sound_instance_proxy
										(
											this,
											*get_sound_propagator_emitter( ),
											specific_user
										);

	sound_instance_proxy_internal* p	= static_cast_checked< sound_instance_proxy_internal* >( result.c_ptr( ) );
	if ( p )
		p->play_once					( position, producer, ignorable_receiver );
}

} // namespace sound
} // namespace xray