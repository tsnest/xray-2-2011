////////////////////////////////////////////////////////////////////////////
//	Module 		: hash_set.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : stdext::hash_set destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_H_INCLUDEDASH_SET_H_INCLUDED
#define CS_GTL_DESTRUCTION_H_INCLUDEDASH_SET_H_INCLUDED

#include <hash_set>
#include <cs/gtl/destruction/stl_container.h>

namespace gtl {
namespace detail {

template <typename K, typename P, typename A>
inline void destroy				(stdext::hash_set<K,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename P, typename A>
inline void destroy				(stdext::hash_multiset<K,P,A> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename P>
inline void destroy				(cs_hash_set<K,P> &object)
{
	stl_container::destroy	(object);
}

template <typename K, typename P>
inline void destroy				(cs_hash_multiset<K,P> &object)
{
	stl_container::destroy	(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_H_INCLUDEDASH_SET_H_INCLUDED