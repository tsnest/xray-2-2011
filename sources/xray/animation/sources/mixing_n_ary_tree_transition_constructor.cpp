////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_transition_constructor.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_weight_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_subtraction_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_transition_node.h"
#include "mixing_animation_clip.h"
#include "mixing_n_ary_tree.h"
#include "base_interpolator.h"
#include "mixing_n_ary_tree_node_comparer.h"
#include "instant_interpolator.h"
#include "bone_mixer.h"

using xray::animation::mixing::n_ary_tree_transition_constructor;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_addition_node;
using xray::animation::mixing::n_ary_tree_subtraction_node;
using xray::animation::mixing::n_ary_tree_multiplication_node;
using xray::animation::mixing::n_ary_tree_transition_node;
using xray::animation::mixing::n_ary_tree_n_ary_operation_node;
using xray::animation::mixing::n_ary_tree;
using xray::animation::base_interpolator;
using xray::animation::mixing::interpolation_direction;
using xray::animation::bone_mixer;

n_ary_tree_transition_constructor::n_ary_tree_transition_constructor	(
		xray::mutable_buffer& buffer,
		bone_mixer& bone_mixer,
		n_ary_tree const& from,
		n_ary_tree const& to,
		base_interpolator const* additive_interpolator,
		base_interpolator const* non_additive_interpolator,
		u32 const animations_count,
		u32 const current_time_in_ms
	) :
	m_buffer					( buffer ),
	m_bone_mixer				( bone_mixer ),
	m_from						( *from.root() ),
	m_to						( *to.root() ),
	m_cloned_tree				( 0 ),
	m_result					( 0 ),
	m_interpolators				( 0 ),
	m_animation_states			( 0 ),
	m_additive_interpolator		( 0 ),
	m_non_additive_interpolator	( 0 ),
	m_reference_counter			( 0 ),
	m_animations_count			( animations_count ),
	m_interpolators_count		( 0 ),
	m_transitions_count			( 0 ),
	m_current_time_in_ms		( current_time_in_ms )
{
	R_ASSERT					( m_animations_count );

	m_reference_counter			= static_cast<n_ary_tree_intrusive_base*>( buffer.c_ptr() );
	buffer						+= sizeof( n_ary_tree_intrusive_base );
	new (m_reference_counter) n_ary_tree_intrusive_base( );

	process_interpolators		( buffer, from, to, additive_interpolator, non_additive_interpolator );

	m_animation_states			= static_cast<animation_state*>( buffer.c_ptr() );
	buffer						+= m_animations_count*sizeof(animation_state);

	animation_state* k			= m_animation_states;
	for (n_ary_tree_animation_node *previous_animation = 0, *i = &m_from, *j = &m_to; i || j; ++k ) {
		add_animation			(
			i ? &i : 0,
			j ? &j: 0,
			previous_animation
		);

		R_ASSERT_CMP			( m_current_time_in_ms, >=, previous_animation->start_time_in_ms() );
		new (k) animation_state(
			previous_animation->animation(),
			previous_animation->bone_mixer_data(),
			float(m_current_time_in_ms - previous_animation->start_time_in_ms())/1000.f,
			-1.f
		);
	}
}

void n_ary_tree_transition_constructor::process_interpolators			(
		xray::mutable_buffer& buffer,
		n_ary_tree const& from,
		n_ary_tree const& to,
		base_interpolator const* additive_interpolator,
		base_interpolator const* non_additive_interpolator
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

	m_interpolators				= static_cast<base_interpolator const**>( buffer.c_ptr() );
	buffer						+= m_interpolators_count*sizeof( base_interpolator* );

	for ( base_interpolator const** i = interpolators_begin, **j = m_interpolators; i != interpolators_end; ++i, ++j ) {
		*j						= (*i)->clone( buffer );

		if ( additive_interpolator && (**i == *additive_interpolator) )
			additive_interpolator		= *j;

		if ( non_additive_interpolator && (**i == *non_additive_interpolator) )
			non_additive_interpolator	= *j;
	}

	m_additive_interpolator		= additive_interpolator;
	m_non_additive_interpolator	= non_additive_interpolator;
}

