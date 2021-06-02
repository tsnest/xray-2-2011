////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.03.2009
//	Author		: Dmitriy Iassenev, Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_LIST_H_INCLUDED
#define XRAY_INTRUSIVE_LIST_H_INCLUDED

#include <xray/threading_policies.h>
#include <xray/size_policies.h>
#include <xray/debug_policies.h>

namespace xray {

template <	typename BaseWithMember, 
			typename PointerType, 
			PointerType BaseWithMember::*MemberNext, 
			typename ThreadingPolicy	=	threading::mutex, 
			typename SizePolicy			=	size_policy,
			typename DebugPolicy		=	debug_policy>
class intrusive_list : public SizePolicy, public ThreadingPolicy, public DebugPolicy {
public:
	typedef				BaseWithMember		base_with_member_type;
	typedef				PointerType			pointer_type;

public:
	inline				intrusive_list		();
	inline				intrusive_list		(intrusive_list const& other);

	inline	void		swap				(intrusive_list& other);
	inline	void		clear				();

	inline	void		push_back			(PointerType object, bool * out_pushed_first = NULL);
	inline	void		push_front			(PointerType object, bool * out_pushed_first = NULL);

	inline	bool		push_back_unique	(PointerType object, bool * out_pushed_first = NULL);
	inline	bool		push_front_unique	(PointerType object, bool * out_pushed_first = NULL);

	inline	PointerType	pop_front			();
	inline	PointerType	pop_front			(bool * out_popped_last);
	inline	PointerType	pop_all_and_clear	( u32 * out_size = NULL);
	inline	PointerType	front				() const;
	inline	PointerType	back				() const;
	inline	bool		empty				() const;
	inline	bool		contains_one_element() const { return m_first && m_first == m_last; }

	inline	bool		erase				(PointerType object);

	template <class Predicate>
	inline	void		for_each			(Predicate const & pred) const;

	template <class Predicate>
	inline	void		for_each			(Predicate & pred) const;
	
	template <class Predicate>
	inline	void		for_each			(Predicate & pred);

	template <class Predicate>
	inline	PointerType	remove_if			(Predicate const & pred);

	template <class Predicate>
	inline	PointerType	remove_if			(Predicate & pred);
	
	template <class Predicate>
	inline	PointerType	find_if				(Predicate const & pred) const;
	
	template <class Predicate>
	inline	PointerType	find_if				(Predicate & pred);

	static  PointerType	get_next_of_object	(PointerType const object) { return (* object).*MemberNext; }
	static	void		set_next_of_object	(PointerType object, PointerType next) { const_cast< typename boost::remove_cv<PointerType>::type &>( (* object).*MemberNext ) = next; }

	inline	bool		contains_object		(PointerType object);
private:	
	inline	bool		contains_object_thread_unsafe	(PointerType object);

	template <class Predicate>
	struct void_predicate_ref : private boost::noncopyable
	{
			void_predicate_ref	(Predicate & pred) : m_predicate_ref(pred) {}
	void	operator()		(PointerType arg) const { m_predicate_ref(arg); }
	private:
		Predicate &	m_predicate_ref;
	}; // struct predicate_ref

	template <class Predicate>
	struct bool_predicate_ref : private boost::noncopyable
	{
			bool_predicate_ref	(Predicate & pred) : m_predicate_ref(pred) {}
	bool	operator()			(PointerType arg) const { return m_predicate_ref(& * arg); }
	private:
		Predicate &	m_predicate_ref;
	}; // struct predicate_ref

	PointerType 		m_first;
	PointerType			m_last;
};

} // namespace xray

template <typename BaseWithMember, typename PointerType, PointerType BaseWithMember::*MemberNext, typename ThreadingPolicy>
inline	void			swap			(xray::intrusive_list<BaseWithMember, PointerType, MemberNext, ThreadingPolicy>& left, 
										 xray::intrusive_list<BaseWithMember, PointerType, MemberNext, ThreadingPolicy>& right);

inline	void			swap			(xray::size_policy & left, xray::size_policy & right);
inline	void			swap			(xray::no_size_policy & left, xray::no_size_policy & right);

#include <xray/intrusive_list_inline.h>

#endif // #ifndef XRAY_INTRUSIVE_LIST_H_INCLUDED