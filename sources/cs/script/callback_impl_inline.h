////////////////////////////////////////////////////////////////////////////
//	Module 		: script_callback_impl_inline.h
//	Created 	: 30.06.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : callback implementation class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_CALLBACK_IMPL_INLINE_H_INCLUDED
#define CS_SCRIPT_CALLBACK_IMPL_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION	template <typename return_type>
#define SCRIPT_CALLBACK			cs::script::callback_impl<return_type>

TEMPLATE_SPECIALIZATION
inline SCRIPT_CALLBACK::callback_impl				()
{
}

TEMPLATE_SPECIALIZATION
inline void SCRIPT_CALLBACK::clear					()
{
	m_functor.~functor_type			();
	new (&m_functor) functor_type	();

	m_object.~object_type			();
	new (&m_object)	object_type		();
}

TEMPLATE_SPECIALIZATION
inline SCRIPT_CALLBACK::callback_impl				(self_type const& callback)
{
	clear				();
	*this				= callback;
}

TEMPLATE_SPECIALIZATION
template <typename T>
inline bool SCRIPT_CALLBACK::valid					(T const& object) const
{
	if (!object.is_valid())
		return			false;
	
	if (!object.interpreter())
		return			false;

	return				true;
}

TEMPLATE_SPECIALIZATION
inline SCRIPT_CALLBACK& SCRIPT_CALLBACK::operator=	(self_type const& callback)
{
	clear				();

	if (valid(callback.m_functor))
		m_functor		= callback.m_functor;

	if (valid(callback.m_object))
		m_object		= callback.m_object;

	return				*this;
}

TEMPLATE_SPECIALIZATION
inline bool SCRIPT_CALLBACK::same_object			(object_type const& object) const
{
	switch (m_object.type()) {
		case LUA_TNIL :
		case LUA_TNONE :
			return		true;
	}

	switch (object.type()) {
		case LUA_TNIL :
		case LUA_TNONE :
			return		true;
	}
	
	return				m_object == object;
}

TEMPLATE_SPECIALIZATION
inline void SCRIPT_CALLBACK::bind					(functor_type const& functor)
{
	clear				();

	if (valid(functor))
		m_functor		= functor;
}

TEMPLATE_SPECIALIZATION
inline void SCRIPT_CALLBACK::bind					(functor_type const& functor, object_type const& object)
{
	clear				();
	
	if (valid(functor))
		m_functor		= functor;
	
	if (valid(object))
		m_object		= object;
}

TEMPLATE_SPECIALIZATION
inline bool SCRIPT_CALLBACK::empty					() const
{
	return				valid(m_functor);
}

TEMPLATE_SPECIALIZATION
inline bool SCRIPT_CALLBACK::operator==				(self_type const& callback) const
{
	if (!(m_functor == callback.m_functor))
		return			false;

	if (!(m_object == callback.m_object))
		return			false;

	return				true;
}

TEMPLATE_SPECIALIZATION
inline bool SCRIPT_CALLBACK::operator<				(self_type const& callback) const
{
	if (m_functor < callback.m_functor)
		return			true;

	if (callback.m_functor < m_functor)
		return			false;

	if (m_object < callback.m_object)
		return			true;

	return				false;
}

TEMPLATE_SPECIALIZATION
inline SCRIPT_CALLBACK::operator typename SCRIPT_CALLBACK::unspecified_bool_type	() const
{
	return valid(m_functor) ? &callback_impl::empty : 0;
}

#undef TEMPLATE_SPECIALIZATION
#undef SCRIPT_CALLBACK

#endif // #ifndef CS_SCRIPT_CALLBACK_IMPL_INLINE_H_INCLUDED