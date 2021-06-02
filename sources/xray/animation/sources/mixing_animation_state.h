////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_ANIMATION_STATE_H_INCLUDED
#define MIXING_ANIMATION_STATE_H_INCLUDED

#include <xray/animation/cubic_spline_skeleton_animation.h>
#include "mixing_n_ary_tree_event_iterator.h"

namespace xray {
namespace animation {

struct bone_mixer_data;

namespace mixing {

class animation_interval;
class animation_clip;
class n_ary_tree_animation_node;

struct XRAY_ANIMATION_API object_movement {
	math::quaternion			rotation;
	float3						translation;
	float3						scale;
}; // struct object_movement

struct XRAY_ANIMATION_API bone_matrices_computer_data {
	inline	bone_matrices_computer_data	( ) : pinned_animation( 0 ) { }

	object_movement				previous_object_movement;
	object_movement				accumulated_object_movement;

	resources::pinned_ptr_const< cubic_spline_skeleton_animation >	pinned_animation;
}; // struct bone_matrices_computer_data

//struct time_calculator_data {
//	u32							last_integration_interval_id;
//	float						last_integration_interval_value;
//}; // struct time_calculator_data

struct XRAY_ANIMATION_API animation_state {
								animation_state	(
									n_ary_tree_animation_node& animation_node,
									u32 time_in_ms,
									u32 initial_event_types,
									u32 animation_interval_id,
									float animation_interval_time,
									float weight,
									subscribed_channel*& channels_head
								);
	inline animation_state&		get_this		( ) { return *this; }

	bone_matrices_computer_data	bone_matrices_computer;
//	time_calculator_data		time_calculator;
	// it's a pity but ordering of the next 3 members is important here
	u32							animation_interval_id;
	float						animation_interval_time;
	n_ary_tree_event_iterator	event_iterator;
	float						weight;
	float						animation_time;
	float						animation_time_threshold;
	bool						are_there_any_weight_transitions;
}; // struct animation_state

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_ANIMATION_STATE_H_INCLUDED