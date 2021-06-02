////////////////////////////////////////////////////////////////////////////
//	Module 		: quad_fixed_storage_inline.h
//	Created 	: 13.05.2005
//  Modified 	: 13.05.2005
//	Author		: Dmitriy Iassenev
//	Description : quad fixed storage template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_QUAD_FIXED_STORAGE_INLINE_H_INCLUDED
#define CS_GTL_QUAD_FIXED_STORAGE_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION		template <typename _object_type>
#define _fixed_storage				gtl::detail::fixed_storage<_object_type>

TEMPLATE_SPECIALIZATION
inline	_fixed_storage::fixed_storage									(u32 max_object_count) :
	m_max_object_count	(max_object_count)
{
	m_objects				= cs_alloc<_object_type>( m_max_object_count, typeid(_object_type).name());
	clear					();
}

TEMPLATE_SPECIALIZATION
inline	_fixed_storage::~fixed_storage									()
{
	cs_free					(m_objects);
}

TEMPLATE_SPECIALIZATION
inline	typename _fixed_storage::_object_type* _fixed_storage::object	()
{
	ASSERT					(m_free_object_list, "free object list is empty");
	_object_type*			node = m_free_object_list;
	m_free_object_list		= m_free_object_list->next();
	mem_fill				(node,0,sizeof(_object_type));
	return					node;
}

TEMPLATE_SPECIALIZATION
inline	void _fixed_storage::clear										()
{
	_object_type*			B = 0;
	_object_type*			I = m_objects;
	_object_type*			E = m_objects + m_max_object_count;
	
	for ( ; I != E; B = I, ++I)
		I->next()			= B;

	if (m_max_object_count)
		m_free_object_list	= E - 1;
	else
		m_free_object_list	= 0;
}

TEMPLATE_SPECIALIZATION
inline	void _fixed_storage::remove										(_object_type* &node)
{
	node->next()			= m_free_object_list;
	m_free_object_list		= node;
	node					= 0;
}

TEMPLATE_SPECIALIZATION
inline	u32 _fixed_storage::count										() const
{
	u32						result = m_max_object_count;
	_object_type*			free_object_list = m_free_object_list;
	for ( ; free_object_list; --result)
		free_object_list	= free_object_list->next();

	return					result;
}

#undef TEMPLATE_SPECIALIZATION
#undef _fixed_storage

#endif // #ifndef CS_GTL_QUAD_FIXED_STORAGE_INLINE_H_INCLUDED