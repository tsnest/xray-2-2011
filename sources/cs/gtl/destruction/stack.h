////////////////////////////////////////////////////////////////////////////
//	Module 		: stack.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::stack destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_STACK_H_INCLUDED
#define CS_GTL_DESTRUCTION_STACK_H_INCLUDED

#include <stack>
#include <cs/gtl/destruction/stl_queue.h>

namespace gtl {
namespace detail {

template <typename T, typename C>
inline void destroy				(std::stack<T,C> &object)
{
	stl_queue::destroy		(object);
}

template <typename T>
inline void destroy				(cs_stack<T> &object)
{
	stl_queue::destroy		(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_STACK_H_INCLUDED