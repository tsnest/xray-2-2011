////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MEMBER_PREDICATES_H_INCLUDED
#define XRAY_MEMBER_PREDICATES_H_INCLUDED

namespace xray {

template <class C, class M>
class member_equal_to_impl;

template <class C, class M>
member_equal_to_impl<C, M>  inline		member_equal_to	(M C::*member_, M value);

template <class Compare, class C, class M>
class compare_member_to_impl;

namespace compare {

struct equal;
struct not_equal;
struct less;
struct greater;
struct greater_equal;
struct less_equal;

} // namespace compare

template <class Compare, class C, class M>
compare_member_to_impl<Compare, C, M>  inline	compare_member_to (M C::*member_, M value);

struct delete_predicate 
{
	delete_predicate					(memory::base_allocator * allocator) : allocator(allocator) {}
	template <class T>
	void operator ()					(T * value) const { XRAY_DELETE_IMPL(allocator, value); }

	memory::base_allocator *			allocator;
};

} // namespace xray

#include "type_predicates_inline.h"

#endif // #ifndef XRAY_MEMBER_PREDICATES_H_INCLUDED