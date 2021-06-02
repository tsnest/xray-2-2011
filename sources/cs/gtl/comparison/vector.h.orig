////////////////////////////////////////////////////////////////////////////
//	Module 		: vector.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::vector compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_VECTOR_H_INCLUDED
#define CS_GTL_COMPARISON_VECTOR_H_INCLUDED

#include <vector>
#include <cs/gtl/comparison/stl_container.h>

namespace gtl {
namespace detail {

template <typename T0, typename A0, typename T1, typename A1, typename P>
inline bool compare				(const std::vector<T0,A0> &_0, const std::vector<T1,A1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

template <typename T0, typename T1, typename P>
inline bool compare				(const cs_vector<T0> &_0, const cs_vector<T1> &_1, P const& p)
{
	return	stl_container::compare(_0,_1,p);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_VECTOR_H_INCLUDED