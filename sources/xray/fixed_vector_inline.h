////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FIXED_VECTOR_INLINE_H_INCLUDED
#define XRAY_FIXED_VECTOR_INLINE_H_INCLUDED

#include <xray/debug/pointer_cast.h>

#define TEMPLATE_SIGNATURE	template < typename T, int max_count >
#define FIXED_VECTOR		xray::fixed_vector< T, max_count >

TEMPLATE_SIGNATURE
inline FIXED_VECTOR::fixed_vector			() :
	super			( pointer_cast<char*>(m_buffer), max_count )
{
}

TEMPLATE_SIGNATURE
inline FIXED_VECTOR::fixed_vector			(size_type const count, value_type const& value) :
	super			( pointer_cast<char*>(m_buffer), max_count, count, value )
{
}

TEMPLATE_SIGNATURE
inline FIXED_VECTOR::fixed_vector			(self_type const& other) :
	super			( pointer_cast<char*>(m_buffer), max_count, other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline FIXED_VECTOR::fixed_vector			(input_iterator const& first, input_iterator const& last) :
	super			( pointer_cast<char*>(m_buffer), max_count, first, last )
{
}

TEMPLATE_SIGNATURE
inline FIXED_VECTOR& FIXED_VECTOR::operator=(self_type const& other)
{
	return			static_cast< FIXED_VECTOR& >( super::operator=( other ) );
}

TEMPLATE_SIGNATURE
inline typename FIXED_VECTOR::size_type FIXED_VECTOR::capacity		( ) const
{
	return			max_count;
}

TEMPLATE_SIGNATURE
inline typename FIXED_VECTOR::size_type FIXED_VECTOR::max_size		( ) const
{
	return			max_count;
}

TEMPLATE_SIGNATURE
inline void	swap							(FIXED_VECTOR& left, FIXED_VECTOR& right)
{
	std::swap		( (super&)left, (super&)right );
}

#undef FIXED_VECTOR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_FIXED_VECTOR_INLINE_H_INCLUDED