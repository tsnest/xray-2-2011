////////////////////////////////////////////////////////////////////////////
//	Module 		: deque.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::deque destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_DEQUE_H_INCLUDED
#define CS_GTL_DESTRUCTION_DEQUE_H_INCLUDED

#include <deque>
#include <cs/gtl/destruction/stl_queue.h>

namespace gtl {
namespace detail {

template <typename T, typename A>
inline void destroy				(std::deque<T,A> &object)
{
	stl_queue::destroy		(object);
}

template <typename T>
inline void destroy				(cs_deque<T> &object)
{
	stl_queue::destroy		(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_DEQUE_H_INCLUDED