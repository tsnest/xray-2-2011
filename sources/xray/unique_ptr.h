////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename T >
class unique_ptr : private boost::noncopyable {
public:
	typedef T* ( unique_ptr< T >::*unspecified_bool_type )	( ) const;

public:
	inline	explicit	unique_ptr			( T* object );
	inline				~unique_ptr			( );
	inline	T&			operator *			( ) const;
	inline	T*			operator ->			( ) const;
	inline	bool		operator !			( ) const;
	inline	operator unspecified_bool_type	( ) const;
	inline	T*			c_ptr				( ) const;

private:
	T*		m_object;
}; // class unique_ptr

template < typename T1,  typename T2 >
inline	bool			operator ==	( unique_ptr< T1 > const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator !=	( unique_ptr< T1 > const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator <	( unique_ptr< T1 > const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator <=	( unique_ptr< T1 > const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator >	( unique_ptr< T1 > const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator >=	( unique_ptr< T1 > const left, unique_ptr< T2 > const right );

template < typename T1,  typename T2 >
inline	bool			operator ==	( unique_ptr< T1 > const left, T2* const right );
template < typename T1,  typename T2 >
inline	bool			operator !=	( unique_ptr< T1 > const left, T2* const right );
template < typename T1,  typename T2 >
inline	bool			operator <	( unique_ptr< T1 > const left, T2* const right );
template < typename T1,  typename T2 >
inline	bool			operator <=	( unique_ptr< T1 > const left, T2* const right );
template < typename T1,  typename T2 >
inline	bool			operator >	( unique_ptr< T1 > const left, T2* const right );
template < typename T1,  typename T2 >
inline	bool			operator >=	( unique_ptr< T1 > const left, T2* const right );

template < typename T1,  typename T2 >
inline	bool			operator ==	( T1* const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator !=	( T1* const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator <	( T1* const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator <=	( T1* const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator >	( T1* const left, unique_ptr< T2 > const right );
template < typename T1,  typename T2 >
inline	bool			operator >=	( T1* const left, unique_ptr< T2 > const right );

#include <xray/unique_ptr_inline.h>