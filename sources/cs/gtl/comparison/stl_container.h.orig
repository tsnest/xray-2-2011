////////////////////////////////////////////////////////////////////////////
//	Module 		: stl_container.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : stl container compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_STL_CONTAINER_H_INCLUDED
#define CS_GTL_COMPARISON_STL_CONTAINER_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {
namespace stl_container {

template <typename T0, typename T1, typename P>
inline bool compare			(T0 const& _0, T1 const& _1, P const& p)
{
	BOOST_STATIC_ASSERT		(type_traits::is_stl_container<T0>::value);
	BOOST_STATIC_ASSERT		(type_traits::is_stl_container<T1>::value);

	if (_0.size() != _1.size())
		return				p();

	T0::const_iterator		I = _0.begin();
	T0::const_iterator		E = _0.end();
	T1::const_iterator		J = _1.begin();
	for ( ; I != E; ++I, ++J)
		if (!::gtl::compare(*I,*J,p))
			return			false;

	return					true;
}

} // namespace stl_container
} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_STL_CONTAINER_H_INCLUDED