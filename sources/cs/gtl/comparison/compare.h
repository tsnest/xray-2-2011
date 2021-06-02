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
namespace detail {

template <typename T0, typename T1, typename P>
struct compare_pointer_helper {
	template <bool a>
	static inline bool compare			(T0 const& _0, T1 const& _1, P const& p)
	{
		return		p(_0,_1);
	}

	template <>
	static inline bool compare<true>	(T0 const& _0, T1 const& _1, P const& p)
	{
		return		gtl::compare(*_0, *_1, p);
	}
};

} // namespace detail

template <typename T0, typename T1, typename P>
inline bool compare						(T0 const& _0, T1 const& _1, P const& p)
{
	return			(
		detail::compare_pointer_helper<T0,T1,P>::compare<
			boost::is_pointer<T0>::value
		>
		(_0, _1, p)
	);
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
	{\
		return		compare(p0,p1,detail::comparator<b>());\
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