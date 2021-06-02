////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_fixed_storage.h
//	Created 	: 13.05.2005
//  Modified 	: 13.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad fixed storage template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_FIXED_STORAGE_H_INCLUDED
#define CS_GTL_QUAD_FIXED_STORAGE_H_INCLUDED

namespace gtl { namespace detail {

template <typename _object_type>
class fixed_storage {
public:
	typedef _object_type	_object_type;

private:
	_object_type*			m_objects;
	_object_type*			m_free_object_list;
	u32						m_max_object_count;

public:
	inline					fixed_storage	(u32 max_object_count);
	inline					~fixed_storage	();
	inline	_object_type*	object			();
	inline	void			clear			();
	inline	void			remove			(_object_type* &object);
	inline	u32				count			() const;
};

}};

#include <cs/gtl/quad_fixed_storage_inline.h>

#endif // #ifndef CS_GTL_QUAD_FIXED_STORAGE_H_INCLUDED