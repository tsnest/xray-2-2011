////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_weight_event_iterator.h"
#include "mixing_n_ary_tree_visitor.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_animation_state.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_weight_transition_end_time_calculator :
	public n_ary_tree_visitor,
	private boost::noncopyable
{
public:
	inline	n_ary_tree_weight_transition_end_time_calculator( ) : m_weight_transition_end_time( u32(-1) ), m_event_type( 0 ) { }
	inline	u32		weight_transition_end_time				( ) const { return m_weight_transition_end_time; }
	inline	u32		event_type								( ) const { return m_event_type; }

	virtual	void	visit									( n_ary_tree_animation_node& node )
	{
		m_weight_transition_end_time	= 0;
		m_min_weight					= 1.f;

		u32 const operands_count		= node.operands_count( );
		n_ary_tree_base_node** const b	= node.operands( sizeof(n_ary_tree_animation_node) );
		n_ary_tree_base_node** i		= b;
		n_ary_tree_base_node** const e	= i + operands_count;

		if ( (*i)->is_time_scale() ) {
			R_ASSERT					( operands_count );
			++i;
		}

		for ( ; i != e; ++i ) {
			R_ASSERT					( (*i)->is_weight() );
			R_ASSERT					( !(*i)->is_time_scale() );
			(*i)->accept				( *this );
		}

		if ( !m_weight_transition_end_time ) {
			m_weight_transition_end_time = u32( -1 );
			m_event_type				= 0;
		}
		else
			m_event_type				= time_event_weight_transitions_ended;

		if ( m_min_weight == 0.f )
			m_event_type				|= time_event_animation_lexeme_ended;
	}

	virtual	void visit										( n_ary_tree_weight_transition_node& node )
	{
		m_weight_transition_end_time	= math::max( m_weight_transition_end_time, node.start_time_in_ms() + math::floor( node.interpolator().transition_time()*1000.f ) );
		m_min_weight					= math::min( m_min_weight, static_cast_checked<n_ary_tree_weight_node&>(node.to()).weight() );
	}

	virtual	void visit										( n_ary_tree_time_scale_transition_node& node )
	{
		XRAY_UNREFERENCED_PARAMETER		( node );
		NODEFAULT						( );
	}

	virtual	void visit										( n_ary_tree_weight_node& node )
	{
		m_min_weight					= math::min( m_min_weight, node.weight() );
	}

	virtual	void visit										( n_ary_tree_time_scale_node& node )
	{
		XRAY_UNREFERENCED_PARAMETER		( node );
		NODEFAULT						( );
	}

	virtual	void visit										( n_ary_tree_addition_node& node )
	{
		n_ary_tree_base_node** i		= node.operands( sizeof(n_ary_tree_addition_node) );
		n_ary_tree_base_node** const e	= i + node.operands_count( );
		for ( ; i != e; ++i )
			(*i)->accept				( *this );
	}

	virtual	void visit										( n_ary_tree_subtraction_node& node )
	{
		n_ary_tree_base_node** i		= node.operands( sizeof(n_ary_tree_addition_node) );
		n_ary_tree_base_node** const e	= i + node.operands_count( );
		for ( ; i != e; ++i )
			(*i)->accept				( *this );
	}

	virtual	void visit										( n_ary_tree_multiplication_node& node )
	{
		n_ary_tree_base_node** i		= node.operands( sizeof(n_ary_tree_addition_node) );
		n_ary_tree_base_node** const e	= i + node.operands_count( );
		for ( ; i != e; ++i )
			(*i)->accept				( *this );
	}

private:
	u32		m_weight_transition_end_time;
	float	m_min_weight;
	u32		m_event_type;
}; // class n_ary_tree_weight_transition_end_time_calculator


n_ary_tree_weight_event_iterator::n_ary_tree_weight_event_iterator(
		n_ary_tree_animation_node& animation,
		u32 const start_time_in_ms,
		u32 const initial_event_types
	) :
	m_animation											( &animation )
{
	n_ary_tree_weight_transition_end_time_calculator	weight_transition_end_time_calculator;
	weight_transition_end_time_calculator.visit			( animation );
	m_time_in_ms										= weight_transition_end_time_calculator.weight_transition_end_time( );
	if ( m_time_in_ms == u32(-1) ) {
		m_animation										= 0;
		m_event_type									= 0;
		return;
	}

	if ( !initial_event_types ) {
		m_event_type									= weight_transition_end_time_calculator.event_type();
		return;
	}

	m_time_in_ms										= start_time_in_ms;
	m_event_type										= time_event_weight_transitions_started;
}

n_ary_tree_weight_event_iterator& n_ary_tree_weight_event_iterator::operator ++	( )
{
	if ( m_event_type & time_event_weight_transitions_ended ) {
		m_time_in_ms									= u32(-1);
		m_animation										= 0;
		m_event_type									= 0;
		return											*this;
	}

	n_ary_tree_weight_transition_end_time_calculator	weight_transition_end_time_calculator;
	weight_transition_end_time_calculator.visit			( *m_animation );
	m_time_in_ms										= weight_transition_end_time_calculator.weight_transition_end_time( );
	m_event_type										= weight_transition_end_time_calculator.event_type();
	return												*this;
}

} // namespace mixing
} // namespace animation
} // namespace xray