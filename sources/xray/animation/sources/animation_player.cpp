////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/animation_player.h>
#include <xray/animation/mixing_expression.h>
#include <xray/animation/base_interpolator.h>
#include "mixing_n_ary_tree_converter.h"
#include "mixing_n_ary_tree_comparer.h"
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_node_comparer.h"
#include "mixing_n_ary_tree_transition_tree_constructor.h"
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID( animation_player_linkage_id );

using xray::animation::animation_player;
using xray::animation::mixing::expression;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_node_comparer;
using xray::animation::mixing::n_ary_tree;
using xray::animation::skeleton;
using xray::animation::callback_type;
using xray::animation::reserved_channel_ids_enum;

inline pvoid animation_player::get_next_buffer	( u32 const buffer_size )
{
	R_ASSERT_CMP_U			( buffer_size, <=, sizeof(m_tree_buffers[0]) );
	m_current_buffer		= (&m_tree_buffers[0] == m_current_buffer) ? &m_tree_buffers[1] : &m_tree_buffers[0];
	return					&(*m_current_buffer)[0];
}

bool animation_player::set_target	 ( expression const& expression, u32 const current_time_in_ms )
{
	mixing::n_ary_tree_converter	builder( expression );
	u32 const buffer_size		= builder.needed_buffer_size( );

	bool const first_time		= !m_mixing_tree.are_there_any_animations();
	pvoid const buffer_raw		=
		!first_time ?
		ALLOCA(buffer_size) :
		get_next_buffer( buffer_size );

	mutable_buffer buffer( buffer_raw, buffer_size );
	n_ary_tree const target_tree =
		builder.constructed_n_ary_tree(
			buffer,
			!m_mixing_tree.are_there_any_animations(),
			current_time_in_ms,
			first_time ? m_mixing_tree.get_object_transform() : float4x4(),
			m_first_subscribed_channel
		);
	R_ASSERT					( !buffer.size(), "buffer calculation failed: %d bytes left", buffer.size() );

	if ( first_time ) {
		m_mixing_tree			= target_tree;
		return					true;
	}
	
	mixing::n_ary_tree_comparer	comparer( m_mixing_tree, target_tree, current_time_in_ms );
	if ( comparer.equal() )
		return					false;

	u32 const mixing_buffer_size = comparer.needed_buffer_size( );
	ASSERT						( mixing_buffer_size );
	mutable_buffer mixing_buffer(
		get_next_buffer( mixing_buffer_size ),
		mixing_buffer_size
	);

	#ifndef MASTER_GOLD
	#ifdef DEBUG
	m_mixing_tree.dump_tree		( current_time_in_ms );
	#endif
	#endif // #ifndef MASTER_GOLD

	mixing::n_ary_tree temp		= 
		mixing::n_ary_tree_transition_tree_constructor(
			mixing_buffer,
			m_mixing_tree,
			target_tree,
			comparer.animations_count(),
			current_time_in_ms,
			m_first_subscribed_channel
		).computed_tree( m_mixing_tree );
	#ifndef MASTER_GOLD
	#ifdef DEBUG
	temp.dump_tree				( current_time_in_ms );
	#endif
	#endif // #ifndef MASTER_GOLD

	R_ASSERT					( !mixing_buffer.size(), "buffer calculation failed: %d bytes left", mixing_buffer.size() );
	m_mixing_tree				= temp;
	return						true;
}

bool animation_player::set_target_and_tick		( expression const& expression, u32 const current_time_in_ms )
{
#ifndef MASTER_GOLD
	if ( m_controller_callback )
		m_controller_callback	( expression, current_time_in_ms );
#endif //#ifndef MASTER_GOLD

	if ( m_mixing_tree.are_there_any_animations() )
		tick					( current_time_in_ms );

	bool const is_new_target	= set_target( expression, current_time_in_ms );
	XRAY_UNREFERENCED_PARAMETER	( is_new_target );

	tick						( current_time_in_ms );

	#ifndef MASTER_GOLD
	#ifdef DEBUG
	if ( is_new_target )
		m_mixing_tree.dump_tree	( current_time_in_ms );
	#endif
	#endif // #ifndef MASTER_GOLD

	return						is_new_target;
}

void animation_player::tick						( u32 const current_time_in_ms )
{
	R_ASSERT					( m_in_tick || m_callbacks_are_actual );

	++m_in_tick;
	m_mixing_tree.tick			( current_time_in_ms, m_first_subscribed_channel, m_callbacks_are_actual );
	--m_in_tick;

	if ( !m_in_tick && !m_callbacks_are_actual )
		compact_callbacks		( );
}

void animation_player::set_object_transform		( xray::math::float4x4 const& object_transform )
{
	m_mixing_tree.set_object_transform	( object_transform );
}

