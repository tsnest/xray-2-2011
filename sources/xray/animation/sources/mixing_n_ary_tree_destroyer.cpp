////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_destroyer.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include <xray/animation/mixing_animation_interval.h>

using xray::animation::mixing::n_ary_tree_destroyer;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_n_ary_operation_node;

template < typename T >
static inline void destroy					( T& node )
{
	node.~T						( );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_animation_node& node )
{
	propagate					( node );

	animation_interval const* i			= node.animation_intervals( );
	animation_interval const* const e	= i + node.animation_intervals_count( );
	for ( ; i != e; ++i )
		i->~animation_interval	( );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_weight_node& node )
{
	destroy						( node );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_time_scale_node& node )
{
	destroy						( node );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_weight_transition_node& node )
{
	node.from().accept			( *this );
	node.to().accept			( *this );
	destroy						( node );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_time_scale_transition_node& node )
{
	node.from().accept			( *this );
	node.to().accept			( *this );
	destroy						( node );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_addition_node& node )
{
	propagate					( node );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_subtraction_node& node )
{
	propagate					( node );
}

void n_ary_tree_destroyer::visit			( n_ary_tree_multiplication_node& node )
{
	propagate					( node );
}

template < typename T >
inline void n_ary_tree_destroyer::propagate	( T& node )
{
	n_ary_tree_base_node** i	= node.operands( sizeof(T) );
	n_ary_tree_base_node** const e = i + node.operands_count( );
	for ( ; i != e; ++i )
		(*i)->accept			( *this );

	destroy						( node );
}