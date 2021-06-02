////////////////////////////////////////////////////////////////////////////
//	Module 		: pair.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::pair destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_PAIR_H_INCLUDED
#define CS_GTL_DESTRUCTION_PAIR_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {

template <typename T1, typename T2>
inline void destroy			(std::pair<T1,T2> &pair)
{
	::gtl::destroy			(pair.first);
	::gtl::destroy			(pair.second);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_PAIR_H_INCLUDED