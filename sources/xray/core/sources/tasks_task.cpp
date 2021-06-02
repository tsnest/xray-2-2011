////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/tasks_task.h>
#include <xray/tasks_thread_tls.h>
#include "tasks_thread_pool.h"

namespace xray {
namespace tasks {

void   deallocate_task (task * const task);

task::task () : 
			m_function(NULL), 
			m_next_task_in_allocator(NULL), 
			m_next_task_in_full_queue(NULL), 
			m_type(NULL), 
			m_ordinal(0),
			m_state(state_allocated),
			m_parent(NULL),
			m_event_wait_for_children(NULL),
			m_child_counter(0),
			m_reference_counter(references_held_by_system_count)
{
}

task::task (function const & function, task_type * const type, u64 const ordinal, task * const parent) : 
		    m_function(function), 
			m_next_task_in_allocator(NULL), 
			m_next_task_in_full_queue(NULL), 
			m_type(type), 
			m_ordinal(ordinal),
			m_state(state_allocated),
			m_parent(parent),
			m_event_wait_for_children(NULL),
			m_child_counter(0),
			m_reference_counter(references_held_by_system_count)
{
}

void   task::on_child_task_added (task * child)
{
	m_children.push_back						(child);
	threading::interlocked_increment			(m_child_counter);
}

bool   thread_can_execute_while_wait_for_children ();

void   task::on_child_task_ended ()
{
	threading::interlocked_decrement			(m_child_counter);
	if ( !m_child_counter )
	{
		if ( m_event_wait_for_children )
			m_event_wait_for_children->set						(true);
	}
}

void   task::execute ()
{
	R_ASSERT									(m_state == state_grabbed);
	if ( m_function )
		m_function								();

	thread_tls * const tls					=	current_task_thread_tls();
	tls->current_task						=	NULL;

	for (;;)
	{
		long const previous_state			=	threading::interlocked_compare_exchange
												(m_state, state_captured, state_grabbed);
		if ( previous_state == state_grabbed )
		{
			if ( m_parent )
				m_parent->on_child_task_ended	();

			long const second_previous_state	=	threading::interlocked_compare_exchange	
												(m_state, state_executed, state_captured);
			ASSERT_U							(second_previous_state == state_captured);
			break;
		}

		ASSERT									(previous_state == state_captured);
	}

	unlink_from_children						();

	decrement_reference_count_and_deallocate_when_zero();
}

void   task::unlink_from_child (task * const child)
{
	if ( child->m_state == state_executed )
		return;

	do {
		long previous_task_state			=	threading::interlocked_compare_exchange
												(child->m_state, state_captured, state_grabbed);

		if ( previous_task_state == task::state_grabbed )
		{
			child->m_parent					=	NULL;
			previous_task_state				=	threading::interlocked_compare_exchange		
												(child->m_state, state_grabbed, state_captured);
			ASSERT_U							(previous_task_state == state_captured);
		}
		else if ( previous_task_state == state_executed )
			return;
		
		ASSERT									(previous_task_state == state_captured);
		
	} while ( identity(true) );
}

void   task::unlink_from_children ()
{
	// tell children they have no parent
	while ( !m_children.empty() )
	{
		task * const current_child			=	m_children.pop_front();
		unlink_from_child						(current_child);
		current_child->decrement_reference_count_and_deallocate_when_zero();
	}
}

task *   task::grab_next_child ()
{
	while ( identity(true) )
	{
		task * const current_child			=	m_children.pop_front();
		if ( !current_child )
			break;

		bool const grabbed					=	current_child->try_grab();
		current_child->decrement_reference_count_and_deallocate_when_zero();

		if ( grabbed )
		{
			thread_tls * const tls			=	current_task_thread_tls();
			tls->current_task				=	current_child;
			tls->last_task_type				=	current_child->m_type;
			return								current_child;	
		}
	}

	return										NULL;
}

void   task::set_event_for_wait_children (threading::event* event)
{
	//if ( event )
		//event->set								(false);
	threading::interlocked_exchange_pointer		((pvoid&)m_event_wait_for_children, event);
}

} // namespace tasks
} // namespace xray




// void   task::unlink_from_children ()
// {
// 	// tell children they have no parent
// 	while ( !m_children.empty() )
// 	{
// 		task * const current_child			=	m_children.pop_front();
// 
// 		bool was_executed					=	false;
// 		try_unlink_from_child					(current_child, & was_executed);
// 
// 		release_state_enum const previous_release_state	=	(release_state_enum)threading::interlocked_compare_exchange
// 												(m_release_state, released_state_by_list, released_state_not);
// 		ASSERT									(previous_release_state != released_state_by_list);
// 
// 		if ( was_executed && previous_release_state == released_state_by_tree )
// 			deallocate_task						(current_child);
// 	}
// }
// 
// bool   task::try_grab_else_if_executed_deallocate (release_state_enum const release_state)
// {
// 	long const previous_state				=	(state_enum)threading::interlocked_compare_exchange	
// 												(m_state, state_grabbed, state_allocated);
// 
// 	release_state_enum const previous_release_state	=	(release_state_enum)threading::interlocked_compare_exchange
// 														(m_release_state, release_state, released_state_not);
// 	ASSERT										(previous_release_state != release_state);
// 
// 	if ( previous_state == task::state_executed )
// 	{
// 		release_state_enum const complement_state	=	(release_state == released_state_by_tree) ?
// 														released_state_by_list : released_state_by_tree;
// 
// 		if ( previous_release_state == complement_state )
// 		{
// 			deallocate_task						(this);
// 			return								false;
// 		}
// 	}
// 
// 	if ( previous_state == task::state_allocated )
// 		return									true;
// 
// 	ASSERT										(previous_state == task::state_grabbed);
// 	return										false;
// }