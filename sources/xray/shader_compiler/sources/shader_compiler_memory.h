////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SHADER_COMPILER_MEMORY_H_INCLUDED
#define SHADER_COMPILER_MEMORY_H_INCLUDED

#include <xray/buffer_vector.h>
#include <xray/fixed_vector.h>
#include <xray/associative_vector.h>
#include <xray/hash_multiset.h>

namespace xray {
namespace shader_compiler {

extern	xray::memory::doug_lea_allocator_type	g_allocator;

#define USER_ALLOCATOR							g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace shader_compiler
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		::xray::shader_compiler::g_allocator, type )
#define DELETE( pointer )						XRAY_DELETE_IMPL(	::xray::shader_compiler::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	::xray::shader_compiler::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	::xray::shader_compiler::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		::xray::shader_compiler::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	::xray::shader_compiler::g_allocator, type, count )

#endif // #ifndef SHADER_COMPILER_MEMORY_H_INCLUDED