static inline bool less	(
		n_ary_tree_animation_node const* const* const left_animation,
		n_ary_tree_animation_node const* const* const right_animation
	)
{
	return						left_animation && (!right_animation || (**left_animation < **right_animation));
}

void n_ary_tree_transition_constructor::add_animation	(
		n_ary_tree_animation_node** const left_animation,
		n_ary_tree_animation_node** const right_animation,
		n_ary_tree_animation_node*& previous_animation
	)
{
	ASSERT						( left_animation || right_animation );

	interpolation_direction const interpolation_direction = 
		less(left_animation, right_animation) ? interpolation_decrease : less(right_animation, left_animation) ? interpolation_increase : interpolation_no_interpolation;

	n_ary_tree_animation_node& animation = interpolation_direction == interpolation_increase ? **right_animation : **left_animation;

	base_interpolator const* const current_interpolator = animation.animation().type() == animation_clip::additive ? m_additive_interpolator : m_non_additive_interpolator;
	R_ASSERT					( current_interpolator );

	n_ary_tree_animation_node* const new_animation = 
		interpolation_direction != interpolation_no_interpolation ?
			add_animation		( animation, *current_interpolator, interpolation_direction ) :
			add_animation		( **left_animation, **right_animation, *current_interpolator );

	if ( previous_animation )
		previous_animation->m_next_animation	= new_animation;

	previous_animation			= new_animation;

	if ( interpolation_direction != interpolation_decrease )
		*right_animation		= (*right_animation)->m_next_animation;

	if ( interpolation_direction != interpolation_increase )
		*left_animation			= (*left_animation)->m_next_animation;
}

n_ary_tree_animation_node* n_ary_tree_transition_constructor::add_animation	(
		n_ary_tree_animation_node& animation,
		base_interpolator const& interpolator,
		interpolation_direction const interpolation_direction
	)
{
	u32 const operands_count	= animation.operands_count() + 1;
	n_ary_tree_animation_node* const new_animation	=
		new (m_buffer.c_ptr()) n_ary_tree_animation_node( animation.animation(), animation.bone_mixer_data(), animation.time_scale(), operands_count, animation.start_time_in_ms(), interpolation_direction == interpolation_decrease );
	m_buffer					+= sizeof(n_ary_tree_animation_node);

	n_ary_tree_base_node** multiplicands	= (n_ary_tree_base_node**)m_buffer.c_ptr();
	m_buffer					+= operands_count*sizeof(n_ary_tree_base_node*);

	if ( !m_result )
		m_result				= new_animation;

	n_ary_tree_base_node* const* i		= animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* const e	= i + animation.operands_count( );
	for ( ; i != e; ++i ) {
		(*i)->accept			( *this );
		*multiplicands++		= m_cloned_tree;
	}

	*multiplicands				= (n_ary_tree_base_node*)m_buffer.c_ptr();
	m_buffer					+= sizeof(n_ary_tree_transition_node);

	bool const is_new_animation	= interpolation_direction == interpolation_increase;
	n_ary_tree_weight_node* const weight_from		=
		new (m_buffer.c_ptr()) n_ary_tree_weight_node( interpolator, is_new_animation ? 0.f : 1.f );
	m_buffer					+= sizeof(n_ary_tree_weight_node);

	n_ary_tree_weight_node* const weight_to			=
		new (m_buffer.c_ptr()) n_ary_tree_weight_node( interpolator, is_new_animation ? 1.f : 0.f );
	m_buffer					+= sizeof(n_ary_tree_weight_node);

	new (*multiplicands) n_ary_tree_transition_node( *weight_from, *weight_to, interpolator, m_current_time_in_ms );
	++m_transitions_count;

	if ( is_new_animation )
		m_bone_mixer.on_new_animation	( new_animation->animation(), new_animation->bone_mixer_data(), m_current_time_in_ms );
	
	return						new_animation;
}

