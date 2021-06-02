////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tasks_allocator.h"

namespace xray {
namespace tasks {

task_allocator::task_allocator ()
{
	m_free_list.m_pointer				=	NULL;
	m_free_list.counter					=	0;

	for ( u32 i=0; i<max_task_count; ++i )
	{
		task * const current_task		=	(task *)((pbyte)m_task_buffer + (i * granularity));
		task * const next_task			=	(task *)((pbyte)m_task_buffer + ((i+1) * granularity));

		current_task->m_state			=	task::state_deallocated;
		current_task->m_next_task_in_allocator	=	(i == max_task_count - 1) ? NULL : next_task;
	}

	m_free_list.m_pointer				=	(task *)m_task_buffer;
}

task *   task_allocator::allocate ()
{
	pointer_and_counter allocated_task;
	pointer_and_counter new_head;
	do 
	{
		allocated_task					=	m_free_list;
		if ( !allocated_task.m_pointer )
			return							NULL;

		new_head.m_pointer				=	allocated_task.pointer()->m_next_task_in_allocator;
		new_head.counter				=	allocated_task.counter + 1;

	} while (
		threading::interlocked_compare_exchange	(
			m_free_list.whole,
			new_head.whole,
			allocated_task.whole
		) != allocated_task.whole
	);

	ASSERT									(allocated_task.pointer()->m_state == task::state_deallocated);
	return									allocated_task.pointer();
}

void   task_allocator::deallocate (task * freeing_task)
{
	ASSERT									(freeing_task->m_state == task::state_executed);
	freeing_task->m_state				=	task::state_deallocated;
	
	pointer_and_counter head;
	pointer_and_counter new_head;
	new_head.m_pointer					=	freeing_task;

	do {
		head										=	m_free_list;
		new_head.pointer()->m_next_task_in_allocator	=	head.pointer();
		new_head.counter							=	head.counter;
	} while (
		threading::interlocked_compare_exchange	(
			m_free_list.whole,
			new_head.whole,
			head.whole
		) != head.whole
	);
}

void   task_allocator::check_all_free ()
{
	u32 free_count						=	0;
	
	task const * current_task			=	m_free_list.pointer();
	while ( current_task )
	{
		++free_count;
		current_task					=	current_task->m_next_task_in_allocator;
	}

	R_ASSERT_CMP							(free_count, ==, (u32)max_task_count);
}

} // namespace tasks
} // namespace xray
