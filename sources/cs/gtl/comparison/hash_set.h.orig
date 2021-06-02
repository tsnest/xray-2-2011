////////////////////////////////////////////////////////////////////////////
//	Module 		: hash_set.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : stdext::hash_set compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_H_INCLUDEDASH_SET_H_INCLUDED
#define CS_GTL_COMPARISON_H_INCLUDEDASH_SET_H_INCLUDED

#include <hash_set>
#include <cs/gtl/comparison/stl_container.h>

namespace gtl {
namespace detail {

template <typename K0, typename P0, typename A0, typename K1, typename P1, typename A1, typename P>
inline bool compare				(const stdext::hash_set<K0,P0,A0> &_0, const stdext::hash_set<K1,P1,A1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename K0, typename P0, typename A0, typename K1, typename P1, typename A1, typename P>
inline bool compare				(const stdext::hash_multiset<K0,P0,A0> &_0, const stdext::hash_multiset<K1,P1,A1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename K0, typename P0, typename K1, typename P1, typename P>
inline bool compare				(const cs_hash_set<K0,P0> &_0, const cs_hash_set<K1,P1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename K0, typename P0, typename K1, typename P1, typename P>
inline bool compare				(const cs_hash_multiset<K0,P0> &_0, const cs_hash_multiset<K1,P1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_H_INCLUDEDASH_SET_H_INCLUDED