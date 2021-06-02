////////////////////////////////////////////////////////////////////////////
//	Module 		: c_string.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : C style string destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_C_STRING_H_INCLUDED
#define CS_GTL_DESTRUCTION_C_STRING_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {

inline void destroy			(pcstr object)
{
	(void)object;
}

inline void destroy			(pstr& object)
{
	cs_free					(object);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_C_STRING_H_INCLUDED