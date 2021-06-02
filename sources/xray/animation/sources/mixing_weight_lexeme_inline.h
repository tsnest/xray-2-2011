////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WEIGHT_LEXEME_INLINE_H_INCLUDED
#define WEIGHT_LEXEME_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline weight_lexeme::weight_lexeme							(
		mutable_buffer& buffer,
		float const weight,
		base_interpolator const* const interpolator
	) :
	binary_tree_weight_node	( weight, interpolator->clone(buffer) ),
	base_lexeme	( buffer )
{
}

inline weight_lexeme::weight_lexeme							(
		mutable_buffer& buffer,
		float const weight,
		base_interpolator const* const interpolator,
		bool const cloned
	) :
	binary_tree_weight_node	( weight, interpolator->clone(buffer) ),
	base_lexeme	( buffer, cloned )
{
}

inline weight_lexeme::weight_lexeme							( weight_lexeme const& other, bool ) :
	binary_tree_weight_node	( other ),
	base_lexeme	( other, true )
{
}

inline weight_lexeme& weight_lexeme::cloned_modified_lexeme	( float const new_weight ) const
{
	weight_lexeme* result = static_cast<weight_lexeme*>( buffer().c_ptr( ) );
	buffer()			+= sizeof( weight_lexeme );
	new (result) weight_lexeme	( buffer(), new_weight, &interpolator_impl(), true );
	return				*result;
}

inline weight_lexeme& weight_lexeme::operator =				( float const new_weight )
{
	set_weight			( new_weight );
	return				*this;
}

inline weight_lexeme* weight_lexeme::cloned_in_buffer		( )
{
	return				base_lexeme::cloned_in_buffer< weight_lexeme >( );
}

inline weight_lexeme& operator +							( float const left, weight_lexeme const& right )
{
	return				right.cloned_modified_lexeme( left - right.weight() );
}

inline weight_lexeme& operator +							( weight_lexeme const& left, float const right )
{
	return				right + left;
}

inline weight_lexeme& operator -							( float const left, weight_lexeme const& right )
{
	return				right.cloned_modified_lexeme( left - right.weight() );
}

inline weight_lexeme& operator -							( weight_lexeme const& left, float const right )
{
	return				left.cloned_modified_lexeme( left.weight() - right );
}

inline weight_lexeme& operator *							( float const left, weight_lexeme const& right )
{
	return				right.cloned_modified_lexeme( left*right.weight() );
}

inline weight_lexeme& operator *							( weight_lexeme const& left, float const right )
{
	return				right * left;
}

inline weight_lexeme& operator /							( float const left, weight_lexeme const& right )
{
	return				(1.f/left) * right;
}

inline weight_lexeme& operator /							( weight_lexeme const& left, float const right )
{
	return				(1.f/right) * left;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef WEIGHT_LEXEME_INLINE_H_INCLUDED