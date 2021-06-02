////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_destroyer.h"
#include "mixing_animation_state.h"

using xray::animation::mixing::n_ary_tree_time_scale_calculator;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( );
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_weight_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( );
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_time_scale_transition_node& node )
{
	m_interpolator		= &node.interpolator();
	
	R_ASSERT_CMP		( m_current_time_in_ms, >=, node.start_time_in_ms() );
	float const transition_time		= float( m_current_time_in_ms - node.start_time_in_ms() )/1000.f;
	float const interpolated_value	= (transition_time >= node.interpolator().transition_time()) ? 1.f : node.interpolator().interpolated_value( transition_time );
	R_ASSERT_CMP		( interpolated_value, >=, 0.f );
	R_ASSERT_CMP		( interpolated_value, <=, 1.f );
	if ( interpolated_value == 1.f ) {
		node.to().accept( *this );
		if ( m_transitions_destroying == forbid_transitions_destroying ) {
			m_result	= &node;
			return;
		}
	
		u32 const end_transition_time_in_ms					= node.start_time_in_ms() + math::floor( node.interpolator().transition_time()*1000.f );
		R_ASSERT_CMP										( end_transition_time_in_ms, <=, m_current_time_in_ms );

		R_ASSERT											( m_animation );
		float const animation_interval_length				= m_animation->animation_intervals()[ m_animation->animation_state().animation_interval_id ].length();

		n_ary_tree_time_scale_node& time_scale_node			= static_cast_checked<n_ary_tree_time_scale_node&>(node.to());
		float animation_time_when_transition_ended			= m_previous_animation_time + m_time_scale*float(end_transition_time_in_ms >= m_previous_time_in_ms ? (end_transition_time_in_ms - m_previous_time_in_ms) : 0)/1000.f;
		animation_time_when_transition_ended				= math::min( math::max( animation_time_when_transition_ended, 0.f ), animation_interval_length);
		R_ASSERT_CMP										( animation_time_when_transition_ended, >=, 0.f );
		R_ASSERT_CMP										( animation_time_when_transition_ended, <=, animation_interval_length );
		
		#ifndef MASTER_GOLD
		LOG_DEBUG											( "ON_TIME_SCALE_TRANSITION_ENDED: [%s] setting time scale start time: %d, %.3f", m_animation->identifier(), end_transition_time_in_ms, animation_time_when_transition_ended );
		#endif // #ifndef MASTER_GOLD

		time_scale_node.set_time_scale_start_time			( end_transition_time_in_ms, animation_time_when_transition_ended );

		m_result		= &node.to();

		n_ary_tree_destroyer	destroyer;
		node.from().accept	( destroyer );
		node.~n_ary_tree_time_scale_transition_node	( );

		return;
	}

	node.from().accept	( *this );
	float const time_scale_from	= m_time_scale;
	node.m_from			= m_result;

	node.to().accept	( *this );
	float const time_scale_to	= m_time_scale;
	node.m_to			= m_result;

	m_result			= &node;
	m_time_scale		= time_scale_from*(1.f - interpolated_value) + time_scale_to*interpolated_value;
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_weight_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( );
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_time_scale_node& node )
{
	m_result			= &node;
	m_time_scale		= node.time_scale( );
	m_interpolator		= &node.interpolator();

	if ( m_time_scale > 0.f )
		++m_forward_time_scale_nodes_count;

	if ( m_time_scale < 0.f )
		++m_backward_time_scale_nodes_count;
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_addition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( );
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_subtraction_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( );
}

void n_ary_tree_time_scale_calculator::visit	( n_ary_tree_multiplication_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( );
}

n_ary_tree_time_scale_calculator::n_ary_tree_time_scale_calculator		(
		u32 const current_time_in_ms,
		float const previous_animation_time,
		u32 const previous_time_in_ms,
		transitions_destroying_enum const transitions_destroying,
		n_ary_tree_animation_node const* const animation
	) :
	m_animation					( animation ),
	m_result					( 0 ),
	m_interpolator				( 0 ),
	m_current_time_in_ms		( current_time_in_ms ),
	m_previous_animation_time	( previous_animation_time ),
	m_previous_time_in_ms		( previous_time_in_ms ),
	m_time_scale				( 0.f ),
	m_transitions_destroying	( transitions_destroying ),
	m_forward_time_scale_nodes_count( 0 ),
	m_backward_time_scale_nodes_count( 0 )
{
}