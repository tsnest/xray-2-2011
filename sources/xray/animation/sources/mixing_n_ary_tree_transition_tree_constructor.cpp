////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_transition_tree_constructor.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "mixing_n_ary_tree_weight_transition_node.h"
#include "mixing_n_ary_tree_time_scale_transition_node.h"
#include <xray/animation/mixing_n_ary_tree.h>
#include "mixing_n_ary_tree_node_comparer.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"
#include "mixing_n_ary_tree_node_comparer.h"
#include "mixing_n_ary_tree_interpolator_selector.h"
#include "mixing_n_ary_tree_transition_tree_constructor_impl.h"
#include "mixing_n_ary_tree_comparer.h"
#include "i_editor_mixer_inline.h"
#include "mixing_n_ary_tree_target_time_scale_calculator.h"
#include "mixing_n_ary_tree_weight_calculator.h"

using xray::animation::mixing::n_ary_tree_transition_tree_constructor;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_transition_node;
using xray::animation::mixing::n_ary_tree_time_scale_transition_node;
using xray::animation::mixing::n_ary_tree;
using xray::animation::base_interpolator;
using xray::animation::mixing::animation_state;

#define XRAY_DEBUG_ANIMATIONS_COUNT			0

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::add_animation_node	(
		n_ary_tree_animation_node& new_animation,
		animation_state const* const previous_animation_state,
		u32 const animation_interval_id,
		float const animation_interval_time,
		bool const is_new_animation
	)
{
	*m_new_animation_event++					= m_new_animation_state;

	u32 initial_event_types						= 0;
	if ( is_new_animation )
		initial_event_types						= time_event_animation_lexeme_started | time_event_weight_transitions_started;
	else {
		if ( previous_animation_state ) {
			if ( !previous_animation_state->are_there_any_weight_transitions )
				initial_event_types					= time_event_weight_transitions_started;
		}
	}

	n_ary_tree_weight_calculator weight_calculator(m_current_time_in_ms);
	weight_calculator.visit( new_animation );

	new ( m_new_animation_state ) animation_state (
		new_animation,
		m_current_time_in_ms,
		initial_event_types,
		animation_interval_id,
		animation_interval_time,
		weight_calculator.weight(),
		m_channels_head
	);

	if ( previous_animation_state ) {
		m_new_animation_state->are_there_any_weight_transitions	= previous_animation_state->are_there_any_weight_transitions;
		m_new_animation_state->bone_matrices_computer			= previous_animation_state->bone_matrices_computer;
	}

#if XRAY_DEBUG_ANIMATIONS_COUNT
	++m_animations_count;
#endif // #if XRAY_DEBUG_ANIMATIONS_COUNT

	new_animation.set_animation_state			( *m_new_animation_state++ );
	return										&new_animation;
}

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::add_animation_node	(
		n_ary_tree_animation_node& new_animation,
		animation_state const* const previous_animation_state,
		bool const is_new_animation
	)
{
	u32 animation_interval_id;
	float animation_interval_time;
	if ( new_animation.driving_animation() ) {
		animation_state const& driving_animation_state	= new_animation.driving_animation()->animation_state();
		animation_interval_id	= driving_animation_state.animation_interval_id;
		animation_interval const* const driving_animation_interval	= new_animation.driving_animation()->animation_intervals() + animation_interval_id;
		animation_interval const* const animation_interval			= new_animation.animation_intervals() + animation_interval_id;
		animation_interval_time	= animation_interval->length() / driving_animation_interval->length() * driving_animation_state.animation_interval_time;
	}
	else {
		R_ASSERT				( previous_animation_state );
		animation_interval_id	= previous_animation_state->animation_interval_id;
		animation_interval_time	= previous_animation_state->animation_interval_time;
	}

	return						add_animation_node( new_animation, previous_animation_state, animation_interval_id, animation_interval_time, is_new_animation );
}

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::new_animation (
		n_ary_tree_animation_node& animation,
		n_ary_tree_animation_node* const driving_animation,
		u32 const operands_count,
		bool const is_transitting_to_zero
	)
{
#ifndef MASTER_GOLD
	u32 const buffer_size	= math::align_up(strings::length( animation.identifier() ) + 1, u32(4))*sizeof(char);
	memory::copy			( m_buffer.c_ptr(), buffer_size, animation.identifier(), buffer_size );
	pcstr const identifier	= static_cast<pcstr>( m_buffer.c_ptr() );
	m_buffer				+= buffer_size;
#endif // #ifndef MASTER_GOLD

	animation_interval const* current_interval	= animation.animation_intervals( );
	animation_interval const* intervals_end		= current_interval + animation.animation_intervals_count( );
	animation_interval const* cloned_intervals_begin	= static_cast<animation_interval const*>( m_buffer.c_ptr() );
	for ( ; current_interval != intervals_end; ++current_interval, m_buffer += sizeof( animation_interval) ) {
		new ( m_buffer.c_ptr() ) animation_interval(
			current_interval->animation(),
			current_interval->start_time(),
			current_interval->length()
		);
	}

	n_ary_tree_animation_node* const result	= static_cast< n_ary_tree_animation_node* >( m_buffer.c_ptr() );
	u32 const additivity_priority	= animation.additivity_priority();
	if ( !driving_animation )
		new ( result ) n_ary_tree_animation_node(
#ifndef MASTER_GOLD
			identifier,
#endif // #ifndef MASTER_GOLD
			cloned_intervals_begin,
			cloned_intervals_begin + animation.animation_intervals_count( ),
			animation.start_cycle_animation_interval_id( ),
			*m_cloner.clone( animation.interpolator() ),
			animation.playing_type(),
			animation.synchronization_group_id(),
			animation.override_existing_animation(),
			additivity_priority,
			operands_count,
			is_transitting_to_zero
		);
	else {
		R_ASSERT_CMP					( driving_animation->animation_intervals_count(), ==, animation.animation_intervals_count() );
		new ( result ) n_ary_tree_animation_node (
#ifndef MASTER_GOLD
			identifier,
#endif // #ifndef MASTER_GOLD
			*driving_animation,
			cloned_intervals_begin,
			cloned_intervals_begin + animation.animation_intervals_count( ),
			*m_cloner.clone( animation.interpolator() ),
			animation.playing_type(),
			animation.override_existing_animation(),
			additivity_priority,
			operands_count,
			is_transitting_to_zero
		);
	}

	m_buffer							+= sizeof( n_ary_tree_animation_node );

#ifndef MASTER_GOLD
	result->user_data					= animation.user_data;
#endif // #ifndef MASTER_GOLD
	
	if ( !m_result )
		m_result								= result;
	else {
		R_ASSERT								( m_previous_animation );
		m_previous_animation->m_next_animation	= result;
	}

	m_previous_animation						= result;

	i_editor_mixer::call_user_callback	( editor_animations_event::new_animation, m_current_time_in_ms, result, 0 );
	return								result;
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_weight_transition (
		base_interpolator const& interpolator,
		float from,
		float to
	)
{
	if ( (interpolator.transition_time() == 0.f) && (to != 0.f) ) {
		n_ary_tree_weight_node*	const weight_to			= static_cast<n_ary_tree_weight_node*>( m_buffer.c_ptr() );
		m_buffer										+= sizeof( n_ary_tree_weight_node );
		new ( weight_to ) n_ary_tree_weight_node		( *m_cloner.clone( interpolator ), to );
		return											weight_to;
	}

	n_ary_tree_weight_transition_node* const transition	= static_cast<n_ary_tree_weight_transition_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_transition_node );

	n_ary_tree_weight_node*	const weight_from			= static_cast<n_ary_tree_weight_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_node );
	new ( weight_from ) n_ary_tree_weight_node			( *m_cloner.clone( interpolator ), from );

	n_ary_tree_weight_node*	const weight_to				= static_cast<n_ary_tree_weight_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_node );
	new ( weight_to ) n_ary_tree_weight_node			( *m_cloner.clone( interpolator ), to );

	new( transition ) n_ary_tree_weight_transition_node	( *weight_from, *weight_to, *m_cloner.clone( interpolator ), m_current_time_in_ms );

	return												transition;
}

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::remove_animation	(
		n_ary_tree_animation_node& animation,
		n_ary_tree_animation_node* const driving_animation,
		base_interpolator const& interpolator
	)
{
	if ( interpolator.transition_time() == 0.f )
		return							0;

	if ( animation.is_transitting_to_zero() ) {
		n_ary_tree_animation_node* const result	= new_animation( animation, driving_animation, animation.operands_count(), animation.is_transitting_to_zero() );

		n_ary_tree_base_node** new_operands		= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr() );
		m_buffer								+= animation.operands_count() * sizeof( n_ary_tree_base_node* );

		for (
				n_ary_tree_base_node** i = animation.operands( sizeof(n_ary_tree_animation_node) ),
				**operands_end	= i + animation.operands_count();
				i != operands_end;
				++i, ++new_operands
			) {
			if ( (*i)->is_time_scale() )
				*new_operands					= m_cloner.clone( **i, 1.f );
			else
				*new_operands					= m_cloner.clone( **i );
		}

		return									add_animation_node( *result, &animation.animation_state(), false );
	}

	n_ary_tree_base_node** const operands_begin	= animation.operands( sizeof(n_ary_tree_animation_node) );
	bool const skip_first_operand		= driving_animation && animation.driving_animation() && operands_begin && (*operands_begin)->is_time_scale();
	u32 const operands_count			= animation.operands_count( ) + (skip_first_operand ? 0 : 1);

	n_ary_tree_animation_node* const result =
		new_animation(
			animation,
			driving_animation,
			operands_count,
			true
		);

	n_ary_tree_base_node** new_operands	= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr() );
	m_buffer							+= operands_count * sizeof( n_ary_tree_base_node* );

