////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_LEXEME_INLINE_H_INCLUDED
#define ANIMATION_LEXEME_INLINE_H_INCLUDED

#include "channel_event_callback.h"
#include "mixing_animation_clip.h"

namespace xray {
namespace animation {
namespace mixing {

inline animation_lexeme::animation_lexeme					(
		mutable_buffer& buffer,
		animation_clip* const animation,
		base_interpolator const* const time_scale_interpolator,
		float const time_scale,
		float const animation_time_offset,
		clip_mix_self mix_self
	) :
	binary_tree_animation_node	(
		buffer,
		animation,
		time_scale,
		time_scale_interpolator,
		animation_time_offset,
		mix_self
	),
	base_lexeme				( buffer ),
	m_cloned_instance		( 0 )
{
}

inline animation_lexeme::animation_lexeme	(
		mutable_buffer& buffer,
		animation_clip* const animation,
		animation_lexeme& animation_to_synchronize_with
	) :
	binary_tree_animation_node	(
		buffer,
		animation,
		animation_to_synchronize_with.cloned_in_buffer()
	),
	base_lexeme				( buffer ),
	m_cloned_instance		( 0 )
{
}

inline animation_lexeme::animation_lexeme					( animation_lexeme& other, bool ) :
	binary_tree_animation_node	( other ),
	base_lexeme				( other, true ),
	m_cloned_instance		( other.m_cloned_instance )
{
	if ( m_cloned_instance == this )
		m_cloned_instance	= 0;
}

inline animation_lexeme* animation_lexeme::cloned_in_buffer	( )
{
	if ( !m_cloned_instance )
		m_cloned_instance	= base_lexeme::cloned_in_buffer< animation_lexeme >( );
		
	return					m_cloned_instance.c_ptr( );
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef ANIMATION_LEXEME_INLINE_H_INCLUDED