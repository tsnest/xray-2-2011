////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BASE_LEXEME_INLINE_H_INCLUDED
#define BASE_LEXEME_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline operands_calculator::operands_calculator	( u32 const operands_count, u32 const and_count, u32 const or_count ) :
	operands_count								( operands_count ),
	and_count									( and_count ),
	or_count									( or_count )
{
}

template < typename T >
inline T const& operator !						( T const& expression )
{
	expression.invert_value						( );
	return										expression;
}

inline operands_calculator operator +			( operands_calculator const& left, operands_calculator const& right )
{
	return operands_calculator					(
				left.operands_count + right.operands_count,
				left.and_count + right.and_count,
				left.or_count + right.or_count
			);
}

inline base_lexeme::base_lexeme					(
		operation_type_enum const operation_type,
		base_lexeme const& left,
		base_lexeme const& right,
		bool const destroy_manually
	) :
		m_left									( &left ),
		m_right									( &right ),
		m_operation_type						( static_cast< u8 >( operation_type ) ),
		m_value									( true ),
		m_destroy_manually						( destroy_manually ),
		m_counter								( 0 )
{
	reset_pointers								( );
}

inline base_lexeme::base_lexeme					( bool const destroy_manually ) :
		m_left									( 0 ),
		m_right									( 0 ),
		m_operation_type						( static_cast< u8 >( operation_type_predicate ) ),
		m_value									( true ),
		m_destroy_manually						( destroy_manually ),
		m_counter								( 0 )
{
	reset_pointers								( );
}

inline u32 base_lexeme::memory_size_for_brackets_expansion	( ) const
{
	operands_calculator const& result			= count_operands_for_brackets_expansion( );
	return										sizeof( base_lexeme ) *
												( result.or_count  + result.and_count );
}

inline base_lexeme operator &&	( base_lexeme const& left, base_lexeme const& right )
{
	return						base_lexeme( base_lexeme::operation_type_and, left, right, false );
}

inline base_lexeme operator ||	( base_lexeme const& left, base_lexeme const& right )
{
	return						base_lexeme( base_lexeme::operation_type_or, left, right, false );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef BASE_LEXEME_INLINE_H_INCLUDED