xray::math::float4x4 animation_player::get_object_transform	( ) const
{
	return						m_mixing_tree.get_object_transform( );
}

void animation_player::compute_bones_matrices	( skeleton const& skeleton, xray::math::float4x4* const begin, xray::math::float4x4* const end ) const
{
	R_ASSERT_CMP				( end - begin, >=, skeleton.get_non_root_bones_count( ) );
	m_mixing_tree.compute_bones_matrices	( skeleton, begin, end );
}

#ifndef MASTER_GOLD
void animation_player::set_controller_callback	( boost::function<void (xray::animation::mixing::expression const&, u32)> const& callback )
{
	m_controller_callback		= callback;
}

void animation_player::reset					( bool const clear_callbacks )
{
	m_mixing_tree				= n_ary_tree( float4x4().identity() );

	if ( clear_callbacks ) {
		destroy_subscriptions					( m_first_subscribed_channel );
		m_first_subscribed_channel				= 0;
		m_callbacks_buffer.~mutable_buffer		( );
		new (&m_callbacks_buffer) mutable_buffer( m_callbacks_buffer_raw, sizeof(m_callbacks_buffer_raw) );
	}
}

void animation_player::fill_animation_states	( xray::vectora<xray::animation::editor_animation_state>& result )
{
	m_mixing_tree.fill_animation_states	( result );
}
#endif // #ifndef MASTER_GOLD

void animation_player::subscribe				( pcstr const channel_id, callback_type const& callback, u8 const callback_id )
{
	R_ASSERT									( callback, "empty callback specified for the animation player" );

	subscribed_channel* i						= m_first_subscribed_channel;
	subscribed_channel* previous				= 0;
	for ( ; i; previous = i, i = i->next ) {
		if ( !strings::equal( (*i).channel_id, channel_id ) )
			continue;

		animation_callback* const new_callback	= static_cast<animation_callback*>( m_callbacks_buffer.c_ptr() );
		m_callbacks_buffer						+= sizeof( animation_callback );
		new ( new_callback ) animation_callback	( callback, callback_id );
		R_ASSERT								( (*i).first_callback );
		for ( animation_callback* j = (*i).first_callback; ; j = j->next ) {
			R_ASSERT							( (*j).callback_id != callback_id );
			if ( !j->next ) {
				j->next							= new_callback;
				break;
			}
		}

		return;
	}

	subscribed_channel* const new_channel		= static_cast<subscribed_channel*>( m_callbacks_buffer.c_ptr() );
	m_callbacks_buffer							+= sizeof( subscribed_channel );
	memory::detail::call_constructor			( new_channel );
	new_channel->next							= 0;

	u32 const buffer_size						= (strings::length( channel_id ) + 1) * sizeof(char);
	memory::copy								( m_callbacks_buffer.c_ptr(), buffer_size, channel_id, buffer_size );
	new_channel->channel_id						= static_cast<pstr>( m_callbacks_buffer.c_ptr() );
	m_callbacks_buffer							+= math::align_up(buffer_size, u32(4));
	
	animation_callback* const new_callback		= static_cast<animation_callback*>( m_callbacks_buffer.c_ptr() );
	m_callbacks_buffer							+= sizeof( animation_callback );
	new ( new_callback ) animation_callback		( callback, callback_id );
	new_channel->first_callback					= new_callback;

	if ( previous )
		previous->next							= new_channel;
	else
		m_first_subscribed_channel				= new_channel;

	++m_subscribed_channels_count;
}

void animation_player::subscribe				( reserved_channel_ids_enum const channel_id, callback_type const& callback, u8 const callback_id )
{
	char channel_id_string[]					= { u8(channel_id), 0 };
	subscribe									( channel_id_string, callback, callback_id );
}

void animation_player::destroy_subscriptions	( xray::animation::subscribed_channel const* const channels_head )
{
	for (subscribed_channel const* i = channels_head; i; ) {
		for ( animation_callback* j = (*i).first_callback; j; ) {
			animation_callback* const temp		= j;
			j									= j->next;
			temp->~animation_callback			( );
		}

		subscribed_channel const* const temp	= i;
		XRAY_UNREFERENCED_PARAMETER				( temp );
		i										= i->next;
		temp->~subscribed_channel				( );
	}
}

