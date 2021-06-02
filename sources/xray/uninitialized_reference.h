////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_UNCONSTRUCTED_STATIC_H_INCLUDED
#define XRAY_UNCONSTRUCTED_STATIC_H_INCLUDED

#include <xray/debug/pointer_cast.h>

namespace xray {

template <class Class>
class uninitialized_reference
{
public:
	inline uninitialized_reference				() : m_initialized(0), m_construction_started(0), m_variable((Class &)m_static_memory) {}

	inline Class *			operator->			() 			{ R_ASSERT(m_initialized); return & m_variable; }
	inline Class &			operator *			() 			{ R_ASSERT(m_initialized); return m_variable; }
	inline Class const *	operator->			() const	{ R_ASSERT(m_initialized); return & m_variable; }
	inline Class const &	operator *			() const	{ R_ASSERT(m_initialized); return m_variable; }

	inline Class *			c_ptr				()			{ R_ASSERT(m_initialized); return & m_variable; }
	inline Class const *	c_ptr				() const	{ R_ASSERT(m_initialized); return & m_variable; }

	inline Class *			operator +			(Class *)	{ R_ASSERT(!m_initialized); threading::interlocked_exchange(m_initialized, 1); return & m_variable; }
	inline operator Class						()			{ return m_variable; }
	inline operator Class const &				() const	{ return m_variable; }

	inline bool				initialized			() const	{ return m_initialized != 0; }
	inline bool				should_initialize	() 
	{ 
		if ( threading::interlocked_exchange(m_construction_started, 1) == 1 )
		{
			while ( !m_initialized ) {;}
			return				false;
		}

		return					true;
	}
	
	inline Class *			construction_memory	() 
	{ 
		ASSERT					(!m_initialized); 
		return					::xray::pointer_cast<Class*>(m_static_memory);
	}

	inline void				destroy				() 
	{ 
		ASSERT					(m_initialized); 
		m_variable.~Class		(); 
		m_initialized		=	false;
	}

private:
	inline uninitialized_reference	( uninitialized_reference const& );
	inline void operator =		(uninitialized_reference const&);

private:
	char XRAY_DEFAULT_ALIGN		m_static_memory[ sizeof(Class) ];
	Class &						m_variable;
	threading::atomic32_type	m_initialized;
	threading::atomic32_type	m_construction_started;
};
					
} // namespace xray

#define XRAY_CONSTRUCT_REFERENCE(uninitialized_ref, Class) \
	uninitialized_ref + new ( uninitialized_ref.construction_memory() ) Class

#define XRAY_CONSTRUCT_REFERENCE_MT_SAFE(uninitialized_ref, Class)						\
	( uninitialized_ref.initialized() || !uninitialized_ref.should_initialize() )	?	\
		NULL : uninitialized_ref + new ( uninitialized_ref.construction_memory() ) Class

#define XRAY_CONSTRUCT_REFERENCE_BY_PREDICATE_MT_SAFE(uninitialized_ref, construction_predicate)	\
	( uninitialized_ref.initialized() || !uninitialized_ref.should_initialize() )	?				\
		NULL : uninitialized_ref + construction_predicate(uninitialized_ref.construction_memory())

#define XRAY_DESTROY_REFERENCE(uninitialized_ref)			\
	uninitialized_ref.destroy();

#endif // #ifndef XRAY_UNCONSTRUCTED_STATIC_H_INCLUDED