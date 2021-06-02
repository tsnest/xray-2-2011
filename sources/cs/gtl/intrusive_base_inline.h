////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_base_inline.h
//	Created 	: 30.06.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : intrusive base class (for intrusive pointer) inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_INTRUSIVE_BASE_INLINE_H_INCLUDED
#define CS_GTL_INTRUSIVE_BASE_INLINE_H_INCLUDED

inline gtl::intrusive_base::intrusive_base	() :
	m_reference_count	(0)
{
}

template <typename T>
inline void gtl::intrusive_base::destroy	(T* object)
{
	gtl::destroy		(object);
}

#endif // #ifndef CS_GTL_INTRUSIVE_BASE_INLINE_H_INCLUDED