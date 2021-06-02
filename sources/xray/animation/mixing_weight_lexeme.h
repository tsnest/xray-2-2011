////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WEIGHT_LEXEME_H_INCLUDED
#define WEIGHT_LEXEME_H_INCLUDED

#include <xray/animation/mixing_binary_tree_weight_node.h>
#include <xray/animation/mixing_base_lexeme.h>
#include <xray/animation/base_interpolator.h>

namespace xray {
namespace animation {
namespace mixing {

class XRAY_ANIMATION_API weight_lexeme :
	public binary_tree_weight_node,
	public base_lexeme
{
public:
	inline					weight_lexeme	(
								mutable_buffer& buffer,
								float const weight,
								base_interpolator const& interpolator
							);
	inline					weight_lexeme	( weight_lexeme const& other, bool );
	inline	weight_lexeme&	cloned_modified_lexeme ( float const new_weight ) const;
	inline	weight_lexeme&	operator =		( float const new_weight );
	inline	weight_lexeme* cloned_in_buffer	( );

private:
	inline					weight_lexeme	(
								mutable_buffer& buffer,
								float const weight,
								base_interpolator const* const interpolator,
								bool const cloned
							);
}; // class weight_lexeme

	inline	weight_lexeme&	operator +		( float const left, weight_lexeme const& right );
	inline	weight_lexeme&	operator +		( weight_lexeme const& left, float const right );

	inline	weight_lexeme&	operator -		( float const left, weight_lexeme const& right );
	inline	weight_lexeme&	operator -		( weight_lexeme const& left, float const right );

	inline	weight_lexeme&	operator *		( float const left, weight_lexeme const& right );
	inline	weight_lexeme&	operator *		( weight_lexeme const& left, float const right );

	inline	weight_lexeme&	operator /		( float const left, weight_lexeme const& right );
	inline	weight_lexeme&	operator /		( weight_lexeme const& left, float const right );

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_weight_lexeme_inline.h>

#endif // #ifndef WEIGHT_LEXEME_H_INCLUDED