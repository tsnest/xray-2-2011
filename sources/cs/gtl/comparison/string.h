////////////////////////////////////////////////////////////////////////////
//	Module 		: string.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::string compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_STRING_H_INCLUDED
#define CS_GTL_COMPARISON_STRING_H_INCLUDED

#include <string>

namespace gtl {
namespace detail {

template <typename C0, typename T0, typename A0, typename C1, typename T1, typename A1, typename P>
inline bool compare				(const std::basic_string<C0,T0,A0> &_0, const std::basic_string<C1,T1,A1> &_1, P const& p)
{
	return	p(_0.compare(_1),0);
}

template <typename P>
inline bool compare				(cs_string const& _0, cs_string const& _1, P const& p)
{
	return	p(_0.compare(_1),0);
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_STRING_H_INCLUDED