////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SENSED_OBJECT_H_INCLUDED
#define SENSED_OBJECT_H_INCLUDED

#include <xray/loose_ptr.h>
#include <xray/ai/game_object.h>
#include "sensed_object_types.h"

namespace xray {
namespace ai {
namespace sensors {

struct sensed_object
{
	sensed_object		(
		float3 owner_position,
		float3 target_direction,
		game_object_ptr const& game_object,
		u32 time,
		sensed_object_types	sensed_object_type,
		float certainty
	) :
	position			( owner_position ),
	direction			( target_direction ),
	object				( game_object ),
	update_time			( time ),
	type				( sensed_object_type ),
	confidence			( certainty )
	{
	}

	float3				position;
	float3				direction;
	game_object_ptr		object;

	u32					update_time;
	
	sensed_object_types	type;
	float				confidence;
}; // struct sensed_object

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef SENSED_OBJECT_H_INCLUDED