////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mixing_n_ary_tree_builder.h"
#include "base_interpolator.h"
#include "mixing_expression.h"
#include "mixing_binary_tree_animation_node.h"
#include "mixing_binary_tree_weight_node.h"
#include "mixing_n_ary_tree_addition_node.h"
#include "mixing_n_ary_tree_multiplication_node.h"
#include "mixing_n_ary_tree_weaver.h"
#include "mixing_n_ary_tree_size_calculator.h"
#include "mixing_n_ary_tree_node_constructor.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "interpolator_size_calculator.h"

using xray::animation::mixing::n_ary_tree_builder;
using xray::animation::mixing::binary_tree_base_node;
using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_weight_node;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::expression;
using xray::animation::mixing::n_ary_tree;
using xray::animation::base_interpolator;
using xray::animation::mixing::animation_clip;

struct animations_predicate {
	inline bool	operator ( )	( animation_clip const* const left, animation_clip const* const right ) const
	{
		return					*left < *right;
	}
}; // struct animations_predicate

n_ary_tree_builder::n_ary_tree_builder	( expression const& expression ) :
	m_root						( expression.node() ),
	m_animations_root			( 0 ),
	m_interpolators_root		( 0 ),
	m_interpolators				( 0 ),
	m_reference_counter			( 0 ),
	m_animation_states			( 0 ),
	m_buffer_size				( 0 ),
	m_animations_count			( 0 )
{
	n_ary_tree_weaver			weaver;
	m_root.accept				( weaver );
	m_animations_root			= weaver.animations_root();
	ASSERT						( m_animations_root );

#ifndef MASTER_GOLD
	u32 const animations_count	= weaver.animations_count( );
	animation_clip const** animations = static_cast<animation_clip const**>( ALLOCA( animations_count*sizeof(animation_clip*) ) );
	animation_clip const** j = animations;
	for (binary_tree_animation_node_ptr i = m_animations_root; i; i = i->m_next_animation, ++j )
		*j						= (*i).animation();

	std::sort					( animations, animations + animations_count, animations_predicate() );
	bool found					= false;
	for (animation_clip const** i = animations + 1, **j = animations, **end = animations + animations_count; i != end; ++i, ++j ) {
		if ( !(**j < **i) ) {
			LOG_ERROR			( "%s == %s", (*j)->identifier(), (*i)->identifier() );
			found				= true;
		}
	}
	R_ASSERT					( !found, "duplicated animations found in mixing expression! see log for details" );
#endif // #ifndef MASTER_GOLD

	m_interpolators_root		= weaver.interpolators_root();
	m_interpolators_count		= weaver.interpolators_count( );

	process_interpolators		( );
	sort_animations				( );
	simplify_weights			( );
	compute_buffer_size			( );

	ASSERT						( m_buffer_size );
	m_buffer_size				+= sizeof(n_ary_tree_intrusive_base);
}

void n_ary_tree_builder::compute_buffer_size		( )
{
	ASSERT						( !m_animations_count );

	n_ary_tree_size_calculator	calculator;
	for ( binary_tree_animation_node_ptr i=m_animations_root; i; ++m_animations_count, i=i->m_next_animation ) {
		i->accept				( calculator );
		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight ) {
			if ( !j->m_same_weight )
				j->accept		( calculator );
		}
	}

	m_buffer_size				+= calculator.calculated_size();
	m_buffer_size				+= m_animations_count*sizeof(animation_state);
}

struct animation_less_predicate {
	inline	bool	operator ( )	( binary_tree_animation_node const* const left, binary_tree_animation_node const* const right ) const
	{
		return					*left->animation() < *right->animation();
	}
};

void n_ary_tree_builder::sort_animations			( )
{
	u32 count					= 0;
	for (binary_tree_animation_node_ptr i = m_animations_root; i; ++count, i = i->m_next_animation ) ;
	ASSERT						( count );

	binary_tree_animation_node** animations = (binary_tree_animation_node**)ALLOCA(count*sizeof(binary_tree_animation_node*));
	binary_tree_animation_node** j = animations;
	for (binary_tree_animation_node_ptr i=m_animations_root; i; *j++ = &*i, i = i->m_next_animation ) ;
	std::sort					( animations, animations + count, animation_less_predicate() );

	binary_tree_animation_node** i		= animations;
	binary_tree_animation_node** const e	= animations + count - 1;
	for ( ; i != e; ++i )
		(*i)->m_next_animation	= *(i+1);

	(*i)->m_next_animation		= 0;
	m_animations_root			= *animations;
}

void n_ary_tree_builder::simplify_weights		( )
{
	for ( binary_tree_animation_node_ptr i=m_animations_root; i; i=i->m_next_animation ) {
		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight )
			j->m_same_weight	= 0;

		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight ) {
			if ( j->m_same_weight )
				continue;

			binary_tree_weight_node* const j_weight = j->cast_weight();
			if ( !j_weight )
				continue;

			for ( binary_tree_base_node* k=j->m_next_weight; k; k=k->m_next_weight ) {
				binary_tree_weight_node const* const k_weight = k->cast_weight();
				if ( !k_weight )
					continue;

				if ( j_weight->interpolator_impl() == k_weight->interpolator_impl() ) {
					k->m_same_weight	= j;
					j_weight->m_simplified_weight	*= k_weight->m_simplified_weight;
				}
			}
		}

		u32 unique_count		= 0;
		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight ) {
			if ( !j->m_same_weight )
				++unique_count;
		}

		i->m_unique_weights_count = unique_count;
	}
}

