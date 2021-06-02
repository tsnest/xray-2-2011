////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_INLINE_H_INCLUDED
#define XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline animation_lexeme::animation_lexeme					( animation_lexeme& other, bool ) :
	binary_tree_animation_node	( other ),
	base_lexeme					( other, true ),
	m_cloned_instance			( 0 )
{
	if ( other.m_cloned_instance != this )
		m_cloned_instance		= other.m_cloned_instance;
}

inline animation_lexeme::~animation_lexeme					( )
{
	if ( is_cloned() )
		destroy_animation_intervals	( );
}

inline animation_lexeme* animation_lexeme::cloned_in_buffer	( )
{
	if ( is_cloned() )
		return					this;

	if ( !m_cloned_instance )
		m_cloned_instance		= base_lexeme::cloned_in_buffer< animation_lexeme >( );

	return						m_cloned_instance.c_ptr( );
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_INLINE_H_INCLUDED