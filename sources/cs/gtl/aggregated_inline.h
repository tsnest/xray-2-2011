////////////////////////////////////////////////////////////////////////////
//	Module 		: aggregated_inline.h
//	Created 	: 17.04.2005
//  Modified 	: 28.04.2008
//	Author		: Dmitriy Iassenev
//	Description : aggregated template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_GTL_AGGREGATED_INLINE_H_INCLUDED
#define CS_GTL_AGGREGATED_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION		template < typename object_type, bool use_pointer >
#define AGGREGATED					gtl::aggregated< object_type, use_pointer >

TEMPLATE_SPECIALIZATION
inline AGGREGATED::aggregated								()
{
	m_object	= 0;
}

TEMPLATE_SPECIALIZATION
inline AGGREGATED* AGGREGATED::construct					(object_type* object)
{
	ASSERT		(object);
	ASSERT		(!m_object, "object has been already initialized");
	m_object	= object;
	return		this;
}

TEMPLATE_SPECIALIZATION
inline typename AGGREGATED::object_type& AGGREGATED::object	() const
{
	ASSERT		(m_object);
	return		*m_object;
}

#undef TEMPLATE_SPECIALIZATION
#undef AGGREGATED

#define TEMPLATE_SPECIALIZATION		template <typename object_type>
#define AGGREGATED					gtl::aggregated<object_type,false>

TEMPLATE_SPECIALIZATION
inline AGGREGATED* AGGREGATED::construct					(object_type const& object)
{
	m_object	= object;
	return		this;
}

TEMPLATE_SPECIALIZATION
inline typename AGGREGATED::object_type& AGGREGATED::object	() const
{
	return		m_object;
}

#undef TEMPLATE_SPECIALIZATION
#undef AGGREGATED

#endif // #ifndef CS_GTL_AGGREGATED_INLINE_H_INCLUDED