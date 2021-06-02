////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_LIST_INLINE_H_INCLUDED
#define XRAY_INTRUSIVE_LIST_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template <typename BaseWithMember, typename PointerType, PointerType BaseWithMember::*MemberNext, typename ThreadingPolicy, typename SizePolicy, typename DebugPolicy>
#define INTRUSIVE_LIST		xray::intrusive_list<BaseWithMember, PointerType, MemberNext, ThreadingPolicy, SizePolicy, DebugPolicy>

TEMPLATE_SIGNATURE
inline INTRUSIVE_LIST::intrusive_list	() 
	: m_first(NULL), m_last(NULL)
{
}

TEMPLATE_SIGNATURE
inline INTRUSIVE_LIST::intrusive_list	(intrusive_list const& other)
	: m_first(NULL), m_last(NULL)
{
	ASSERT_U								(other.empty());
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_LIST::swap		(intrusive_list & other)
{
	ThreadingPolicy::lock					();
	other.lock								();
	
	std::swap								(m_first, other.m_first);
	std::swap								(m_last,  other.m_last);
	std::swap								((SizePolicy &) * this, (SizePolicy &) other);

	other.unlock							();
	ThreadingPolicy::unlock					();
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_LIST::clear		()
{
	typename ThreadingPolicy::mutex_raii	raii(*this);
	m_first								=	 NULL;
	m_last								=	 NULL;
	SizePolicy::set_zero_size				();
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_LIST::push_back	(PointerType const object, bool * out_pushed_first)
{
	set_next_of_object						(object, NULL);

	typename ThreadingPolicy::mutex_raii	raii(*this);

	R_ASSERT								(!debug_enabled || !contains_object_thread_unsafe(object));

	SizePolicy::increment_size				();

	if ( out_pushed_first )
		* out_pushed_first				=	!m_first;

	if ( !m_first ) 
	{
		m_first							=	object;
		m_last							=	object;
		return;
	}

	set_next_of_object						 (m_last, object);
	m_last								=	object;
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_LIST::push_front	(PointerType const object, bool * out_pushed_first)
{
	typename ThreadingPolicy::mutex_raii	raii	(*this);

	R_ASSERT								(!debug_enabled || !contains_object_thread_unsafe(object));

	SizePolicy::increment_size				();

	if ( out_pushed_first )
		* out_pushed_first				=	!m_first;

	if ( !m_first ) 
	{
		set_next_of_object					(object, NULL);
		m_first							=	 object;
		m_last							=	 object;
		return;
	}

	set_next_of_object						 (object, m_first);
	m_first								=	object;
}

TEMPLATE_SIGNATURE
inline PointerType INTRUSIVE_LIST::pop_front	(bool * out_popped_last)
{
	if ( empty() )
	{
		if ( out_popped_last )
			* out_popped_last			=	false;
		return								NULL;
	}

	typename ThreadingPolicy::mutex_raii	raii(*this);

	if ( m_first )
	{
		SizePolicy::decrement_size			();
		PointerType result				=	m_first;
		m_first							=	get_next_of_object(m_first);
		if ( !m_first )
		{
			m_last						=	NULL;
			if ( out_popped_last )
				* out_popped_last		=	true;
		}
		else if ( out_popped_last )
				* out_popped_last		=	false;

		set_next_of_object					(result, NULL);
		return								result;
	}
	else
	{
		if ( out_popped_last )
			* out_popped_last			=	false;
	}

	return									NULL;
}

TEMPLATE_SIGNATURE
inline PointerType INTRUSIVE_LIST::pop_front	()
{
	if ( empty() )
		return								NULL;

	typename ThreadingPolicy::mutex_raii	raii	(*this);

	if ( m_first )
	{
		SizePolicy::decrement_size			();
		PointerType result				=	 m_first;
		m_first							=	 get_next_of_object(m_first);
		if ( !m_first )
			m_last						=	NULL;
		set_next_of_object					(result, NULL);
		return								result;
	}

	return									NULL;
}

TEMPLATE_SIGNATURE
inline PointerType INTRUSIVE_LIST::pop_all_and_clear	(u32 * out_size)
{
	if ( empty() )
	{
		if ( out_size )
			* out_size					=	0;
		return								NULL;
	}

	typename ThreadingPolicy::mutex_raii	raii(*this);

	PointerType result					=	 m_first;
	m_first								=	 NULL;
	m_last								=	 NULL;
	if ( out_size )
		* out_size						=	 this->size	();
	SizePolicy::set_zero_size				();

	return									result;
}

TEMPLATE_SIGNATURE
inline	PointerType INTRUSIVE_LIST::front	() const
{
	return									m_first;
}

TEMPLATE_SIGNATURE
inline	PointerType INTRUSIVE_LIST::back	() const
{
	return									m_last;
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_LIST::empty		() const
{
	return									!m_first;
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_LIST::erase		(PointerType object)
{
	if ( empty() )
		return								false;

	typename ThreadingPolicy::mutex_raii	raii(*this);

	PointerType previous_i				=	0;
	PointerType i						=	m_first;
	for ( ; i && (i != object); previous_i = i, i = get_next_of_object(i) );

	if ( i != object )
		return			false;

	SizePolicy::decrement_size();

	if ( previous_i )
		set_next_of_object					(previous_i, get_next_of_object(i));
	else
		m_first							=	get_next_of_object(i);

	if ( !get_next_of_object(i) )
		m_last							=	previous_i ? previous_i : m_first;
	return				true;
}

TEMPLATE_SIGNATURE
template <class Predicate>
PointerType   INTRUSIVE_LIST::find_if	(Predicate const & pred) const
{
	if ( empty() )
		return								NULL;

	typename ThreadingPolicy::mutex_raii	raii(*this);
	for ( PointerType	current	=	m_first;
						current;	)
	{
		PointerType const next			=	get_next_of_object(current);
		if ( pred(& * current) )
			return							current;
		current							=	next;
	}

	return									NULL;
}

TEMPLATE_SIGNATURE
template <class Predicate>
PointerType   INTRUSIVE_LIST::find_if	(Predicate & pred)
{
	return									find_if(bool_predicate_ref<Predicate>(pred));
}

TEMPLATE_SIGNATURE
template <class Predicate>
PointerType   INTRUSIVE_LIST::remove_if	(Predicate const & predicate)
{
	if ( empty() )
		return								NULL;

	typename ThreadingPolicy::mutex_raii	raii(*this);

	PointerType	current					=	m_first;
	PointerType previous				=	NULL;
	PointerType result					=	NULL;
	while ( current )
	{
		PointerType const next			=	get_next_of_object(current);
		if ( !predicate(& * current) ) 
		{
			previous					=	current;
			current						=	next;
			continue;
		}

		if ( !result )
			result						=	current;

		SizePolicy::decrement_size	( );

		if ( previous )
			set_next_of_object				(previous, next);
		else
			m_first						=	next;

		current							=	next;
	}

	m_last								=	previous;
	return									result;
}

TEMPLATE_SIGNATURE
template <class Predicate>
PointerType   INTRUSIVE_LIST::remove_if (Predicate & predicate)
{
	return									remove_if(bool_predicate_ref<Predicate>(predicate));
}

TEMPLATE_SIGNATURE
template <class Predicate>
void   INTRUSIVE_LIST::for_each			(Predicate const & pred) const
{
	if ( empty() )
		return;

	typename ThreadingPolicy::mutex_raii	raii(*this);
	for ( PointerType	current	=	m_first;
						current;				)
	{
		PointerType const next			=	get_next_of_object(current);
		pred								(& * current);
		current							=	next;
	}
}

TEMPLATE_SIGNATURE
template <class Predicate>
void   INTRUSIVE_LIST::for_each			(Predicate & pred) const
{
	for_each								(void_predicate_ref<Predicate>(pred));
}

TEMPLATE_SIGNATURE
template <class Predicate>
void   INTRUSIVE_LIST::for_each			(Predicate & pred)
{
	for_each								(void_predicate_ref<Predicate>(pred));
}

TEMPLATE_SIGNATURE
bool   INTRUSIVE_LIST::contains_object	(PointerType object)
{
	if ( empty() )
		return								false;

	typename ThreadingPolicy::mutex_raii	raii(*this);
	return									contains_object_thread_unsafe(object);
}

TEMPLATE_SIGNATURE
bool   INTRUSIVE_LIST::contains_object_thread_unsafe	(PointerType object)
{
	for ( BaseWithMember *	current	=	m_first ? (& * m_first) : NULL;
							current;
							current	=	current->*MemberNext ? & * (current->*MemberNext) : NULL )
	{
		if ( current == & * object )
			return							true;
	}

	return									false;
}

TEMPLATE_SIGNATURE
bool   INTRUSIVE_LIST::push_back_unique	(PointerType object, bool * out_pushed_first)
{
	typename ThreadingPolicy::mutex_raii		raii(*this);
	if ( !contains_object_thread_unsafe(object) )
	{
		push_back							(object, out_pushed_first);
		return								true;
	}

	return									false;
}

TEMPLATE_SIGNATURE
bool   INTRUSIVE_LIST::push_front_unique	(PointerType object, bool * out_pushed_first)
{
	typename ThreadingPolicy::mutex_raii	raii(*this);
	if ( !contains_object_thread_unsafe(object) )
	{
		push_front							(object, out_pushed_first);
		return								true;
	}

	return									false;
}

TEMPLATE_SIGNATURE
inline void   swap						(INTRUSIVE_LIST& left, INTRUSIVE_LIST& right)
{
	left.swap								(right);
}

#undef INTRUSIVE_LIST
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_INTRUSIVE_LIST_INLINE_H_INCLUDED