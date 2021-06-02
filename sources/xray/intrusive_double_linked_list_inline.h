////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Sergey Chechin, Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_DOUBLE_LINKED_LIST_INLINE_H_INCLUDED
#define XRAY_INTRUSIVE_DOUBLE_LINKED_LIST_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template <typename BaseWithMember, typename PointerType, PointerType BaseWithMember::*MemberPrev, PointerType BaseWithMember::*MemberNext, typename ThreadingPolicy, typename SizePolicy, typename DebugPolicy>
#define DOUBLE_LINKED_LIST	xray::intrusive_double_linked_list<BaseWithMember, PointerType, MemberPrev, MemberNext, ThreadingPolicy, SizePolicy, DebugPolicy>

TEMPLATE_SIGNATURE
inline DOUBLE_LINKED_LIST::intrusive_double_linked_list () :
	m_first								(NULL),
	m_last								(NULL)
{
}

TEMPLATE_SIGNATURE
bool   DOUBLE_LINKED_LIST::contains_object (PointerType object)
{
	if ( empty() )
		return								false;

	typename ThreadingPolicy::mutex_raii		raii(m_policy);
	return					contains_object_thread_unsafe(object);
}

TEMPLATE_SIGNATURE
bool   DOUBLE_LINKED_LIST::contains_object_thread_unsafe (PointerType object)
{
	for ( BaseWithMember *	current	=	m_first ? (& * m_first) : NULL;
							current;
							current	=	current->*MemberNext ? & * (current->*MemberNext) : NULL )
	{
		if ( current == & * object )
			return			true;
	}

	return					false;
}

TEMPLATE_SIGNATURE
inline void   DOUBLE_LINKED_LIST::swap (intrusive_double_linked_list & other)
{
	typename ThreadingPolicy::mutex_raii	raii(m_policy);
	typename ThreadingPolicy::mutex_raii	raii_other(other.m_policy);
	
	std::swap							(m_first, other.m_first);
	std::swap							(m_last,  other.m_last);
	std::swap							((SizePolicy &) * this, (SizePolicy &)other);
}

TEMPLATE_SIGNATURE
inline void   DOUBLE_LINKED_LIST::clear ()
{
	typename ThreadingPolicy::mutex_raii raii	(m_policy);
	m_first							=	NULL;
	m_last							=	NULL;
	SizePolicy::set_zero_size			();
}

TEMPLATE_SIGNATURE
inline void   DOUBLE_LINKED_LIST::push_back (PointerType const object, bool * out_pushed_first)
{
	typename ThreadingPolicy::mutex_raii raii	(m_policy);

	ASSERT ( !debug_enabled || !contains_object_thread_unsafe(object) );

	SizePolicy::increment_size			();

	set_next_of_object					(object, NULL);

	if ( out_pushed_first )
		* out_pushed_first			=	!m_first;

	if ( !m_first ) 
	{
		set_prev_of_object				(object, NULL);
		m_first						=	object;
		m_last						=	object;
		return;
	}

	set_prev_of_object					(object, m_last);
	set_next_of_object					(m_last, object);
	m_last							= 	object;
}

TEMPLATE_SIGNATURE
inline void   DOUBLE_LINKED_LIST::push_front (PointerType const object, bool * out_pushed_first)
{
	typename ThreadingPolicy::mutex_raii raii	(m_policy);

	R_ASSERT ( !debug_enabled || !contains_object_thread_unsafe(object) );

	SizePolicy::increment_size			();

	set_prev_of_object					(object, NULL);

	if ( out_pushed_first )
		* out_pushed_first			=	!m_first;

	if ( !m_first ) 
	{
		set_next_of_object				(object, NULL);
		m_first						= 	object;
		m_last						= 	object;
		return;
	}

	set_next_of_object					(object, m_first);
	set_prev_of_object					(m_first, object);
	m_first							=	object;
}

TEMPLATE_SIGNATURE
inline PointerType   DOUBLE_LINKED_LIST::pop_front (bool * out_popped_last)
{
	if ( empty() )
	{
		if ( out_popped_last )
			* out_popped_last		=	false;
		return							NULL;
	}

	typename ThreadingPolicy::mutex_raii raii	(m_policy);

	if ( m_first )
	{
		SizePolicy::decrement_size		();
		PointerType result					= 	m_first;
		m_first						= 	get_next_of_object(m_first);
		if ( m_first )
		{
			set_prev_of_object			(m_first, NULL);
			if ( out_popped_last )
				* out_popped_last	=	false;
		}
		else
		{
			m_last					=	NULL;
			if ( out_popped_last )
				* out_popped_last	=	true;
		}

		set_next_of_object				(result, NULL);
		set_prev_of_object				(result, NULL);
		return							result;
	}
	else
	{
		if ( out_popped_last )
			* out_popped_last		=	false;
	}

	return								NULL;
}

