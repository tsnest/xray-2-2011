////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_MOVEMENT_TARGET_H_INCLUDED
#define XRAY_AI_MOVEMENT_TARGET_H_INCLUDED

namespace xray {
namespace ai {

struct animation_item;

struct movement_target
{
	movement_target			(
		float3 const& position,
		float3 const& eyes_direction,
		float3 const& preferable_velocity,		
		animation_item const* const animation
	);

	float3					target_position;
	float3					direction;
	float3					velocity;
	animation_item const*	preferable_animation;
	fixed_string< 42 >		caption;
}; // struct movement_target

typedef fixed_vector< movement_target const*, 32 >		movement_targets_type;

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_MOVEMENT_TARGET_H_INCLUDED