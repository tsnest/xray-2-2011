////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_animation_time_calculator.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_animation_state.h"

using xray::animation::mixing::n_ary_tree_animation_time_calculator;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_weight_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_weight_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_addition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_subtraction_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_multiplication_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_time_scale_node& node )
{
	R_ASSERT_CMP							( m_target_time_in_ms, >=, node.time_scale_start_time_in_ms() );
	m_animation_time						=
		node.animation_time_before_scale_starts() + 
		(m_target_time_in_ms - node.time_scale_start_time_in_ms())*node.time_scale()/1000.f;
	m_animation_time						= math::min( math::max( m_animation_time, 0.f ), m_animation_interval_length);
	R_ASSERT_CMP							( m_animation_time, >=, 0.f );
	R_ASSERT_CMP							( m_animation_time, <=, m_animation_interval_length );
}

void n_ary_tree_animation_time_calculator::visit		( n_ary_tree_time_scale_transition_node& node )
{
	m_animation_time						= m_start_animation_time;
	R_ASSERT_CMP							( m_start_time_in_ms, >=, m_start_time_in_ms );
	float const integration_interval_length	= integration_interval_length_in_ms/1000.f;

	n_ary_tree_base_node* time_scale_node	= &node;

	u32 const full_intervals_count			= (m_target_time_in_ms - m_start_time_in_ms)/integration_interval_length_in_ms;
	for ( u32 i = 0; i <= full_intervals_count; ++i ) {
		u32 const start_interval_time_in_ms		= i < full_intervals_count ? (m_start_time_in_ms + (2*i+1)*(integration_interval_length_in_ms/2)) : m_target_time_in_ms;
		u32 const target_interval_time_in_ms	= i < full_intervals_count ? (m_start_time_in_ms + (i ? 2*i-1 : 0)*(integration_interval_length_in_ms/2)) : (m_start_time_in_ms + full_intervals_count*integration_interval_length_in_ms);
		n_ary_tree_time_scale_calculator time_scale_calculator(
			start_interval_time_in_ms,
			m_animation_time,
			target_interval_time_in_ms,
			m_is_read_only ? n_ary_tree_time_scale_calculator::forbid_transitions_destroying : n_ary_tree_time_scale_calculator::permit_transitions_destroying,
			&m_animation
		);
		time_scale_node->accept				( time_scale_calculator );
		if ( time_scale_node != time_scale_calculator.result() ) {
			R_ASSERT						( time_scale_calculator.result() );
			time_scale_node					= time_scale_calculator.result();
			m_new_time_scale_node			= time_scale_node;
		}

		float const interval_portion		= (i < full_intervals_count ? 1.f : float((m_target_time_in_ms - m_start_time_in_ms)%integration_interval_length_in_ms)/float(integration_interval_length_in_ms)) * time_scale_calculator.time_scale() * integration_interval_length;
		m_animation_time					+= interval_portion;
		m_animation_time					= math::min( math::max( m_animation_time, 0.f ), m_animation_interval_length);
		R_ASSERT_CMP						( m_animation_time, >=, 0.f );
		R_ASSERT_CMP						( m_animation_time, <=, m_animation_interval_length );
	}
}

n_ary_tree_animation_time_calculator::n_ary_tree_animation_time_calculator	(
		n_ary_tree_animation_node& animation,
		u32 const start_time_in_ms,
		float const start_animation_time,
		u32 const target_time_in_ms,
		bool const is_read_only
	) :
	m_animation								( animation ),
	m_start_time_in_ms						( start_time_in_ms ),
	m_start_animation_time					( start_animation_time ),
	m_target_time_in_ms						( target_time_in_ms ),
	m_animation_interval_length				( (animation.animation_intervals() + animation.animation_state().animation_interval_id)->length() ),
	m_is_read_only							( is_read_only )
{
	m_new_time_scale_node					= *animation.operands( sizeof(n_ary_tree_animation_node) );
	m_new_time_scale_node->accept			( *this );
	*animation.operands( sizeof(n_ary_tree_animation_node) )	= m_new_time_scale_node;
	R_ASSERT_CMP							( m_animation_time, >=, 0.f );
	R_ASSERT_CMP							( m_animation_time, <=, m_animation_interval_length );
}