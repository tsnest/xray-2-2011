////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_binary_tree_expression_simplifier.h"
#include <xray/animation/mixing_expression.h>
#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_binary_tree_weight_node.h>
#include <xray/animation/mixing_binary_tree_addition_node.h>
#include <xray/animation/mixing_binary_tree_subtraction_node.h>
#include <xray/animation/mixing_binary_tree_multiplication_node.h>
#include <xray/animation/base_interpolator.h>

using xray::animation::mixing::binary_tree_expression_simplifier;
using xray::animation::mixing::binary_tree_base_node;
using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_weight_node;
using xray::animation::mixing::binary_tree_addition_node;
using xray::animation::mixing::binary_tree_multiplication_node;
using xray::animation::mixing::binary_tree_subtraction_node;
using xray::animation::mixing::binary_tree_binary_operation_node;
using xray::animation::base_interpolator;

binary_tree_expression_simplifier::binary_tree_expression_simplifier( binary_tree_base_node& expression_node, xray::mutable_buffer& buffer ) :
	m_buffer						( buffer ),
	m_result						( 0 ),
	m_result_weight					( 0 )
{
	expression_node.accept			( *this );
	R_ASSERT						( m_result, "empty animation expression found" );
}

void binary_tree_expression_simplifier::visit		( binary_tree_animation_node& node )
{
	m_result						= &node;
}

void binary_tree_expression_simplifier::visit		( binary_tree_weight_node& node )
{
	m_result						= &node;
	m_result_weight					= &node;
}

template < typename Operation >
inline void binary_tree_expression_simplifier::process_null_weight	(
		Operation const& operation,
		binary_tree_expression_simplifier const& left_simplifier,
		binary_tree_expression_simplifier const& right_simplifier
	)
{
	XRAY_UNREFERENCED_PARAMETERS	( &operation, &left_simplifier, &right_simplifier );
}

inline void binary_tree_expression_simplifier::process_null_weight	(
		std::multiplies<float> const& operation,
		binary_tree_expression_simplifier const& left_simplifier,
		binary_tree_expression_simplifier const& right_simplifier
	)
{
	XRAY_UNREFERENCED_PARAMETER		( operation );

	if ( left_simplifier.m_result_weight ) {
		if ( left_simplifier.m_result_weight->weight() == 0.f ) {
			m_result				= left_simplifier.m_result_weight.c_ptr();
			m_result_weight			= left_simplifier.m_result_weight;
			return;
		}

		if ( left_simplifier.m_result_weight->weight() == 1.f ) {
			m_result				= right_simplifier.m_result;
			m_result_weight			= right_simplifier.m_result_weight;
			return;
		}
	}

	if ( right_simplifier.m_result_weight ) {
		if ( right_simplifier.m_result_weight->weight() == 0.f ) {
			m_result				= right_simplifier.m_result_weight.c_ptr();
			m_result_weight			= right_simplifier.m_result_weight;
			return;
		}

		if ( right_simplifier.m_result_weight->weight() == 1.f ) {
			m_result				= left_simplifier.m_result;
			m_result_weight			= left_simplifier.m_result_weight;
			return;
		}
	}
}

binary_tree_weight_node* binary_tree_expression_simplifier::new_weight	( float const weight, base_interpolator const* const interpolator ) const
{
	binary_tree_weight_node* const result = static_cast<binary_tree_weight_node*>( m_buffer.c_ptr() );
	m_buffer				+= sizeof( binary_tree_weight_node );
	new ( result ) binary_tree_weight_node( weight, interpolator );
	return					result;
}

template < typename T, typename Operation >
void binary_tree_expression_simplifier::process		( T& node, Operation const& operation )
{
	binary_tree_expression_simplifier	left_simplifier( m_buffer );
	node.left().accept				( left_simplifier );

	binary_tree_expression_simplifier	right_simplifier( m_buffer );
	node.right().accept				( right_simplifier );

	if ( left_simplifier.m_result_weight && right_simplifier.m_result_weight ) {
		if ( compare( left_simplifier.m_result_weight->interpolator(), right_simplifier.m_result_weight->interpolator()) == equal ) {
			m_result_weight			= new_weight( operation(left_simplifier.m_result_weight->weight(), right_simplifier.m_result_weight->weight()), &left_simplifier.m_result_weight->interpolator() );
			m_result				= m_result_weight.c_ptr();
			return;
		}
	}

	if ( (&node.left() != left_simplifier.m_result.c_ptr()) || (&node.right() != right_simplifier.m_result.c_ptr()) ) {
		T* const result				= static_cast<T*>( m_buffer.c_ptr() );
		m_buffer					+= sizeof( T );
		new ( result ) T( left_simplifier.m_result.c_ptr(), right_simplifier.m_result.c_ptr() );
		m_result					= result;
		m_result_weight				= 0;
	}
	else
		m_result					= &node;

	process_null_weight				( operation, left_simplifier, right_simplifier );
}

void binary_tree_expression_simplifier::visit		( binary_tree_addition_node& node )
{
	process							( node, std::plus<float>() );
}

void binary_tree_expression_simplifier::visit		( binary_tree_subtraction_node& node )
{
	process							( node, std::minus<float>() );
}

void binary_tree_expression_simplifier::visit		( binary_tree_multiplication_node& node )
{
	process							( node, std::multiplies<float>() );
}