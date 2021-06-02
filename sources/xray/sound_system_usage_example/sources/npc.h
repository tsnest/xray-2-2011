////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/sound/sound_receiver.h>
#include <xray/sound/sound_emitter.h>
#include <xray/sound/sound_instance_proxy.h>
#include <xray/sound/sound_producer.h>

namespace xray {
namespace sound_system_usage_example {

class npc : public sound::sound_receiver,
			public sound::sound_producer
{
public:
					npc					(
											float3 const& position,
											float3 const& direction,
											sound::sound_emitter_ptr sound_emitter,
											pcstr description
										);
					~npc				( );

			void	register_as_sound_receiver	( sound::sound_scene_ptr scene, sound::world_user& user );
			void	unregister_as_sound_receiver( sound::sound_scene_ptr scene, sound::world_user& user );

			void	emit				( sound::sound_scene_ptr scene, sound::world_user& user );
			void	say					( );
			void	say_once			( sound::sound_scene_ptr scene, sound::world_user& user );

	virtual pcstr	get_description		( ) const;
	virtual	float3	get_source_position	( float3 const& requester ) const;

	virtual void	on_sound_event	( sound::sound_producer const& sound_source );

	sound::sound_instance_proxy_ptr		get_sound		( ) { return m_proxy; }

private:
	float3								m_position;
	float3								m_direction;
	sound::sound_emitter_ptr			m_sound_emitter;
	sound::sound_instance_proxy_ptr		m_proxy;
	pcstr								m_description;

}; // class npc

} // namespace sound_system_usage_example
} // namespace xray

#endif // #ifndef NPC_H_INCLUDED