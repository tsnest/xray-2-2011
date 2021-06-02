////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_cloner.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include <xray/animation/mixing_n_ary_tree.h>
#include <xray/animation/mixing_animation_interval.h>

using xray::animation::mixing::n_ary_tree_cloner;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree;
using xray::mutable_buffer;
using xray::animation::base_interpolator;

n_ary_tree_cloner::n_ary_tree_cloner							(
		mutable_buffer& buffer,
		u32 const start_time_in_ms
	) :
	m_result					( 0 ),
	m_buffer					( buffer ),
	m_animation_interpolator	( 0 ),
	m_interpolators				( 0 ),
	m_interpolators_count		( 0 ),
	m_time_scale_factor			( xray::memory::uninitialized_value<float>() ),
	m_animation_interval_time	( 0 ),
	m_start_time_in_ms			( start_time_in_ms )
{
}

void n_ary_tree_cloner::initialize								(
		n_ary_tree const& from,
		n_ary_tree const& to
	)
{
	u32 const from_interpolators_count	= from.interpolators_count();
	u32 const to_interpolators_count	= to.interpolators_count();
	base_interpolator const* const* const from_interpolators_begin = from.interpolators( );
	base_interpolator const* const* const to_interpolators_begin = to.interpolators( );

	u32 const total_interpolators_count				= from_interpolators_count + to_interpolators_count;
	base_interpolator const** const interpolators_begin	= static_cast<base_interpolator const**>( ALLOCA( total_interpolators_count*sizeof(base_interpolator const*) ) );
	base_interpolator const** interpolators_end		=
		std::unique(
			interpolators_begin,
			std::merge(
				from_interpolators_begin,
				from_interpolators_begin + from_interpolators_count,
				to_interpolators_begin,
				to_interpolators_begin + to_interpolators_count,
				interpolators_begin,
				merge_interpolators_predicate()
			),
			unique_interpolators_predicate()
		);
	m_interpolators_count		= u32(interpolators_end - interpolators_begin);

	m_interpolators				= static_cast<base_interpolator const**>( m_buffer.c_ptr() );
	m_buffer					+= m_interpolators_count*sizeof( base_interpolator* );

	for ( base_interpolator const** i = interpolators_begin, **j = m_interpolators; i != interpolators_end; ++i, ++j )
		*j						= (*i)->clone( m_buffer );
}

n_ary_tree_base_node* n_ary_tree_cloner::clone				(
		n_ary_tree_base_node& node_to_clone,
		base_interpolator const* animation_interpolator
	)
{
	R_ASSERT					( !node_to_clone.is_time_scale() );

	m_result					= 0;
	m_animation_interpolator	= animation_interpolator;
	node_to_clone.accept		( *this );
	R_ASSERT_CMP				( m_animation_interpolator, ==, animation_interpolator );
	m_animation_interpolator	= 0;
	return						m_result;
}

n_ary_tree_base_node* n_ary_tree_cloner::clone				(
		n_ary_tree_base_node& node_to_clone,
		float const time_scale_factor,
		float const animation_interval_time
	)
{
	m_result					= 0;
	m_time_scale_factor			= time_scale_factor;
	m_animation_interval_time	= &animation_interval_time;
	node_to_clone.accept		( *this );
	R_ASSERT_CMP				( m_animation_interval_time, ==, &animation_interval_time );
	R_ASSERT_CMP				( m_time_scale_factor, ==, time_scale_factor );

	m_animation_interval_time	= 0;
	m_time_scale_factor			= 1.f;

	return						m_result;
}

n_ary_tree_base_node* n_ary_tree_cloner::clone				(
		n_ary_tree_base_node& node_to_clone,
		float const time_scale_factor
	)
{
	m_result					= 0;
	m_time_scale_factor			= time_scale_factor;
	m_animation_interval_time	= 0;
	node_to_clone.accept		( *this );
	R_ASSERT_CMP				( m_time_scale_factor, ==, time_scale_factor );
	R_ASSERT					( !m_animation_interval_time );

	m_time_scale_factor			= 1.f;

	return						m_result;
}

template < typename T >
inline T* n_ary_tree_cloner::new_constructed					( T& node )
{
	T* const result				= new ( m_buffer.c_ptr() ) T ( node );
	m_buffer					+= sizeof( T );
	return						result;
}

inline n_ary_tree_animation_node* n_ary_tree_cloner::new_constructed	( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT					( );
}

void n_ary_tree_cloner::visit( n_ary_tree_animation_node& node )
{
	XRAY_UNREFERENCED_PARAMETER	( node );
	NODEFAULT					( );
}

