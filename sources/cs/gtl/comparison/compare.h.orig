////////////////////////////////////////////////////////////////////////////
//	Module 		: compare.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_COMPARE_H_INCLUDED
#define CS_GTL_COMPARISON_COMPARE_H_INCLUDED

#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {

template < typename LeftType, typename RightType, typename PredicateType >
inline bool compare						(
		LeftType const& left,
		RightType const& right,
		PredicateType const& predicate
	)
{
	return		predicate( left, right );
}

template < typename LeftType, typename RightType, typename PredicateType >
inline bool compare						(
		LeftType* const left,
		RightType* const right,
		PredicateType* const predicate
	)
{
	return		gtl::compare( *left, *right, predicate );
}

namespace detail {
	template <template <typename _1> class P>
	struct comparator {
		template <typename T>
		inline bool operator()			(T const& _1, T const& _2) const
		{
			return	P<T>()(_1,_2);
		}

		inline bool operator()			() const
		{
			return	P<bool>()(false,true);
		}
	};
};

#define CS_DECLARE_COMPARER(a,b) \
	template <typename T1, typename T2>\
	inline	bool a						(T1 const& p0, T2 const& p1)\
	{ \
		return		compare(p0,p1,detail::comparator<b>()); \
	}

CS_DECLARE_COMPARER(equal,			std::equal_to);
CS_DECLARE_COMPARER(greater_equal,	std::greater_equal);
CS_DECLARE_COMPARER(greater,		std::greater);
CS_DECLARE_COMPARER(less_equal,		std::less_equal);
CS_DECLARE_COMPARER(less,			std::less);
CS_DECLARE_COMPARER(not_equal,		std::not_equal_to);
CS_DECLARE_COMPARER(logical_and,	std::logical_and);
CS_DECLARE_COMPARER(logical_or,		std::logical_or);

#undef CS_DECLARE_COMPARER

} // namespace gtl

#include <cs/gtl/comparison/c_string.h>
#include <cs/gtl/comparison/pair.h>
#include <cs/gtl/comparison/array.h>

#endif // #ifndef CS_GTL_COMPARISON_COMPARE_H_INCLUDED