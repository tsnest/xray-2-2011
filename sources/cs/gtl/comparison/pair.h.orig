////////////////////////////////////////////////////////////////////////////
//	Module 		: pair.h
//	Created 	: 30.05.2007
//  Modified 	: 30.05.2007
//	Author		: Dmitriy Iassenev
//	Description : std::pair compare templates
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_COMPARISON_PAIR_H_INCLUDED
#define CS_GTL_COMPARISON_PAIR_H_INCLUDED

#include <boost/static_assert.hpp>
#include <cs/gtl/type_traits.h>

namespace gtl {
namespace detail {

template <typename T01, typename T02, typename T11, typename T12, typename P>
inline bool compare			(const std::pair<T01,T02> &_0, const std::pair<T11,T12> &_1, P const& p)
{
	if (!::gtl::compare(_0.first,_1.first,p))
		return				false;

	if (!::gtl::compare(_0.second,_1.second,p))
		return				false;

	return					true;
}

} // namespace detail
} // namespace gtl

#endif // #ifndef CS_GTL_COMPARISON_PAIR_H_INCLUDED