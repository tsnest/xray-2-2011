////////////////////////////////////////////////////////////////////////////
//	Module 		: any_cast.h
//	Created 	: 03.05.2005
//  Modified 	: 03.05.2005
//	Author		: Dmitriy Iassenev
//	Description : any cast template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_ANY_CAST_H_INCLUDED
#define CS_GTL_ANY_CAST_H_INCLUDED

namespace gtl {

template <typename T1, typename T2>
union any_cast {
	T1	_1;
	T2	_2;

	inline		any_cast	(T1 const& t1);
	inline		any_cast	(T2 const& t2);
};

};

#include <cs/gtl/any_cast_inline.h>

#endif // #ifndef CS_GTL_ANY_CAST_H_INCLUDED