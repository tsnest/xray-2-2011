////////////////////////////////////////////////////////////////////////////
//	Module 		: memory.h
//	Created 	: 18.03.2007
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : cs core memory
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MEMORY_H_INCLUDED
#define CS_CORE_MEMORY_H_INCLUDED

#include <cs/config.h>

#ifndef CS_CORE_API
#	define CS_CORE_API										CS_API
#	define CS_CORE_API_DEFINED_IN_MEMORY_H_INCLUDED
#endif // #ifndef CS_CORE_API

#include <luabind/luabind_types.h>

namespace cs {
namespace core {

typedef luabind::memory_allocation_function_pointer			memory_allocation_function_pointer;
typedef luabind::memory_allocation_function_parameter		memory_allocation_function_parameter;
typedef memory_allocation_function_pointer					maf_ptr;
typedef memory_allocation_function_parameter				maf_parameter;

	CS_CORE_API maf_ptr			memory_allocator			();
	CS_CORE_API maf_parameter	memory_allocator_parameter	();
	CS_CORE_API void			memory_allocator			(maf_ptr memory_allocator, maf_parameter parameter);
	CS_CORE_API size_t			memory_stats				();

} // namespace core
} // namespace cs

#ifdef CS_CORE_API_DEFINED_IN_MEMORY_H_INCLUDED
#	undef CS_CORE_API
#	undef CS_CORE_API_DEFINED_IN_MEMORY_H_INCLUDED
#endif // #ifdef CS_CORE_API_defined_in_memory_h

#endif // #ifndef CS_CORE_MEMORY_H_INCLUDED
