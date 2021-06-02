////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_weaver.h"
#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_binary_tree_weight_node.h>
#include <xray/animation/mixing_binary_tree_addition_node.h>
#include <xray/animation/mixing_binary_tree_subtraction_node.h>
#include <xray/animation/mixing_binary_tree_multiplication_node.h>
#include <xray/animation/base_interpolator.h>

using xray::animation::mixing::n_ary_tree_weaver;
using xray::animation::mixing::binary_tree_base_node;
using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_weight_node;
using xray::animation::mixing::binary_tree_addition_node;
using xray::animation::mixing::binary_tree_multiplication_node;
using xray::animation::mixing::binary_tree_subtraction_node;
using xray::animation::base_interpolator;

n_ary_tree_weaver::n_ary_tree_weaver	( xray::mutable_buffer& buffer ) :
	m_buffer					( buffer ),
	m_animations_root			( 0 ),
	m_weights_root				( 0 ),
	m_current_animations_root	( 0 ),
	m_interpolators_root		( 0 ),
	m_interpolators_count		( 0 )
{
}

template < typename T >
static inline void clean				( T& node )
{
	node.m_next_weight			= 0;
}

void n_ary_tree_weaver::add_interpolator( binary_tree_base_node& node, base_interpolator const& interpolator )
{
	XRAY_UNREFERENCED_PARAMETER	( interpolator );
	++m_interpolators_count;
	node.m_next_unique_interpolator	= m_interpolators_root;
	m_interpolators_root		= &node;
}

static bool is_unique_animation_lexeme	( binary_tree_animation_node& node, binary_tree_animation_node* const animations_root )
{
	if ( node.m_next_animation )
		return					false;

	binary_tree_animation_node* i	= animations_root;
	for ( ; i; i = i->m_next_animation.c_ptr() ) {
		if ( i != &node )
			continue;

		R_ASSERT				( !i->m_next_animation );
		return					false;
	}

	return						true;
}

void n_ary_tree_weaver::visit			( binary_tree_animation_node& node )
{
	binary_tree_animation_node* unique_node;
	if ( !is_unique_animation_lexeme(node, m_current_animations_root) ) {
		LOG_WARNING				( "the same animation lexeme is used twice in the expression" );

		unique_node				= static_cast<binary_tree_animation_node*>( m_buffer.c_ptr() );
		m_buffer				+= sizeof( binary_tree_animation_node );
		new ( unique_node ) binary_tree_animation_node( node );
	}
	else
		unique_node				= &node;

	m_new_node					= unique_node;

	clean						( *unique_node );

	unique_node->m_unique_weights_count	= 0;

	unique_node->m_next_animation		= m_animations_root;
	m_animations_root					= unique_node;

	if ( !unique_node->driving_animation() )
		add_interpolator		( *unique_node, *unique_node->time_scale_interpolator() );
	else
		R_ASSERT				( !unique_node->time_scale_interpolator() );
}

void n_ary_tree_weaver::visit			( binary_tree_weight_node& node )
{
	m_new_node					= &node;

	node.m_next_weight			= m_weights_root;
	m_weights_root				= &node;

	add_interpolator			( node, node.interpolator() );
}

template < typename T >
inline void n_ary_tree_weaver::propagate( T& node, n_ary_tree_weaver& weaver )
{
	weaver.m_interpolators_root	= m_interpolators_root;
	weaver.m_weights_root		= m_weights_root;
	weaver.m_current_animations_root	= m_current_animations_root;

	node.accept					( weaver );

	m_interpolators_root		= weaver.m_interpolators_root;
	m_interpolators_count		+= weaver.m_interpolators_count;
	m_weights_root				= weaver.m_weights_root;
}

void n_ary_tree_weaver::join_animations	( n_ary_tree_weaver const& other )
{
	binary_tree_animation_node* previous = 0;
	for ( binary_tree_animation_node* i = other.m_animations_root; i; previous = i, i = i->m_next_animation.c_ptr() );

	if ( !previous )
		return;

	previous->m_next_animation	= m_animations_root;
	m_animations_root			= other.m_animations_root;
	if ( !m_current_animations_root )
		m_current_animations_root	= m_animations_root;
}

template < typename T >
inline void n_ary_tree_weaver::propagate( T& node, n_ary_tree_weaver& left, n_ary_tree_weaver& right )
{
	m_new_node					= &node;

	propagate					( node.left(), left );
	join_animations				( left );

	propagate					( node.right(), right );
	join_animations				( right );

	node.m_left					= left.m_new_node;
	node.m_right				= right.m_new_node;
}

void n_ary_tree_weaver::visit			( binary_tree_addition_node& node )
{
	clean						( node );

	n_ary_tree_weaver			left( m_buffer ), right( m_buffer );
	propagate					( node, left, right );

	if ( !left.m_animations_root && !right.m_animations_root )
		m_weights_root			= &node;
}

void n_ary_tree_weaver::visit			( binary_tree_subtraction_node& node )
{
	clean						( node );

	n_ary_tree_weaver			left( m_buffer ), right( m_buffer );
	propagate					( node, left, right );

	R_ASSERT					( !left.m_animations_root, "it is impossible to subtract from animation" );
	R_ASSERT					( !right.m_animations_root, "it is impossible to subtract an animation" );

	m_weights_root				= &node;
}

static void update_weights				( binary_tree_animation_node* const animations_root, binary_tree_base_node* const weights_root )
{
	xray::animation::mixing::binary_tree_animation_node_ptr current = animations_root;
	do {
		binary_tree_base_node* j= current.c_ptr( );
		for (binary_tree_base_node* i=current->m_next_weight; i && (j != weights_root); j=i, i=i->m_next_weight );
		ASSERT					( j );
		if ( j != weights_root ) {
			ASSERT				( !j->m_next_weight );
			j->m_next_weight	= weights_root;
		}
		
		current					= current->m_next_animation;
	} while ( current );
}

void n_ary_tree_weaver::visit			( binary_tree_multiplication_node& node )
{
	clean						( node );

	n_ary_tree_weaver			left( m_buffer ), right( m_buffer );
	propagate					( node, left, right );

	if ( left.m_animations_root ) {
		ASSERT					( !right.m_animations_root );
		ASSERT					( right.m_weights_root );
		update_weights			( left.m_animations_root, right.m_weights_root );
		m_weights_root			= 0;
		return;
	}

	if ( !right.m_animations_root )
		return;

	ASSERT						( !left.m_animations_root && right.m_animations_root );

	update_weights				( right.m_animations_root, left.m_weights_root );
	m_weights_root				= 0;
}