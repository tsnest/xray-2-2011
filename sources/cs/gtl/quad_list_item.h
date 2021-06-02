////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_list_item.h
//	Created 	: 13.05.2005
//  Modified 	: 13.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad list item template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_LIST_ITEM_H_INCLUDED
#define CS_GTL_QUAD_LIST_ITEM_H_INCLUDED

namespace gtl { namespace detail {

template <typename _object_type>
class quad_list_item {
public:
	typedef _object_type	_object_type;

public:
	_object_type*			m_object;
	quad_list_item*			m_next;

public:
	inline	quad_list_item*	&next		();
};

}};

#include <cs/gtl/quad_list_item_inline.h>

#endif // #ifndef CS_GTL_QUAD_LIST_ITEM_H_INCLUDED