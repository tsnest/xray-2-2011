////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INTRUSIVE_MPMC_QUEUE_H_INCLUDED
#define INTRUSIVE_MPMC_QUEUE_H_INCLUDED

//#include <xray/intrusive_spsc_queue.h>
//
//namespace xray {
//
//template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
//class intrusive_mpmc_queue : private intrusive_spsc_queue<T,BaseWithMember,MemberNext> {
//public:
//	inline			intrusive_mpmc_queue	( ) { }
//	inline			~intrusive_mpmc_queue	( ) { }
//	inline	void	push_back				( T* const value );
//	inline	T*		pop_front				( T*& item_to_delete );
//	
//private:
//	typedef intrusive_mpsc_queue<T,BaseWithMember,MemberNext>	super;
//
//public:
//	using super::empty;
//	using super::set_push_thread_id;
//	using super::push_null_node;
//	using super::set_pop_thread_id;
//	using super::pop_null_node;
//}; // class intrusive_spsc_queue
//
//} // namespace xray
//
//#include <xray/intrusive_mpmc_queue_inline.h>

#endif // #ifndef INTRUSIVE_MPMC_QUEUE_H_INCLUDED