////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_comparer.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include <xray/animation/mixing_n_ary_tree.h>
#include "mixing_n_ary_tree_node_comparer.h"
#include "mixing_n_ary_tree_size_calculator.h"
#include "interpolator_size_calculator.h"
#include "mixing_n_ary_tree_interpolator_selector.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_n_ary_tree_transition_tree_constructor_impl.h"
#include "mixing_n_ary_tree_target_time_scale_calculator.h"

using xray::animation::mixing::n_ary_tree_comparer;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_time_scale_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree_n_ary_operation_node;
using xray::animation::mixing::n_ary_tree;
using xray::animation::base_interpolator;
using xray::animation::mixing::binary_tree_base_node;
using xray::animation::bone_mixer;

void n_ary_tree_comparer::increase_buffer_size			( n_ary_tree_base_node& node )
{
	n_ary_tree_size_calculator	calculator;
	node.accept					( calculator );
	m_needed_buffer_size		+= calculator.calculated_size( );
}

template < typename T >
inline void n_ary_tree_comparer::increase_buffer_size	( T& node )
{
	n_ary_tree_size_calculator	calculator;
	calculator.visit			( node );
	m_needed_buffer_size		+= calculator.calculated_size( );
}

bool n_ary_tree_comparer::equal							( ) const
{
	return						m_equal;
}

u32 n_ary_tree_comparer::needed_buffer_size				( ) const
{
	return						m_needed_buffer_size;
}

bool xray::animation::mixing::animation_comparer_less_predicate::operator ( )	(
		n_ary_tree_animation_node const& left,
		n_ary_tree_animation_node const& right
	) const
{
	if ( left.synchronization_group_id() < right.synchronization_group_id() )
		return			true;

	if ( right.synchronization_group_id() < left.synchronization_group_id() )
		return			false;

	if ( m_use_synchronized_animations && (left.driving_animation() || right.driving_animation()) ) {
		n_ary_tree_animation_node const* const new_left = left.driving_animation() ? left.driving_animation() : &left;
		n_ary_tree_animation_node const* const new_right = right.driving_animation() ? right.driving_animation() : &right;
		if ( (*this)( *new_left, *new_right) )
			return		true;

		if ( (*this)( *new_right, *new_left) )
			return		false;

		if ( new_left == &left )
			return		true;

		if ( new_right == &right )
			return		false;

		comparison_result_enum const result = compare_animation_intervals(left, right);
		if ( result == less )
			return		true;

		if ( result == more )
			return		false;

		if ( !right.override_existing_animation() )
			return		false;

		if ( left.animation_state().animation_interval_id < right.animation_state().animation_interval_id )
			return		true;

		if ( right.animation_state().animation_interval_id < left.animation_state().animation_interval_id )
			return		false;

		return			left.animation_state().animation_interval_time < right.animation_state().animation_interval_time;
	}

	comparison_result_enum const result = compare_animation_intervals(left, right);
	if ( result == less )
		return			true;

	if ( result == more )
		return			false;

	if ( right.override_existing_animation() ) {
		if ( left.animation_state().animation_interval_id < right.animation_state().animation_interval_id )
			return		true;

		if ( right.animation_state().animation_interval_id < left.animation_state().animation_interval_id )
			return		false;

		return			left.animation_state().animation_interval_time < right.animation_state().animation_interval_time;
	}
	
	return				false;
}

bool xray::animation::mixing::animation_comparer_equal_predicate::operator ( )	( n_ary_tree_animation_node const& left, n_ary_tree_animation_node const& right ) const
{
	if ( left.synchronization_group_id() != right.synchronization_group_id() )
		return			false;

	if ( compare_animation_intervals(left, right) != equal )
		return			false;

	if ( left.driving_animation() || right.driving_animation() ) {
		if ( left.driving_animation() && !right.driving_animation() )
			return		false;

		if ( right.driving_animation() && !left.driving_animation() )
			return		false;

		R_ASSERT		( left.driving_animation() != &left );
		R_ASSERT		( right.driving_animation() != &right );

		if ( animation_comparer_less_predicate()( *left.driving_animation(), *right.driving_animation()) )
			return		false;

		if ( animation_comparer_less_predicate()( *right.driving_animation(), *left.driving_animation()) )
			return		false;

		if ( !right.override_existing_animation() )
			return		true;

		if ( left.animation_state().animation_interval_id != right.animation_state().animation_interval_id )
			return		false;

		return			left.animation_state().animation_interval_time == right.animation_state().animation_interval_time;
	}

	if ( left.animation_state().animation_interval_id != right.animation_state().animation_interval_id )
		return			false;

	return				left.animation_state().animation_interval_time == right.animation_state().animation_interval_time;
}