void n_ary_tree_transition_constructor::compute_operands_count	(
		n_ary_tree_animation_node& from_animation,
		n_ary_tree_animation_node& to_animation,
		u32& operands_count,
		u32& left_unique_count,
		u32& right_unique_count
	) const
{
	operands_count				= 0;
	left_unique_count			= 0;
	right_unique_count			= 0;

	n_ary_tree_node_comparer	comparer;

	n_ary_tree_base_node* const* const i_b	= from_animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* i			= i_b;
	n_ary_tree_base_node* const* const i_e	= i_b + from_animation.operands_count( );
	n_ary_tree_base_node* const* const j_b	= to_animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* j			= j_b;
	n_ary_tree_base_node* const* const j_e	= j_b + to_animation.operands_count( );
	for ( ; (i != i_e) && (j != j_e); ) {
		switch ( comparer.compare(**i, **j) ) {
			case n_ary_tree_node_comparer::less : {
				++left_unique_count;
				++i;
				break;
			}
			case n_ary_tree_node_comparer::more : {
				++right_unique_count;
				++j;
				break;
			}
			case n_ary_tree_node_comparer::equal : {
				++operands_count;
				++i;
				++j;
				break;
			}
		}
	}

	ASSERT						( (i == i_e) || (j == j_e) );
	left_unique_count			+= u32(i_e - i);
	right_unique_count			+= u32(j_e - j);
}

void n_ary_tree_transition_constructor::fill_operands	(
		n_ary_tree_animation_node& from_animation,
		n_ary_tree_animation_node& to_animation,
		n_ary_tree_base_node**& operands,
		n_ary_tree_base_node** left_unique_nodes,
		n_ary_tree_base_node** right_unique_nodes
	)
{
	n_ary_tree_node_comparer	comparer;

	n_ary_tree_base_node* const* const i_b	= from_animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* i			= i_b;
	n_ary_tree_base_node* const* const i_e	= i_b + from_animation.operands_count( );
	n_ary_tree_base_node* const* const j_b	= to_animation.operands( sizeof(n_ary_tree_animation_node) );
	n_ary_tree_base_node* const* j			= j_b;
	n_ary_tree_base_node* const* const j_e	= j_b + to_animation.operands_count( );

	for ( ; (i != i_e) && (j != j_e); ) {
		switch ( comparer.compare(**i, **j) ) {
			case n_ary_tree_node_comparer::less : {
				*left_unique_nodes++	= *i++;
				break;
			}
			case n_ary_tree_node_comparer::more : {
				*right_unique_nodes++	= *j++;
				break;
			}
			case n_ary_tree_node_comparer::equal : {
				(*i)->accept	( *this );
				*operands++		= m_cloned_tree;
				++i;
				++j;
				break;
			}
		}
	}

	while ( i != i_e )
		*left_unique_nodes++	= *i++;

	while ( j != j_e )
		*right_unique_nodes++	= *j++;
}

