////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string_inline.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_SHARED_STRING_INLINE_H_INCLUDED
#define CS_CORE_SHARED_STRING_INLINE_H_INCLUDED

inline shared_string::shared_string									()
{
}

inline shared_string::shared_string									(pcstr value)
{
	m_pointer	= g_shared_string_manager->shared_string(value);
}

inline shared_string::shared_string									(shared_string const& object)
{
	m_pointer	= object.m_pointer;
}

inline pcstr shared_string::c_str									() const
{
	if (m_pointer)
		return	m_pointer->value();

	return		0;
}

inline pcstr shared_string::operator*								() const
{
	return		c_str();
}

inline bool shared_string::operator!								() const
{
	return		!m_pointer;
}

inline u32  shared_string::length								() const
{
	return		m_pointer->length();
}

inline u32 shared_string::size										() const
{
	if (!m_pointer)
		return	0;

	return		length();
}

inline shared_string::operator shared_string::unspecified_bool_type	() const
{
	if (!m_pointer)
		return	0;

	return		&shared_string::c_str;
}

inline bool	shared_string::operator<								(shared_string const& string) const
{
	return		m_pointer < string.m_pointer;
}

inline bool shared_string::operator==								(shared_string const& string) const
{
	return		m_pointer == string.m_pointer;
}

inline bool shared_string::operator!=								(shared_string const& string) const
{
	return		m_pointer != string.m_pointer;
}

#endif // #ifndef CS_CORE_SHARED_STRING_INLINE_H_INCLUDED