#ifndef MASTER_GOLD
	n_ary_tree_base_node** const new_operands_begin = new_operands;
#endif // #ifndef MASTER_GOLD

	n_ary_tree_weight_node temp( interpolator, 0.f );
	n_ary_tree_node_comparer comparer;

	bool has_weight_transition_been_added = false;
	n_ary_tree_base_node** const operands_end	= operands_begin + animation.operands_count( );
	for ( n_ary_tree_base_node** i = operands_begin + (skip_first_operand ? 1 : 0); i != operands_end; ++i, ++new_operands ) {
		if ( comparer.compare( **i, temp) != more ) {
			*new_operands				= (*i)->is_time_scale() ? m_cloner.clone( **i, 1.f ) : m_cloner.clone( **i );
			continue;
		}

		R_ASSERT						( !has_weight_transition_been_added );
		has_weight_transition_been_added = true;

		*new_operands++					= new_weight_transition( *m_cloner.clone( interpolator ), 1.f, 0.f );

		for ( ; i != operands_end; ++i, ++new_operands ) 
			*new_operands				= m_cloner.clone( **i );

		break;
	}

	if ( !has_weight_transition_been_added )
		*new_operands++					= new_weight_transition( *m_cloner.clone( interpolator ), 1.f, 0.f );

#ifndef MASTER_GOLD
	std::sort							( new_operands_begin, new_operands, n_ary_tree_node_comparer() );
	for (n_ary_tree_base_node** i = new_operands_begin, **j = i + 1; j != new_operands; ++i, ++j )
		R_ASSERT_CMP					( n_ary_tree_node_comparer().compare( **i, **j ), !=, more );
