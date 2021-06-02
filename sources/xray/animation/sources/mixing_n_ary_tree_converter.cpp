////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_converter.h"
#include <xray/animation/base_interpolator.h>
#include <xray/animation/mixing_expression.h>
#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_binary_tree_weight_node.h>
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_weaver.h"
#include "mixing_n_ary_tree_size_calculator.h"
#include "mixing_n_ary_tree_node_constructor.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_time_scale_node.h"
#include "interpolator_size_calculator.h"
#include "mixing_n_ary_tree_redundant_multiplicands_detector.h"
#include "mixing_binary_tree_null_weight_searcher.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_node_comparer.h"
#include "mixing_binary_tree_expression_simplifier.h"
#include "mixing_n_ary_tree_transition_tree_constructor.h"
#include <xray/animation/mixing_binary_tree_multiplication_node.h>
#include <xray/animation/mixing_binary_tree_addition_node.h>
#include "mixing_n_ary_tree_animation_node.h"
#include <xray/animation/i_editor_mixer.h>
#include "mixing_n_ary_tree_weight_calculator.h"
#include "i_editor_mixer_inline.h"
#include "interpolator_comparer.h"
#include <xray/animation/mixing_animation_interval.h>
#include "mixing_animation_state.h"

using xray::animation::mixing::n_ary_tree_converter;
using xray::animation::mixing::binary_tree_base_node;
using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::expression;
using xray::animation::mixing::n_ary_tree;
using xray::animation::base_interpolator;
using xray::animation::mixing::binary_tree_null_weight_searcher;

static inline void set_animation_user_data	( n_ary_tree_animation_node& n_ary_animation, binary_tree_animation_node const& binary_animation )
{
#ifndef MASTER_GOLD
	n_ary_animation.user_data	= binary_animation.user_data;
#else // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETERS( &n_ary_animation, &binary_animation );
#endif // #ifndef MASTER_GOLD
}

n_ary_tree_converter::n_ary_tree_converter	( expression const& expression ) :
	m_root						( binary_tree_expression_simplifier( expression.node(), expression.buffer() ).result() ),
	m_animations_root			( 0 ),
	m_binary_interpolators		( 0 ),
	m_interpolators				( 0 ),
	m_animation_states			( 0 ),
	m_animation_events			( 0 ),
	m_reference_counter			( 0 ),
	m_nodes_to_destroy_manually	( 0 ),
	m_animations_count			( 0 ),
	m_interpolators_count		( 0 ),
	m_buffer_size				( 0 )
{
	n_ary_tree_weaver			weaver( expression.buffer() );
	m_root->accept				( weaver );
	m_animations_root			= weaver.animations_root();
	ASSERT						( m_animations_root );

	process_interpolators		( weaver.interpolators_root(), weaver.interpolators_count(), expression.buffer() );
	simplify_weights			( );
	sort_animations				( expression.buffer() );
	compute_buffer_size			( );

	ASSERT						( m_buffer_size );
	m_buffer_size				+= sizeof(n_ary_tree_intrusive_base);
}

n_ary_tree_converter::~n_ary_tree_converter			( )
{
}

static void fill_weights	( binary_tree_animation_node& animation )
{
	for ( binary_tree_base_node* i = animation.m_next_weight; i; i = i->m_next_weight ) {
		i->m_same_weight						= 0;
		i->cast_weight()->m_simplified_weight	= i->cast_weight()->weight();
	}

	for ( binary_tree_base_node* j = animation.m_next_weight; j; j = j->m_next_weight ) {
		if ( j->m_same_weight )
			continue;

		binary_tree_weight_node* const j_weight = j->cast_weight();
		if ( !j_weight )
			continue;

		for ( binary_tree_base_node* k = j->m_next_weight; k; k = k->m_next_weight ) {
			binary_tree_weight_node const* const k_weight = k->cast_weight();
			if ( !k_weight )
				continue;

			if ( j_weight->interpolator() == k_weight->interpolator() ) {
				k->m_same_weight				= j;
				j_weight->m_simplified_weight	*= k_weight->weight();
			}
		}
	}
}

