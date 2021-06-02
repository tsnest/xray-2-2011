////////////////////////////////////////////////////////////////////////////
//	Created		: 17.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_animation_state.h"
#include "mixing_n_ary_tree_animation_node.h"

using xray::animation::mixing::animation_state;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::skeleton_animation_ptr;

animation_state::animation_state	(
		n_ary_tree_animation_node& animation_node,
		u32 const time_in_ms,
		u32 const initial_event_types,
		u32 const animation_interval_id,
		float const animation_interval_time,
		float const weight,
		xray::animation::subscribed_channel*& channels_head
	) :
	animation_interval_id	( animation_interval_id ),
	animation_interval_time	( animation_interval_time ),
	animation_time			( animation_node.animation_intervals()[ animation_interval_id ].start_time() + animation_interval_time ),
	event_iterator			( get_this(), animation_node, time_in_ms, initial_event_types, channels_head ),
	weight					( weight ),
	are_there_any_weight_transitions( false )
{
	float const animation_length	= cubic_spline_skeleton_animation_pinned(animation_node.animation_intervals()[ animation_interval_id ].animation())->length_in_frames()/default_fps;
	if ( animation_time >= animation_length ) {
		animation_time				-= animation_length;
		animation_time_threshold	= animation_length;
	}
	else
		animation_time_threshold	= 0.f;

#ifndef MASTER_GOLD
	if ( !animation_node.driving_animation() )
		return;

	animation_state const& driving_animation_state	= animation_node.driving_animation()->animation_state();
	R_ASSERT_CMP			( animation_interval_id, ==, driving_animation_state.animation_interval_id );
	animation_interval const* const driving_animation_interval	= animation_node.driving_animation()->animation_intervals() + driving_animation_state.animation_interval_id;
	animation_interval const* const animation_interval			= animation_node.animation_intervals() + animation_interval_id;
	R_ASSERT_CMP			( animation_interval_time, ==, animation_interval->length() / driving_animation_interval->length() * driving_animation_state.animation_interval_time );
#endif // #ifndef MASTER_GOLD
}