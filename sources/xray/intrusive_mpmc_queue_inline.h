////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INTRUSIVE_MPMC_QUEUE_INLINE_H_INCLUDED
#define INTRUSIVE_MPMC_QUEUE_INLINE_H_INCLUDED

//namespace xray {
//
//template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
//inline void intrusive_mpmc_queue<T,BaseWithMember,MemberNext>::push_back		( T* const value )
//{
//	R_ASSERT_CMP			( m_head, !=, 0, "do not call push_back after pop_null_node called" );
//	R_ASSERT_CMP			( m_tail, !=, 0, "unexpected situation" );
//	R_ASSERT_CMP			( m_push_thread_id, ==, (long)threading::current_thread_id() );
//
//	value->*MemberNext		= 0;
//	while ( threading::interlocked_compare_exchange_pointer( (pvoid&)(m_head->*MemberNext), value, 0) ) ;
//	threading::interlocked_exchange_pointer	( m_head, value );
//}
//
//template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
//inline T* intrusive_mpmc_queue<T,BaseWithMember,MemberNext>::pop_front	( T*& item_to_delete )
//{
//	R_ASSERT_CMP			( m_head, !=, 0, "do not call pop_front after pop_null_node called" );
//	R_ASSERT_CMP			( m_tail, !=, 0, "unexpected situation" );
//	R_ASSERT_CMP			( m_pop_thread_id, ==, (long)threading::current_thread_id() );
//	do {
//		item_to_delete		= m_tail;
//		T* const value		= item_to_delete->*MemberNext;
//		if ( !value )
//			return			0;
//	} while ( threading::interlocked_compare_exchange_pointer( m_tail, item_to_delete->*MemberNext, item_to_delete) != item_to_delete );
//	return					item_to_delete->*MemberNext;
//}
//
//} // namespace xray

#endif // #ifndef INTRUSIVE_MPMC_QUEUE_INLINE_H_INCLUDED