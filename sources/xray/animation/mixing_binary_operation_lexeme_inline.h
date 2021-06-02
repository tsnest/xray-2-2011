////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_OPERATION_LEXEME_INLINE_H_INCLUDED
#define BINARY_OPERATION_LEXEME_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline binary_operation_lexeme::binary_operation_lexeme					( mutable_buffer& buffer ) :
	super	( buffer )
{
}

inline binary_operation_lexeme::binary_operation_lexeme					( base_lexeme const& other, bool ) :
	super	( other, true )
{
}

template < typename T >
inline T* binary_operation_lexeme::cloned_in_buffer						( T& value )
{
	return	value.cloned_in_buffer();
}

inline binary_tree_base_node* binary_operation_lexeme::cloned_in_buffer	( expression& value )
{
	return	&value.node();
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BINARY_OPERATION_LEXEME_INLINE_H_INCLUDED