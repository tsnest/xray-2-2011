////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LIMITED_INTRUSIVE_LIST_H_INCLUDED
#define XRAY_LIMITED_INTRUSIVE_LIST_H_INCLUDED

namespace xray {

template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
class limited_intrusive_list : private boost::noncopyable 
{
private:
	typedef limited_intrusive_list< T, BaseWithMember, MemberNext >		self_type;

public:
	inline				limited_intrusive_list	( );

	inline	void		swap					( self_type& other );
	inline	void		clear					( );

	inline	bool		push_back				( T* object );
	inline	T*			pop_front				( T*& to_delete );
	inline	T*			pop_all_and_clear		( );
	inline	bool		empty					( ) const;

	inline	void		set_push_thread_id		( ); // sets current thread id as push thread id
	inline	void		set_pop_thread_id		( ); // sets current thread id as pop thread id

	inline void push_null_node	( T* const null_node )
	{
		m_null			= null_node;
	}

	inline	T*	pop_null_node	( )
	{
		return			m_null;
	}

private:

#ifndef MASTER_GOLD
	u32					m_push_thread_id;
	u32					m_pop_thread_id;
#endif // #ifndef MASTER_GOLD

	threading::mutex	m_mutex;
	T* volatile			m_first;
	T*					m_last;
	T*					m_null;
};

} // namespace xray

template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
inline	void			swap			( xray::limited_intrusive_list< T, BaseWithMember, MemberNext >& left, 
										  xray::limited_intrusive_list< T, BaseWithMember, MemberNext >& right );

#include <xray/limited_intrusive_list_inline.h>

#endif // #ifndef XRAY_LIMITED_INTRUSIVE_LIST_H_INCLUDED