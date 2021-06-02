////////////////////////////////////////////////////////////////////////////
//	Module 		: string.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::string destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_STRING_H_INCLUDED
#define CS_GTL_DESTRUCTION_STRING_H_INCLUDED

#include <string>

namespace gtl {
namespace detail {

template <typename C, typename T, typename A>
inline void destroy				(std::basic_string<C,T,A> &object)
{
	(void)object;
}

inline void destroy				(cs_string& object)
{
	(void)object;
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_STRING_H_INCLUDED