////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2005
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ASSOCIATIVE_VECTOR_COMPARE_PREDICATE_INLINE_H_INCLUDED
#define XRAY_ASSOCIATIVE_VECTOR_COMPARE_PREDICATE_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	\
	template <\
		typename key_type,\
		typename value_type,\
		typename compare_predicate_type\
	>

#define ASSOCIATIVE_VECTOR_COMPARE_PREDICATE	\
	associative_vector_compare_predicate<\
		key_type,\
		value_type,\
		compare_predicate_type\
	>

TEMPLATE_SIGNATURE
inline ASSOCIATIVE_VECTOR_COMPARE_PREDICATE::associative_vector_compare_predicate	( )
{
}

TEMPLATE_SIGNATURE
inline ASSOCIATIVE_VECTOR_COMPARE_PREDICATE::associative_vector_compare_predicate	( compare_predicate_type const& compare_predicate ) :
	super	( compare_predicate )
{
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR_COMPARE_PREDICATE::operator()						( key_type const& lhs, key_type const& rhs ) const
{
	return		( super::operator( ) ( lhs, rhs ) );
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR_COMPARE_PREDICATE::operator()						( value_type const& lhs, value_type const& rhs ) const
{
	return		( operator( ) ( lhs.first, rhs.first ) );
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR_COMPARE_PREDICATE::operator()						( value_type const& lhs, key_type const& rhs ) const
{
	return		( operator( ) ( lhs.first, rhs ) );
}

TEMPLATE_SIGNATURE
inline bool ASSOCIATIVE_VECTOR_COMPARE_PREDICATE::operator()						( key_type const& lhs, value_type const& rhs ) const
{
	return		( operator( ) ( lhs, rhs.first ) );
}

#undef ASSOCIATIVE_VECTOR_COMPARE_PREDICATE
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_ASSOCIATIVE_VECTOR_COMPARE_PREDICATE_INLINE_H_INCLUDED