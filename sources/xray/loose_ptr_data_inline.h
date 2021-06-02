////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOOSE_PTR_DATA_INLINE_H_INCLUDED
#define XRAY_LOOSE_PTR_DATA_INLINE_H_INCLUDED

inline xray::loose_ptr_data::loose_ptr_data	( loose_ptr_base* const pointer ) :
	m_pointer								( pointer ),
	m_reference_count						( 0 )
{
	ASSERT		( pointer );
}

template < typename T >
inline void xray::loose_ptr_data::destroy	( T* object ) const
{
	MT_DELETE	( object );
}

#endif // #ifndef XRAY_LOOSE_PTR_DATA_INLINE_H_INCLUDED