base_interpolator const* n_ary_tree_cloner::clone				( base_interpolator const& interpolator, bool assert_on_failure ) const
{
	base_interpolator const** i			= m_interpolators;
	base_interpolator const** const e	= m_interpolators + m_interpolators_count;
	for ( ; i != e; ++i ) {
		if ( interpolator == **i )
			return				*i;
	}

	R_ASSERT_U					( !assert_on_failure, "unique interpolator which is not in the interpolators list found" );
	return						0;
}

void n_ary_tree_cloner::visit	( n_ary_tree_weight_transition_node& node )
{
	n_ary_tree_weight_transition_node* const result	= (n_ary_tree_weight_transition_node*)m_buffer.c_ptr();
	m_buffer					+= sizeof(n_ary_tree_weight_transition_node);

	node.from().accept			( *this );
	n_ary_tree_base_node* const from = m_result;

	node.to().accept			( *this );
	n_ary_tree_base_node* const to = m_result;

	base_interpolator const* cloned_interpolator	= clone( node.interpolator(), false );
	// this is the case when weight transition uses animation interpolator
	if ( !cloned_interpolator ) {
		R_ASSERT				( m_animation_interpolator );
		R_ASSERT				( m_animation_interpolator == &node.interpolator() );
		cloned_interpolator		= &node.interpolator( );
	}

	new ( result ) n_ary_tree_weight_transition_node (
		*from,
		*to,
		*cloned_interpolator,
		node.start_time_in_ms()
	);

	m_result				= result;
}

void n_ary_tree_cloner::visit( n_ary_tree_time_scale_transition_node& node )
{
	n_ary_tree_time_scale_transition_node* const result	= (n_ary_tree_time_scale_transition_node*)m_buffer.c_ptr();
	m_buffer					+= sizeof(n_ary_tree_time_scale_transition_node);

	node.from().accept			( *this );
	n_ary_tree_base_node* const from = m_result;

	node.to().accept			( *this );
	n_ary_tree_base_node* const to = m_result;

	new ( result ) n_ary_tree_time_scale_transition_node (
		*from,
		*to,
		*clone( node.interpolator() ),
		m_animation_interval_time ? m_start_time_in_ms : node.start_time_in_ms(),
		m_animation_interval_time ? 0 : node.last_integration_interval_id(),
		m_animation_interval_time ? *m_animation_interval_time : node.last_integration_interval_value()
	);

	m_result				= result;
}

void n_ary_tree_cloner::visit( n_ary_tree_weight_node& node )
{
	base_interpolator const* cloned_interpolator	= clone( node.interpolator(), false );
	// this is the case when weight uses animation interpolator
	if ( !cloned_interpolator ) {
		R_ASSERT				( m_animation_interpolator );
		R_ASSERT				( m_animation_interpolator == &node.interpolator() );
		cloned_interpolator		= &node.interpolator( );
	}

	m_result					=
		new ( m_buffer.c_ptr() ) n_ary_tree_weight_node(
			*cloned_interpolator,
			node.weight()
		);
	m_buffer					+= sizeof( n_ary_tree_weight_node );
}

void n_ary_tree_cloner::visit( n_ary_tree_time_scale_node& node )
{
	m_result				=
		new ( m_buffer.c_ptr() ) n_ary_tree_time_scale_node(
			*clone( node.interpolator() ),
			node.time_scale() * m_time_scale_factor,
			m_animation_interval_time ? *m_animation_interval_time : node.animation_time_before_scale_starts(),
			m_animation_interval_time ? m_start_time_in_ms : node.time_scale_start_time_in_ms()
		);
	m_buffer					+= sizeof( n_ary_tree_time_scale_node );
}

void n_ary_tree_cloner::visit( n_ary_tree_addition_node& node )
{
	propagate					( node );
}

void n_ary_tree_cloner::visit( n_ary_tree_subtraction_node& node )
{
	propagate					( node );
}

void n_ary_tree_cloner::visit( n_ary_tree_multiplication_node& node )
{
	propagate					( node );
}

template < typename T >
void n_ary_tree_cloner::propagate			( T& node )
{
	T* const result				= new_constructed( node );

	u32 const operands_count	= node.operands_count( );
	m_buffer					+= operands_count*sizeof( n_ary_tree_base_node*);

	n_ary_tree_base_node* const* i			= node.operands( sizeof(T) );
	n_ary_tree_base_node* const* const e	= i + operands_count;
	n_ary_tree_base_node** j				= result->operands( sizeof(T) );
	for ( ; i != e; ++i, ++j ) {
		(*i)->accept			( *this );
		*j						= m_result;
	}

	m_result				= result;
}