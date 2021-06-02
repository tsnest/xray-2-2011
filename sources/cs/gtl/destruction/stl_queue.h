////////////////////////////////////////////////////////////////////////////
//	Module 		: stl_container.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : stl container destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_STL_CONTAINER_H_INCLUDED
#define CS_GTL_DESTRUCTION_STL_CONTAINER_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {
namespace stl_queue {

template <typename T>
inline void destroy				(T& queue)
{
	BOOST_STATIC_ASSERT		(type_traits::is_stl_container<T>::value);

	for ( ; !queue.empty(); queue.pop())
		gtl::destroy		(queue.front());
}

} // namespace stl_container
} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_STL_CONTAINER_H_INCLUDED