void n_ary_tree_converter::compute_buffer_size		( )
{
	ASSERT						( !m_animations_count );

	n_ary_tree_size_calculator	calculator;
	for ( binary_tree_animation_node_ptr i=m_animations_root; i; ++m_animations_count, i=i->m_next_animation ) {
		if ( (*i).m_null_weight_found ) {
			--m_animations_count;
			continue;
		}

		fill_weights			( *i );

		i->accept				( calculator );

		if ( !i->driving_animation() ) {
			m_buffer_size		+= sizeof(n_ary_tree_time_scale_node);
			m_buffer_size		+= sizeof(n_ary_tree_base_node*);
		}

		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight ) {
			if ( !j->m_same_weight )
				j->accept		( calculator );
		}
	}

	m_buffer_size				+= calculator.calculated_size( );
	m_buffer_size				+= m_animations_count*sizeof(animation_state);
	m_buffer_size				+= m_animations_count*sizeof(animation_state*);
}

struct animation_less_predicate {
	inline bool operator ( )	( xray::animation::mixing::binary_tree_animation_node const& left, xray::animation::mixing::binary_tree_animation_node const& right ) const
	{
		if ( left.synchronization_group_id() < right.synchronization_group_id() )
			return			true;

		if ( right.synchronization_group_id() < left.synchronization_group_id() )
			return			false;

		if ( !left.driving_animation() )
			return			true;

		if ( !right.driving_animation() )
			return			false;

		R_ASSERT			( left.driving_animation() == right.driving_animation(), "more than one driving animation found in synchronization group %d", left.synchronization_group_id() );

		if ( left.animation_intervals_count() < right.animation_intervals_count() )
			return			true;

		if ( right.animation_intervals_count() < left.animation_intervals_count() )
			return			false;

		xray::animation::mixing::animation_interval const* i		= left.animation_intervals_begin();
		xray::animation::mixing::animation_interval const* const e	= left.animation_intervals_end();
		xray::animation::mixing::animation_interval const* j		= right.animation_intervals_begin();
		for ( ; i != e; ++i, ++j ) {
			if ( (*i).animation() < (*j).animation() )
				return		true;

			if ( (*j).animation() < (*i).animation() )
				return		false;
		}

		if ( left.playing_type() != right.playing_type() )
			return			left.playing_type() < right.playing_type();

		if ( left.start_animation_interval_id() < right.start_animation_interval_id() )
			return			true;

		if ( left.start_animation_interval_id() > right.start_animation_interval_id() )
			return			false;

		return				left.start_animation_interval_time() < right.start_animation_interval_time();
	}

	inline bool operator ( )	( xray::animation::mixing::binary_tree_animation_node const* left, xray::animation::mixing::binary_tree_animation_node const* right ) const
	{
		return				(*this)( *left, *right );
	}
}; // struct animation_less_predicate

binary_tree_base_node* n_ary_tree_converter::create_binary_multipliers				(
		xray::mutable_buffer& buffer,
		xray::animation::mixing::binary_tree_base_node* const start_weight
	)
{
	binary_tree_base_node* left = start_weight;
	for (binary_tree_base_node* right = (*left).m_next_weight; right; right = right->m_next_weight ) {
		using xray::animation::mixing::binary_tree_multiplication_node;
		binary_tree_multiplication_node* const new_multiplier	= static_cast<binary_tree_multiplication_node*>( buffer.c_ptr() );
		buffer					+= sizeof( binary_tree_multiplication_node );
		new ( new_multiplier ) binary_tree_multiplication_node( left, right );
		left					= new_multiplier;
	}
	return						left;
}

