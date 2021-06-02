////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_transition_to_one_detector.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"

using xray::animation::mixing::n_ary_tree_transition_to_one_detector;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_weight_transition_node& node )
{
	m_has_transitions			= true;
	node.to().accept			( *this );
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_time_scale_transition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_weight_node& node )
{
	m_result					= node.weight() == 1.f; 
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_time_scale_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_addition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_subtraction_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_transition_to_one_detector::visit	( n_ary_tree_multiplication_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}