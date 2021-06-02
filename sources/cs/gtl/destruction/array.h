////////////////////////////////////////////////////////////////////////////
//	Module 		: array.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : array destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_ARRAY_H_INCLUDED
#define CS_GTL_DESTRUCTION_ARRAY_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {

template <typename T, int n>
inline void destroy			(T (&array)[n])
{
	T*						I = array;
	T*						E = array + n;
	for ( ; I != E; ++I)
		::gtl::destroy		(*I);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_ARRAY_H_INCLUDED