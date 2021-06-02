////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_QUEUE_INLINE_H_INCLUDED
#define XRAY_INTRUSIVE_QUEUE_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template <typename T, typename BaseWithMember, T* BaseWithMember::*MemberNext, typename Policy>
#define INTRUSIVE_QUEUE		xray::intrusive_queue<T, BaseWithMember, MemberNext, Policy>

TEMPLATE_SIGNATURE
INTRUSIVE_QUEUE::intrusive_queue	()
{
	m_pop_list						=	NULL;
	m_push_list						=	NULL;
}

TEMPLATE_SIGNATURE
void   INTRUSIVE_QUEUE::push_back (T* const object)
{
	T * prev_push_list;
	do 
	{
		prev_push_list				=	m_push_list;
		object->*MemberNext			=	prev_push_list;
	}
	while ( threading::interlocked_compare_exchange_pointer	((pvoid &)m_push_list, object, prev_push_list) 
			!= prev_push_list )	;
}

TEMPLATE_SIGNATURE
T*   INTRUSIVE_QUEUE::pop_front ()
{
	if ( m_pop_list )
	{
pop_front_existing:
		T * result					=	m_pop_list;

		while ( result && 
				threading::interlocked_compare_exchange_pointer	((pvoid &)m_pop_list, result->*MemberNext, result) 
				!= result )
		{
			result					=	m_pop_list;
		}

 		if ( result  )
			return						result;
	}

	typename Policy::mutex_raii raii	(m_threading_policy);
	if ( m_pop_list )
		goto							pop_front_existing;

	if ( !m_push_list )
		return							NULL;

	T * current_node				=	(T *)threading::interlocked_exchange_pointer((pvoid &)m_push_list, NULL);
	R_ASSERT							( current_node );
	T * previous_node				=	NULL;
	
	while ( current_node )
	{
		T * const next_node			=	current_node->*MemberNext;
		current_node->*MemberNext	=	previous_node;
		previous_node				=	current_node;
		current_node				=	next_node;
	}

	T * const result				=	previous_node;
	m_pop_list						=	result->*MemberNext;

	return								result;
}

TEMPLATE_SIGNATURE
T*   INTRUSIVE_QUEUE::front ()
{
	if ( m_pop_list )
	{
pop_front_existing:
		T * result					=	m_pop_list;
 		if ( result )
			return						result;
	}

	typename Policy::mutex_raii raii	(m_threading_policy);
	if ( m_pop_list )
		goto							pop_front_existing;

	if ( !m_push_list )
		return							NULL;

	ASSERT								(!m_pop_list);
	ASSERT								(m_push_list);
	T * current_node				=	(T *)threading::interlocked_exchange_pointer((pvoid &)m_push_list, NULL);
	T * previous_node				=	NULL;
	
	while ( current_node )
	{
		T * const next_node			=	current_node->*MemberNext;
		current_node->*MemberNext	=	previous_node;
		previous_node				=	current_node;
		current_node				=	next_node;
	}

	m_pop_list						=	previous_node;

	return								m_pop_list;
}

TEMPLATE_SIGNATURE
bool   INTRUSIVE_QUEUE::empty () const
{
	return								!m_pop_list && !m_push_list;
}

#undef INTRUSIVE_QUEUE
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_INTRUSIVE_QUEUE_INLINE_H_INCLUDED