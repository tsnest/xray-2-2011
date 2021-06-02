////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOOSE_PTR_BASE_INLINE_H_INCLUDED
#define XRAY_LOOSE_PTR_BASE_INLINE_H_INCLUDED

#include <xray/debug/call_stack.h>

inline xray::loose_ptr_base::loose_ptr_base		( )
{
	m_pointer								= MT_NEW( loose_ptr_data ) ( this );
	++m_pointer->m_reference_count;
}

inline xray::loose_ptr_base::~loose_ptr_base	( )
{
	ASSERT									( m_pointer );
	if ( m_pointer->m_reference_count > 1 ) {
		LOGI_WARNING						( "debug", "dangling pointer found(%d): 0x%08x", m_pointer->m_reference_count - 1, this );
		xray::debug::dump_call_stack		( "debug", true, 2, 0, 0 );
	}

	--m_pointer->m_reference_count;
	if ( !m_pointer->m_reference_count )
	{
		MT_DELETE							( m_pointer );
	}
	else
	{
		m_pointer->m_pointer				= 0;
	}
}

#endif // #ifndef XRAY_LOOSE_PTR_BASE_INLINE_H_INCLUDED