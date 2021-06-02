////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_time_in_ms_calculator.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_animation_event.h"

using xray::animation::mixing::n_ary_tree_time_in_ms_calculator;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_time_scale_calculator;

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_weight_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_weight_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_addition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_subtraction_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_multiplication_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_time_scale_node& node )
{
	if ( node.time_scale() == 0.f )
		m_time_in_ms						= u32( -1 );
	else {
		if ( node.time_scale_start_time_in_ms() >= m_start_time_in_ms) {
#ifndef MASTER_GOLD
			if ( node.time_scale() >= 0.f ) {
				if ( m_target_animation_time < node.animation_time_before_scale_starts() ) {
					LOG_DEBUG			( "[%d] >= [%d]", node.time_scale_start_time_in_ms(), m_start_time_in_ms );
				}
				R_ASSERT_CMP			( m_target_animation_time, >=, node.animation_time_before_scale_starts() );
			}
			else
				R_ASSERT_CMP			( m_target_animation_time, <=, node.animation_time_before_scale_starts() );
#endif // #ifndef MASTER_GOLD
			m_time_in_ms						=
				node.time_scale_start_time_in_ms() +
				math::floor( (m_target_animation_time - node.animation_time_before_scale_starts())*1000.f/node.time_scale() );
		}
		else
			m_time_in_ms						=
				m_start_time_in_ms +
				math::floor( (m_target_animation_time - m_start_animation_time)*1000.f/node.time_scale() );
	}
}

void n_ary_tree_time_in_ms_calculator::visit		( n_ary_tree_time_scale_transition_node& node )
{
	// here we use numerical integration to find how much time passed during time scale transition

	R_ASSERT_CMP							( m_start_time_in_ms, >=, node.start_time_in_ms() );
	u32 const time_passed_since_last_query	= m_start_time_in_ms - node.start_time_in_ms();
	float accumulated_animation_time		= m_start_animation_time;

	float const integration_interval_length	= integration_interval_length_in_ms/1000.f;

	u32 const start_integration_interval_id	= time_passed_since_last_query/integration_interval_length_in_ms;
	u32 const transition_start_time_in_ms	= node.start_time_in_ms();

	n_ary_tree_time_scale_calculator time_scale_calculator(
		m_start_time_in_ms,
		accumulated_animation_time,
		transition_start_time_in_ms + (start_integration_interval_id ? 2*start_integration_interval_id-1 : 0)*integration_interval_length_in_ms/2,
		n_ary_tree_time_scale_calculator::forbid_transitions_destroying
	);
	time_scale_calculator.visit				( node );
	float current_time_direction			= math::sign( time_scale_calculator.time_scale() );

	for ( u32 i = start_integration_interval_id; ; ++i) {
		n_ary_tree_time_scale_calculator time_scale_calculator(
			transition_start_time_in_ms + (2*i+1)*integration_interval_length_in_ms/2,
			accumulated_animation_time,
			transition_start_time_in_ms + (i ? 2*i-1 : 0)*integration_interval_length_in_ms/2,
			n_ary_tree_time_scale_calculator::forbid_transitions_destroying
		);
		time_scale_calculator.visit			( node );
		R_ASSERT							( &node == time_scale_calculator.result() );

		float const time_scale				= time_scale_calculator.time_scale( );
		if ( (current_time_direction == 0.f) && (time_scale != 0.f) )
			current_time_direction			= math::sign(time_scale);

		bool const has_time_direction_changed	= (current_time_direction != 0.f) && (math::sign(time_scale) * current_time_direction <= 0.f);

		float const animation_time_to_add	= time_scale * integration_interval_length;
		if (
			!(
				(accumulated_animation_time < m_target_animation_time) && (accumulated_animation_time + animation_time_to_add >= m_target_animation_time)
				||
				(accumulated_animation_time > m_target_animation_time) && (accumulated_animation_time + animation_time_to_add <= m_target_animation_time)
			)
		) {
			if ( has_time_direction_changed ) {
				m_event_time				= accumulated_animation_time + animation_time_to_add/2.f;
				m_time_in_ms				= transition_start_time_in_ms + (2*i+1)*integration_interval_length_in_ms/2;
				m_event_type				= time_event_time_direction_changed;
				return;
			}

			accumulated_animation_time		+= animation_time_to_add;
			continue;
		}

		m_time_in_ms						=
			math::floor(
				(
				(m_target_animation_time - accumulated_animation_time)/animation_time_to_add)*
					float(integration_interval_length_in_ms)
				) + transition_start_time_in_ms +
				(2*i+1)*integration_interval_length_in_ms/2;

		if ( has_time_direction_changed )
			m_event_type					|= time_event_time_direction_changed;

		return;
	}
}

n_ary_tree_time_in_ms_calculator::n_ary_tree_time_in_ms_calculator	(
		n_ary_tree_animation_node& animation,
		u32 const start_time_in_ms,
		float const start_animation_time,
		float const target_animation_time,
		u32 const event_type
	) :
	m_start_time_in_ms( start_time_in_ms ),
	m_start_animation_time( start_animation_time ),
	m_target_animation_time( target_animation_time ),
	m_event_time( target_animation_time ),
	m_event_type( event_type )
{
	(**animation.operands( sizeof(n_ary_tree_animation_node) )).accept( *this );
}