#endif // #ifndef MASTER_GOLD

	return								add_animation_node( *result, &animation.animation_state(), false );
}

void n_ary_tree_transition_tree_constructor::remove_synchronization_group	(
		n_ary_tree_animation_node* const begin,
		n_ary_tree_animation_node* const end
	)
{
	n_ary_tree_animation_node* const first_animation	= remove_animation( *begin, 0, begin->interpolator() );
	if ( !first_animation )
		return;

	n_ary_tree_animation_node* const driving_animation	= is_asynchronous_animation( *first_animation ) ? 0 : first_animation;
	for ( n_ary_tree_animation_node* i = begin->m_next_animation; i != end; i = i->m_next_animation )
		remove_animation				(
			*i,
			driving_animation,
			driving_animation ? driving_animation->interpolator() : (*i).interpolator()
		);
}

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::add_animation(
		n_ary_tree_animation_node& animation,
		n_ary_tree_animation_node* const driving_animation,
		base_interpolator const& interpolator
	)
{
	u32 const operands_count			= animation.operands_count() + ((interpolator.transition_time() != 0.f) ? 1 : 0);
	n_ary_tree_animation_node* const result =
		new_animation(
			animation,
			driving_animation,
			operands_count,
			false
		);

	n_ary_tree_base_node** new_operands	= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr() );
	m_buffer							+= operands_count * sizeof( n_ary_tree_base_node* );

#ifndef MASTER_GOLD
	n_ary_tree_base_node** const new_operands_begin = new_operands;
#endif // #ifndef MASTER_GOLD

	n_ary_tree_weight_node temp( interpolator, 1.f );
	n_ary_tree_node_comparer comparer;
	bool has_weight_transition_been_added = false;

	n_ary_tree_base_node** const operands_begin	= animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node** const operands_end	= operands_begin + animation.operands_count( );
	for ( n_ary_tree_base_node** i = operands_begin; i != operands_end; ++i, ++new_operands ) {
		if ( comparer.compare(**i, temp) != more ) {
#ifndef MASTER_GOLD
			n_ary_tree_transition_to_one_detector detector;
			(*i)->accept				( detector );
			R_ASSERT					( !detector.result() || detector.has_transitions() );
#endif // #ifndef MASTER_GOLD

			*new_operands				= (*i)->is_time_scale() ? m_cloner.clone( **i, 1.f ) : m_cloner.clone( **i );
			continue;
		}

		R_ASSERT						( !has_weight_transition_been_added );
		has_weight_transition_been_added = true;

		if ( interpolator.transition_time() != 0.f )
			*new_operands++				= new_weight_transition( *m_cloner.clone( interpolator ), 0.f, 1.f );

		for ( ; i != operands_end; ++i, ++new_operands )  {
#ifndef MASTER_GOLD
			n_ary_tree_transition_to_one_detector detector;
			(*i)->accept				( detector );
			R_ASSERT					( !detector.result() || detector.has_transitions() );
#endif // #ifndef MASTER_GOLD

			*new_operands				= m_cloner.clone( **i );
		}

		break;
	}

	if ( !has_weight_transition_been_added && (interpolator.transition_time() != 0.f) )
		*new_operands++					= new_weight_transition( *m_cloner.clone( interpolator ), 0.f, 1.f );

	std::sort							( operands_begin, operands_end, n_ary_tree_node_comparer() );
#ifndef MASTER_GOLD
	for (n_ary_tree_base_node** i = new_operands_begin, **j = i + 1; j != new_operands; )
		R_ASSERT_CMP					( n_ary_tree_node_comparer().compare( **i++, **j++ ), !=, more );
#endif // #ifndef MASTER_GOLD

	return								add_animation_node( *result, &animation.animation_state(), true );
}

void n_ary_tree_transition_tree_constructor::add_synchronization_group		(
		n_ary_tree_animation_node* const begin,
		n_ary_tree_animation_node* const end
	)
{
	n_ary_tree_animation_node* const first_animation	= add_animation( *begin, 0, begin->interpolator() );
	n_ary_tree_animation_node* const driving_animation	= is_asynchronous_animation( *first_animation ) ? 0 : first_animation;
	for ( n_ary_tree_animation_node* i = begin->m_next_animation; i != end; i = i->m_next_animation )
		add_animation					(
			*i,
			driving_animation,
			driving_animation ? driving_animation->interpolator() : (*i).interpolator()
		);
}

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::new_driving_animation(
		n_ary_tree_animation_node& animation,
		u32 const animation_interval_id,
		float const animation_interval_time,
		n_ary_tree_base_node* const time_scale_node,
		base_interpolator const& interpolator
	)
{
	n_ary_tree_base_node** const operands_begin	= animation.operands( sizeof(n_ary_tree_animation_node) );
	u32 const offset					= *operands_begin ? ((*operands_begin)->is_time_scale() ? 1 : 0) : 0;

	u32 const operands_count			= (animation.operands_count() - offset) + 1 + ((interpolator.transition_time() != 0.f) ? 1 : 0); // for time scale and weight transition nodes
	n_ary_tree_animation_node* const result	=
		new_animation(
			animation,
			0,
			operands_count,
			false
		);

	n_ary_tree_base_node** new_operands	= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr() );
	m_buffer							+= operands_count * sizeof( n_ary_tree_base_node* );

