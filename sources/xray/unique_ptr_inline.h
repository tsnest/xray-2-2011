////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename T >
inline unique_ptr< T >::unique_ptr		( T* const object ) :
	m_object		( object )
{
}

template < typename T >
inline unique_ptr< T >::~unique_ptr		( )
{
	XRAY_DELETE_IMPL( USER_ALLOCATOR, m_object );
}

template < typename T >
inline T& unique_ptr< T >::operator *	( ) const
{
	R_ASSERT		( m_object );
	return			*m_object;
}

template < typename T >
inline T* unique_ptr< T >::operator ->	( ) const
{
	R_ASSERT		( m_object );
	return			m_object;
}

template < typename T >
inline bool unique_ptr< T >::operator !	( ) const
{
	return			!m_object;
}

template < typename T >
inline unique_ptr< T >::operator typename unique_ptr< T >::unspecified_bool_type ( ) const
{
	if ( !m_object )
		return		0;

	return			&unique_ptr< T >::c_ptr;
}

template < typename T >
inline T* unique_ptr< T >::c_ptr		( ) const
{
	return			m_object;
}


template < typename T1,  typename T2 >
inline bool operator ==					( unique_ptr< T1 > const left, unique_ptr< T2 > const right )
{
	return			left.c_ptr() == right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator !=					( unique_ptr< T1 > const left, unique_ptr< T2 > const right )
{
	return			left.c_ptr() != right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator <					( unique_ptr< T1 > const left, unique_ptr< T2 > const right )
{
	return			left.c_ptr() < right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator <=					( unique_ptr< T1 > const left, unique_ptr< T2 > const right )
{
	return			left.c_ptr() <= right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator >					( unique_ptr< T1 > const left, unique_ptr< T2 > const right )
{
	return			left.c_ptr() > right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator >=					( unique_ptr< T1 > const left, unique_ptr< T2 > const right )
{
	return			left.c_ptr() >= right.c_ptr();
}


template < typename T1,  typename T2 >
inline bool operator ==					( unique_ptr< T1 > const left, T2* const right )
{
	return			left.c_ptr() == right;
}

template < typename T1,  typename T2 >
inline bool operator !=					( unique_ptr< T1 > const left, T2* const right )
{
	return			left.c_ptr() != right;
}

template < typename T1,  typename T2 >
inline bool operator <					( unique_ptr< T1 > const left, T2* const right )
{
	return			left.c_ptr() < right;
}

template < typename T1,  typename T2 >
inline bool operator <=					( unique_ptr< T1 > const left, T2* const right )
{
	return			left.c_ptr() <= right;
}

template < typename T1,  typename T2 >
inline bool operator >					( unique_ptr< T1 > const left, T2* const right )
{
	return			left.c_ptr() > right;
}

template < typename T1,  typename T2 >
inline bool operator >=					( unique_ptr< T1 > const left, T2* const right )
{
	return			left.c_ptr() >= right;
}


template < typename T1,  typename T2 >
inline bool operator ==					( T1* const left, unique_ptr< T2 > const right )
{
	return			left == right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator !=					( T1* const left, unique_ptr< T2 > const right )
{
	return			left != right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator <					( T1* const left, unique_ptr< T2 > const right )
{
	return			left < right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator <=					( T1* const left, unique_ptr< T2 > const right )
{
	return			left <= right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator >					( T1* const left, unique_ptr< T2 > const right )
{
	return			left > right.c_ptr();
}

template < typename T1,  typename T2 >
inline bool operator >=					( T1* const left, unique_ptr< T2 > const right )
{
	return			left >= right.c_ptr();
}