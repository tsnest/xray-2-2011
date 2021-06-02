////////////////////////////////////////////////////////////////////////////
//	Module 		: stack.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::stack compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_STACK_H_INCLUDED
#define CS_GTL_COMPARISON_STACK_H_INCLUDED

#include <stack>
#include <cs/gtl/comparison/stl_queue.h>

namespace gtl {
namespace detail {

template <typename T0, typename C0, typename T1, typename C1, typename P>
inline bool compare				(const std::stack<T0,C0> &_0, const std::stack<T1,C1> &_1, P const& p)
{
	return	stl_queue::compare(_0,_1,p);
}

template <typename T0, typename T1>
inline bool compare				(const cs_stack<T0> &_0, const cs_stack<T1> &_1, P const& p)
{
	return	stl_queue::compare(_0,_1,p);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_STACK_H_INCLUDED