////////////////////////////////////////////////////////////////////////////
//	Module 		: stl_container.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : stl container compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_STL_CONTAINER_H_INCLUDED
#define CS_GTL_DESTRUCTION_STL_CONTAINER_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {
namespace stl_queue {

template <typename T0, typename T1, typename P>
inline bool compare				(T0 const& _0, T1 const& _1, P const& p)
{
	BOOST_STATIC_ASSERT		(type_traits::is_stl_container<T0>::value);
	BOOST_STATIC_ASSERT		(type_traits::is_stl_container<T1>::value);

	if (_0.size() != _1.size())
		return				p();

	T0						__0 = _0;
	T0						__1 = _1;

	for ( ; !_0.empty(); _0.pop(), _1.pop())
		if (!::gtl::compare(_0.front(),_1.front(),p))
			return			false;

	return					true;
}

} // namespace stl_container
} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_STL_CONTAINER_H_INCLUDED