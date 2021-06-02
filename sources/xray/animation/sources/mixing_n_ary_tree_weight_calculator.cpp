////////////////////////////////////////////////////////////////////////////
//	Unit		: n_ary_tree_weight_calculator.cpp
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_weight_calculator.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_destroyer.h"

using xray::animation::mixing::n_ary_tree_weight_calculator;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;

void n_ary_tree_weight_calculator::visit		( n_ary_tree_animation_node& node )
{
	float weight		= 1.f;

	u32 const operands_count		= node.operands_count( );
	R_ASSERT						( operands_count );
	n_ary_tree_base_node** const b	= node.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node** i		= b;
	n_ary_tree_base_node** const e	= i + operands_count;

	if ( (*i)->is_time_scale() ) {
		R_ASSERT_CMP				( operands_count, >=, 1 );
		++i;
	}

	for ( ; i != e; ++i ) {
		R_ASSERT		( (*i)->is_weight() );
		R_ASSERT		( !(*i)->is_time_scale() );
		(*i)->accept	( *this );
		weight			*= m_weight;
		if ( weight == 0.f )
			break;

		if ( !m_result )
			continue;

		*i				= m_result;
	}

	m_weight			= weight;
	m_result			= 0;
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_weight_transition_node& node )
{
	float const transition_time		= float( m_current_time_in_ms - node.start_time_in_ms() )/1000.f;
	float const interpolated_value	= (transition_time >= node.interpolator().transition_time()) ? 1.f : node.interpolator().interpolated_value( transition_time );
	R_ASSERT_CMP		( interpolated_value, >=, 0.f );
	R_ASSERT_CMP		( interpolated_value, <=, 1.f );
	if ( interpolated_value == 1.f ) {
		node.to().accept( *this );
		if ( m_weight == 0.f ) {
			m_weight_transition_ended_time_in_ms = node.start_time_in_ms() + math::floor( node.interpolator().transition_time()*1000.f );
			m_null_weight_found	= true;
			return;
		}

		m_result		= &node.to();
		n_ary_tree_destroyer	destroyer;
		node.from().accept	( destroyer );
		node.~n_ary_tree_weight_transition_node	( );
		return;
	}

	node.from().accept	( *this );
	if ( m_result )
		node.on_from_changed( *m_result );

	float const weight_from	= m_weight;

	node.to().accept	( *this );
	float const weight_to	= m_weight;

	m_weight			= weight_from*(1.f - interpolated_value) + weight_to*interpolated_value;
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_time_scale_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( m_weight = 1.f );
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_weight_node& node )
{
	m_weight			= node.weight( );
	m_result			= 0;
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_time_scale_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT			( m_weight = 1.f );
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_addition_node& node )
{
	float weight						= 0.f;

	u32 const operands_count			= node.operands_count( );
	R_ASSERT							( operands_count );
	n_ary_tree_base_node** i			= node.operands( sizeof(n_ary_tree_addition_node) );
	n_ary_tree_base_node** const e		= i + operands_count;
	for ( ; i != e; ++i ) {
		(*i)->accept	( *this );
		weight			+= m_weight;

		if ( !m_result )
			continue;

		*i				= m_result;
	}

	m_weight			= weight;
	m_result			= 0;
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_subtraction_node& node )
{
	bool first_operand	= true;
	float weight		= 0.f;

	u32 const operands_count		= node.operands_count( );
	R_ASSERT						( operands_count );
	n_ary_tree_base_node** i		= node.operands( sizeof(n_ary_tree_subtraction_node) );
	n_ary_tree_base_node** const e	= i + operands_count;
	for ( ; i != e; ++i ) {
		(*i)->accept	( *this );
		if ( first_operand ) {
			first_operand	= false;
			weight		= m_weight;
		}
		else
			weight		-= m_weight;

		if ( !m_result )
			continue;

		*i				= m_result;
	}

	m_weight			= weight;
	m_result			= 0;
}

void n_ary_tree_weight_calculator::visit		( n_ary_tree_multiplication_node& node )
{
	float weight		= 1.f;

	u32 const operands_count		= node.operands_count( );
	R_ASSERT						( operands_count );
	n_ary_tree_base_node** i		= node.operands( sizeof(n_ary_tree_multiplication_node) );
	n_ary_tree_base_node** const e	= i + operands_count;
	for ( ; i != e; ++i ) {
		(*i)->accept	( *this );
		weight			*= m_weight;
		if ( weight == 0.f )
			break;

		if ( !m_result )
			continue;

		*i				= m_result;
	}

	m_weight			= weight;
	m_result			= 0;
}