#ifndef MASTER_GOLD
	n_ary_tree_base_node** const new_operands_begin = new_operands;
#endif // #ifndef MASTER_GOLD

	*new_operands++						= time_scale_node;

	n_ary_tree_weight_node temp( interpolator, 1.f );
	n_ary_tree_node_comparer comparer;
	bool has_weight_transition_been_added = false;

	n_ary_tree_base_node** const operands_end	= operands_begin + animation.operands_count( );
	for ( n_ary_tree_base_node** i = operands_begin + offset; i != operands_end; ++i, ++new_operands ) {
		if ( comparer.compare(**i, temp) != more ) {
			*new_operands				= m_cloner.clone( **i );
			continue;
		}

		R_ASSERT						( !has_weight_transition_been_added );
		has_weight_transition_been_added = true;

		if ( interpolator.transition_time() != 0.f )
			*new_operands++				= new_weight_transition( *m_cloner.clone( interpolator ), 0.f, 1.f );

		for ( ; i != operands_end; ++i, ++new_operands ) 
			*new_operands				= m_cloner.clone( **i );

		break;
	}

	if ( !has_weight_transition_been_added && ( interpolator.transition_time() != 0.f ) )
		*new_operands++					= new_weight_transition( *m_cloner.clone( interpolator ), 0.f, 1.f );

	std::sort							( operands_begin, operands_end, n_ary_tree_node_comparer() );
#ifndef MASTER_GOLD
	for (n_ary_tree_base_node** i = new_operands_begin, **j = i + 1; j != new_operands; )
		R_ASSERT_CMP					( n_ary_tree_node_comparer().compare( **i++, **j++ ), !=, more );
