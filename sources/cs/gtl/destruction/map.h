////////////////////////////////////////////////////////////////////////////
//	Module 		: map.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::map destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_MAP_H_INCLUDED
#define CS_GTL_DESTRUCTION_MAP_H_INCLUDED

#include <map>
#include <cs/gtl/destruction/stl_container.h>

namespace gtl {
namespace detail {

template <typename K, typename V, typename P, typename A>
inline void destroy				(std::map<K,V,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename V, typename P, typename A>
inline void destroy				(std::multimap<K,V,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename V, typename P>
inline void destroy				(cs_map<K,V,P> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename V, typename P>
inline void destroy				(cs_multimap<K,V,P> &object)
{
	stl_container::destroy	(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_MAP_H_INCLUDED