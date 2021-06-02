////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_event_iterator.h"
#include "mixing_animation_state.h"
#include "mixing_n_ary_tree_animation_node.h"

using xray::animation::mixing::n_ary_tree_event_iterator;

n_ary_tree_event_iterator::n_ary_tree_event_iterator	(
		xray::animation::mixing::animation_state& animation,
		xray::animation::mixing::n_ary_tree_animation_node& animation_node,
		u32 const start_time_in_ms,
		u32 const initial_event_types,
		xray::animation::subscribed_channel*& channels_head
	) :
	m_animation_event_iterator		( animation, animation_node, start_time_in_ms, initial_event_types & time_event_animation_lexeme_started, channels_head ),
	m_weight_event_iterator			( animation_node, start_time_in_ms, initial_event_types & time_event_weight_transitions_started ),
	m_animation_node				( &animation_node ),
	m_value							( u32(-1), 0 )
{
	select_state					( );
}

void n_ary_tree_event_iterator::select_state						( )
{
	if ( (*m_weight_event_iterator).event_time_in_ms < (*m_animation_event_iterator).event_time_in_ms ) {
		m_state					= weight_transition_event_state;
		m_value					= *m_weight_event_iterator;
	}
	else {
		m_state					= animation_event_state;
		m_value					= *m_animation_event_iterator;
		if ( (*m_weight_event_iterator).event_time_in_ms == m_value.event_time_in_ms ) {
			m_state				|= weight_transition_event_state;
			m_value.event_type	|= (*m_weight_event_iterator).event_type;
		}
	}
}

n_ary_tree_event_iterator& n_ary_tree_event_iterator::operator ++	( )
{
	if ( m_state & animation_event_state )
		++m_animation_event_iterator;

	if ( m_state & weight_transition_event_state )
		++m_weight_event_iterator;

	R_ASSERT					( m_state & (animation_event_state | weight_transition_event_state) );

	select_state				( );
	return						*this;
}

bool n_ary_tree_event_iterator::is_less								( n_ary_tree_event_iterator const& other ) const
{
	// at the first, we compare by event_time_in_ms value
	if ( m_value.event_time_in_ms < other.m_value.event_time_in_ms )
		return					true;

	if ( m_value.event_time_in_ms > other.m_value.event_time_in_ms )
		return					false;

	// at the second, we compare by synchronization_group_id
	if ( animation().synchronization_group_id() < other.animation().synchronization_group_id() )
		return					true;

	if ( animation().synchronization_group_id() > other.animation().synchronization_group_id() )
		return					false;

	// at the third, we make driving animation first inside synchronization group
	if ( animation().driving_animation() < other.animation().driving_animation() )
		return					true;

	if ( animation().driving_animation() > other.animation().driving_animation() )
		return					false;

	// at the last, we just compare events
	return						m_value < other.m_value;
}