void n_ary_tree_converter::sort_animations			( xray::mutable_buffer& buffer )
{
	u32 animations_count		= 0;
	for (binary_tree_animation_node_ptr i = m_animations_root; i; ++animations_count, i = i->m_next_animation ) ;
	R_ASSERT					( animations_count );

	binary_tree_animation_node** animations = (binary_tree_animation_node**)ALLOCA(animations_count*sizeof(binary_tree_animation_node*));
	binary_tree_animation_node** j = animations;
	for (binary_tree_animation_node_ptr i=m_animations_root; i; *j++ = i.c_ptr(), i = i->m_next_animation ) {
		if ( !(*i).driving_animation() )
			continue;

		binary_tree_animation_node* k = i.c_ptr( );
		while ( (*k).driving_animation() ) {
			R_ASSERT_CMP		( (*k).synchronization_group_id(), ==, (*k).driving_animation()->synchronization_group_id() );
			k					= (*k).driving_animation().c_ptr();
		}

		(*i).set_driving_animation	( k );
	}

	std::sort					( animations, animations + animations_count, animation_less_predicate() );

	animation_less_predicate predicate;

	binary_tree_animation_node** i	= animations;
	binary_tree_animation_node** e	= animations + animations_count;
	binary_tree_animation_node** new_e_minus_1	= animations + animations_count - 1;
	for ( j = i + 1; j != e; ) {
		if ( predicate(**i, **j) ) {
			++i;
			++j;
			continue;
		}

		LOG_WARNING				( "the same animation is used twice in the expression" );

		R_ASSERT				( !predicate(**j, **i), "less predicate for binary tree animation nodes is broken" );

		binary_tree_base_node_ptr current_hierarchy	= create_binary_multipliers( buffer, (*i)->m_next_weight );
		do {
			binary_tree_base_node_ptr const right	= create_binary_multipliers( buffer, (*j)->m_next_weight );

			binary_tree_addition_node* const new_weight	= static_cast<binary_tree_addition_node*>( buffer.c_ptr() );
			buffer						+= sizeof( binary_tree_addition_node );
			new ( new_weight ) binary_tree_addition_node( current_hierarchy.c_ptr(), right.c_ptr() );
			current_hierarchy			= new_weight;

			++j;
			--new_e_minus_1;
		} while ( (j != e) && !predicate(**i, **j) );

		(*i)->m_next_weight				= binary_tree_expression_simplifier( *current_hierarchy, buffer ).result( );
		++i;
	}

	for (i = animations; i != new_e_minus_1; ++i )
		(*i)->m_next_animation	= *(i+1);

	(*i)->m_next_animation		= 0;
	m_animations_root			= *animations;
}

void n_ary_tree_converter::simplify_weights		( )
{
	for ( binary_tree_animation_node_ptr i=m_animations_root; i; i=i->m_next_animation ) {
		fill_weights			( *i );

		binary_tree_null_weight_searcher searcher;

		binary_tree_base_node* last_multiplicand	= 0;
		u32 unique_count		= 0;
		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight ) {
			if ( !j->m_same_weight ) {
				(*j).accept			( searcher );
				last_multiplicand	= j;
				++unique_count;
			}
		}

		if ( unique_count == 1 ) {
			n_ary_tree_redundant_multiplicands_detector	detector( i->weight_interpolator() );
			last_multiplicand->accept	( detector );
			if ( detector.result() )
				unique_count		= 0;
		}
		i->m_unique_weights_count	= unique_count;
		i->m_null_weight_found		= searcher.result( );
	}
}

u32 n_ary_tree_converter::needed_buffer_size		( )
{
	return						m_buffer_size;
}

struct binary_tree_sort_interpolators_predicate {
	inline	bool	operator ( )	( base_interpolator const* const left, base_interpolator const* const right ) const
	{
		return					*left < *right;
	}
}; // struct sort_interpolators_predicate

struct binary_tree_unique_interpolators_predicate {
	inline	bool	operator ( )	( base_interpolator const* const left, base_interpolator const* const right ) const
	{
		return					*left == *right;
	}
}; // struct unique_interpolators_predicate

