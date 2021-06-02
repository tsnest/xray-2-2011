////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_animation_event_iterator.h"
#include "mixing_n_ary_tree_time_in_ms_calculator.h"
#include <xray/animation/mixing_animation_interval.h>
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_animation_state.h"
#include <xray/animation/animation_callback.h>

using xray::animation::mixing::n_ary_tree_animation_event_iterator;
using xray::animation::mixing::animation_interval;
using xray::animation::mixing::animation_state;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::channel_event_callback_base;
using xray::animation::subscribed_channel;

n_ary_tree_animation_event_iterator::n_ary_tree_animation_event_iterator	(
		animation_state& animation_state,
		n_ary_tree_animation_node& animation_node,
		u32 const start_time_in_ms,
		u32 const event_types,
		subscribed_channel*& channels_head
	) :
	m_animation							( &animation_node ),
	m_channels_head						( &channels_head ),
	m_value								(
		animation_state.animation_interval_id,
		animation_state.animation_interval_time,
		start_time_in_ms,
		event_types
	)
{
	advance								( event_types );
}

float n_ary_tree_animation_event_iterator::get_nearest_animation_interval_event_time	(
		animation_interval const& interval,
		float const start_time,
		float target_time,
		u32& event_type
	)
{
	int const time_direction			= start_time <= target_time ? 1 : -1;

	cubic_spline_skeleton_animation_pinned pinned_animation( interval.animation() );

	event_type							= time_event_animation_interval_ended;
	float const animation_length		= pinned_animation->length_in_frames() / default_fps;
	float const animation_end_time		= animation_length - interval.start_time();
	if ( (time_direction*(interval.start_time() + start_time) < time_direction*animation_length ) ) {
		if ( time_direction*(interval.start_time() + target_time) >= time_direction*animation_length ) {
			if ( interval.start_time() + target_time == animation_length )
				event_type				|= time_direction == 1 ? time_event_animation_ended_in_positive_direction : time_event_animation_ended_in_negative_direction;
			else
				event_type				= time_direction == 1 ? time_event_animation_ended_in_positive_direction : time_event_animation_ended_in_negative_direction;

			target_time					= animation_end_time;
		}
	}
	u32 const channels_count			= pinned_animation->event_channels().channels_count();
	if ( !channels_count )
		return							target_time;

	for ( subscribed_channel const* subscribed_channel = *m_channels_head; subscribed_channel; subscribed_channel = subscribed_channel->next ) {
		u32 const channel_id			= pinned_animation->event_channels().get_channel_id( subscribed_channel->channel_id );
		if ( channel_id == u32(-1) )
			continue;

		event_channel const& channel	= pinned_animation->event_channels().channel( channel_id );
		u32 const knots_count			= channel.knots_count( );
		float const* current_knot		= time_direction == 1 ? channel.knots() : (channel.knots() + knots_count - 1);
		float const* const knots_end	= time_direction == 1 ? (channel.knots() + knots_count) : (channel.knots() - 1);
		for ( ; current_knot != knots_end; current_knot += time_direction ) {
			float const knot_time		=  (*current_knot) / default_fps - interval.start_time();
			R_ASSERT_CMP				( knot_time, >=, 0.f);
			if ( knot_time * time_direction <= start_time * time_direction )
				continue;

			if ( knot_time * time_direction > target_time * time_direction )
				break;

			if ( knot_time == target_time )
				event_type				|= time_event_channel_callback_should_be_fired;
			else
				event_type				= time_event_channel_callback_should_be_fired;

			target_time					= knot_time;
			break;
		}
	}

	return								target_time;
}

u32 n_ary_tree_animation_event_iterator::get_time_in_ms				(
		u32 const start_time_in_ms,
		float const time_from_interval_start,
		float& event_time,
		u32& event_type
	)
{
	xray::animation::mixing::n_ary_tree_time_in_ms_calculator time_in_ms_calculator(
		*(m_animation->driving_animation() ? m_animation->driving_animation() : m_animation),
		start_time_in_ms,
		time_from_interval_start,
		event_time,
		event_type
	);
	u32 const result					= time_in_ms_calculator.time_in_ms( );
	R_ASSERT_CMP						( start_time_in_ms, <=, result );
	event_type							= time_in_ms_calculator.event_type( );
	event_time							= time_in_ms_calculator.event_time( );
	return								result;
}

