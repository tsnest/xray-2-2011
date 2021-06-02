////////////////////////////////////////////////////////////////////////////
//	Module 		: type_traits.h
//	Created 	: 03.05.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : type traits
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_TYPE_TRAITS_H_INCLUDED
#define CS_GTL_TYPE_TRAITS_H_INCLUDED

#include <boost/type_traits/detail/yes_no_type.hpp>

namespace gtl {
namespace type_traits {

template <template <typename _1> class T1, typename T2>
struct is_base_and_derived_or_same_from_template_1_1 {
	template <typename P>
	static boost::type_traits::yes_type	select(T1<P>*);
	static boost::type_traits::no_type	select(...);

	enum { value = sizeof(boost::type_traits::yes_type) == sizeof(select((T2*)0))};
};

#define declare_has(a) \
	template <typename T>\
	struct has_##a {\
		template <typename P> static boost::type_traits::yes_type	select(typename P::a*);\
		template <typename P> static boost::type_traits::no_type	select(...);\
		enum { value = sizeof(boost::type_traits::yes_type) == sizeof(select<T>(0)) };\
	};

declare_has(iterator);
declare_has(const_iterator);
declare_has(value_type);
declare_has(size_type);
declare_has(value_compare);

#undef declare_has

template <typename T>
struct is_stl_container {
	enum { 
		value = 
			has_iterator<T>::value &&
			has_const_iterator<T>::value &&
			has_size_type<T>::value &&
			has_value_type<T>::value
	};
};

template <typename _T>
struct is_tree_structure {
	enum { 
		value = 
			has_value_compare<_T>::value
	};
};

} // namespace type_traits
} // namespace gtl

#endif // #ifndef CS_GTL_TYPE_TRAITS_H_INCLUDED