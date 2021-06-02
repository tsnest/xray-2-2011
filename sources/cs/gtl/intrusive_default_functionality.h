////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_default_functionality.h
//	Created 	: 30.06.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : intrusive pointer default functionality template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_INTRUSIVE_DEFAULT_FUNCTIONALITY_H_INCLUDED
#define CS_GTL_INTRUSIVE_DEFAULT_FUNCTIONALITY_H_INCLUDED

namespace gtl {

template <typename ChildType>
class intrusive_default_functionality {
protected:
	inline	ChildType&			self	();
	inline	ChildType const&	self	() const;
};

} // namespace gtl

#include <cs/gtl/intrusive_default_functionality_inline.h>

#endif // #ifndef CS_GTL_INTRUSIVE_DEFAULT_FUNCTIONALITY_H_INCLUDED