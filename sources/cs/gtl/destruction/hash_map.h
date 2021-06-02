////////////////////////////////////////////////////////////////////////////
//	Module 		: hash_map.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : stdext::hash_map destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_H_INCLUDEDASH_MAP_H_INCLUDED
#define CS_GTL_DESTRUCTION_H_INCLUDEDASH_MAP_H_INCLUDED

#include <hash_map>
#include <cs/gtl/destruction/stl_container.h>

namespace gtl {
namespace detail {

template <typename K, typename V, typename P, typename A>
inline void destroy				(stdext::hash_map<K,V,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename V, typename P, typename A>
inline void destroy				(stdext::hash_multimap<K,V,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename V, typename P>
inline void destroy				(cs_hash_map<K,V,P> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename V, typename P>
inline void destroy				(cs_hash_multimap<K,V,P> &object)
{
	stl_container::destroy	(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_H_INCLUDEDASH_MAP_H_INCLUDED