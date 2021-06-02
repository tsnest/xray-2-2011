////////////////////////////////////////////////////////////////////////////
//	Module 		: set.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::set destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_SET_H_INCLUDED
#define CS_GTL_DESTRUCTION_SET_H_INCLUDED

#include <set>
#include <cs/gtl/destruction/stl_container.h>

namespace gtl {
namespace detail {

template <typename K, typename P, typename A>
inline void destroy				(std::set<K,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename P, typename A>
inline void destroy				(std::multiset<K,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename T, typename P>
inline void destroy				(cs_set<T,P> &object)
{
	stl_container::destroy	(object);
}

template <typename T, typename P>
inline void destroy				(cs_multiset<T,P> &object)
{
	stl_container::destroy	(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_SET_H_INCLUDED