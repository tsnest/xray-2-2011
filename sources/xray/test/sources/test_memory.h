////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.11.2008
//	Author		: Lain
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_MEMORY_H_INCLUDED
#define TEST_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

#include <map>
#include <vector>
#include <set>

extern	xray::memory::doug_lea_allocator_type	g_test_allocator;

#define USER_ALLOCATOR							g_test_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

#define TEST_NEW( type )								XRAY_NEW_IMPL(		g_test_allocator, type )
#define TEST_DELETE( pointer )							XRAY_DELETE_IMPL(	g_test_allocator, pointer )
#define TEST_MALLOC( size, description )				XRAY_MALLOC_IMPL(	g_test_allocator, size, description )
#define TEST_REALLOC( pointer, size, description )		XRAY_REALLOC_IMPL(	g_test_allocator, pointer, size, description )
#define TEST_FREE( pointer )							XRAY_FREE_IMPL(		g_test_allocator, pointer )
#define TEST_ALLOC( type, count )						XRAY_ALLOC_IMPL(	g_test_allocator, type, count )

#endif // #ifndef TEST_MEMORY_H_INCLUDED