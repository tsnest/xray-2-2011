////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#define	_WIN32_WINNT	0x0500
#include <xray/os_include.h>

#include <xray/memory_crt_allocator.h>
#include "memory_process_allocator.h"
#include <errno.h>

static xray::command_line::key s_no_memory_usage_stats("no_memory_usage_stats", "", "memory", "disable CRT and GetProcessHeap() memory usage stats detection. Use this option if another program injected thread into application, which uses one of these allocators." );

using xray::memory::crt_allocator;
using xray::memory::process_allocator;

static inline u32 execute_handler_filter( ... )
{
	return						EXCEPTION_EXECUTE_HANDLER;
}

static int heap_walk (
	    HANDLE heap_handle,
        struct _heapinfo *_entry
        )
{
        PROCESS_HEAP_ENTRY Entry;
        DWORD errval;
        int errflag;
        int retval = _HEAPOK;

        Entry.wFlags = 0;
        Entry.iRegionIndex = 0;
		Entry.cbData = 0;
        if ( (Entry.lpData = _entry->_pentry) == NULL ) {
            if ( !HeapWalk( heap_handle, &Entry ) ) {
                if ( GetLastError() == ERROR_CALL_NOT_IMPLEMENTED ) {
                    _doserrno = ERROR_CALL_NOT_IMPLEMENTED;
                    errno = ENOSYS;
                    return _HEAPEND;
                }
                return _HEAPBADBEGIN;
            }
        }
        else {
            if ( _entry->_useflag == _USEDENTRY ) {
                if ( !HeapValidate( heap_handle, 0, _entry->_pentry ) )
                    return _HEAPBADNODE;
                Entry.wFlags = PROCESS_HEAP_ENTRY_BUSY;
            }
nextBlock:
            /*
             * Guard the HeapWalk call in case we were passed a bad pointer
             * to an allegedly free block.
             */
            __try {
                errflag = 0;
                if ( !HeapWalk( heap_handle, &Entry ) )
                    errflag = 1;
            }
            __except( execute_handler_filter( GetExceptionCode( ), GetExceptionInformation( ) ) ) {
                errflag = 2;
            }

            /*
             * Check errflag to see how HeapWalk fared...
             */
            if ( errflag == 1 ) {
                /*
                 * HeapWalk returned an error.
                 */
                if ( (errval = GetLastError()) == ERROR_NO_MORE_ITEMS ) {
                    return _HEAPEND;
                }
                else if ( errval == ERROR_CALL_NOT_IMPLEMENTED ) {
                    _doserrno = errval;
                    errno = ENOSYS;
                    return _HEAPEND;
                }
                return _HEAPBADNODE;
            }
            else if ( errflag == 2 ) {
                /*
                 * Exception occurred during the HeapWalk!
                 */
                return _HEAPBADNODE;
            }
        }

        if ( Entry.wFlags & (PROCESS_HEAP_REGION |
             PROCESS_HEAP_UNCOMMITTED_RANGE) )
        {
            goto nextBlock;
        }

        _entry->_pentry = (int*)Entry.lpData;
        _entry->_size = Entry.cbData;
        if ( Entry.wFlags & PROCESS_HEAP_ENTRY_BUSY ) {
            _entry->_useflag = _USEDENTRY;
        }
        else {
            _entry->_useflag = _FREEENTRY;
        }

        return( retval );
}

static u32	mem_usage		(HANDLE heap_handle, u32* pBlocksUsed, u32* pBlocksFree)
{
	if ( !xray::debug::is_debugger_present( ) )
		return		( 0 );

	if ( s_no_memory_usage_stats.is_set() )
		return		( 0 );

	_HEAPINFO		hinfo;
	int				heapstatus;
	hinfo._pentry	= NULL;
	size_t	total	= 0;
	u32	blocks_free	= 0;
	u32	blocks_used	= 0;
	while( ( heapstatus = heap_walk( heap_handle, &hinfo ) ) == _HEAPOK )
	{ 
		if (hinfo._useflag == _USEDENTRY)	{
			total		+= hinfo._size;
			blocks_used	+= 1;
		} else {
			blocks_free	+= 1;
		}
	}
	if (pBlocksFree)	*pBlocksFree= 1024*(u32)blocks_free;
	if (pBlocksUsed)	*pBlocksUsed= 1024*(u32)blocks_used;

	switch( heapstatus )
	{
	case _HEAPEMPTY:
		break;
	case _HEAPEND:
		break;
	case _HEAPBADPTR:
		LOG_WARNING		( "bad pointer to heap" );
		return			0;
		break;
	case _HEAPBADBEGIN:
		LOG_WARNING		( "bad start of heap" );
		return			0;
	case _HEAPBADNODE:
		LOG_WARNING		( "bad node in heap" );
		return			0;
	}
	return (u32) total;
}

crt_allocator::crt_allocator		( ) :
	m_malloc_ptr					( 0 ),
	m_free_ptr						( 0 ),
	m_realloc_ptr					( 0 )
{
#ifdef _DLL
#	if _MSC_VER == 1500
		pcstr const library_name	= "msvcr90.dll";
#	else // #if _MSC_VER == 1500
#		error define correct library name here
#	endif // #if _MSC_VER == 1500

	HMODULE const handle			= GetModuleHandle( library_name );
	R_ASSERT						( handle );

	m_malloc_ptr					= (malloc_ptr_type)( GetProcAddress(handle, "malloc") );
	R_ASSERT						( m_malloc_ptr );

	m_free_ptr						= (free_ptr_type)( GetProcAddress(handle, "free") );
	R_ASSERT						( m_free_ptr );

	m_realloc_ptr					= (realloc_ptr_type)( GetProcAddress(handle, "realloc") );
	R_ASSERT						( m_free_ptr );
#endif // #elif !defined(MASTER_GOLD)
}

size_t crt_allocator::total_size		( ) const
{
	return						( mem_usage((HANDLE)_get_heap_handle(),0,0) );
}

size_t crt_allocator::allocated_size	( ) const
{
	return						( mem_usage((HANDLE)_get_heap_handle(),0,0) );
}

size_t process_allocator::total_size	( ) const
{
	if ( !memory::try_lock_process_heap	( ) )
		return					( 0 );

	u32 const result			= mem_usage( GetProcessHeap() , 0, 0 );
	memory::unlock_process_heap	( );
	return						( result );
}

size_t process_allocator::allocated_size( ) const
{
	if ( !memory::try_lock_process_heap	( ) )
		return					( 0 );

	u32 const result			= mem_usage( GetProcessHeap() , 0, 0 );
	memory::unlock_process_heap	( );
	return						( result );
}