#endif // #ifndef MASTER_GOLD

	return								add_animation_node( *result, 0, animation_interval_id, animation_interval_time, true );
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_time_scale_transition	(
		n_ary_tree_animation_node& from_animation,
		n_ary_tree_base_node& from,
		n_ary_tree_base_node& to,
		base_interpolator const& to_interpolator
	)
{
	if ( n_ary_tree_node_comparer().compare( from, to ) == n_ary_tree_node_comparer::equal )
		return									m_cloner.clone( to, 1.f, from_animation.animation_state().animation_interval_time );

	if ( to_interpolator.transition_time() == 0.f )
		return									m_cloner.clone( to, 1.f, from_animation.animation_state().animation_interval_time );

	n_ary_tree_time_scale_transition_node* const result	= static_cast< n_ary_tree_time_scale_transition_node *>( m_buffer.c_ptr() );
	m_buffer									+= sizeof( n_ary_tree_time_scale_transition_node );

	n_ary_tree_base_node* const time_scale_from	= m_cloner.clone( from, 1.f );
	n_ary_tree_base_node* const time_scale_to	= m_cloner.clone( to, 1.f );

	n_ary_tree_interpolator_selector			interpolator_selector;
	time_scale_to->accept						( interpolator_selector );
	base_interpolator const* const interpolator	= m_cloner.clone( *interpolator_selector.result() );

	new ( result ) n_ary_tree_time_scale_transition_node (
		*time_scale_from,
		*time_scale_to,
		*interpolator,
		m_current_time_in_ms,
		0,
		from_animation.animation_state().animation_interval_time
	);

	return										result;
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_time_scale_transition	(
		n_ary_tree_animation_node& from_animation,
		n_ary_tree_base_node& from,
		float const to
	)
{
	n_ary_tree_interpolator_selector	interpolator_selector;
	from.accept							( interpolator_selector );
	if ( interpolator_selector.result()->transition_time() == 0.f ) {
		base_interpolator const* const interpolator	= m_cloner.clone( *interpolator_selector.result() );
		n_ary_tree_time_scale_node* const time_scale_to	= static_cast<n_ary_tree_time_scale_node*>( m_buffer.c_ptr() );
		m_buffer						+= sizeof( n_ary_tree_time_scale_node );
		new ( time_scale_to ) n_ary_tree_time_scale_node( *interpolator, to, from_animation.animation_state().animation_interval_time, m_current_time_in_ms );
		return							time_scale_to;
	}

	n_ary_tree_time_scale_transition_node* const result	= static_cast<n_ary_tree_time_scale_transition_node*>( m_buffer.c_ptr() );
	m_buffer					+= sizeof( n_ary_tree_time_scale_transition_node );

	n_ary_tree_base_node* const time_scale_from = m_cloner.clone( from, 1.f );
	base_interpolator const* const interpolator	= m_cloner.clone( *interpolator_selector.result() );

	n_ary_tree_time_scale_node* const time_scale_to	= static_cast<n_ary_tree_time_scale_node*>( m_buffer.c_ptr() );
	m_buffer					+= sizeof( n_ary_tree_time_scale_node );
	new ( time_scale_to ) n_ary_tree_time_scale_node( *interpolator, to, from_animation.animation_state().animation_interval_time, m_current_time_in_ms );

	new ( result ) n_ary_tree_time_scale_transition_node (
		*time_scale_from,
		*time_scale_to,
		*interpolator,
		m_current_time_in_ms,
		0,
		from_animation.animation_state().animation_interval_time
	);

	return						result;
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_time_scale_transition	(
		float const animation_time,
		float const from,
		n_ary_tree_base_node& to,
		base_interpolator const& to_interpolator
	)
{
	if ( to_interpolator.transition_time() == 0.f )
		return											m_cloner.clone( to, 1.f, animation_time );

	n_ary_tree_time_scale_transition_node* const result	= static_cast<n_ary_tree_time_scale_transition_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_time_scale_transition_node );

	n_ary_tree_base_node* const time_scale_to			= m_cloner.clone( to, 1.f );
	
	n_ary_tree_interpolator_selector					interpolator_selector;
	time_scale_to->accept								( interpolator_selector );
	base_interpolator const* const interpolator			= m_cloner.clone( *interpolator_selector.result() );

	n_ary_tree_time_scale_node* const time_scale_from	= static_cast<n_ary_tree_time_scale_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_time_scale_node );
	new ( time_scale_from ) n_ary_tree_time_scale_node	( *interpolator, from, animation_time, m_current_time_in_ms );

	new ( result ) n_ary_tree_time_scale_transition_node(
		*time_scale_from,
		*time_scale_to,
		*interpolator,
		m_current_time_in_ms,
		0,
		0.f
	);

	return												result;
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_weight_transition	(
		n_ary_tree_base_node& from,
		n_ary_tree_base_node& to
	)
{
	if ( static_cast_checked<n_ary_tree_weight_node&>(to).interpolator().transition_time() == 0.f )
		return											m_cloner.clone( to );

	n_ary_tree_weight_transition_node* const transition	= static_cast<n_ary_tree_weight_transition_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_transition_node );

	n_ary_tree_base_node* const weight_from				= m_cloner.clone( from );
	n_ary_tree_base_node* const weight_to				= m_cloner.clone( to );

	n_ary_tree_interpolator_selector					interpolator_selector;
	weight_to->accept									( interpolator_selector );
	base_interpolator const* const interpolator			= m_cloner.clone( *interpolator_selector.result() );

	new( transition ) n_ary_tree_weight_transition_node	( *weight_from, *weight_to, *interpolator, m_current_time_in_ms );

	return												transition;
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_weight_transition	(
		base_interpolator const& from_animation_interpolator,
		n_ary_tree_base_node& from,
		float to
	)
{
	if ( from_animation_interpolator.transition_time() == 0.f ) {
		base_interpolator const* const interpolator		= m_cloner.clone( from_animation_interpolator );
		n_ary_tree_weight_node*	const weight_to			= static_cast<n_ary_tree_weight_node*>( m_buffer.c_ptr() );
		m_buffer										+= sizeof( n_ary_tree_weight_node );
		new ( weight_to ) n_ary_tree_weight_node		( *interpolator, to );
		return											weight_to;
	}

	n_ary_tree_weight_node weight( from_animation_interpolator, to );
	if ( n_ary_tree_node_comparer().compare(from, weight) == n_ary_tree_node_comparer::equal )
		return											m_cloner.clone( from );

	n_ary_tree_weight_transition_node* const transition	= static_cast<n_ary_tree_weight_transition_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_transition_node );

	n_ary_tree_base_node* const	weight_from				= m_cloner.clone( from );

	base_interpolator const* const interpolator			= m_cloner.clone( from_animation_interpolator );

	n_ary_tree_weight_node*	const weight_to				= static_cast<n_ary_tree_weight_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_node );
	new ( weight_to ) n_ary_tree_weight_node			( *interpolator, to );

	new ( transition ) n_ary_tree_weight_transition_node( *weight_from, *weight_to, *interpolator, m_current_time_in_ms );

	return												transition;
}

n_ary_tree_base_node* n_ary_tree_transition_tree_constructor::new_weight_transition	(
		base_interpolator const& to_animation_interpolator,
		float from,
		n_ary_tree_base_node& to
	)
{
	if ( static_cast_checked<n_ary_tree_weight_node&>(to).interpolator().transition_time() == 0.f )
		return											m_cloner.clone( to );

	n_ary_tree_weight_node weight( to_animation_interpolator, from );
	if ( n_ary_tree_node_comparer().compare(weight, to) == n_ary_tree_node_comparer::equal )
		return											m_cloner.clone( to );

	n_ary_tree_weight_transition_node* const transition	= static_cast<n_ary_tree_weight_transition_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_transition_node );

	n_ary_tree_base_node* const	weight_to				= m_cloner.clone( to );

	n_ary_tree_interpolator_selector					interpolator_selector;
	weight_to->accept									( interpolator_selector );
	base_interpolator const* const interpolator			= m_cloner.clone( *interpolator_selector.result() );

	n_ary_tree_weight_node*	const weight_from			= static_cast<n_ary_tree_weight_node*>( m_buffer.c_ptr() );
	m_buffer											+= sizeof( n_ary_tree_weight_node );
	new ( weight_from ) n_ary_tree_weight_node			( *interpolator, from );

	new ( transition ) n_ary_tree_weight_transition_node( *weight_from, *weight_to, *interpolator, m_current_time_in_ms );

	return												transition;
}

