////////////////////////////////////////////////////////////////////////////
//	Module 		: object_constructor_base_inline.h
//	Created 	: 27.05.2004
//  Modified 	: 21.04.2005
//	Author		: Dmitriy Iassenev
//	Description : object constructor base class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_OBJECT_CONSTRUCTOR_BASE_INLINE_H_INCLUDED
#define CS_GTL_OBJECT_CONSTRUCTOR_BASE_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION template <\
	typename _base_class_type,\
	typename _clsid_type\
>
#define _object_constructor_base gtl::object_constructor_base<_base_class_type,_clsid_type>

TEMPLATE_SPECIALIZATION
inline	_object_constructor_base::object_constructor_base	(_clsid_type const& clsid)
{
	m_clsid		= clsid;
}

TEMPLATE_SPECIALIZATION
inline	typename _object_constructor_base::_clsid_type const& _object_constructor_base::clsid	() const
{
	return		m_clsid;
}

#undef TEMPLATE_SPECIALIZATION
#undef _object_constructor_base

#endif // #ifndef CS_GTL_OBJECT_CONSTRUCTOR_BASE_INLINE_H_INCLUDED