void n_ary_tree_comparer::process_interpolators		(
		n_ary_tree const& from,
		n_ary_tree const& to
	)
{
	u32 const from_interpolators_count			= from.interpolators_count();
	u32 const to_interpolators_count			= to.interpolators_count();
	base_interpolator const* const* const from_interpolators_begin = from.interpolators( );
	base_interpolator const* const* const to_interpolators_begin = to.interpolators( );

	u32 const total_interpolators_count			= from_interpolators_count + to_interpolators_count;
	base_interpolator const** const interpolators_begin	= static_cast<base_interpolator const**>( ALLOCA( total_interpolators_count*sizeof(base_interpolator const*) ) );
	base_interpolator const** const interpolators_end	=
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
	u32 const unique_interpolators_count		= u32(interpolators_end - interpolators_begin);

	interpolator_size_calculator				size_calculator;
	for ( base_interpolator const** i = interpolators_begin; i != interpolators_end; ++i )
		(*i)->accept							( size_calculator );

	m_needed_buffer_size						+= unique_interpolators_count*sizeof(base_interpolator const*) + size_calculator.calculated_size();
}

void n_ary_tree_comparer::new_animation			( n_ary_tree_animation_node& animation )
{
#ifndef MASTER_GOLD
	m_needed_buffer_size					+= math::align_up(strings::length( animation.identifier() ) + 1, u32(4))*sizeof(char);
#endif // #ifndef MASTER_GOLD
	m_needed_buffer_size					+= animation.animation_intervals_count()*sizeof(animation_interval);
	m_needed_buffer_size					+= sizeof( n_ary_tree_animation_node );
	m_needed_buffer_size					+= sizeof( n_ary_tree_event_iterator* );
	++m_animations_count;
}

void n_ary_tree_comparer::new_weight_transition	( float const from, float const to )
{
	XRAY_UNREFERENCED_PARAMETERS			( from, to );
	m_needed_buffer_size					+= 2*sizeof( n_ary_tree_weight_node );
	m_needed_buffer_size					+= sizeof( n_ary_tree_weight_transition_node );
	m_equal									= false;
}

void n_ary_tree_comparer::remove_animation	(
		n_ary_tree_animation_node& animation,
		n_ary_tree_animation_node const* const driving_animation,
		base_interpolator const& interpolator
	)
{
	if ( interpolator.transition_time() == 0.f )
		return;

	if ( animation.is_transitting_to_zero() ) {
		increase_buffer_size				( animation );
		++m_animations_count;
		return;
	}

	m_equal									= false;
	n_ary_tree_base_node** const operands_begin	= animation.operands( sizeof(n_ary_tree_animation_node) );
	bool const skip_first_operand			= driving_animation && animation.driving_animation() && operands_begin && (*operands_begin)->is_time_scale();
	u32 const operands_count				= animation.operands_count( ) + (skip_first_operand ? 0 : 1);
	new_animation							( animation );

	m_needed_buffer_size					+= operands_count * sizeof( n_ary_tree_base_node* );

	n_ary_tree_base_node** const operands_end	= operands_begin + animation.operands_count( );
	for ( n_ary_tree_base_node** i = operands_begin + (skip_first_operand ? 1 : 0); i != operands_end; ++i )
		increase_buffer_size				( **i );

	new_weight_transition					( 1.f, 0.f );
}

