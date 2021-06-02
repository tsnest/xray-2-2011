////////////////////////////////////////////////////////////////////////////
//	Created		: 03.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_time_scale_start_time_modifier.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"

using xray::animation::mixing::n_ary_tree_time_scale_start_time_modifier;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_weight_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_weight_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_addition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_subtraction_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_multiplication_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
	NODEFAULT								( );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_time_scale_node& node )
{
	R_ASSERT_CMP							( m_new_start_time_in_ms, >=, node.time_scale_start_time_in_ms() );
	node.set_time_scale_start_time			( m_new_start_time_in_ms, m_animation_interval_time );
}

void n_ary_tree_time_scale_start_time_modifier::visit		( n_ary_tree_time_scale_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER				( node );
}

n_ary_tree_time_scale_start_time_modifier::n_ary_tree_time_scale_start_time_modifier	(
		n_ary_tree_animation_node& animation,
		u32 const new_start_time_in_ms,
		float const animation_interval_time
	) :
	m_new_start_time_in_ms( new_start_time_in_ms ),
	m_animation_interval_time( animation_interval_time )
{
	(**animation.operands( sizeof(n_ary_tree_animation_node) )).accept( *this );
}