////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_PLAY_ANIMATION_BEHAVIOUR_H_INCLUDED
#define NPC_PLAY_ANIMATION_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"

namespace stalker2 {

class npc_play_animation_behaviour : public object_behaviour
{
	typedef object_behaviour super;

public:
					npc_play_animation_behaviour( object_scene_job* owner, configs::binary_config_value const& data, pcstr name );
	private:
	virtual void	attach_to_object			( object_controlled* o );
	virtual void	detach_from_object			( object_controlled* o );	

			void	on_behaviour_loaded			( resources::queries_result& data );

private:
	resources::unmanaged_resource_ptr			m_behaviour;
	object_controlled*							m_object;
}; // class npc_play_animation_behaviour

} // namespace stalker2

#endif // #ifndef NPC_PLAY_ANIMATION_BEHAVIOUR_H_INCLUDED
