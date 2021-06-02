////////////////////////////////////////////////////////////////////////////
//	Module 		: map.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::map compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_MAP_H_INCLUDED
#define CS_GTL_COMPARISON_MAP_H_INCLUDED

#include <map>
#include <cs/gtl/comparison/stl_container.h>

namespace gtl {
namespace detail {

template <typename K0, typename V0, typename P0, typename A0, typename K1, typename V1, typename P1, typename A1, typename P>
inline bool compare				(const std::map<K0,V0,P0,A0> &_0, const std::map<K1,V1,P1,A1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename K0, typename V0, typename P0, typename A0, typename K1, typename V1, typename P1, typename A1, typename P>
inline bool compare				(const std::multimap<K0,V0,P0,A0> &_0, const std::multimap<K1,V1,P1,A1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename K0, typename V0, typename P0, typename K1, typename V1, typename P1, typename P>
inline bool compare				(const cs_map<K0,V0,P0> &_0, const cs_map<K1,V1,P1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename K0, typename V0, typename P0, typename K1, typename V1, typename P1, typename P>
inline bool compare				(const cs_multimap<K0,V0,P0> &_0, const cs_multimap<K1,V1,P1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_MAP_H_INCLUDED