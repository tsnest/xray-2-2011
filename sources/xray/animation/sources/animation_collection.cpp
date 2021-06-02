////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "animation_collection.h"
#include <xray/animation/mixing_expression.h>
#include <xray/animation/mixing_animation_lexeme.h>

namespace xray {
namespace animation {

using xray::animation::mixing::expression;
using xray::animation::mixing::animation_lexeme;

animation_collection::animation_collection	(
		collection_playing_types type,
		bool can_repeat_successively,
		bool is_cyclic_repeating,
		pvoid buffer,
		size_t max_count,
		u32 last_time
	) :	
	m_animations							( buffer, ( animations_type::size_type ) max_count ),
	m_random_number							( last_time ),
	m_current_animation_index				( (u32)-1 ),
	m_type									( type ),
	m_is_cyclic_repeating					( is_cyclic_repeating ),
	m_can_repeat_successively				( can_repeat_successively ),
	m_is_child_last_animation				( true )
{
	LOG_DEBUG								( "animation_collection address: 0x%8x", this );
	if( max_count == 1 )
		m_can_repeat_successively = true;
}

animation_collection::~animation_collection ( )
{
}

expression animation_collection::emit_impl	( mutable_buffer& buffer, animation_lexeme* const driving_animation, bool& is_last_animation ) const
{
	R_ASSERT								( !m_animations.empty( ) );

	switch ( m_type )
	{
		case collection_playing_type_random:
		{
			is_last_animation				= !m_is_cyclic_repeating;
			u32 const animations_count		= m_animations.size( );

			if( m_can_repeat_successively )
			{
				m_current_animation_index	= m_random_number.random( animations_count );
				break;
			}

			u32 previous_animation_index	= m_current_animation_index;
			do
			{
				m_current_animation_index	= m_random_number.random( animations_count );
			} while ( previous_animation_index == m_current_animation_index );
		
			break;
		}

		case collection_playing_type_sequential:
		{
			if( !m_is_child_last_animation )
				break;

			u32 const animations_count			= m_animations.size( );
			m_current_animation_index			= ( m_current_animation_index + 1 ) % animations_count;
			is_last_animation					= !m_is_cyclic_repeating && ( m_current_animation_index == animations_count - 1 );
			break;
		}

		default:
			NODEFAULT							( m_current_animation_index = u32(-1) );
	}	

	animation_expression_emitter_ptr const emitter	= m_animations[ m_current_animation_index ];

	if( driving_animation )
	{
		expression ret_expression	= emitter->emit( buffer, *driving_animation, m_is_child_last_animation );
		is_last_animation			= is_last_animation && m_is_child_last_animation; 
		return ret_expression;
	}
	else
	{
		expression ret_expression	= emitter->emit( buffer, m_is_child_last_animation );
		is_last_animation			= is_last_animation && m_is_child_last_animation;
		return ret_expression;
	}
}

expression animation_collection::emit			( mutable_buffer& buffer, bool& is_last_animation ) const
{
	return										emit_impl( buffer, 0, is_last_animation );
}

expression animation_collection::emit			( mutable_buffer& buffer, animation_lexeme& driving_animation, bool& is_last_animation ) const
{
	return										emit_impl( buffer, &driving_animation, is_last_animation );
}

animation_types_enum animation_collection::type ( ) const
{
	return										m_animations[0]->type();
}

void animation_collection::serialize			( memory::writer& w ) const
{
	w.write										( &m_type, sizeof( m_type ) );
	m_random_number.serialize					( w );
	w.write										( &m_is_cyclic_repeating, sizeof( bool ) );
	w.write_u32									( m_current_animation_index );
	w.write										( &m_can_repeat_successively, sizeof( bool ) );

	buffer_vector< animation_expression_emitter_ptr >::const_iterator begin	= m_animations.begin();
	buffer_vector< animation_expression_emitter_ptr >::const_iterator end	= m_animations.end();

	for ( ; begin != end; ++begin )
		(*begin)->serialize						( w );
}

void animation_collection::deserialize			( memory::reader& r )
{
	r.r											( &m_type, sizeof( collection_playing_types ), sizeof( collection_playing_types ) );
	m_random_number.deserialize					( r );
	r.r											( &m_is_cyclic_repeating, sizeof( bool ), sizeof( bool ) );
	m_current_animation_index					= r.r_u32( );
	r.r											( &m_can_repeat_successively, sizeof( bool ), sizeof( bool ) );

	buffer_vector< animation_expression_emitter_ptr >::const_iterator begin	= m_animations.begin();
	buffer_vector< animation_expression_emitter_ptr >::const_iterator end	= m_animations.end();

	for ( ; begin != end; ++begin )
		(*begin)->deserialize					( r );
}

void animation_collection::add_animation		( animation_expression_emitter_ptr emitter )
{
	m_animations.push_back						( emitter );
}

} // namespace animation
} // namespace xray
