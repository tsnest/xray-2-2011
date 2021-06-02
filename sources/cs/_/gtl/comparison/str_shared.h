////////////////////////////////////////////////////////////////////////////
//	Module 		: str_shared.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : str_shared compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_STR_SHARED_H_INCLUDED
#define CS_GTL_COMPARISON_STR_SHARED_H_INCLUDED

namespace gtl {
namespace detail {

template <typename P>
inline void destroy				(str_shared const& _0, str_shared const& _1, P const& p)
{
	return	p(sz_cmp(_0,_1),0);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_STR_SHARED_H_INCLUDED