void n_ary_tree_transition_tree_constructor::add_operands	(
		n_ary_tree_animation_node& from,
		n_ary_tree_animation_node& to,
		n_ary_tree_base_node** const operands_begin,
		n_ary_tree_base_node** const operands_end,
		bool const skip_time_scale_node
	)
{
	n_ary_tree_base_node** operands			= operands_begin;

	base_interpolator const& from_interpolator	= from.interpolator();
	base_interpolator const& to_interpolator	= to.interpolator();

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
				*operands++					= new_time_scale_transition( from, **i, **j, to_interpolator );
			++i;
			++j;
		}
		else {
			if ( !skip_time_scale_node )
				*operands++					= new_time_scale_transition( from, **i, 1.f );
			++i;
		}
	}
	else {
		if ( (*j)->is_time_scale() ) {
			if ( !skip_time_scale_node )
				*operands++					= new_time_scale_transition( from.animation_state().animation_interval_time, 1.f, **j, to_interpolator );
			++j;
		}
	}

	n_ary_tree_interpolator_selector		interpolator_selector;

	for ( ; (i != i_e) && (j != j_e); ) {
		if ( comparer.compare(**i, **j) == equal ) {
			n_ary_tree_transition_to_one_detector detector;
			(*i)->accept					( detector );
			if ( !detector.result() || detector.has_transitions() )
				*operands++					= m_cloner.clone( **i, &from_interpolator );
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
					*operands++				= new_weight_transition( *i_interpolator, **i, 1.f );
				++i;
				break;
			}
			case more : {
				*operands++					= new_weight_transition( *j_interpolator, 1.f, **j++ );
				break;
			}
			case equal : {
				*operands++					= new_weight_transition( **i, **j );
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
			*operands++						= new_weight_transition( from.interpolator(), **i, 1.f );
	}

	for ( ; j != j_e; )
		*operands++							= new_weight_transition( to.interpolator(), 1.f, **j++ );

	R_ASSERT_CMP							( operands, ==, operands_end );

	if ( operands == operands_begin )
		return;

	std::sort								( operands_begin, operands_end, n_ary_tree_node_comparer() );
#ifndef MASTER_GOLD
	for ( i = operands_begin, j = i + 1; j != operands_end; )
		R_ASSERT_CMP						( n_ary_tree_node_comparer().compare( **i++, **j++ ), !=, more );
#endif // #ifndef MASTER_GOLD
}

u32 xray::animation::mixing::computed_operands_count	(
		n_ary_tree_animation_node& from,
		n_ary_tree_animation_node& to,
		bool const skip_time_scale_node
	)
{
	u32 result								= 0;

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
				++result;
			++i;
			++j;
		}
		else {
			if ( !skip_time_scale_node )
				++result;
			++i;
		}
	}
	else {
		if ( (*j)->is_time_scale() ) {
			if ( !skip_time_scale_node )
				++result;
			++j;
		}
	}

	n_ary_tree_interpolator_selector		interpolator_selector;
	for ( ; (i != i_e) && (j != j_e); ) {
		R_ASSERT							( (**i).is_weight() );
		R_ASSERT							( !(**i).is_time_scale() );
		R_ASSERT							( (**j).is_weight() );
		R_ASSERT							( !(**j).is_time_scale() );
		if ( comparer.compare(**i, **j) == equal ) {
			n_ary_tree_transition_to_one_detector detector;
			(*i)->accept					( detector );
			if ( !detector.result() || detector.has_transitions() )
				++result;
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
					++result;
				++i;
				break;
			}
			case more : {
				++result;
				++j;
				break;
			}
			case equal : {
				++result;
				++i;
				++j;
				break;
			}
			default : NODEFAULT( );
		}
	}

	for ( ; i != i_e; ) {
		n_ary_tree_transition_to_one_detector detector;
		(*i)->accept				( detector );
		if ( !detector.result() || detector.has_transitions() )
			++result;
		++i;
	}

	for ( ; j != j_e; ) {
		++j;
		++result;
	}

	return									result;
}

n_ary_tree_animation_node* n_ary_tree_transition_tree_constructor::new_driving_animation(
		n_ary_tree_animation_node& new_driving_animation,
		n_ary_tree_animation_node& new_driving_animation_in_previous_target,
		u32 const animation_interval_id,
		float const animation_interval_time,
		n_ary_tree_base_node* const time_scale_node
	)
{
	bool const skip_time_scale_node		= time_scale_node != 0;
	u32 const operands_count			= computed_operands_count( new_driving_animation_in_previous_target, new_driving_animation, skip_time_scale_node ) + 1;
	n_ary_tree_animation_node* const result	=
		new_animation(
			new_driving_animation,
			0,
			operands_count,
			false
		);

	n_ary_tree_base_node** operands		= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr() );
	m_buffer							+= operands_count*sizeof( n_ary_tree_base_node*);

	*operands++							= time_scale_node;

	add_operands						(
		new_driving_animation_in_previous_target,
		new_driving_animation,
		operands,
		operands + operands_count - 1,
		skip_time_scale_node
	);

	return
		add_animation_node(
			*result,
			&new_driving_animation_in_previous_target.animation_state(),
			animation_interval_id,
			animation_interval_time,
			false
		);
}