void animation_player::compact_callbacks		( )
{
	R_ASSERT									( !m_callbacks_are_actual );
	m_callbacks_are_actual						= true;
	// clone current enabled callbacks to stack
	subscribed_channel const* i					= m_first_subscribed_channel;
	subscribed_channel const* first_cloned_channel	= 0;
	subscribed_channel* previous_channel		= 0;
	for ( ; i; i = i->next ) {
		subscribed_channel* const new_channel	= static_cast<subscribed_channel*>( ALLOCA( sizeof(subscribed_channel) ) );
		if ( !first_cloned_channel )
			first_cloned_channel				= new_channel;
		else
			previous_channel->next				= new_channel;

		memory::detail::call_constructor		( new_channel );
		pstr temp;
		STR_DUPLICATEA							( temp, (*i).channel_id );
		new_channel->channel_id					= (*i).channel_id;
		new_channel->first_callback				= 0;
		new_channel->next						= 0;

		R_ASSERT								( (*i).first_callback );
		for ( animation_callback* j = (*i).first_callback, *k = 0; j; k = j, j = j->next ) {
			if ( !(*j).enabled )
				continue;

			animation_callback* const new_callback	= static_cast<animation_callback*>( ALLOCA( sizeof(animation_callback) ) );
			R_ASSERT							( (*j).callback );
			new ( new_callback ) animation_callback	( (*j).callback, (*j).callback_id );
			if ( k )
				k->next							= new_callback;
			else
				new_channel->first_callback		= new_callback;
		}
		
		if ( !new_channel->first_callback ) {
			if ( previous_channel )
				previous_channel->next			= 0;
			else {
				R_ASSERT						( first_cloned_channel );
				first_cloned_channel			= 0;
			}
		}
		else
			previous_channel					= new_channel;
	}

	destroy_subscriptions						( m_first_subscribed_channel );

	m_first_subscribed_channel					= 0;

	m_callbacks_buffer.~mutable_buffer			( );
	new (&m_callbacks_buffer) mutable_buffer	( m_callbacks_buffer_raw, sizeof(m_callbacks_buffer_raw) );
	// clone callbacks from stack
	u32 channels_count							= 0;
	for ( i = first_cloned_channel; i; i = i->next ) {
		R_ASSERT								( i->first_callback );
		++channels_count;
		subscribed_channel* const new_channel	= static_cast<subscribed_channel*>( m_callbacks_buffer.c_ptr() );
		m_callbacks_buffer						+= sizeof( subscribed_channel );
		if ( !m_first_subscribed_channel )
			m_first_subscribed_channel			= new_channel;
		else
			previous_channel->next				= new_channel;

		memory::detail::call_constructor		( new_channel );
		
		u32 const buffer_size					= (strings::length( (*i).channel_id ) + 1) * sizeof(char);
		memory::copy							( m_callbacks_buffer.c_ptr(), buffer_size, (*i).channel_id, buffer_size );
		new_channel->channel_id					= static_cast<pstr>( m_callbacks_buffer.c_ptr() );
		m_callbacks_buffer						+= math::align_up( buffer_size, u32(4) );
		new_channel->next						= 0;

		R_ASSERT								( (*i).first_callback );
		for ( animation_callback* j = (*i).first_callback, *k = 0; j; k = j, j = j->next ) {
			R_ASSERT							( (*j).enabled );
			animation_callback* const new_callback	= static_cast<animation_callback*>( m_callbacks_buffer.c_ptr() );
			m_callbacks_buffer					+= sizeof( animation_callback );
			R_ASSERT							( (*j).callback );
			new ( new_callback ) animation_callback	( (*j).callback, (*j).callback_id );
			if ( k )
				k->next							= new_callback;
			else
				new_channel->first_callback		= new_callback;
		}

		previous_channel						= new_channel;
	}

	destroy_subscriptions						( first_cloned_channel );

	if ( !channels_count )
		m_first_subscribed_channel				= 0;

#ifndef MASTER_GOLD
	for ( subscribed_channel const* i = m_first_subscribed_channel; i; i = i->next ) {
		R_ASSERT								( (*i).first_callback );
		for ( animation_callback* j = (*i).first_callback; j; j = j->next )
			R_ASSERT							( (*j).enabled );
	}
#endif // #ifndef MASTER_GOLD
}

void animation_player::unsubscribe				( pcstr const channel_id, u8 const callback_id )
{
	R_ASSERT									( m_callbacks_are_actual );

	subscribed_channel const* i					= m_first_subscribed_channel;
	for ( ; i; i = i->next ) {
		if ( !strings::equal( (*i).channel_id, channel_id ) )
			continue;

		R_ASSERT								( (*i).first_callback );
		for ( animation_callback* j = (*i).first_callback; ; j = j->next ) {
			if ( (*j).callback_id == callback_id ) {
				(*j).callback					= callback_type();
				(*j).enabled					= false;
				m_callbacks_are_actual			= false;
				break;
			}
		}

		R_ASSERT								( !m_callbacks_are_actual, "Cannot find callback[%d], being subscribed on channel %s", callback_id, channel_id );

		if ( !m_in_tick )
			compact_callbacks					( );

		return;
	}

	NODEFAULT									( );
}

void animation_player::unsubscribe				( reserved_channel_ids_enum const channel_id, u8 const callback_id )
{
	char channel_id_string[]					= { u8(channel_id), 0 };
	unsubscribe									( channel_id_string, callback_id );
}