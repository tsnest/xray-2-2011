////////////////////////////////////////////////////////////////////////////
//	Created		: 15.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

#include <map>
#include <vector>
#include <set>

extern	xray::memory::doug_lea_allocator_type	g_allocator;

#define USER_ALLOCATOR							g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

#define NEW( type )								XRAY_NEW_IMPL(		g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED