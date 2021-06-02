////////////////////////////////////////////////////////////////////////////
//	Created		: 11.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_OBJECT_COMMANDS_INLINE_H_INCLUDED
#define SOUND_OBJECT_COMMANDS_INLINE_H_INCLUDED

namespace xray {
namespace sound {



template < typename SuperClass, typename T >
functor_command_with_data< SuperClass, T >::functor_command_with_data( functor_type const& receiver_function, T const& data ) :
	m_functor			( receiver_function ),
	m_data				( data )
{
}

template < typename SuperClass, typename T >
void functor_command_with_data< SuperClass, T >::execute	( )
{
	return m_functor	( m_data );
}


} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_OBJECT_COMMANDS_INLINE_H_INCLUDED