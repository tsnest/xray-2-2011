////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_ptr_inline.h
//	Created 	: 30.06.2005
//  Modified 	: 28.05.2007
//	Author		: Dmitriy Iassenev
//	Description : intrusive pointer template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_INTRUSIVE_PTR_INLINE_H_INCLUDED
#define CS_GTL_INTRUSIVE_PTR_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION\
	template <\
		typename object_type,\
		typename base_type,\
		template <typename> class _functionaly_type\
	>

#define the_intrusive_ptr\
	gtl::intrusive_ptr<\
		object_type,\
		base_type,\
		_functionaly_type\
	>

TEMPLATE_SPECIALIZATION
inline the_intrusive_ptr::intrusive_ptr		()
{
	m_object		= 0;
}

TEMPLATE_SPECIALIZATION
inline the_intrusive_ptr::intrusive_ptr		(object_type* object)
{
	m_object		= 0;
	set				(object);
}

TEMPLATE_SPECIALIZATION
inline the_intrusive_ptr::intrusive_ptr		(self_type const& object)
{
	m_object		= 0;
	set				(object);
}

TEMPLATE_SPECIALIZATION
inline the_intrusive_ptr::~intrusive_ptr	()
{
	dec				();
}

TEMPLATE_SPECIALIZATION
inline void the_intrusive_ptr::dec			()
{
	if (!m_object)
		return;
	
	--m_object->base_type::m_reference_count;

	if (!m_object->base_type::m_reference_count)
		m_object->base_type::destroy	(m_object);
}

TEMPLATE_SPECIALIZATION
inline object_type& the_intrusive_ptr::operator*			() const
{
	ASSERT			(m_object, "dereferencing null pointer");
	return			*m_object;
}

TEMPLATE_SPECIALIZATION
inline object_type* the_intrusive_ptr::operator->			() const
{
	ASSERT			(m_object, "dereferencing null pointer");
	return			m_object;
}

TEMPLATE_SPECIALIZATION
inline bool the_intrusive_ptr::operator!	() const
{
	return			!m_object;
}

TEMPLATE_SPECIALIZATION
inline the_intrusive_ptr::operator typename the_intrusive_ptr::unspecified_bool_type	() const
{
	if (!m_object)
		return		0;

	return			&intrusive_ptr::get;
}

TEMPLATE_SPECIALIZATION
inline typename the_intrusive_ptr::self_type& the_intrusive_ptr::operator=				(object_type* object)
{
	set				(object);
	return			(self_type&)*this;
}

TEMPLATE_SPECIALIZATION
inline typename the_intrusive_ptr::self_type& the_intrusive_ptr::operator=				(self_type const& object)
{
	set				(object);
	return			(self_type&)*this;
}

TEMPLATE_SPECIALIZATION
inline bool the_intrusive_ptr::operator==	(the_intrusive_ptr const& object) const
{
	return			get() == object.get();
}

TEMPLATE_SPECIALIZATION
inline bool the_intrusive_ptr::operator!=	(the_intrusive_ptr const& object) const
{
	return			get() != object.get();
}

TEMPLATE_SPECIALIZATION
inline bool the_intrusive_ptr::operator<	(the_intrusive_ptr const& object) const
{
	return			get() < object.get();
}

TEMPLATE_SPECIALIZATION
inline bool the_intrusive_ptr::operator>	(the_intrusive_ptr const& object) const
{
	return			get() > object.get();
}

TEMPLATE_SPECIALIZATION
inline void the_intrusive_ptr::swap			(self_type& object)
{
	object_type*		tmp = m_object; 
	m_object		= rhs.m_object; 
	object.m_object	= tmp;	
}

TEMPLATE_SPECIALIZATION
inline bool the_intrusive_ptr::equal		(self_type const& object)	const
{
	return			m_object == object.m_object;
}

TEMPLATE_SPECIALIZATION
inline void the_intrusive_ptr::set			(object_type* object)
{
	if (m_object == object)
		return;
	
	dec				();
	m_object		= object;

	if (!m_object)
		return;

	++m_object->m_reference_count;
}

TEMPLATE_SPECIALIZATION
inline void the_intrusive_ptr::set			(self_type const& object)
{
	if (m_object == object.m_object)
		return;

	dec				();
	m_object		= object.m_object;
	
	if (!m_object)
		return;

	++m_object->m_reference_count;
}

TEMPLATE_SPECIALIZATION
inline object_type* the_intrusive_ptr::get					()	const
{
	return			m_object;
}

TEMPLATE_SPECIALIZATION
inline void swap							(the_intrusive_ptr& _0, the_intrusive_ptr& _1)
{
	_0.swap			(_1);
}

#undef TEMPLATE_SPECIALIZATION
#undef the_intrusive_ptr

#endif // #ifndef CS_GTL_INTRUSIVE_PTR_INLINE_H_INCLUDED