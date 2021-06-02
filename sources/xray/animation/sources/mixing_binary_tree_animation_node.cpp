////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_binary_tree_visitor.h>
#include "interpolator_comparer.h"
#include <xray/animation/mixing_animation_lexeme.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>

using xray::animation::mixing::binary_tree_animation_node;
using xray::animation::mixing::binary_tree_visitor;
using xray::animation::base_interpolator;
using xray::animation::mixing::animation_interval;

binary_tree_animation_node::binary_tree_animation_node			( xray::animation::mixing::animation_lexeme_parameters const& parameters ) :
	m_buffer							( &parameters.buffer() ),
#ifndef MASTER_GOLD
	m_identifier						( clone( parameters.buffer(), parameters.identifier() ) ),
#endif // #ifndef MASTER_GOLD
	m_animation_intervals				( clone( parameters.buffer(), parameters.animation_intervals(), parameters.animation_intervals() + parameters.animation_intervals_count() ) ),
	m_animation_intervals_count			( parameters.animation_intervals_count() ),
	m_start_animation_interval_id		( parameters.start_animation_interval_id() ),
	m_start_animation_interval_time		( parameters.start_animation_interval_time() ),
	m_start_cycle_animation_interval_id	( parameters.start_cycle_animation_interval_id() ),
	m_next_animation					( 0 ),
	m_driving_animation					( parameters.driving_animation() ),
	m_n_ary_driving_animation			( 0 ),
	m_unique_weights_count				( 0 ),
	m_weight_interpolator				(
		parameters.weight_interpolator() ?
		parameters.weight_interpolator()->clone( parameters.buffer() ) :
		instant_interpolator().clone( parameters.buffer() )
	),
	m_time_scale						( parameters.time_scale() ),
	m_time_scale_interpolator			(
		parameters.driving_animation() ?
		0 :
		(
			parameters.time_scale_interpolator() ?
			parameters.time_scale_interpolator()->clone( parameters.buffer() ) :
			instant_interpolator().clone( parameters.buffer() )
		)
	),
	m_null_weight_found					( false ),
	m_playing_type						( parameters.playing_type() ),
	m_synchronization_group_id			( parameters.synchronization_group_id() ),
	m_override_existing_animation		( parameters.override_existing_animation() ),
	m_additivity_priority				( parameters.additivity_priority() )
{
	assign_uninitialized_user_data		( );
}

void binary_tree_animation_node::accept								( binary_tree_visitor& visitor )
{
	visitor.visit						( *this );
}

animation_interval const* binary_tree_animation_node::clone			(
		xray::mutable_buffer& buffer,
		xray::animation::mixing::animation_interval const* const animation_intervals_begin,
		xray::animation::mixing::animation_interval const* const animation_intervals_end
	)
{
	animation_interval* const result	= static_cast<animation_interval*>( buffer.c_ptr( ) );
	buffer								+= u32(animation_intervals_end - animation_intervals_begin) * sizeof( animation_interval );

	animation_interval* j				= result;
	for ( animation_interval const* i = animation_intervals_begin; i != animation_intervals_end; ++i, ++j )
		new ( j ) animation_interval	( (*i).animation(), (*i).start_time(), (*i).length() );

	return								result;
}

void binary_tree_animation_node::destroy_animation_intervals	( )
{
	for ( animation_interval const* i = m_animation_intervals, *e = i + m_animation_intervals_count; i != e; ++i )
		(*i).~animation_interval		( );
}

#ifndef MASTER_GOLD
pcstr binary_tree_animation_node::clone							(
		xray::mutable_buffer& buffer,
		pcstr identifier
	)
{
	u32 const buffer_size				= math::align_up(strings::length( identifier ) + 1, u32(4))*sizeof(char);
	memory::copy						( buffer.c_ptr(), buffer_size, identifier, buffer_size );
	pcstr const result					= static_cast<pcstr>( buffer.c_ptr() );
	buffer								+= buffer_size;
	return								result;
}
#endif // #ifndef MASTER_GOLD