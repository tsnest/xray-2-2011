////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MULTIPLICATION_LEXEME_H_INCLUDED
#define MULTIPLICATION_LEXEME_H_INCLUDED

#include <xray/animation/mixing_binary_tree_multiplication_node.h>
#include <xray/animation/mixing_binary_operation_lexeme.h>
#include <xray/animation/mixing_expression.h>

namespace xray {
namespace animation {
namespace mixing {

class XRAY_ANIMATION_API multiplication_lexeme :
	public binary_tree_multiplication_node,
	public binary_operation_lexeme
{
public:
	template < typename T1, typename T2 >
	inline					multiplication_lexeme	( T1& left, T2& right );
	inline					multiplication_lexeme	( multiplication_lexeme const& other, bool );
	inline	multiplication_lexeme* cloned_in_buffer	( );

protected:
	using binary_operation_lexeme::cloned_in_buffer;
}; // class multiplication_lexeme

	template < typename T1, typename T2 >
	inline	multiplication_lexeme&	operator *		( T1& left, T2& right );

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_multiplication_lexeme_inline.h>

#endif // #ifndef MULTIPLICATION_LEXEME_H_INCLUDED