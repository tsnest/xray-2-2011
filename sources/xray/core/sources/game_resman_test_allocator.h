////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_TEST_ALLOCATOR_H_INCLUDED
#define GAME_RESMAN_TEST_ALLOCATOR_H_INCLUDED

namespace xray {
namespace resources {

struct test_allocator
{
									test_allocator	(pcstr allocator_name);
	memory_type						memory_type;

	memory::doug_lea_allocator *	allocator	();

	void							finalize	();

private:
	memory::doug_lea_allocator *	m_allocator;
	pvoid							m_arena;
	bool							m_allocator_initialized;
};

enum { test_allocators_count	=	3 };
extern test_allocator	s_test_allocators[test_allocators_count];

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_TEST_ALLOCATOR_H_INCLUDED