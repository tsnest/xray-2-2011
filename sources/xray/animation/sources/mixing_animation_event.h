////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_ANIMATION_EVENT_H_INCLUDED
#define MIXING_ANIMATION_EVENT_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

enum time_event_types_enum {
	time_event_animation_lexeme_started					= u32(1) << 0,
	time_event_animation_lexeme_ended					= u32(1) << 1,
	time_event_animation_interval_ended					= u32(1) << 2,
	time_event_animation_ended_in_positive_direction	= u32(1) << 3,
	time_event_animation_ended_in_negative_direction	= u32(1) << 4,
	time_event_channel_callback_should_be_fired			= u32(1) << 5,
	time_event_time_direction_changed					= u32(1) << 6,
	time_event_weight_transitions_started				= u32(1) << 7,
	time_event_weight_transitions_ended					= u32(1) << 8,
	time_event_need_new_object_transform				=
		time_event_animation_lexeme_started |
		time_event_animation_lexeme_ended |
		time_event_animation_interval_ended |
		time_event_animation_ended_in_positive_direction |
		time_event_animation_ended_in_negative_direction |
		time_event_time_direction_changed |
		time_event_weight_transitions_started |
		time_event_weight_transitions_ended,

	time_event_all_events								=
		time_event_animation_lexeme_started | 
		time_event_animation_lexeme_ended |
		time_event_animation_interval_ended |
		time_event_animation_ended_in_positive_direction |
		time_event_animation_ended_in_negative_direction |
		time_event_channel_callback_should_be_fired |
		time_event_time_direction_changed |
		time_event_weight_transitions_started |
		time_event_weight_transitions_ended,
}; // time_event_types_enum

struct XRAY_ANIMATION_API animation_event {
	inline	animation_event( u32 const event_time_in_ms, u32 const event_type ) :
		animation_interval_id	( u32(-1) ),
		animation_interval_time	( xray::memory::uninitialized_value<float>() ),
		event_time_in_ms		( event_time_in_ms ),
		event_type				( event_type )
	{
	}

	inline	animation_event(
			u32 const animation_interval_id,
			float const animation_interval_time,
			u32 const event_time_in_ms,
			u32 const event_type
		) :
		animation_interval_id	( animation_interval_id ),
		animation_interval_time ( animation_interval_time ),
		event_time_in_ms		( event_time_in_ms ),
		event_type				( event_type )
	{
	}

	u32							animation_interval_id;
	float						animation_interval_time;
	u32							event_time_in_ms;
	u32							event_type;
}; // struct animation_event

inline bool operator == ( animation_event const& left, animation_event const& right )
{
	return
		(left.animation_interval_id == right.animation_interval_id) &&
		(left.animation_interval_time == right.animation_interval_time) &&
		(left.event_time_in_ms == right.event_time_in_ms) &&
		(left.event_type == right.event_type);
}

inline bool operator != ( animation_event const& left, animation_event const& right )
{
	return						!(left == right);
}

inline bool operator < ( animation_event const& left, animation_event const& right )
{
	if ( left.event_time_in_ms < right.event_time_in_ms )
		return					true;

	if ( left.event_time_in_ms > right.event_time_in_ms )
		return					false;

	return						left.event_type < right.event_type;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_ANIMATION_EVENT_H_INCLUDED