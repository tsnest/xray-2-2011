////////////////////////////////////////////////////////////////////////////
//	Module 		: deque.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::deque compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_DEQUE_H_INCLUDED
#define CS_GTL_COMPARISON_DEQUE_H_INCLUDED

#include <deque>
#include <cs/gtl/comparison/stl_queue.h>

namespace gtl {
namespace detail {

template <typename T0, typename A0, typename T1, typename A1, typename P>
inline bool compare				(const std::deque<T0,A0> &_0, const std::deque<T1,A1> &_1, P const& p)
{
	stl_queue::compare		(_0,_1,p);
}

template <typename T0, typename T1, typename P>
inline bool compare				(const cs_deque<T0> &_0, const cs_deque<T1> &_1, P const& p)
{
	stl_queue::compare		(_0,_1,p);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_DEQUE_H_INCLUDED