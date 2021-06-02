////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_POINTER_CAST_H_INCLUDED
#define XRAY_DEBUG_POINTER_CAST_H_INCLUDED

#include <boost/type_traits/remove_pointer.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/type_traits/is_const.hpp>

namespace xray {
namespace detail {
namespace pointer_cast {

template < typename T0, typename T1, bool is_const >
struct helper {
	static T1 convert		( T0* const value )
	{
		pvoid const temp	= static_cast< pvoid >( value );
		return				( static_cast< T1 >( temp ) );
	}
}; // struct helper

template < typename T0, typename T1 >
struct helper<T0,T1,true> {
	static T1 convert		( T0* const value )
	{
		pcvoid const temp	= static_cast< pcvoid >( value );
		return				( static_cast< T1 >( temp ) );
	}
}; // struct helper

} // namespace pointer_cast
} // namespace detail

template < typename destination_type, typename source_type >
inline destination_type pointer_cast( source_type* const source )
{
	typedef typename boost::remove_pointer<destination_type>::type					destination_pointerless_type;
	typedef typename boost::remove_reference<destination_pointerless_type>::type	pure_destination_type;
	typedef typename boost::remove_reference<source_type>::type						pure_source_type;

	enum {
		const_correctness = boost::is_const< pure_source_type >::value == boost::is_const< pure_destination_type >::value,
		is_base_and_derived_from_source = boost::is_base_and_derived< pure_source_type, pure_destination_type >::value,
		is_base_and_derived_from_destination = boost::is_base_and_derived< pure_destination_type, pure_source_type >::value,
		are_both_classes = boost::is_class< pure_source_type >::value && boost::is_class< pure_destination_type >::value,
	};

	COMPILE_ASSERT			( const_correctness,					 types_are_not_const_correct);
	COMPILE_ASSERT			( !is_base_and_derived_from_source,		 downcast_detected__use_static_cast_checked_instead );
	COMPILE_ASSERT			( !is_base_and_derived_from_destination, upcast_detected__use_implicit_conversion_instead );
	COMPILE_ASSERT			( !are_both_classes,					 crosscast_detected__use_dynamic_cast_instead );
	return					
		detail::pointer_cast::helper<
			pure_source_type,
			destination_type,
			boost::is_const<
				pure_source_type
			>::value
		>::convert			( source );
}

} // namespace xray

#endif // #ifndef XRAY_DEBUG_POINTER_CAST_H_INCLUDED