void n_ary_tree_comparer::remove_synchronization_group	(
		n_ary_tree_animation_node* const begin,
		n_ary_tree_animation_node* const end
	)
{
	n_ary_tree_animation_node* const driving_animation	= is_asynchronous_animation( *begin ) ? 0 : begin;
	m_equal									= false;
	for ( n_ary_tree_animation_node* i = begin; i != end; i = i->m_next_animation )
		remove_animation					( *i, begin, driving_animation ? driving_animation->interpolator() : (*i).interpolator() );
}

void n_ary_tree_comparer::add_animation					(
		n_ary_tree_animation_node& animation,
		base_interpolator const& interpolator
	)
{
	m_equal									= false;
	u32 const operands_count				= animation.operands_count( ) + (interpolator.transition_time() != 0.f ? 1 : 0);
	new_animation							( animation );

	m_needed_buffer_size					+= operands_count * sizeof( n_ary_tree_base_node* );

	n_ary_tree_base_node** const operands_begin	= animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node** const operands_end	= operands_begin + animation.operands_count( );
	for ( n_ary_tree_base_node** i = operands_begin; i != operands_end; ++i )
		increase_buffer_size				( **i );

	if ( animation.interpolator().transition_time() != 0.f )
		new_weight_transition				( 0.f, 1.f );
}

void n_ary_tree_comparer::add_synchronization_group	(
		n_ary_tree_animation_node* begin,
		n_ary_tree_animation_node* end
	)
{
	m_equal									= false;

	n_ary_tree_animation_node* const driving_animation	= is_asynchronous_animation( *begin ) ? 0 : begin;
	for ( n_ary_tree_animation_node* i = begin; i != end; i = i->m_next_animation )
		add_animation						( *i, driving_animation ? driving_animation->interpolator() : (*i).interpolator() );
}

void n_ary_tree_comparer::new_time_scale_transition	(
		n_ary_tree_base_node& from,
		n_ary_tree_base_node& to
	)
{
	if ( n_ary_tree_node_comparer().compare( from, to ) == n_ary_tree_node_comparer::equal ) {
		increase_buffer_size				( from );
		return;
	}

	if ( static_cast_checked<n_ary_tree_time_scale_node&>(to).interpolator().transition_time() == 0.f ) {
		increase_buffer_size				( to );
		return;
	}

	m_equal									= false;

	increase_buffer_size					( from );
	increase_buffer_size					( to );
	m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_transition_node );
}

void n_ary_tree_comparer::new_time_scale_transition	(
		n_ary_tree_base_node& from,
		float to
	)
{
	XRAY_UNREFERENCED_PARAMETER				( to );
	m_equal									= false;
	increase_buffer_size					( from );
	m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_node );
	m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_transition_node );
}

void n_ary_tree_comparer::new_time_scale_transition	(
		float from,
		n_ary_tree_base_node& to
	)
{
	XRAY_UNREFERENCED_PARAMETER				( from );

	m_equal									= false;

	if ( static_cast_checked<n_ary_tree_time_scale_node&>(to).interpolator().transition_time() == 0.f ) {
		increase_buffer_size				( to );
		return;
	}

	m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_node );
	increase_buffer_size					( to );
	m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_transition_node );
}

void n_ary_tree_comparer::new_weight_transition			(
		n_ary_tree_base_node& from,
		n_ary_tree_base_node& to
	)
{
	m_equal									= false;

	if ( static_cast_checked<n_ary_tree_weight_node&>(to).interpolator().transition_time() == 0.f ) {
		increase_buffer_size				( to );
		return;
	}

	increase_buffer_size					( from );
	increase_buffer_size					( to );
	m_needed_buffer_size					+= sizeof( n_ary_tree_weight_transition_node );
}

void n_ary_tree_comparer::new_weight_transition			(
		base_interpolator const& from_animation_interpolator,
		n_ary_tree_base_node& from,
		float to
	)
{
	if ( from_animation_interpolator.transition_time() == 0.f ) {
		m_equal								= false;
		m_needed_buffer_size				+= sizeof( n_ary_tree_weight_node );
		return;
	}

	increase_buffer_size					( from );
	n_ary_tree_weight_node weight( from_animation_interpolator, to );
	if ( n_ary_tree_node_comparer().compare(from, weight) == n_ary_tree_node_comparer::equal )
		return;

	m_equal									= false;
	m_needed_buffer_size					+= sizeof( n_ary_tree_weight_node );
	m_needed_buffer_size					+= sizeof( n_ary_tree_weight_transition_node );
}

