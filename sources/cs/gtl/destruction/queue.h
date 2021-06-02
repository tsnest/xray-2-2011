////////////////////////////////////////////////////////////////////////////
//	Module 		: queue.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::queue destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_QUEUE_H_INCLUDED
#define CS_GTL_DESTRUCTION_QUEUE_H_INCLUDED

#include <queue>
#include <cs/gtl/destruction/stl_queue.h>

namespace gtl {
namespace detail {

template <typename T, typename S>
inline void destroy				(std::queue<T,S> &object)
{
	stl_queue::destroy	(object);
}

template <typename T, typename S>
inline void destroy				(std::priority_queue<T,S> &object)
{
	stl_queue::destroy	(object);
}

template <typename T>
inline void destroy				(cs_queue<T> &object)
{
	stl_queue::destroy	(object);
}

template <typename T>
inline void destroy				(cs_priority_queue<T> &object)
{
	stl_queue::destroy	(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_QUEUE_H_INCLUDED