void n_ary_tree_transition_constructor::create_transition_node	(
		n_ary_tree_base_node**& operands,
		u32 const left_unique_count,
		u32 const right_unique_count,
		n_ary_tree_base_node** left_unique_nodes_begin,
		n_ary_tree_base_node** right_unique_nodes_begin,
		base_interpolator const& interpolator
	)
{
	n_ary_tree_base_node** const left_unique_nodes_end = left_unique_nodes_begin + left_unique_count;
	n_ary_tree_base_node** const right_unique_nodes_end = right_unique_nodes_begin + right_unique_count;

	*operands					= (n_ary_tree_base_node*)m_buffer.c_ptr();
	m_buffer					+= sizeof(n_ary_tree_transition_node);

	n_ary_tree_base_node* new_left = 0;
	if ( left_unique_count > 1 ) {
		new_left				= new (m_buffer.c_ptr()) n_ary_tree_multiplication_node( left_unique_count );
		m_buffer				+= sizeof(n_ary_tree_multiplication_node);

		n_ary_tree_base_node** operands	= (n_ary_tree_base_node**)m_buffer.c_ptr();
		m_buffer				+= left_unique_count*sizeof(n_ary_tree_base_node*);

		for (n_ary_tree_base_node** i = left_unique_nodes_begin; i != left_unique_nodes_end; ++i ) {
			(*i)->accept		( *this );
			*operands++	= m_cloned_tree;
		}
	}
	else {
		if ( left_unique_count == 1 ) {
			(*left_unique_nodes_begin)->accept( *this );
			new_left			= m_cloned_tree;
		}
		else {
			new_left			= new (m_buffer.c_ptr()) n_ary_tree_weight_node( interpolator, 0.f );
			m_buffer			+= sizeof(n_ary_tree_weight_node);
		}
	}

	n_ary_tree_base_node* new_right = 0;
	if ( right_unique_count > 1 ) {
		new_right				= new (m_buffer.c_ptr()) n_ary_tree_multiplication_node( right_unique_count );
		m_buffer				+= sizeof(n_ary_tree_multiplication_node);

		n_ary_tree_base_node** operands	= (n_ary_tree_base_node**)m_buffer.c_ptr();
		m_buffer				+= right_unique_count*sizeof(n_ary_tree_base_node*);

		for (n_ary_tree_base_node** i = right_unique_nodes_begin; i != right_unique_nodes_end; ++i ) {
			(*i)->accept		( *this );
			*operands++	= m_cloned_tree;
		}
	}
	else {
		if ( right_unique_count == 1 ) {
			(*right_unique_nodes_begin)->accept( *this );
			new_right			= m_cloned_tree;
		}
		else {
			new_right			= new (m_buffer.c_ptr()) n_ary_tree_weight_node( interpolator, 1.f );
			m_buffer			+= sizeof(n_ary_tree_weight_node);
		}
	}

	new (*operands) n_ary_tree_transition_node( *new_left, *new_right, interpolator, m_current_time_in_ms );
	++m_transitions_count;
}

n_ary_tree_animation_node* n_ary_tree_transition_constructor::add_animation	(
		n_ary_tree_animation_node& from_animation,
		n_ary_tree_animation_node& to_animation,
		base_interpolator const& interpolator
	)
{
	u32 operands_count, left_unique_count, right_unique_count;
	compute_operands_count		( from_animation, to_animation, operands_count, left_unique_count, right_unique_count );

	n_ary_tree_animation_node* new_animation	= 0;
	n_ary_tree_base_node** operands				= 0;

	if ( left_unique_count + right_unique_count ) {
		++operands_count;

		new_animation			= new (m_buffer.c_ptr()) n_ary_tree_animation_node( from_animation.animation(), from_animation.bone_mixer_data(), from_animation.time_scale(), operands_count, from_animation.start_time_in_ms(), from_animation.is_transiting_to_zero() );
		m_buffer				+= sizeof(n_ary_tree_animation_node);

		operands				= (n_ary_tree_base_node**)m_buffer.c_ptr();
		m_buffer				+= operands_count*sizeof(n_ary_tree_base_node*);
	}
	else {
		from_animation.accept	( *this );
		new_animation			= static_cast_checked<n_ary_tree_animation_node*>( m_cloned_tree );
	}

	if ( !m_result )
		m_result				= new_animation;

	if ( !(left_unique_count + right_unique_count) )
		return					new_animation;

	n_ary_tree_base_node** const left_unique_nodes	= (n_ary_tree_base_node**)ALLOCA(left_unique_count*sizeof(n_ary_tree_base_node*));
	n_ary_tree_base_node** const right_unique_nodes	= (n_ary_tree_base_node**)ALLOCA(right_unique_count*sizeof(n_ary_tree_base_node*));
	fill_operands				( from_animation, to_animation, operands, left_unique_nodes, right_unique_nodes );

	create_transition_node		(
		operands,
		left_unique_count,
		right_unique_count,
		left_unique_nodes,
		right_unique_nodes,
		interpolator
	);
	
	return						new_animation;
}