void n_ary_tree_comparer::new_weight_transition			(
		base_interpolator const& to_animation_interpolator,
		float from,
		n_ary_tree_base_node& to
	)
{
	increase_buffer_size					( to );
	n_ary_tree_weight_node weight( to_animation_interpolator, from );
	if ( n_ary_tree_node_comparer().compare(weight, to) == n_ary_tree_node_comparer::equal )
		return;

	m_equal									= false;

	if ( static_cast_checked<n_ary_tree_weight_node&>(to).interpolator().transition_time() == 0.f )
		return;

	m_needed_buffer_size					+= sizeof( n_ary_tree_weight_node );
	m_needed_buffer_size					+= sizeof( n_ary_tree_weight_transition_node );
}

void n_ary_tree_comparer::add_operands				(
		n_ary_tree_animation_node& from,
		n_ary_tree_animation_node& to,
		bool const skip_time_scale_node
	)
{
	n_ary_tree_node_comparer				comparer;
	n_ary_tree_base_node* const* const i_b	= from.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* i			= i_b;
	n_ary_tree_base_node* const* const i_e	= i_b + from.operands_count( );
	n_ary_tree_base_node* const* const j_b	= to.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* j			= j_b;
	n_ary_tree_base_node* const* const j_e	= j_b + to.operands_count( );

	if ( (*i)->is_time_scale()  ) {
		if ( (*j)->is_time_scale() ) {
			if ( !skip_time_scale_node )
				new_time_scale_transition	( **i, **j );
			++i;
			++j;
		}
		else {
			if ( !skip_time_scale_node )
				new_time_scale_transition	( **i, 1.f );
			++i;
		}
	}
	else {
		if ( (*j)->is_time_scale() ) {
			if ( !skip_time_scale_node )
				new_time_scale_transition	( 1.f, **j );
			++j;
		}
	}

	n_ary_tree_interpolator_selector		interpolator_selector;

	for ( ; (i != i_e) && (j != j_e); ) {
		if ( comparer.compare(**i, **j) == animation::equal ) {
			n_ary_tree_transition_to_one_detector detector;
			(*i)->accept				( detector );
			if ( !detector.result() || detector.has_transitions() )
				increase_buffer_size		( **i );
			++i;
			++j;
			continue;
		}

		(*i)->accept						( interpolator_selector );
		base_interpolator const* const i_interpolator	= interpolator_selector.result( );

		(*j)->accept						( interpolator_selector );
		base_interpolator const* const j_interpolator	= interpolator_selector.result( );

		switch ( compare( *i_interpolator, *j_interpolator ) ) {
			case less : {
				n_ary_tree_transition_to_one_detector detector;
				(*i)->accept				( detector );
				if ( !detector.result() || detector.has_transitions() )
					new_weight_transition	( *i_interpolator, **i, 1.f );
				++i;
				break;
			}
			case more : {
				new_weight_transition		( *j_interpolator, 1.f, **j++ );
				break;
			}
			case animation::equal : {
				new_weight_transition		( **i, **j );
				++i;
				++j;
				break;
			}
			default : NODEFAULT( );
		}
	}

	for ( ; i != i_e; ++i ) {
		n_ary_tree_transition_to_one_detector detector;
		(*i)->accept						( detector );
		if ( !detector.result() || detector.has_transitions() )
			new_weight_transition			( from.interpolator(), **i, 1.f );
	}

	for ( ; j != j_e; )
		new_weight_transition				( to.interpolator(), 1.f, **j++ );
}

