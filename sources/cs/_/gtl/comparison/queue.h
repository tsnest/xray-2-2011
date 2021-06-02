////////////////////////////////////////////////////////////////////////////
//	Module 		: queue.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::queue compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_QUEUE_H_INCLUDED
#define CS_GTL_COMPARISON_QUEUE_H_INCLUDED

#include <queue>
#include <cs/gtl/comparison/stl_queue.h>

namespace gtl {
namespace detail {

template <typename T0, typename S0, typename T1, typename S1, typename P>
inline bool compare				(const std::queue<T0,S0> &_0, const std::queue<T1,S1> &_1, P const& p)
{
	return	stl_queue::compare(_0,_1,p);
}

template <typename T0, typename S0, typename T1, typename S1, typename P>
inline bool compare				(const std::priority_queue<T0,S0> &_0, const std::priority_queue<T1,S1> &_1, P const& p)
{
	return	stl_queue::compare(_0,_1,p);
}

template <typename T0, typename T1, typename P>
inline bool compare				(const cs_queue<T0> &_0, const cs_queue<T1> &_1, P const& p)
{
	return	stl_queue::compare(_0,_1,p);
}

template <typename T0, typename T1, typename P>
inline bool compare				(const cs_priority_queue<T0> &_0, const cs_priority_queue<T1> &_1, P const& p)
{
	return	stl_queue::compare(_0,_1,p);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_QUEUE_H_INCLUDED