////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_BASE_INLINE_H_INCLUDED
#define XRAY_INTRUSIVE_BASE_INLINE_H_INCLUDED

inline xray::intrusive_base::intrusive_base							( ) :
	m_reference_count	( 0 )
{
}

template <typename T>
inline void xray::intrusive_base::destroy							( T* object ) const
{
	MT_DELETE			( object );
}

inline xray::intrusive_interlocked_base::intrusive_interlocked_base	( ) :
	m_reference_count	( 0 )
{
}

template <typename T>
inline void xray::intrusive_interlocked_base::destroy				( T* object ) const
{
	MT_DELETE			( object );
}

#endif // #ifndef XRAY_INTRUSIVE_BASE_INLINE_H_INCLUDED