void n_ary_tree_comparer::change_animation				(
		n_ary_tree_animation_node& from,
		n_ary_tree_animation_node& to
	)
{
	bool const skip_time_scale_node		= from.driving_animation() && !to.driving_animation();
	u32 const operands_count			= computed_operands_count( from, to, skip_time_scale_node );
	new_animation						( from );

	m_needed_buffer_size				+= operands_count*sizeof( n_ary_tree_base_node*);

	add_operands						(
		from,
		to,
		skip_time_scale_node
	);
}

void n_ary_tree_comparer::merge_asynchronous_groups		(
		n_ary_tree_animation_node* const from_begin,
		n_ary_tree_animation_node* const from_end,
		n_ary_tree_animation_node* const to_begin,
		n_ary_tree_animation_node* const to_end
	)
{
	R_ASSERT							( !from_end );
	R_ASSERT							( !to_end );

	n_ary_tree_node_comparer			comparer;
	n_ary_tree_animation_node* i		= from_begin;
	n_ary_tree_animation_node* j		= to_begin;
	while ( i && j ) {
		switch ( comparer.compare( *i, *j) ) {
			case less : {
				remove_animation		( *i, 0, i->interpolator() );
				i						= i->m_next_animation;
				continue;
			}
			case more : {
				add_animation			( *j, j->interpolator() );
				j						= j->m_next_animation;
				continue;
			}
			case animation::equal : {
				change_animation		( *i, *j );
				i						= i->m_next_animation;
				j						= j->m_next_animation;
				continue;
			}
			default : NODEFAULT();
		}
	}

	for ( ; i; i = i->m_next_animation )
		remove_animation				( *i, 0, i->interpolator() );

	for ( ; j; j = j->m_next_animation )
		add_animation					( *j, j->interpolator() );
}

void n_ary_tree_comparer::new_driving_animation	(
		n_ary_tree_animation_node& animation,
		base_interpolator const& interpolator
	)
{
	m_equal								= false;

	n_ary_tree_base_node** const operands_begin	= animation.operands( sizeof(n_ary_tree_animation_node) );
	u32 const offset					= *operands_begin ? ((*operands_begin)->is_time_scale() ? 1 : 0) : 0;

	u32 const operands_count			= (animation.operands_count() - offset) + 1 + (interpolator.transition_time() != 0.f ? 1 : 0); // for time scale and weight transition nodes
	new_animation						( animation );

	m_needed_buffer_size				+= operands_count * sizeof( n_ary_tree_base_node* );

	n_ary_tree_weight_node temp( interpolator, 1.f );
	n_ary_tree_node_comparer comparer;
	bool has_weight_transition_been_added = false;

	n_ary_tree_base_node** const operands_end	= operands_begin + animation.operands_count( );
	for ( n_ary_tree_base_node** i = operands_begin + offset; i != operands_end; ++i ) {
		if ( comparer.compare(**i, temp) != more ) {
			increase_buffer_size		( **i );
			continue;
		}

		R_ASSERT						( !has_weight_transition_been_added );
		has_weight_transition_been_added = true;

		if ( interpolator.transition_time() != 0.f )
			new_weight_transition		( 0.f, 1.f );

		for ( ; i != operands_end; ++i ) 
			increase_buffer_size		( **i );

		break;
	}

	if ( !has_weight_transition_been_added && ( interpolator.transition_time() != 0.f ) )
		new_weight_transition			( 0.f, 1.f );
}

void n_ary_tree_comparer::new_driving_animation	(
		n_ary_tree_animation_node& new_driving_animation,
		n_ary_tree_animation_node& new_driving_animation_in_previous_target,
		bool const skip_time_scale_node
	)
{
	if ( new_driving_animation_in_previous_target.driving_animation() )
		m_equal							= false;

	u32 const operands_count			= computed_operands_count( new_driving_animation_in_previous_target, new_driving_animation, skip_time_scale_node ) + 1;
	new_animation						( new_driving_animation );

	m_needed_buffer_size				+= operands_count*sizeof( n_ary_tree_base_node*);

	add_operands						(
		new_driving_animation_in_previous_target,
		new_driving_animation,
		skip_time_scale_node
	);
}

