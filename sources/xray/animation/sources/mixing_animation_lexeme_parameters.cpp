////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include <xray/animation/cubic_spline_skeleton_animation.h>

using xray::animation::mixing::animation_lexeme_parameters;
using xray::animation::skeleton_animation_ptr;

static char animation_intervals_channel_id[] = "anim_intervals";

void animation_lexeme_parameters::create_animation_intervals( skeleton_animation_ptr const& animation )
{
	animation_interval* const animation_intervals	= static_cast<animation_interval*>( m_buffer.c_ptr() );
	R_ASSERT						( animation_intervals == m_animation_intervals );
	m_buffer						+= m_animation_intervals_count * sizeof(animation_interval);

	cubic_spline_skeleton_animation_pinned pinned_animation( animation );
	float const animation_length	= pinned_animation->length_in_frames()/default_fps;

	u32 const channel_id			= pinned_animation->event_channels().get_channel_id( animation_intervals_channel_id );
	if ( channel_id == u32(-1) ) {
#ifndef MASTER_GOLD
		LOG_WARNING					( "animation [%s] has no animation intervals, assuming single interval", m_identifier );
#endif
		new ( animation_intervals ) animation_interval( animation, 0.f, animation_length );
		m_start_animation_interval_id	= 0;
		m_start_animation_interval_time	= 0.f;
		return;
	}

	event_channel const& channel	= pinned_animation->event_channels().channel( channel_id );

	R_ASSERT_CMP					( m_animation_intervals_count, ==, channel.domains_count() );
	R_ASSERT						( channel.knots_count() );
	float previous_knot				= channel.knot(0);
	for ( u32 knot_id = 1, knots_count = channel.knots_count(), interval_id = 0; knot_id < knots_count; ++knot_id, ++interval_id ) {
		float const knot			= channel.knot(knot_id);
		animation_interval* interval = animation_intervals + interval_id;
		new ( interval ) animation_interval( animation, previous_knot/default_fps, (knot - previous_knot)/default_fps );
		if ( interval->start_time() + interval->length() > animation_length ) {
			m_start_animation_interval_id	= interval_id;
			m_start_animation_interval_time	= animation_length - interval->start_time();
			R_ASSERT_CMP			( knot_id + 1, ==, knots_count );
			break;
		}
		previous_knot				= knot;
	}
}

u32 animation_lexeme_parameters::animation_intervals_count	( skeleton_animation_ptr const& animation )
{
	cubic_spline_skeleton_animation_pinned pinned_animation( animation );

	u32 const channel_id			= pinned_animation->event_channels().get_channel_id( animation_intervals_channel_id );
	if ( channel_id == u32(-1) )
		return						1;

	event_channel const& channel	= pinned_animation->event_channels().channel( channel_id );
	return							channel.domains_count( );
}