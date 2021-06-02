////////////////////////////////////////////////////////////////////////////
//	Created 	: 14.10.2005
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ASSOCIATIVE_VECTOR_COMPARE_PREDICATE_H_INCLUDED
#define XRAY_ASSOCIATIVE_VECTOR_COMPARE_PREDICATE_H_INCLUDED

#include <utility> // for std::pair

template <
	typename key_type,
	typename data_type,
	typename compare_predicate_type
>
class associative_vector_compare_predicate : public compare_predicate_type {
private:
	typedef compare_predicate_type									super;

public:
    typedef std::pair< key_type, data_type >						value_type;

public:
	inline					associative_vector_compare_predicate	( );
    explicit inline			associative_vector_compare_predicate	( compare_predicate_type const& compare_predicate );
	inline	bool			operator ( )							( key_type const& lhs, key_type const& rhs ) const;
    inline	bool			operator ( )							( value_type const& lhs, value_type const& rhs ) const;
    inline	bool			operator ( )							( value_type const& lhs, key_type const& rhs ) const;
    inline	bool			operator ( )							( key_type const& lhs, value_type const& rhs ) const;
};

#include <xray/associative_vector_compare_predicate_inline.h>

#endif // #ifndef XRAY_ASSOCIATIVE_VECTOR_COMPARE_PREDICATE_H_INCLUDED