void n_ary_tree_comparer::merge_synchronization_groups	(
		n_ary_tree_animation_node* from_begin,
		n_ary_tree_animation_node* from_end,
		n_ary_tree_animation_node* to_begin,
		n_ary_tree_animation_node* to_end,
		n_ary_tree_animation_node& new_driving_animation,
		base_interpolator const& interpolator
	)
{
	u32 from_animations_count			= 0;
	for ( n_ary_tree_animation_node* i = from_begin; i != from_end; i = i->m_next_animation )
		++from_animations_count;
	
	bool new_driving_animation_in_old_target_found	= false;
	n_ary_tree_animation_node** const new_from_begin = static_cast< n_ary_tree_animation_node** >( ALLOCA(from_animations_count*sizeof(n_ary_tree_animation_node*)) );
	n_ary_tree_animation_node* j		= from_begin;
	for ( n_ary_tree_animation_node** i = new_from_begin; j != from_end; j = j->m_next_animation ) {
		if (
				(compare_animation_intervals(*j, new_driving_animation) != xray::animation::equal) ||
				(
					new_driving_animation.override_existing_animation() &&
					(
						((*j).animation_state().animation_interval_time != new_driving_animation.animation_state().animation_interval_time) ||
						((*j).animation_state().animation_interval_id != new_driving_animation.animation_state().animation_interval_id)
					)
				)
			)
			*i++						= j;
		else {
			R_ASSERT					( !new_driving_animation_in_old_target_found );
			new_driving_animation_in_old_target_found	= true;
		}
	}

	if ( new_driving_animation_in_old_target_found )
		--from_animations_count;

	std::sort							( new_from_begin, new_from_begin + from_animations_count, animation_comparer_less_predicate(false) );

	n_ary_tree_node_comparer			comparer;
	j									= to_begin;
	n_ary_tree_animation_node** i		= new_from_begin;
	n_ary_tree_animation_node** const e	= new_from_begin + from_animations_count;
	while ( (i != e) && (j != to_end) ) {
		switch ( comparer.compare( **i, *j) ) {
			case less : {
				remove_animation		( **i++, &new_driving_animation, interpolator );
				continue;
			}
			case more : {
				add_animation			( *j, interpolator );
				j						= j->m_next_animation;
				continue;
			}
			case animation::equal : {
				change_animation		( **i++, *j );
				j						= j->m_next_animation;
				continue;
			}
			default : NODEFAULT();
		}
	}

	for ( ; i != e; )
		remove_animation				( **i++, &new_driving_animation, interpolator );

	for ( ; j != to_end; j = j->m_next_animation )
		add_animation					( *j, interpolator );
}

