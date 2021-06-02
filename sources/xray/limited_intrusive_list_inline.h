////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev, Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LIMITED_INTRUSIVE_LIST_INLINE_H_INCLUDED
#define XRAY_LIMITED_INTRUSIVE_LIST_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE		template <typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
#define LIMITED_INTRUSIVE_LIST	xray::limited_intrusive_list<T, BaseWithMember, MemberNext>

TEMPLATE_SIGNATURE
inline LIMITED_INTRUSIVE_LIST::limited_intrusive_list	( ) :
#ifndef MASTER_GOLD
	m_push_thread_id( u32(-1) ),
	m_pop_thread_id	( u32(-1) ),
#endif // #ifndef MASTER_GOLD
	m_first				( 0 ),
	m_last				( 0 )
{
}

TEMPLATE_SIGNATURE
inline void LIMITED_INTRUSIVE_LIST::swap				( self_type& other )
{
	m_mutex.lock		( );
	other.m_mutex.lock	( );
	
	std::swap			( m_first, other.m_first );
	std::swap			( m_last,  other.m_last  );
	std::swap			( m_mutex, other.m_mutex );

	other.m_mutex.unlock( );
	m_mutex.unlock		( );
}

TEMPLATE_SIGNATURE
inline void LIMITED_INTRUSIVE_LIST::clear				( )
{
	R_ASSERT			  (threading::current_thread_id() == m_push_thread_id);
	threading::interlocked_exchange_pointer	(m_first, NULL);
}

TEMPLATE_SIGNATURE
inline bool LIMITED_INTRUSIVE_LIST::push_back			(  T* const object )
{
	R_ASSERT			  (threading::current_thread_id() == m_push_thread_id);

	object->*MemberNext	= 0;

	m_mutex.lock		( );
	if ( !m_first ) {
		m_mutex.unlock	( );
		m_first			= object;
		m_last			= object;
		return			true;
	}

	m_last->*MemberNext	= object;
	m_mutex.unlock		( );
	m_last				= object;
	return				false;
}

TEMPLATE_SIGNATURE
inline T* LIMITED_INTRUSIVE_LIST::pop_front				( T*& to_delete )
{
	R_ASSERT			(threading::current_thread_id() == m_pop_thread_id);

	threading::mutex_raii raii(m_mutex);

	if ( m_first )
	{
		T* result		= m_first;
		m_first			= m_first->*MemberNext;
		result->*MemberNext	=	NULL;
		to_delete		= result;
		return			result;
	}

	return				NULL;
}

TEMPLATE_SIGNATURE
inline T* LIMITED_INTRUSIVE_LIST::pop_all_and_clear	( )
{
	R_ASSERT			(threading::current_thread_id() == m_pop_thread_id);

	threading::mutex_raii raii(m_mutex);

	T* result			= m_first;
	m_first				= NULL;
	return				result;
}

TEMPLATE_SIGNATURE
inline bool LIMITED_INTRUSIVE_LIST::empty				( ) const
{
	return				( !m_first );
}

TEMPLATE_SIGNATURE
inline void LIMITED_INTRUSIVE_LIST::set_push_thread_id	( )
{
#ifndef MASTER_GOLD
	m_push_thread_id	= threading::current_thread_id( );
#endif // #ifndef MASTER_GOLD
}

TEMPLATE_SIGNATURE
inline void	LIMITED_INTRUSIVE_LIST::set_pop_thread_id	( )
{
#ifndef MASTER_GOLD
	m_pop_thread_id		= threading::current_thread_id( );
#endif // #ifndef MASTER_GOLD
}

TEMPLATE_SIGNATURE
inline void swap		( LIMITED_INTRUSIVE_LIST& left, LIMITED_INTRUSIVE_LIST& right )
{
	left.swap			( right );
}

#undef LIMITED_INTRUSIVE_LIST
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_LIMITED_INTRUSIVE_LIST_INLINE_H_INCLUDED