////////////////////////////////////////////////////////////////////////////
//	Module 		: property_holder_inline.h
//	Created 	: 12.11.2005
//  Modified 	: 12.11.2005
//	Author		: Dmitriy Iassenev
//	Description : property holder class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_H_INCLUDEDOLDER_INLINE_H_INCLUDED
#define PROPERTY_H_INCLUDEDOLDER_INLINE_H_INCLUDED

inline pcstr cs::script::detail::property_holder::id														() const
{
	ASSERT	(m_id.size(), "null shared string");
	return	*m_id;
}

inline cs::script::detail::property_holder::object_type const& cs::script::detail::property_holder::object	() const
{
	ASSERT	(m_object, "invalid luabind::object");
	return	m_object;
}

inline cs::script::detail::property_holder::object_type const& cs::script::detail::property_holder::sections() const
{
	return	object();
}

inline bool cs::script::detail::property_holder::bool_value													(pcstr string)
{
	return	(
		!sz_cmp(string,"on")
		||
		!sz_cmp(string,"yes")
		||
		!sz_cmp(string,"true")
		||
		!sz_cmp(string,"1")
	);
}

#endif // #ifndef PROPERTY_H_INCLUDEDOLDER_INLINE_H_INCLUDED