void n_ary_tree_comparer::change_synchronization_group	(
		n_ary_tree_animation_node* const from_begin,
		n_ary_tree_animation_node* const from_end,
		n_ary_tree_animation_node* const to_begin,
		n_ary_tree_animation_node* const to_end,
		u32 const current_time_in_ms
	)
{
	if ( is_asynchronous_animation(*from_begin) ) {
		merge_asynchronous_groups				( from_begin, from_end, to_begin, to_end );
		return;
	}

	n_ary_tree_animation_node& new_driving_animation	= *to_begin;
	n_ary_tree_animation_node& old_driving_animation	= *from_begin;

	R_ASSERT_CMP								( new_driving_animation.synchronization_group_id(), ==, old_driving_animation.synchronization_group_id() );

	R_ASSERT									( new_driving_animation.animation_intervals_count() == old_driving_animation.animation_intervals_count() );
	u32 const current_animation_interval_id		= old_driving_animation.animation_state().animation_interval_id;
	float const new_driving_animation_length	= new_driving_animation.animation_intervals()[current_animation_interval_id].length( );
	R_ASSERT_CMP								( new_driving_animation_length, >, 0.f );

	float const old_driving_animation_length	= old_driving_animation.animation_intervals()[current_animation_interval_id].length( );
	R_ASSERT_CMP								( old_driving_animation_length, >, 0.f );
	
	float const time_scale_factor				= new_driving_animation_length/old_driving_animation_length;

	// get new driving animation time offset
	float const old_driving_animation_time		= from_begin->animation_state().animation_interval_time;
	R_ASSERT_CMP								( old_driving_animation_time, >=, 0.f );

	// get new driving animation target time scale
	n_ary_tree_time_scale_calculator time_scale_calculator	( current_time_in_ms, 0.f, current_time_in_ms, n_ary_tree_time_scale_calculator::forbid_transitions_destroying );
	(**new_driving_animation.operands( sizeof(n_ary_tree_animation_node)) ).accept	( time_scale_calculator );
	float const new_driving_animation_target_time_scale	= time_scale_calculator.time_scale();

	// clone all current time scale transitions from old driving animation, multiplying them by time_scale_factor
	R_ASSERT									( !math::is_zero(time_scale_factor) );
	n_ary_tree_base_node* const old_time_scale_node	= *old_driving_animation.operands( sizeof(n_ary_tree_animation_node));
	increase_buffer_size						( *old_time_scale_node );

	// get new driving animation current time scale
	float const new_driving_animation_current_time_scale	= n_ary_tree_target_time_scale_calculator( old_driving_animation ).result() * time_scale_factor;

	// create new time scale transition if needed
	if ( new_driving_animation_target_time_scale != new_driving_animation_current_time_scale ) {
		m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_node );
		m_needed_buffer_size					+= sizeof( n_ary_tree_time_scale_transition_node );
		m_equal									= false;
	}

	n_ary_tree_animation_node* const new_driving_animation_in_old_target = find_animation( from_begin, from_end, *to_begin );
	if ( new_driving_animation_in_old_target )
		this->new_driving_animation (
			new_driving_animation,
			*new_driving_animation_in_old_target,
			true
		);
	else
		this->new_driving_animation (
			new_driving_animation,
			*time_scale_calculator.interpolator()
		);

	merge_synchronization_groups	(
		from_begin,
		from_end,
		to_begin->m_next_animation,
		to_end,
		*to_begin,
		*time_scale_calculator.interpolator()
	);
}

void n_ary_tree_comparer::merge_trees					(
		n_ary_tree const& from,
		n_ary_tree const& to,
		u32 const current_time_in_ms
	)
{
	n_ary_tree_animation_node* i_begin		= from.root();
	n_ary_tree_animation_node* i_end		= synchronization_group_end( i_begin );
	n_ary_tree_animation_node* j_begin		= to.root();
	n_ary_tree_animation_node* j_end		= synchronization_group_end( j_begin );
	while ( i_begin && j_begin ) {
		if ( (*i_begin).synchronization_group_id() < (*j_begin).synchronization_group_id() ) {
			m_equal							= false;
			remove_synchronization_group	( i_begin, i_end );
			get_next_synchronization_group	( i_begin, i_end );
			continue;
		}

		if ( (*i_begin).synchronization_group_id() > (*j_begin).synchronization_group_id() ) {
			m_equal							= false;
			add_synchronization_group		( j_begin, j_end );
			get_next_synchronization_group	( j_begin, j_end );
			continue;
		}

		change_synchronization_group		( i_begin, i_end, j_begin, j_end, current_time_in_ms );
		get_next_synchronization_group		( i_begin, i_end );
		get_next_synchronization_group		( j_begin, j_end );
	}

	while ( i_begin ) {
		m_equal								= false;
		remove_synchronization_group		( i_begin, i_end );
		get_next_synchronization_group		( i_begin, i_end );
	}

	while ( j_begin ) {
		m_equal								= false;
		add_synchronization_group			( j_begin, j_end );
		get_next_synchronization_group		( j_begin, j_end );
	}
}

n_ary_tree_comparer::n_ary_tree_comparer				(
		n_ary_tree const& from,
		n_ary_tree const& to,
		u32 const current_time_in_ms
	) :
	m_animations_count						( 0 ),
	m_needed_buffer_size					( 0 ),
	m_equal									( true )
{
	m_needed_buffer_size					+= sizeof(n_ary_tree_intrusive_base);

	process_interpolators					( from, to );

	merge_trees								( from, to, current_time_in_ms );

	m_needed_buffer_size					+= m_animations_count*sizeof(animation_state);
}