void n_ary_tree_animation_event_iterator::advance					( u32 const initial_event_types )
{
	animation_interval const* current_interval	= m_animation->animation_intervals() + m_value.animation_interval_id;
	R_ASSERT_CMP						( (*current_interval).length(), >=, m_value.animation_interval_time );

	xray::animation::mixing::n_ary_tree_time_scale_calculator time_scale_calculator(
		m_value.event_time_in_ms,
		m_value.animation_interval_time,
		m_value.event_time_in_ms,
		xray::animation::mixing::n_ary_tree_time_scale_calculator::forbid_transitions_destroying
	);
	n_ary_tree_animation_node& driving_animation = *(m_animation->driving_animation() ? m_animation->driving_animation() : m_animation);
	R_ASSERT							( !driving_animation.driving_animation() );
	(*driving_animation.operands( sizeof(n_ary_tree_animation_node) ))->accept	( time_scale_calculator );
	float time_scale					= time_scale_calculator.time_scale();
	// check if we start at the time moment, where time scale is 0,
	// but it changes over time
	// (it is possible to have animation with time scale = 0 as a constant)
	if ( !initial_event_types && (time_scale == 0.f) ) {
		xray::animation::mixing::n_ary_tree_time_scale_calculator time_scale_calculator(
			m_value.event_time_in_ms + 1,
			m_value.animation_interval_time,
			m_value.event_time_in_ms + 1,
			xray::animation::mixing::n_ary_tree_time_scale_calculator::forbid_transitions_destroying
		);
		(*driving_animation.operands( sizeof(n_ary_tree_animation_node) ))->accept	( time_scale_calculator );
		time_scale						= time_scale_calculator.time_scale();
	}

	if ( !initial_event_types && (time_scale == 0.f) ) {
		m_animation						= 0;
		m_value							= animation_event( u32(-1), 0 );
		return;
	}

	// here we assume, that driving and driven animations must have the same interval ids and interval cycles
	animation_interval const* current_driving_interval	= (m_animation->driving_animation() ? m_animation->driving_animation() : m_animation )->animation_intervals() + m_value.animation_interval_id;
	float const driving_animation_factor = current_driving_interval->length() / current_interval->length();
	m_value.event_type					= initial_event_types;
	u32 event_type;
	if ( time_scale >= 0.f ) {
		float const channel_event_time	= get_nearest_animation_interval_event_time( *current_interval, m_value.animation_interval_time, (*current_interval).length(), event_type );
		R_ASSERT_CMP					( channel_event_time, <=, (*current_interval).length() );
		float const event_time			= channel_event_time;//? xray::math::min( channel_event_time, (*current_interval).length() );
		float new_event_time			= event_time*driving_animation_factor;
		u32 const new_event_time_in_ms	= get_time_in_ms( m_value.event_time_in_ms, m_value.animation_interval_time*driving_animation_factor, new_event_time, event_type );
		if ( initial_event_types && (new_event_time_in_ms != m_value.event_time_in_ms) )
			return;

		new_event_time					/= driving_animation_factor;
		R_ASSERT_CMP					( m_value.event_time_in_ms, <=, new_event_time_in_ms );
		m_value.event_time_in_ms		= new_event_time_in_ms;
		m_value.animation_interval_time	= new_event_time;
		m_value.event_type				|= event_type;

		if ( m_value.event_type & time_event_animation_interval_ended ) {
			if ( ++m_value.animation_interval_id == m_animation->animation_intervals_count() )
				m_value.animation_interval_id	= m_animation->start_cycle_animation_interval_id( );

			m_value.animation_interval_time	= 0.f;
		}
	}
	else {
		float const channel_event_time	= get_nearest_animation_interval_event_time( *current_interval, m_value.animation_interval_time, 0.f, event_type );
		R_ASSERT_CMP					( channel_event_time, >=, 0.f );
		float const event_time			= channel_event_time;//? xray::math::max( channel_event_time, 0.f );
		float new_event_time			= event_time*driving_animation_factor;
		u32 const new_event_time_in_ms	= get_time_in_ms( m_value.event_time_in_ms, m_value.animation_interval_time*driving_animation_factor, new_event_time, event_type );
		if ( initial_event_types && (new_event_time_in_ms != m_value.event_time_in_ms) )
			return;

		new_event_time					/= driving_animation_factor;
		R_ASSERT_CMP					( m_value.event_time_in_ms, <=, new_event_time_in_ms );
		m_value.event_time_in_ms		= new_event_time_in_ms;
		m_value.animation_interval_time	= new_event_time;
		m_value.event_type				|= event_type;

		if ( m_value.event_type & time_event_animation_interval_ended ) {
			if ( !m_value.animation_interval_id || (--m_value.animation_interval_id < m_animation->start_cycle_animation_interval_id()) )
				m_value.animation_interval_id	= m_animation->animation_intervals_count() - 1;

			m_value.animation_interval_time	= m_animation->animation_intervals()[m_value.animation_interval_id].length( );
		}
	}
}

n_ary_tree_animation_event_iterator& n_ary_tree_animation_event_iterator::operator ++	( )
{
	advance								( 0 );
	return								*this;
}
