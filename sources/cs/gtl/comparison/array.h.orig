////////////////////////////////////////////////////////////////////////////
//	Module 		: array.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : array compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_ARRAY_H_INCLUDED
#define CS_GTL_COMPARISON_ARRAY_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {

template <typename T0, int n, typename T1, int m, typename P>
inline bool compare			(const T0 (&_0)[n], const T1 (&_1)[m], P const& p)
{
	if ( ::debug::identity(n != m) )
		return				p();

	T0 const*				I = _0;
	T0 const*				E = _0 + n;
	T1 const*				J = _1;
	for ( ; I != E; ++I, ++J)
		if (!::gtl::compare(*I,*J,p))
			return			false;

	return					true;
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_ARRAY_H_INCLUDED