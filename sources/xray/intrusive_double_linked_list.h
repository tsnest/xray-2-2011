////////////////////////////////////////////////////////////////////////////
//	Created 	: 18.03.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_DOUBLE_LINKED_LIST_H_INCLUDED
#define XRAY_INTRUSIVE_DOUBLE_LINKED_LIST_H_INCLUDED

#include <xray/threading_policies.h>
#include <xray/size_policies.h>
#include <xray/debug_policies.h>

namespace xray {

template <	typename BaseWithMember,
			typename PointerType,
			PointerType BaseWithMember::*MemberPrev, 
			PointerType BaseWithMember::*MemberNext, 
			typename ThreadingPolicy	=	threading::mutex, 
			typename SizePolicy			=	no_size_policy,
			typename DebugPolicy		=	debug_policy
		 >
class XRAY_CORE_API intrusive_double_linked_list : public SizePolicy, public DebugPolicy {
public:
	typedef	PointerType									pointer_type;
	typedef	typename ThreadingPolicy::mutex_raii		mutex_raii;

public:
	inline				intrusive_double_linked_list	();
	inline	ThreadingPolicy& policy						() { return m_policy; }

	inline	void		swap							(intrusive_double_linked_list & other);
	inline	void		clear							();

	inline	void		push_back						(PointerType object, bool * out_pushed_first = NULL);
	inline	void		push_front						(PointerType object, bool * out_pushed_first = NULL);

	inline	PointerType	pop_back						(bool * out_popped_last = NULL);
	inline	PointerType	pop_front						(bool * out_popped_last = NULL);
	inline	PointerType	pop_all_and_clear				();
	inline	PointerType	pop_back_and_clear				();
	inline	void		erase							(PointerType object);
	inline	PointerType	front							() const;
	inline	PointerType	back							() const;
	inline	bool		empty							() const;

	template <class Predicate>
	inline	void		for_each						(Predicate const & pred);
	template <class Predicate>
	inline	void		for_each						(Predicate & pred);
	
	template <class Predicate>
	inline	PointerType	find_if							(Predicate const & pred) const;
	
	template <class Predicate>
	inline	PointerType	find_if							(Predicate & pred);

	static	PointerType	get_next_of_object				(PointerType object) { return (* object).*MemberNext; }
	static	PointerType	get_prev_of_object				(PointerType object) { return (* object).*MemberPrev; }
	static	void		set_next_of_object				(PointerType object, PointerType next) { (* object).*MemberNext	=	next; }
	static	void		set_prev_of_object				(PointerType object, PointerType prev) { (* object).*MemberPrev	=	prev; }

	inline	bool		contains_object					(PointerType object);

	ThreadingPolicy &	synchronization_primitive		() { return m_policy; }
private:	
	inline	bool		contains_object_thread_unsafe	(PointerType object);

	template <class Predicate>
	class predicate_ref : private boost::noncopyable
	{
	public:
			predicate_ref	(Predicate & pred) : m_predicate_ref(pred) {}
	void	operator ()		(PointerType arg) const { m_predicate_ref(arg); }
	private:
		Predicate &		m_predicate_ref;
	}; // class predicate_ref

	template <class Predicate>
	struct bool_predicate_ref : private boost::noncopyable
	{
			bool_predicate_ref	(Predicate & pred) : m_predicate_ref(pred) {}
	bool	operator()			(PointerType arg) const { return m_predicate_ref(& * arg); }
	private:
		Predicate &	m_predicate_ref;
	}; // struct predicate_ref

	PointerType			m_first;
	PointerType			m_last;
	ThreadingPolicy		m_policy;
};

} // namespace xray

template <	typename BaseWithMember, typename PointerType, PointerType BaseWithMember::*MemberPrev, PointerType BaseWithMember::*MemberNext, typename ThreadingPolicy, typename SizePolicy>
inline	void	swap	(xray::intrusive_double_linked_list<BaseWithMember, PointerType, MemberPrev, MemberNext, ThreadingPolicy, SizePolicy>& left, 
						 xray::intrusive_double_linked_list<BaseWithMember, PointerType, MemberPrev, MemberNext, ThreadingPolicy, SizePolicy>& right);

#include <xray/intrusive_double_linked_list_inline.h>

#endif // #ifndef XRAY_INTRUSIVE_DOUBLE_LINKED_LIST_H_INCLUDED