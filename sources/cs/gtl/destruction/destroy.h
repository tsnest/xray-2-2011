////////////////////////////////////////////////////////////////////////////
//	Module 		: destroy.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_DESTROY_H_INCLUDED
#define CS_GTL_DESTRUCTION_DESTROY_H_INCLUDED

#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {

template <typename T>
inline void destroy				(T const& object);

namespace detail {

template <typename T, bool is_pointer>
struct destroy_pointer_helper {
	static inline void destroy	(T& object)
	{
		BOOST_STATIC_ASSERT	(!type_traits::is_stl_container<T>::value);
		(void)object;
	}

};

template <typename T>
struct destroy_pointer_helper<T,true> {
	static inline void destroy	(T& object)
	{
		if (object)
			::gtl::destroy	(*object);

		cs_delete			(object);
	}
};

template <typename T>
inline void destroy			(T& object)
{
	destroy_pointer_helper<
		T,
		boost::is_pointer<T>::value
	>::destroy				(object);
}

} // namespace detail

template <typename T>
inline void destroy				(T const& object)
{
	detail::destroy			(const_cast<typename boost::remove_const<T>::type&>(object));
}

} // namespace gtl

#include <cs/gtl/destruction/c_string.h>
#include <cs/gtl/destruction/pair.h>
#include <cs/gtl/destruction/array.h>

#endif // #ifndef CS_GTL_DESTRUCTION_DESTROY_H_INCLUDED