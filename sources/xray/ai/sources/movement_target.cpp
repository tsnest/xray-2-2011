////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/movement_target.h>

namespace xray {
namespace ai {

movement_target::movement_target(
		float3 const& position,
		float3 const& eyes_direction,
		float3 const& preferable_velocity,		
		animation_item const* const animation
	) :
	target_position				( position ),
	direction					( eyes_direction ),
	velocity					( preferable_velocity ),
	preferable_animation		( animation )
{
	sprintf						(
		caption.c_str(),
		caption.fixed_size,
		"target position: %.2f %.2f %.2f",
		target_position.x,
		target_position.y,
		target_position.z
	);
}

} // namespace ai
} // namespace xray