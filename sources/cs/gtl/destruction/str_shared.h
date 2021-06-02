////////////////////////////////////////////////////////////////////////////
//	Module 		: str_shared.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : str_shared destroy templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_DESTRUCTION_STR_SHARED_H_INCLUDED
#define CS_GTL_DESTRUCTION_STR_SHARED_H_INCLUDED

namespace gtl {
namespace detail {

inline void destroy				(str_shared& object)
{
	(void)object;
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_DESTRUCTION_STR_SHARED_H_INCLUDED