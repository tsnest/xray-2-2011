////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_redundant_multiplicands_detector.h"
#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_binary_tree_weight_node.h>
#include <xray/animation/mixing_binary_tree_addition_node.h>
#include <xray/animation/mixing_binary_tree_subtraction_node.h>
#include <xray/animation/mixing_binary_tree_multiplication_node.h>
#include <xray/animation/base_interpolator.h>

using xray::animation::mixing::n_ary_tree_redundant_multiplicands_detector;
using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_weight_node;
using xray::animation::mixing::binary_tree_addition_node;
using xray::animation::mixing::binary_tree_multiplication_node;
using xray::animation::mixing::binary_tree_subtraction_node;
using xray::animation::mixing::binary_tree_binary_operation_node;
using xray::animation::base_interpolator;

n_ary_tree_redundant_multiplicands_detector::n_ary_tree_redundant_multiplicands_detector	( base_interpolator const& interpolator ) :
	m_interpolator				( interpolator ),
	m_result					( false )
{
}

void n_ary_tree_redundant_multiplicands_detector::visit	( binary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	UNREACHABLE_CODE			( );
}

void n_ary_tree_redundant_multiplicands_detector::visit	( binary_tree_weight_node& node )
{
	if ( node.weight() != 1.f )
		return;

	m_result					= xray::animation::compare( m_interpolator, node.interpolator() ) == equal;
}

void n_ary_tree_redundant_multiplicands_detector::visit	( binary_tree_addition_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_redundant_multiplicands_detector::visit	( binary_tree_subtraction_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}

void n_ary_tree_redundant_multiplicands_detector::visit	( binary_tree_multiplication_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
}