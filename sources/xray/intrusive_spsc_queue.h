////////////////////////////////////////////////////////////////////////////
//	Created		: 14.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INTRUSIVE_SPSC_QUEUE_H_INCLUDED
#define INTRUSIVE_SPSC_QUEUE_H_INCLUDED

namespace xray {

template < typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext >
class XRAY_CORE_API intrusive_spsc_queue {
public:
	typedef T			value_type;
	typedef value_type*	pointer_type;

public:
	inline			intrusive_spsc_queue	( );
	inline			~intrusive_spsc_queue	( );

	inline	void	push_back				( T* const value );
	inline	T*		pop_front				( T*& item_to_delete );
	inline	bool	empty					( ) const;

	inline	void	set_push_thread_id		( );
	inline	void	push_null_node			( T* const null_node );

	inline	void	set_pop_thread_id		( );
	inline	T*		pop_null_node			( );

	inline	T*		null_node				( ) const;
	inline	void	clear_push_thread_id	( ) { m_push_thread_id = threading::atomic32_value_type(-1); }

	inline	threading::atomic32_type	push_thread_id	( ) const { return m_push_thread_id; }
	inline	threading::atomic32_type	pop_thread_id	( ) const { return m_pop_thread_id; }

private:
	T*							m_head;
	threading::atomic32_type	m_push_thread_id;
	threading::atomic32_type	m_pop_thread_id;
	char						m_cache_line_pad[XRAY_MAX_CACHE_LINE_SIZE - 1*sizeof(T*) -2*sizeof(threading::atomic32_type)];
	T*							m_tail;
}; // class intrusive_spsc_queue

} // namespace xray

#include <xray/intrusive_spsc_queue_inline.h>

#endif // #ifndef INTRUSIVE_SPSC_QUEUE_H_INCLUDED