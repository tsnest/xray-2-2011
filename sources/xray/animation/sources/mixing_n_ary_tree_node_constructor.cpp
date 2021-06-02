////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_node_constructor.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_binary_tree_weight_node.h>
#include <xray/animation/mixing_binary_tree_addition_node.h>
#include <xray/animation/mixing_binary_tree_subtraction_node.h>
#include <xray/animation/mixing_binary_tree_multiplication_node.h>

using xray::animation::mixing::n_ary_tree_node_constructor;
using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_weight_node;
using xray::animation::mixing::binary_tree_addition_node;
using xray::animation::mixing::binary_tree_multiplication_node;
using xray::animation::mixing::binary_tree_subtraction_node;
using xray::animation::mixing::binary_tree_binary_operation_node;
using xray::animation::base_interpolator;

n_ary_tree_node_constructor::n_ary_tree_node_constructor	(
		xray::mutable_buffer& buffer,
		base_interpolator const* const* const interpolators_begin,
		base_interpolator const* const*	const interpolators_end
	) :
	m_buffer					( buffer ),
	m_interpolators_begin		( interpolators_begin ),
	m_interpolators_end			( interpolators_end ),
	m_result					( 0 )
{
}

void n_ary_tree_node_constructor::visit						( binary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	UNREACHABLE_CODE			( );
}

void n_ary_tree_node_constructor::visit						( binary_tree_weight_node& node )
{
	base_interpolator const* found	= 0;
	base_interpolator const* interpolator = &node.interpolator();
	for ( base_interpolator const* const* i = m_interpolators_begin, * const* const e = m_interpolators_end; i != e; ++i ) {
		if ( **i == *interpolator ) {
			found				= *i;
			break;
		}
	}

	ASSERT						( found );

	m_result					= new ( m_buffer.c_ptr() ) n_ary_tree_weight_node ( *found, node.m_simplified_weight );
	m_buffer					+= sizeof( n_ary_tree_weight_node );
}

void n_ary_tree_node_constructor::visit						( binary_tree_addition_node& node )
{
	m_result					= new ( m_buffer.c_ptr() ) n_ary_tree_addition_node ( 2 );
	m_buffer					+= sizeof( n_ary_tree_addition_node );
	propagate					( node );
}

void n_ary_tree_node_constructor::visit						( binary_tree_subtraction_node& node )
{
	m_result					= new ( m_buffer.c_ptr() ) n_ary_tree_subtraction_node ( 2 );
	m_buffer					+= sizeof( n_ary_tree_subtraction_node );
	propagate					( node );
}

void n_ary_tree_node_constructor::visit						( binary_tree_multiplication_node& node )
{
	m_result					= new ( m_buffer.c_ptr() ) n_ary_tree_multiplication_node ( 2 );
	m_buffer					+= sizeof( n_ary_tree_multiplication_node );
	propagate					( node );
}

void n_ary_tree_node_constructor::propagate					( binary_tree_binary_operation_node& node )
{
	n_ary_tree_base_node** children	= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr( ) );
	m_buffer					+= 2 * sizeof(n_ary_tree_base_node*);

	n_ary_tree_node_constructor	left( m_buffer, m_interpolators_begin, m_interpolators_end );
	node.left().accept			( left );

	n_ary_tree_node_constructor	right( m_buffer, m_interpolators_begin, m_interpolators_end );
	node.right().accept			( right );

	*children++					= left.result();
	*children					= right.result();
}