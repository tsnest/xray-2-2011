////////////////////////////////////////////////////////////////////////////
//	Module 		: list.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::list destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_LIST_H_INCLUDED
#define CS_GTL_DESTRUCTION_LIST_H_INCLUDED

#include <list>
#include <cs/gtl/destruction/stl_container.h>

namespace gtl {
namespace detail {

template <typename T, typename A>
inline void destroy				(std::list<T,A> &object)
{
	stl_container::destroy	(object);
}

template <typename T>
inline void destroy				(cs_list<T> &object)
{
	stl_container::destroy	(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_LIST_H_INCLUDED