void n_ary_tree_transition_tree_constructor::change_animation				(
		n_ary_tree_animation_node& from,
		n_ary_tree_animation_node& to,
		n_ary_tree_animation_node* const driving_animation
	)
{
	bool const skip_time_scale_node		= from.driving_animation() && !to.driving_animation();
	u32 const operands_count			= computed_operands_count( from, to, skip_time_scale_node );
	n_ary_tree_animation_node* const result	=
		new_animation(
			from,
			driving_animation,
			operands_count,
			false
		);

	n_ary_tree_base_node** operands		= static_cast<n_ary_tree_base_node**>( m_buffer.c_ptr() );
	m_buffer							+= operands_count*sizeof( n_ary_tree_base_node*);

	add_operands						(
		from,
		to,
		operands,
		operands + operands_count,
		skip_time_scale_node
	);

	add_animation_node					( *result, &from.animation_state(), false );
}

void n_ary_tree_transition_tree_constructor::merge_synchronization_groups	(
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
				add_animation			( *j, &new_driving_animation, interpolator );
				j						= j->m_next_animation;
				continue;
			}
			case equal : {
				change_animation		( **i++, *j, &new_driving_animation );
				j						= j->m_next_animation;
				continue;
			}
			default : NODEFAULT();
		}
	}

	for ( ; i != e; )
		remove_animation				( **i++, &new_driving_animation, interpolator );

	for ( ; j != to_end; j = j->m_next_animation )
		add_animation					( *j, &new_driving_animation, interpolator );
}

void n_ary_tree_transition_tree_constructor::merge_asynchronous_groups	(
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
				add_animation			( *j, 0, j->interpolator() );
				j						= j->m_next_animation;
				continue;
			}
			case equal : {
				change_animation		( *i, *j, 0 );
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
		add_animation					( *j, 0, j->interpolator() );
}

void n_ary_tree_transition_tree_constructor::change_synchronization_group	(
		n_ary_tree_animation_node* const from_begin,
		n_ary_tree_animation_node* const from_end,
		n_ary_tree_animation_node* const to_begin,
		n_ary_tree_animation_node* const to_end
	)
{
	if ( is_asynchronous_animation(*from_begin) ) {
		merge_asynchronous_groups				( from_begin, from_end, to_begin, to_end );
		return;
	}

	n_ary_tree_animation_node& new_driving_animation	= *to_begin;
	n_ary_tree_animation_node& old_driving_animation	= *from_begin;

	R_ASSERT_CMP								( new_driving_animation.synchronization_group_id(), ==, old_driving_animation.synchronization_group_id() );

	u32 const new_driving_animation_interval_id	= old_driving_animation.animation_state().animation_interval_id;
	float const new_driving_animation_length	= new_driving_animation.animation_intervals()[ new_driving_animation_interval_id ].length();
	R_ASSERT_CMP								( new_driving_animation_length, >, 0.f );

	float const old_driving_animation_length	= old_driving_animation.animation_intervals()[ new_driving_animation_interval_id ].length();
	R_ASSERT_CMP								( old_driving_animation_length, >, 0.f );
	
	float const time_scale_factor				= new_driving_animation_length/old_driving_animation_length;

	// get new driving animation time offset
	float const old_driving_animation_time		= old_driving_animation.animation_state().animation_interval_time;
	R_ASSERT_CMP								( old_driving_animation_time, >=, 0.f );
	float const new_driving_animation_time_offset	=
		(new_driving_animation.override_existing_animation() && (compare_animation_intervals(new_driving_animation, old_driving_animation) == equal) ) ?
		new_driving_animation.animation_state().animation_interval_time : 
		old_driving_animation_time*time_scale_factor;

	// get new driving animation target time scale
	n_ary_tree_time_scale_calculator time_scale_calculator	( m_current_time_in_ms, 0.f, m_current_time_in_ms, n_ary_tree_time_scale_calculator::forbid_transitions_destroying );
	(**new_driving_animation.operands( sizeof(n_ary_tree_animation_node)) ).accept	( time_scale_calculator );
	float const new_driving_animation_target_time_scale	= time_scale_calculator.time_scale();

	// get new driving animation time scale interpolator
	R_ASSERT									( time_scale_calculator.interpolator( ) );
	base_interpolator const* const new_driving_animation_time_scale_interpolator = m_cloner.clone( *time_scale_calculator.interpolator() );

	// clone all current time scale transitions from old driving animation, multiplying them by time_scale_factor
	R_ASSERT									( !math::is_zero(time_scale_factor) );
	n_ary_tree_base_node* const old_time_scale_node	= *old_driving_animation.operands( sizeof(n_ary_tree_animation_node));
	n_ary_tree_base_node* const old_driving_animation_time_scale_cloned = m_cloner.clone( *old_time_scale_node, time_scale_factor );
	R_ASSERT									( old_driving_animation_time_scale_cloned );

	// get new driving animation current time scale
	float const new_driving_animation_current_time_scale	= n_ary_tree_target_time_scale_calculator( old_driving_animation ).result() * time_scale_factor;

	// create new time scale transition if needed
	n_ary_tree_base_node* time_scale_node		= old_driving_animation_time_scale_cloned;
	if ( new_driving_animation_target_time_scale != new_driving_animation_current_time_scale ) {
		n_ary_tree_time_scale_node* time_scale	=
			new ( m_buffer.c_ptr() ) n_ary_tree_time_scale_node(
				*new_driving_animation_time_scale_interpolator,
				new_driving_animation_target_time_scale,
				new_driving_animation_time_offset,
				m_current_time_in_ms
			);
		m_buffer								+= sizeof( n_ary_tree_time_scale_node );

		time_scale_node =
			new ( m_buffer.c_ptr() ) n_ary_tree_time_scale_transition_node(
				*old_driving_animation_time_scale_cloned,
				*time_scale,
				*new_driving_animation_time_scale_interpolator,
				m_current_time_in_ms,
				0,
				new_driving_animation_time_offset
			);
		m_buffer								+= sizeof( n_ary_tree_time_scale_transition_node );
	}

	n_ary_tree_animation_node* result			= 0;

	n_ary_tree_animation_node* const new_driving_animation_in_old_target = find_animation( from_begin, from_end, *to_begin );
	if ( new_driving_animation_in_old_target )
		result						=
			this->new_driving_animation (
				new_driving_animation,
				*new_driving_animation_in_old_target,
				new_driving_animation_interval_id,
				new_driving_animation_time_offset,
				time_scale_node
			);
	else
		result						=
			this->new_driving_animation (
				new_driving_animation,
				new_driving_animation_interval_id,
				new_driving_animation_time_offset,
				time_scale_node,
				*new_driving_animation_time_scale_interpolator
			);

	merge_synchronization_groups	(
		from_begin,
		from_end,
		to_begin->m_next_animation,
		to_end,
		*result,
		*new_driving_animation_time_scale_interpolator
	);
}

