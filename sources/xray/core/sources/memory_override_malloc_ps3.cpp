/*
 *   SCE CONFIDENTIAL
 *   PlayStation(R)3 Programmer Tool Runtime Library 360.001 
 *   Copyright (C) 2008 Sony Computer Entertainment Inc.
 *   All Rights Reserved. 
 */

/*E
 * File: user_malloc.c
 * Description:
 *   A simple example to replace malloc(). This sample shows an implementation of
 *   functions which are related to malloc() by using mspace_malloc() series. Because
 *   of the specification of mspace_malloc(), the heap area for these malloc() series 
 *   can not be enlarged after creating the area.
 *   You can also implement your own malloc() here, instead of these ones.
 *
 *   To replace malloc() series, you should redefine all of the functions below.
 *
 *     void   _malloc_init(void)
 *     void   _malloc_finalize(void)
 *     void*  malloc(size_t size)
 *     void   free(void *ptr)
 *     void*  calloc(size_t nelem, size_t size)
 *     void*  realloc(void *ptr, size_t size)
 *     void*  memalign(size_t boundary, size_t size)
 *     void*  reallocalign(void *ptr, size_t size, size_t boundary)
 *     int    malloc_stats(struct malloc_managed_size *mmsize)
 *     size_t malloc_usable_size(void *ptr)
 *
 *   Notice that _malloc_init() is called at the beginning of the program, and 
 *   _malloc_finalize() is called at the end of the program. Therefore, no global
 *   constructor is run before executing _malloc_init() and no global destructor
 *   (also the function which is registered by atexit()) is run after executing
 *   _malloc_finalize(). Furthermore, no stream can be accessed in these two functions,
 *   i.e., printf() is not available in these functions.
 */

#define XRAY_MEMORY_OVERRIDE_OPERATORS_H_INCLUDED
#include "pch.h"
#include <stdlib.h>
#include <xray/memory_extensions.h>
#include <xray/memory_doug_lea_mt_allocator.h>

//#include <mspace.h>
//#include <sys/memory.h>
//
///*E 2MB for a heap area */
//#define HEAP_SIZE (1024 * 1024 * 2)
//
///*E Address of a heap area */
//static sys_addr_t heap_area;
//
///*E Global mspace */
//static mspace global_mspace;
//
///*E Function prototypes */
//void _malloc_init(void);
//void _malloc_finalize(void);

namespace xray {
namespace memory {

void initialize_crt_allocator	( );

extern "C" {

/*E 
 * void _malloc_init()
 * Initialize function for malloc series. This function is called at the beginning
 * of the program, before executing `main()' and also all of the global constructors.
 * Notice that there are some limitation below for this function.
 *   - CANNOT access any streams (i.e., no printf(), no fprintf(), etc..).
 *   - CANNOT access any global objects which need the constructor to initialize.
 */
void _malloc_init()
{
	R_ASSERT								( !xray::memory::g_crt_allocator );
	xray::memory::initialize_crt_allocator	( );
	R_ASSERT								( xray::memory::g_crt_allocator );
}

/*E 
 * void _malloc_finalize()
 * Finalize function for malloc series. This function is called at the end
 * of the program, after executing `main()' and also all of the global destructors.
 * Notice that there are some limitation below for this function.
 *   - CANNOT access any streams (i.e., no printf(), no fprintf(), etc..).
 *   - CANNOT access any global objects which need the destructor to finalize.
 */
void _malloc_finalize() 
{
	R_ASSERT					( xray::memory::g_crt_allocator );
}

/*E 
 * void *malloc(size_t size)
 * User defined malloc().
 */
pvoid malloc					( size_t size )
{
	R_ASSERT					( g_crt_allocator, "CRT allocation detected. do not use CRT allocations or setup crt allocator (the last doesn't work in MASTER_GOLD configuration)" );
#ifdef DEBUG
	return						g_crt_allocator->malloc_impl( size, "from malloc", __FUNCTION__, __FILE__, __LINE__ );
#else // #ifdef 				DEBUG
	return						g_crt_allocator->malloc_impl( size );
#endif // #ifdef DEBUG
}

/*E 
 * void free(void *ptr)
 * User defined free().
 */
void free						( pvoid pointer )
{
	R_ASSERT					( g_crt_allocator, "CRT allocation detected. do not use CRT allocations or setup crt allocator (the last doesn't work in MASTER_GOLD configuration)" );
#ifdef DEBUG
	g_crt_allocator->free_impl	( pointer, __FUNCTION__, __FILE__, __LINE__ );
#else // #ifdef DEBUG
	g_crt_allocator->free_impl	( pointer );
#endif // #ifdef DEBUG
}

/*E 
 * void *calloc(size_t nelem, size_t size)
 * User defined calloc().
 */
pvoid calloc						( size_t count, size_t element_size )
{
	pvoid const result			= malloc( count*element_size );
	memset						( result, 0, count*element_size );
	return						result;
}

/*E 
 * void *realloc(void *ptr, size_t size)
 * User defined realloc().
 */
void *realloc(void *pointer, size_t size) 
{
	R_ASSERT					( g_crt_allocator, "CRT allocation detected. do not use CRT allocations or setup crt allocator (the last doesn't work in MASTER_GOLD configuration)" );
#ifdef DEBUG
	return						g_crt_allocator->realloc_impl( pointer, size, "from realloc", __FUNCTION__, __FILE__, __LINE__ );
#else // #ifdef DEBUG
	return						g_crt_allocator->realloc_impl( pointer, size );
#endif // #ifdef DEBUG
}

/*E 
 * void *memalign(size_t boundary, size_t size)
 * User defined memalign().
 */
void *memalign(size_t boundary, size_t size) 
{
	R_ASSERT					( xray::memory::g_crt_allocator );
	NOT_IMPLEMENTED				( return 0 );
}

/*E 
 * void *reallocalign(void *ptr, size_t size, size_t boundary)
 * User defined reallocalign.
 */
void *reallocalign(void *ptr, size_t size, size_t boundary) 
{
	R_ASSERT					( xray::memory::g_crt_allocator );
	NOT_IMPLEMENTED				( return 0 );
}

/*E 
 * int malloc_stats(struct malloc_managed_size *mmsize)
 * User defined malloc_stats().
 */
int malloc_stats(struct malloc_managed_size *mmsize) 
{
	R_ASSERT					( xray::memory::g_crt_allocator );
	NOT_IMPLEMENTED				( return 0 );
}

/*E 
 * size_t malloc_usable_size(void *ptr)
 * User defined malloc_usable_size().
 */
size_t malloc_usable_size(void *ptr) 
{
	R_ASSERT					( xray::memory::g_crt_allocator );
	return						xray::memory::g_crt_allocator->usable_size( ptr );
}

} // extern "C"

} // namespace memory
} // namespace xray