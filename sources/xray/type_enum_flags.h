////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_TYPE_ENUM_FLAGS_H_INCLUDED
#define XRAY_TYPE_ENUM_FLAGS_H_INCLUDED

#include <xray/threading_policies.h>

namespace xray {

template <class enum_type>
class XRAY_CORE_API enum_flags
{
public:
	enum_flags (u32 flags = 0) : m_flags(flags) {}

	enum_flags	operator |=	(u32 flags) 				{ m_flags |= flags; return *this; }
	enum_flags	operator |=	(enum_flags flags) 			{ m_flags |= (u32)flags; return *this; }
	enum_flags	operator &=	(u32 flags) 				{ m_flags &= flags; return *this; }
	enum_flags	operator &=	(enum_flags flags) 			{ m_flags &= (u32)flags; return *this; }

	enum_flags	operator |  (u32 flags) 		const 	{ return enum_flags(m_flags | flags); }
	enum_flags	operator |  (enum_type flags) 	const 	{ return enum_flags(m_flags | flags); }
	enum_flags	operator |  (enum_flags flags) 	const 	{ return enum_flags(m_flags | flags); }
	enum_flags	operator &  (u32 flags) 		const 	{ return enum_flags(m_flags & flags); }
	enum_flags	operator &  (enum_type flags) 	const 	{ return enum_flags(m_flags & flags); }
	enum_flags	operator &  (enum_flags flags) 	const 	{ return enum_flags(m_flags & (u32)flags); }
	enum_flags	operator ~	()					const 	{ return enum_flags(~m_flags); }

	bool		operator ==	(enum_flags flags)	const	{ return m_flags == (u32)flags; }
	bool		operator ==	(u32 flags)			const	{ return m_flags == flags; }
	bool		operator ==	(enum_type flags)	const	{ return m_flags == (u32)flags; }
	operator	bool		()					const	{ return !!m_flags; }
	operator	u32			()					const 	{ return m_flags; }
	operator	enum_type	()					const 	{ return (enum_type)m_flags; }

private:
	u32			m_flags;
};

template <typename FlagsEnum, typename ThreadingPolicy = threading::single_threading_policy>
class XRAY_CORE_API flags_type
{
public:
	typedef	flags_type											flags_type_typedef;
	typedef	typename threading::policy_resolver< ThreadingPolicy >::policy_type		policy_type;

public:
							flags_type		() : m_flags(0) {}
							flags_type		(enum_flags<FlagsEnum> flags) : m_flags((u32)flags) { }
	bool					has				(enum_flags<FlagsEnum> flags) const { return (m_flags & (u32)flags) == (u32)flags; }
	u32						set				(enum_flags<FlagsEnum> flags) { return policy_type::operator_or(m_flags, (u32)flags); }
	u32						unset			(enum_flags<FlagsEnum> flags) { return policy_type::operator_and(m_flags, ~(u32)flags); }
	void					set				(enum_flags<FlagsEnum> flags, bool value) { if ( value ) set(flags); else unset(flags); }

	void					operator	|=	(enum_flags<FlagsEnum> flags) { set(flags); }
	void					operator	&=	(enum_flags<FlagsEnum> flags) { policy_type::operator_and(m_flags, (u32)flags); }
	bool					operator	&	(enum_flags<FlagsEnum> flags) const	{ return has(flags); }

private:
	typename policy_type::u32_type	m_flags;
};

} // namespace xray

#endif // /#ifndef XRAY_TYPE_ENUM_FLAGS_H_INCLUDED