n_ary_tree n_ary_tree_transition_constructor::computed_tree( xray::memory::base_allocator* const allocator )
{
	return
		n_ary_tree(
			m_result,
			allocator,
			m_interpolators,
			m_animation_states,
			m_reference_counter,
			m_animations_count,
			m_interpolators_count,
			m_transitions_count
		);
}

template < typename T >
inline T* n_ary_tree_transition_constructor::new_constructed			( T& node )
{
	T* const result				= new ( m_buffer.c_ptr() ) T ( node );
	m_buffer					+= sizeof( T );
	return						result;
}

void n_ary_tree_transition_constructor::visit( n_ary_tree_animation_node& node )
{
	propagate					( node );
}

base_interpolator const& n_ary_tree_transition_constructor::cloned_interpolator	( base_interpolator const& interpolator ) const
{
	R_ASSERT					( m_non_additive_interpolator );
	if ( *m_non_additive_interpolator == interpolator )
		return					*m_non_additive_interpolator;

	if ( m_additive_interpolator && (*m_additive_interpolator == interpolator) )
		return					*m_additive_interpolator;

	base_interpolator const* found		= 0;
	base_interpolator const** i			= m_interpolators;
	base_interpolator const** const e	= m_interpolators + m_interpolators_count;
	for ( ; i != e; ++i ) {
		if ( interpolator == **i ) {
			found				= *i;
			break;
		}
	}

	R_ASSERT					( found, "unique interpolator which is not in the interpolators list found" );
	return						*found;
}

void n_ary_tree_transition_constructor::visit( n_ary_tree_transition_node& node )
{
	n_ary_tree_transition_node* const result	= (n_ary_tree_transition_node*)m_buffer.c_ptr();
	m_buffer					+= sizeof(n_ary_tree_transition_node);

	node.from().accept			( *this );
	n_ary_tree_base_node* const from = m_cloned_tree;

	node.to().accept			( *this );
	n_ary_tree_base_node* const to = m_cloned_tree;

	new ( result ) n_ary_tree_transition_node (
		*from,
		*to,
		cloned_interpolator( node.interpolator() ),
		m_current_time_in_ms
	);
	++m_transitions_count;

	m_cloned_tree				= result;
}

void n_ary_tree_transition_constructor::visit( n_ary_tree_weight_node& node )
{
	m_cloned_tree				=
		new ( m_buffer.c_ptr() ) n_ary_tree_weight_node(
			cloned_interpolator( node.interpolator() ),
			node.weight()
		);
	m_buffer					+= sizeof( n_ary_tree_weight_node );
}

void n_ary_tree_transition_constructor::visit( n_ary_tree_addition_node& node )
{
	propagate					( node );
}

void n_ary_tree_transition_constructor::visit( n_ary_tree_subtraction_node& node )
{
	propagate					( node );
}

void n_ary_tree_transition_constructor::visit( n_ary_tree_multiplication_node& node )
{
	propagate					( node );
}

template < typename T >
void n_ary_tree_transition_constructor::propagate			( T& node )
{
	T* const result				= new_constructed( node );

	u32 const operands_count	= node.operands_count( );
	m_buffer					+= operands_count*sizeof( n_ary_tree_base_node*);

	n_ary_tree_base_node* const* i			= node.operands( sizeof(T) );
	n_ary_tree_base_node* const* const e	= i + operands_count;
	n_ary_tree_base_node** j	= result->operands( sizeof(T) );
	for ( ; i != e; ++i, ++j ) {
		(*i)->accept			( *this );
		*j						= m_cloned_tree;
	}

	m_cloned_tree				= result;
}