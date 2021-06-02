////////////////////////////////////////////////////////////////////////////
//	Module 		: c_string.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : C style string compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_C_STRING_H_INCLUDED
#define CS_GTL_COMPARISON_C_STRING_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {

template <typename P>
inline bool compare			(pcstr const& _0, pcstr const& _1, P const& p)
{
	return	p(sz_cmp(_0,_1),0);
}

template <typename P>
inline bool compare			(pstr const& _0, pstr const& _1, P const& p)
{
	return	p(sz_cmp(_0,_1),0);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_C_STRING_H_INCLUDED