TEMPLATE_SIGNATURE
inline PointerType   DOUBLE_LINKED_LIST::pop_back (bool * out_popped_last)
{
	if ( empty() )
	{
		if ( out_popped_last )
			* out_popped_last		=	false;
		return							NULL;
	}

	typename ThreadingPolicy::mutex_raii raii	(m_policy);

	if ( m_first )
	{
		SizePolicy::decrement_size		();
		PointerType const result				= 	m_last;
		m_last						= 	get_prev_of_object(m_last);
		if ( m_last )
		{
			set_next_of_object			(m_last, NULL);
			if ( out_popped_last )
				* out_popped_last	=	false;
		}
		else
		{
			m_first					=	NULL;
			if ( out_popped_last )
				* out_popped_last	=	true;
		}

		set_next_of_object				(result, NULL);
		set_prev_of_object				(result, NULL);
		return							result;
	}
	else
	{
		if ( out_popped_last )
			* out_popped_last		=	false;
	}

	return								NULL;
}

TEMPLATE_SIGNATURE
inline PointerType   DOUBLE_LINKED_LIST::pop_all_and_clear	()
{
	if ( empty() )
		return								NULL;

	typename ThreadingPolicy::mutex_raii raii	(m_policy);
	PointerType result						= 	m_first;
	m_first							= 	NULL;
	m_last							=	NULL;
	SizePolicy::set_zero_size			();
	return								result;
}

TEMPLATE_SIGNATURE
inline PointerType   DOUBLE_LINKED_LIST::pop_back_and_clear	()
{
	if ( empty() )
		return								NULL;

	typename ThreadingPolicy::mutex_raii raii	(m_policy);
	PointerType result						= 	m_last;
	m_first							=	NULL;
	m_last							= 	NULL;
	SizePolicy::set_zero_size			();
	return								result;
}

TEMPLATE_SIGNATURE
inline	PointerType   DOUBLE_LINKED_LIST::front () const
{
	return								m_first;
}

TEMPLATE_SIGNATURE
inline	PointerType   DOUBLE_LINKED_LIST::back () const
{
	return								m_last;
}

TEMPLATE_SIGNATURE
inline void	  DOUBLE_LINKED_LIST::erase	(PointerType object)
{
	if ( empty() )
		return;

	typename ThreadingPolicy::mutex_raii raii	(m_policy);

	PointerType const prev					=	get_prev_of_object(object);
	PointerType const next					=	get_next_of_object(object);

	set_prev_of_object					(object, NULL);
	set_next_of_object					(object, NULL);

	if ( prev )
	{
		set_next_of_object				(prev, next);
	}
	else 
	{
		ASSERT							(m_first == object);
		m_first						=	next;
	}

	if ( next )
	{
		set_prev_of_object				(next, prev);
	}
	else
	{
		ASSERT							(m_last == object);
		m_last						=	prev;
	}

	set_prev_of_object					(object, NULL);
	set_next_of_object					(object, NULL);

	SizePolicy::decrement_size			();
}	

TEMPLATE_SIGNATURE
inline bool   DOUBLE_LINKED_LIST::empty	() const
{
	return								!m_first;
}

TEMPLATE_SIGNATURE
template <class Predicate>
void   DOUBLE_LINKED_LIST::for_each (Predicate const & pred)
{
	if ( empty() )
		return;

	typename ThreadingPolicy::mutex_raii raii(m_policy);
	for ( PointerType current = m_first; current; )
	{
		PointerType const next	=	get_next_of_object(current);
		pred				(& * current);
		current			=	next;
	}
}

TEMPLATE_SIGNATURE
template <class Predicate>
void   DOUBLE_LINKED_LIST::for_each (Predicate & pred)
{
	for_each(predicate_ref<Predicate>(pred));
}

TEMPLATE_SIGNATURE
template <class Predicate>
PointerType   DOUBLE_LINKED_LIST::find_if (Predicate const & pred) const
{
	if ( empty() )
		return			NULL;

	typename ThreadingPolicy::mutex_raii	raii(m_policy);
	for ( PointerType	current	=	m_first;
						current;				)
	{
		PointerType const next	=	get_next_of_object(current);
		if ( pred(& * current) )
			return			current;
		current			=	next;
	}

	return					NULL;
}

TEMPLATE_SIGNATURE
template <class Predicate>
PointerType   DOUBLE_LINKED_LIST::find_if (Predicate & pred)
{
	return					find_if(bool_predicate_ref<Predicate>(pred));
}

TEMPLATE_SIGNATURE
inline void   swap (DOUBLE_LINKED_LIST& left, DOUBLE_LINKED_LIST& right)
{
	left.swap							(right);
}


#undef DOUBLE_LINKED_LIST
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_INTRUSIVE_DOUBLE_LINKED_LIST_INLINE_H_INCLUDED