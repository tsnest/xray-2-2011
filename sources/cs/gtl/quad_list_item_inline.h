////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_list_item_inline.h
//	Created 	: 13.05.2005
//  Modified 	: 13.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad list item template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_LIST_ITEM_INLINE_H_INCLUDED
#define CS_GTL_QUAD_LIST_ITEM_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION	<typename _object_type>
#define _quad_list_item			gtl::detail::quad_list_item<_object_type>

template <typename _object_type>
inline	_quad_list_item* &_quad_list_item::next	()
{
	return	m_next;
}

#undef TEMPLATE_SPECIALIZATION
#undef _quad_list_item

#endif // #ifndef CS_GTL_QUAD_LIST_ITEM_INLINE_H_INCLUDED