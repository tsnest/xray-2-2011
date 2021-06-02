////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INTRUSIVE_SPMC_QUEUE_H_INCLUDED
#define INTRUSIVE_SPMC_QUEUE_H_INCLUDED

//#include <xray/intrusive_spsc_queue.h>
//
//namespace xray {
//
//template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
//class intrusive_mpsc_queue : private intrusive_spsc_queue<T,BaseWithMember,MemberNext> {
//public:
//	inline			intrusive_mpsc_queue	( ) { }
//	inline			~intrusive_mpsc_queue	( ) { }
//	inline	T*		pop_front				( T*& item_to_delete );
//	
//private:
//	typedef intrusive_mpsc_queue<T,BaseWithMember,MemberNext>	super;
//
//public:
//	using super::push_back;
//	using super::empty;
//	using super::set_push_thread_id;
//	using super::push_null_node;
//	using super::set_pop_thread_id;
//	using super::pop_null_node;
//}; // class intrusive_spsc_queue
//
//} // namespace xray
//
//#include <xray/intrusive_spmc_queue_inline.h>

#endif // #ifndef INTRUSIVE_SPMC_QUEUE_H_INCLUDED