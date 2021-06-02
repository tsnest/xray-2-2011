////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef NPC_EMPTY_SWITCHER_BEHAVIOUR_H_INCLUDED
#define NPC_EMPTY_SWITCHER_BEHAVIOUR_H_INCLUDED

#include "object_behaviour.h"

namespace stalker2 {

	class npc_empty_switcher_behaviour : public object_behaviour
	{
		typedef object_behaviour super;

	public:
		npc_empty_switcher_behaviour( object_scene_job* owner, configs::binary_config_value const& data, pcstr name )
		: super ( owner, data, name )
		{

		}
	}; // class npc_reach_position_behaviour

} // namespace stalker2

#endif // #ifndef NPC_EMPTY_SWITCHER_BEHAVIOUR_H_INCLUDED