u32 n_ary_tree_builder::needed_buffer_size		( )
{
	return						m_buffer_size;
}

struct binary_tree_sort_interpolators_predicate {
	inline	bool	operator ( )	( binary_tree_base_node const* const left, binary_tree_base_node const* const right ) const
	{
		return					*(left->interpolator()) < *(right->interpolator());
	}
}; // struct sort_interpolators_predicate

struct binary_tree_unique_interpolators_predicate {
	inline	bool	operator ( )	( binary_tree_base_node const* const left, binary_tree_base_node const* const right ) const
	{
		return					*(left->interpolator()) == *(right->interpolator());
	}
}; // struct unique_interpolators_predicate

void n_ary_tree_builder::process_interpolators	( )
{
	if ( !m_interpolators_count )
		return;

	binary_tree_base_node** interpolators	= static_cast<binary_tree_base_node**>( ALLOCA( m_interpolators_count*sizeof(binary_tree_base_node*) ) );
	binary_tree_base_node* root = m_interpolators_root;
	binary_tree_base_node** i	= interpolators;
	for ( ; root; root = root->m_next_unique_interpolator, ++i )
		*i						= root;

	std::sort					( interpolators, i, binary_tree_sort_interpolators_predicate() );
	binary_tree_base_node** const e	= std::unique( interpolators, i, binary_tree_unique_interpolators_predicate() );
	i							= interpolators;

	interpolator_size_calculator	size_calculator;
	R_ASSERT					( i != e );
	m_interpolators_root		= *i++;
	for ( root = m_interpolators_root; i != e; root = *i, ++i ) {
		root->interpolator()->accept		( size_calculator );
		root->m_next_unique_interpolator	= *i;
	}

	root->interpolator()->accept		( size_calculator );
	root->m_next_unique_interpolator	= 0;

	m_interpolators_count		= u32(i - interpolators);
	m_buffer_size				+= size_calculator.calculated_size( ) + m_interpolators_count*sizeof(base_interpolator*);
}

bool n_ary_tree_builder::need_transitions		( ) const
{
	for ( binary_tree_animation_node_ptr i = m_animations_root; i; i = i->m_next_animation ) {
		if ( i->animation()->interpolator().transition_time() > 0.f )
			return				true;
	}

	if ( !m_interpolators_root )
		return					false;

	if ( m_interpolators_root->m_next_unique_interpolator )
		return					true;

	base_interpolator const* const interpolator	= m_interpolators_root->interpolator();
	ASSERT						( interpolator );
	return						interpolator->transition_time() > 0.f;
}

n_ary_tree n_ary_tree_builder::constructed_n_ary_tree	( xray::mutable_buffer& buffer, xray::memory::base_allocator* allocator, u32 const current_time_in_ms )
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

	base_interpolator const** i = m_interpolators;
	for ( binary_tree_base_node* root = m_interpolators_root; root; root = root->m_next_unique_interpolator, ++i )
		*i						= root->interpolator()->clone( buffer );

	m_animation_states						= static_cast<animation_state*>( buffer.c_ptr() );
	buffer						+= m_animations_count*sizeof(animation_state);

	n_ary_tree_animation_node* root		= 0;
	n_ary_tree_animation_node* previous	= 0;

	n_ary_tree_node_constructor	node_constructor(buffer, m_interpolators, m_interpolators + m_interpolators_count);
	for ( binary_tree_animation_node_ptr i=m_animations_root; i; i = i->m_next_animation ) {
		n_ary_tree_animation_node* const animation =
			new ( buffer.c_ptr() ) n_ary_tree_animation_node ( *i->animation(), i->time_scale(), i->m_unique_weights_count, current_time_in_ms, false );
		buffer					+= sizeof( n_ary_tree_animation_node );

		if ( !root ) {
			root				= animation;
			animation->m_next_animation	= 0;
		}
		else
			previous->m_next_animation	= animation;

		previous				= animation;

		if ( !i->m_unique_weights_count )
			continue;

		n_ary_tree_base_node** multiplicands = static_cast<n_ary_tree_base_node**>( buffer.c_ptr() );
		buffer					+= (i->m_unique_weights_count) * sizeof(n_ary_tree_base_node*);
		for ( binary_tree_base_node* j=i->m_next_weight; j; j=j->m_next_weight ) {
			if ( j->m_same_weight )
				continue;

			j->accept			( node_constructor );
			n_ary_tree_base_node* const weight = node_constructor.result( );
			*multiplicands		= weight;
			++multiplicands;
		}
	}

	return						n_ary_tree( root, allocator, m_interpolators, m_animation_states, m_reference_counter, m_animations_count, m_interpolators_count, 0 );
}