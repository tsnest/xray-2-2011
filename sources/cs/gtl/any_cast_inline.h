////////////////////////////////////////////////////////////////////////////
//	Module 		: any_cast_inline.h
//	Created 	: 03.05.2005
//  Modified 	: 09.05.2005
//	Author		: Dmitriy Iassenev
//	Description : any cast template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_ANY_CAST_INLINE_H_INCLUDED
#define CS_GTL_ANY_CAST_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION	template <typename T1, typename T2>
#define ANY_CAST				gtl::any_cast<T1,T2>

namespace gtl {

TEMPLATE_SPECIALIZATION
inline	ANY_CAST::any_cast	(T1 const& t1)
{
	_1	= t1;
}

TEMPLATE_SPECIALIZATION
inline	ANY_CAST::any_cast	(T2 const& t2)
{
	_2	= t2;
}

};

#undef TEMPLATE_SPECIALIZATION
#undef ANY_CAST

#endif // #ifndef CS_GTL_ANY_CAST_INLINE_H_INCLUDED