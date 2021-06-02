////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_COMPRESSOR_TEST_MEMORY_H_INCLUDED
#define TEXTURE_COMPRESSOR_TEST_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

#include <map>
#include <vector>
#include <set>

namespace xray {
namespace texture_compressor_test {

extern	xray::memory::doug_lea_allocator_type	g_allocator;

#define USER_ALLOCATOR							g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace texture_compressor_test
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		xray::texture_compressor_test::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	xray::texture_compressor_test::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	xray::texture_compressor_test::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	xray::texture_compressor_test::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		xray::texture_compressor_test::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	xray::texture_compressor_test::g_allocator, type, count )

#endif // #ifndef TEXTURE_COMPRESSOR_TEST_MEMORY_H_INCLUDED