void n_ary_tree_converter::process_interpolators	( binary_tree_base_node* const interpolators_root, u32 const interpolators_count, xray::mutable_buffer& buffer )
{
	u32 animations_count		= 0;
	for ( binary_tree_animation_node_ptr i = m_animations_root; i; i = i->m_next_animation )
		++animations_count;

	m_interpolators_count		= interpolators_count + 2*animations_count;

	base_interpolator const** interpolators	= static_cast<base_interpolator const**>( ALLOCA( m_interpolators_count*sizeof(base_interpolator*) ) );
	binary_tree_base_node* root = interpolators_root;
	base_interpolator const** i	= interpolators;
	for ( ; root; root = root->m_next_unique_interpolator ) {
		binary_tree_weight_node const* weight = root->cast_weight( );
		if ( weight )
			*i++				= &weight->interpolator();
	}

	for ( binary_tree_animation_node_ptr j = m_animations_root; j; j = j->m_next_animation ) {
		*i++					= &(*j).weight_interpolator();
		if ( (*j).time_scale_interpolator() )
			*i++				=  (*j).time_scale_interpolator();
	}

	std::sort					( interpolators, i, binary_tree_sort_interpolators_predicate() );
	base_interpolator const** const e	= std::unique( interpolators, i, binary_tree_unique_interpolators_predicate() );
	i							= interpolators;

	R_ASSERT					( i != e );
	m_interpolators_count		= u32(e - i);

	m_binary_interpolators		= static_cast< base_interpolator const** >( buffer.c_ptr() );
	u32 const buffer_size		= m_interpolators_count * sizeof( base_interpolator* );
	buffer						+= buffer_size;

	memory::copy				( m_binary_interpolators, buffer_size, i, buffer_size );

	interpolator_size_calculator size_calculator;
	for ( ; i != e; ++i )
		(*i)->accept			( size_calculator );

	m_buffer_size				+= size_calculator.calculated_size( ) + m_interpolators_count*sizeof(base_interpolator*);
}

struct node_predicate {
	inline	bool	operator ( )	( n_ary_tree_base_node* const left, n_ary_tree_base_node* const right ) const
	{
		return		xray::animation::mixing::n_ary_tree_node_comparer().compare( *left, *right ) == xray::animation::less;
	}
}; // struct node_predicate

class interpolator_predicate {
public:
	inline	interpolator_predicate	( base_interpolator const& interpolator ) :
		m_interpolator	( &interpolator )
	{
	}

	inline	bool	operator ( )	( base_interpolator const* const interpolator ) const
	{
		xray::animation::interpolator_comparer comparer;
		m_interpolator->accept	( comparer, *interpolator );
		return					comparer.result == xray::animation::equal;
	}

private:
	base_interpolator const* m_interpolator;
}; // struct interpolator_predicate

n_ary_tree n_ary_tree_converter::constructed_n_ary_tree	(
		xray::mutable_buffer& buffer,
		bool const is_final_tree,
		u32 const current_time_in_ms,
		xray::math::float4x4 const& object_transform,
		xray::animation::subscribed_channel*& channels_head
	)
{
	ASSERT						( m_animations_root );
	ASSERT						( m_buffer_size );
	ASSERT						( buffer.size() >= m_buffer_size );

	m_reference_counter			= static_cast<n_ary_tree_intrusive_base*>( buffer.c_ptr() );
	buffer						+= sizeof( n_ary_tree_intrusive_base );
	new (m_reference_counter) n_ary_tree_intrusive_base( );

	ASSERT						( !m_interpolators );
	m_interpolators				= static_cast<base_interpolator const**>( buffer.c_ptr() );
	buffer						+= m_interpolators_count*sizeof( base_interpolator* );

	base_interpolator const** interpolator = m_interpolators;
	for ( base_interpolator const** i = m_binary_interpolators, ** const e = i + m_interpolators_count; i != e; ++i, ++interpolator )
		*interpolator			= (*i)->clone( buffer );

	m_animation_states			= static_cast<animation_state*>( buffer.c_ptr() );
	buffer						+= m_animations_count*sizeof( animation_state );

	m_animation_events			= static_cast<animation_state**>( buffer.c_ptr() );
	buffer						+= m_animations_count*sizeof( animation_state* );

	n_ary_tree_animation_node* root		= 0;
	n_ary_tree_animation_node* previous	= 0;

	animation_state* j			= m_animation_states;
	animation_state** event		= m_animation_events;

	n_ary_tree_animation_node* driving_animation_tester = 0;

#ifndef MASTER_GOLD
	float full_animations_weight	= 0.f;
	u32 full_animations_count		= 0;
#endif // #ifndef MASTER_GOLD

	n_ary_tree_node_constructor	node_constructor(buffer, m_interpolators, m_interpolators + m_interpolators_count);
	for ( binary_tree_animation_node_ptr i=m_animations_root; i; i = i->m_next_animation ) {
		if ( (*i).m_null_weight_found )
			continue;

#ifndef MASTER_GOLD
		u32 const buffer_size	= math::align_up(strings::length( (*i).identifier() ) + 1, u32(4))*sizeof(char);
		memory::copy			( buffer.c_ptr(), buffer_size, (*i).identifier(), buffer_size );
		pcstr const identifier	= static_cast<pcstr>( buffer.c_ptr() );
		buffer					+= buffer_size;
#endif // #ifndef MASTER_GOLD
		
		R_ASSERT				( !i->driving_animation() || i->driving_animation()->n_ary_driving_animation() );
		n_ary_tree_animation_node* const driving_animation	=
			i->driving_animation() ? 
			i->driving_animation()->n_ary_driving_animation() :
			0;

		if ( driving_animation_tester && (driving_animation_tester->synchronization_group_id() != -1) ) {
			if ( !driving_animation )
				R_ASSERT		(
					driving_animation_tester->synchronization_group_id() != i->synchronization_group_id(),
					"several driving animations for synchronization group with id %d has been found",
					i->synchronization_group_id()
				);
			else if ( driving_animation_tester != driving_animation )
				R_ASSERT_CMP	(
					driving_animation_tester->synchronization_group_id(), !=, driving_animation->synchronization_group_id(),
					"several driving animations for synchronization group with id %d has been found",
					i->synchronization_group_id()
				);
		}

		base_interpolator const** const found	= std::find_if( m_interpolators, m_interpolators + m_interpolators_count, interpolator_predicate( (*i).weight_interpolator() ) );
		R_ASSERT				( found != m_interpolators + m_interpolators_count );

		animation_interval const* current_interval	= i->animation_intervals_begin( );
		animation_interval const* intervals_end		= i->animation_intervals_end( );
		animation_interval const* cloned_intervals_begin	= static_cast<animation_interval const*>( buffer.c_ptr() );
		for ( ; current_interval != intervals_end; ++current_interval, buffer += sizeof( animation_interval) )
			new ( buffer.c_ptr() ) animation_interval(
				current_interval->animation(),
				current_interval->start_time(),
				current_interval->length()
			);

		n_ary_tree_animation_node* const animation =
			driving_animation ? 
			new ( buffer.c_ptr() ) n_ary_tree_animation_node (
#ifndef MASTER_GOLD
				identifier,
#endif // #ifndef MASTER_GOLD
				*driving_animation,
				cloned_intervals_begin,
				cloned_intervals_begin + i->animation_intervals_count(),
				**found,
				i->playing_type(),
				i->override_existing_animation(),
				i->additivity_priority(),
				i->m_unique_weights_count,
				false
			) :
			new ( buffer.c_ptr() ) n_ary_tree_animation_node (
#ifndef MASTER_GOLD
				identifier,
#endif // #ifndef MASTER_GOLD
				cloned_intervals_begin,
				cloned_intervals_begin + i->animation_intervals_count(),
				i->start_cycle_animation_interval_id(),
				**found,
				i->playing_type(),
				i->synchronization_group_id(),
				i->override_existing_animation(),
				i->additivity_priority(),
				i->m_unique_weights_count + 1, // one for time scale node,
				false
			);

#ifndef MASTER_GOLD
		if ( !animation->additivity_priority() )
			++full_animations_count;
#endif // #ifndef MASTER_GOLD

		driving_animation_tester = driving_animation ? driving_animation : animation;
				
		set_animation_user_data(*animation, *i);
		if ( is_final_tree )
			i_editor_mixer::call_user_callback( editor_animations_event::new_animation, current_time_in_ms, animation, 0 );
		
		i->set_n_ary_driving_animation	( animation );
		buffer					+= sizeof( n_ary_tree_animation_node );

		if ( !root ) {
			root				= animation;
			animation->m_next_animation	= 0;
		}
		else
			previous->m_next_animation	= animation;

		previous				= animation;

		n_ary_tree_base_node** multiplicands = static_cast<n_ary_tree_base_node**>( buffer.c_ptr() );
		buffer					+= i->m_unique_weights_count * sizeof(n_ary_tree_base_node*);

		if ( !i->driving_animation() ) {
			buffer				+= sizeof(n_ary_tree_base_node*);

			base_interpolator const** const found	= std::find_if( m_interpolators, m_interpolators + m_interpolators_count, interpolator_predicate( *(*i).time_scale_interpolator() ) );
			R_ASSERT				( found != m_interpolators + m_interpolators_count );
			n_ary_tree_time_scale_node* const time_scale_node	= 
				new ( buffer.c_ptr() ) n_ary_tree_time_scale_node(
					**found, i->time_scale(), i->start_animation_interval_time(), current_time_in_ms
				);
			buffer				+= sizeof(n_ary_tree_time_scale_node);
			*multiplicands		= time_scale_node;
			++multiplicands;
		}

		if ( !i->m_unique_weights_count ) {
#ifndef MASTER_GOLD
			if ( !animation->additivity_priority() ) {
				R_ASSERT_CMP			( full_animations_weight, <=, 0.f );
				full_animations_weight	+= 1.f;
			}
#endif // #ifndef MASTER_GOLD
		}
		else {
			n_ary_tree_base_node** const multiplicands_begin = multiplicands;

			fill_weights			( *i );

#ifndef MASTER_GOLD
			float animation_weight	= 1.f;
#endif // #ifndef MASTER_GOLD
			for ( binary_tree_base_node* k=i->m_next_weight; k; k=k->m_next_weight ) {
				if ( k->m_same_weight )
					continue;

				k->accept			( node_constructor );
				n_ary_tree_base_node* const weight = node_constructor.result( );
				*multiplicands		= weight;
				++multiplicands;

#ifndef MASTER_GOLD
				if ( !animation->additivity_priority() ) {
					n_ary_tree_weight_calculator	weight_calculator( 0 );
					(*weight).accept				( weight_calculator );
					float const weight_value		= weight_calculator.weight( );
					animation_weight				*= weight_value;
				}
#endif // #ifndef MASTER_GOLD
			}

#ifndef	MASTER_GOLD
			R_ASSERT_CMP			( animation_weight, >=, 0.f );
			R_ASSERT_CMP			( animation_weight, <=, 1.f );
			if ( !animation->additivity_priority() )
				full_animations_weight	+= animation_weight;
#endif // #ifndef MASTER_GOLD

			std::sort				( multiplicands_begin, multiplicands, node_predicate() );
		}

		*event++				= j;
		n_ary_tree_weight_calculator weight_calculator(current_time_in_ms);
		weight_calculator.visit( *animation );
		animation_state const* const driving_animation_state = driving_animation ? &driving_animation->animation_state() : 0;
		new ( j ) animation_state	(
			*animation,
			current_time_in_ms,
			time_event_animation_lexeme_started,
			driving_animation ? driving_animation_state->animation_interval_id : i->start_animation_interval_id(),
			driving_animation ?
					animation->animation_intervals()[ driving_animation_state->animation_interval_id ].length() /
					driving_animation->animation_intervals()[ driving_animation_state->animation_interval_id ].length() *
					driving_animation_state->animation_interval_time :
				i->start_animation_interval_time(),
			weight_calculator.weight(),
			channels_head
		);
		animation->set_animation_state	( *j++ );
	}

#ifndef MASTER_GOLD
	if ( !math::is_zero( full_animations_weight - 1.f, math::epsilon_3) )
		LOG_ERROR				( "animation mixing weight error: sum of weights is not equal to 1 (%f)", full_animations_weight );
#endif // #ifndef MASTER_GOLD

	return						n_ary_tree( root, m_interpolators, m_animation_states, m_animation_events, m_reference_counter, m_animations_count, m_interpolators_count, current_time_in_ms, object_transform );
}