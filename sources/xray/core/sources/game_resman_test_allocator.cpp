////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_test_allocator.h"

namespace xray {
namespace resources {

test_allocator	 s_test_allocators [test_allocators_count]	=	
{	
	test_allocator							("test allocator 1"), 
	test_allocator							("test allocator 2"), 
	test_allocator							("test allocator 3") 
};

test_allocator::test_allocator			(pcstr allocator_name) 
:	memory_type								(allocator_name), 
	m_allocator								(NULL), 
	m_allocator_initialized					(false), 
	m_arena									(NULL)
{
}

static u32 const	full_allocator_size	=	1024 * 10 * 2;

memory::doug_lea_allocator *   test_allocator::allocator ()
{
	if ( !m_allocator_initialized )
	{
		m_allocator						=	MT_NEW(memory::doug_lea_allocator) (memory::thread_id_const_true, false, true);
		m_arena							=	MT_ALLOC(char, full_allocator_size);
		m_allocator->initialize				(m_arena, 
											 full_allocator_size, 
											 "game res man test allocator");
		m_allocator->user_current_thread_id	();
		m_allocator_initialized			=	true;
	}
	
	return									m_allocator;
}

void   test_allocator::finalize ()
{
	if ( m_allocator_initialized )
	{
		MT_DELETE							(m_allocator);
		MT_FREE								(m_arena);
	}
}

} // namespace resources
} // namespace xray