void n_ary_tree_transition_tree_constructor::merge_trees						(
		n_ary_tree const& from,
		n_ary_tree const& to
	)
{
	n_ary_tree_animation_node* i_begin		= from.root();
	n_ary_tree_animation_node* i_end		= synchronization_group_end( i_begin );
	n_ary_tree_animation_node* j_begin		= to.root();
	n_ary_tree_animation_node* j_end		= synchronization_group_end( j_begin );
	while ( i_begin && j_begin ) {
		if ( (*i_begin).synchronization_group_id() < (*j_begin).synchronization_group_id() ) {
			remove_synchronization_group	( i_begin, i_end );
			get_next_synchronization_group	( i_begin, i_end );
			continue;
		}

		if ( (*i_begin).synchronization_group_id() > (*j_begin).synchronization_group_id() ) {
			add_synchronization_group		( j_begin, j_end );
			get_next_synchronization_group	( j_begin, j_end );
			continue;
		}

		change_synchronization_group		( i_begin, i_end, j_begin, j_end );
		get_next_synchronization_group		( i_begin, i_end );
		get_next_synchronization_group		( j_begin, j_end );
	}

	while ( i_begin ) {
		remove_synchronization_group		( i_begin, i_end );
		get_next_synchronization_group		( i_begin, i_end );
	}

	while ( j_begin ) {
		add_synchronization_group			( j_begin, j_end );
		get_next_synchronization_group		( j_begin, j_end );
	}
}

n_ary_tree_transition_tree_constructor::n_ary_tree_transition_tree_constructor	(
		xray::mutable_buffer& buffer,
		n_ary_tree const& from,
		n_ary_tree const& to,
		u32 const animations_count,
		u32 const current_time_in_ms,
		xray::animation::subscribed_channel*& channels_head
	) :
	m_cloner					( buffer, current_time_in_ms ),
	m_buffer					( buffer ),
	m_result					( 0 ),
	m_animation_states			( 0 ),
	m_reference_counter			( 0 ),
	m_previous_animation		( 0 ),
#if !XRAY_DEBUG_ANIMATIONS_COUNT
	m_animations_count			( animations_count ),
#else // #if XRAY_DEBUG_ANIMATIONS_COUNT
	m_animations_count			( 0 ),
#endif // #if XRAY_DEBUG_ANIMATIONS_COUNT
	m_current_time_in_ms		( current_time_in_ms ),
	m_channels_head				( channels_head )
{
	R_ASSERT					( animations_count );

	m_reference_counter			= static_cast<n_ary_tree_intrusive_base*>( buffer.c_ptr() );
	buffer						+= sizeof( n_ary_tree_intrusive_base );
	new (m_reference_counter) n_ary_tree_intrusive_base( );

	m_cloner.initialize			( from, to );

	m_animation_states			= static_cast<animation_state*>( buffer.c_ptr() );
	buffer						+= animations_count*sizeof( animation_state );
	m_new_animation_state		= m_animation_states;

	m_animation_events			= static_cast<animation_state**>( buffer.c_ptr() );
	buffer						+= animations_count*sizeof( animation_state* );
	m_new_animation_event		= m_animation_events;

	merge_trees					( from, to );

#if XRAY_DEBUG_ANIMATIONS_COUNT
	R_ASSERT_CMP				( m_animations_count, ==, animations_count );
#endif // #if XRAY_DEBUG_ANIMATIONS_COUNT
}

n_ary_tree n_ary_tree_transition_tree_constructor::computed_tree( n_ary_tree const& previous )
{
	return
		n_ary_tree(
			m_result,
			m_cloner.interpolators(),
			m_animation_states,
			m_animation_events,
			m_reference_counter,
			m_animations_count,
			m_cloner.interpolators_count(),
			m_current_time_in_ms,
			previous
		);
}