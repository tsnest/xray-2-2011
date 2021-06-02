////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_base.h
//	Created 	: 30.06.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : intrusive base class (for intrusive pointer)
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_INTRUSIVE_BASE_H_INCLUDED
#define CS_GTL_INTRUSIVE_BASE_H_INCLUDED

#include <cs/gtl/destruction/destroy.h>

namespace gtl {

#pragma pack(push,4)

class intrusive_base {
public:
	unsigned int		m_reference_count;

public:
	inline				intrusive_base		();

	template <typename T>
	inline	void		destroy				(T* object);
};

#pragma pack(pop)

} // namespace gtl

#include <cs/gtl/intrusive_base_inline.h>

#endif // #ifndef CS_GTL_INTRUSIVE_BASE_H_INCLUDED