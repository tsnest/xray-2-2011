////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_functions.h
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : memory functions
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

#define USE_DOUG_LEA_MEMORY_ALLOCATOR	1

#if USE_DOUG_LEA_MEMORY_ALLOCATOR
#	include "doug_lea_memory_allocator.h"
#	define library_free(a)		dlfree(a)
#	define library_malloc(a)	dlmalloc(a)
#	define library_realloc(a,b)	dlrealloc(a,b)
//#	define CATCH_MEMORY_LEAK	1
#	if CATCH_MEMORY_LEAK
		static const u32 s_catch_address = 0xbaadf00d;//0x0115c848;
#	endif // #if CATCH_MEMORY_LEAK
#else // #if USE_DOUG_LEA_MEMORY_ALLOCATOR
#	define library_free(a)		free(a)
#	define library_malloc(a)	malloc(a)
#	define library_realloc(a,b)	realloc(a,b)
#endif // #if USE_DOUG_LEA_MEMORY_ALLOCATOR

#include "memory_monitor.h"

#if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	static cs::core::memory_allocation_function_pointer		s_allocator = 0;
	static cs::core::memory_allocation_function_parameter	s_allocator_parameter = 0;
#endif // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)

cs_memory						memory;

static threading::mutex*			s_mutex = 0;

namespace cs {
namespace core {
namespace memory {

struct mutex_guard {
	inline	mutex_guard			()
	{
		if (s_mutex)
			s_mutex->lock		();
	}

	inline	~mutex_guard		()
	{
		if (s_mutex)
			s_mutex->unlock		();
	}
}; // struct mutex_guard

} // namespace memory
} // namespace core
} // namespace cs

using cs::core::memory::mutex_guard;

#if CS_USE_MEMORY_MONITOR && !defined(CS_NO_SCRIPTS)
static pvoid CS_CALL allocator_wrapper(
	cs::core::maf_parameter parameter,
	pcvoid buffer,
	size_t const new_buffer_size
);

static pvoid CS_CALL library_allocation_function(
	cs::core::maf_parameter parameter,
	pcvoid buffer,
	size_t const new_buffer_size
);
#endif // #if CS_DEBUG_LIBRARIES && !defined(CS_NO_SCRIPTS)

#if CS_DEBUG_LIBRARIES
pvoid cs_memory::mem_realloc		( pcvoid buffer, size_t const& new_buffer_size, pcstr const description )
#else // #if CS_DEBUG_LIBRARIES
pvoid cs_memory::mem_realloc		( pcvoid buffer, size_t const& new_buffer_size )
#endif // #if CS_DEBUG_LIBRARIES
{
#if CS_USE_MEMORY_MONITOR
	pvoid result	= 
		s_allocator(
			s_allocator_parameter,
			buffer,
			new_buffer_size
		);

	if (!new_buffer_size)
		memory_monitor::add_deallocation	(buffer);
	else {
		if (!buffer)
			memory_monitor::add_allocation	(result,new_buffer_size,description);
		else {
			memory_monitor::add_deallocation(buffer);
			memory_monitor::add_allocation	(result,new_buffer_size,description);
		}
	}

	return					result ;
#else // #if CS_USE_MEMORY_MONITOR
#	if CS_DEBUG_LIBRARIES
		CS_UNREFERENCED_PARAMETER	( description );
#	endif // #if CS_DEBUG_LIBRARIES

#	ifdef CS_NO_SCRIPTS
		return				(
			s_allocator(
				s_allocator_parameter,
				buffer,
				new_buffer_size
			)
		);
#	else // #ifdef CS_NO_SCRIPTS
		return				(
			::luabind::allocator(
				::luabind::allocator_parameter,
				buffer,
				new_buffer_size
			)
		);
#	endif // #ifdef CS_NO_SCRIPTS
#endif // #if CS_USE_MEMORY_MONITOR
}

void cs::core::memory_allocator		(
		cs::core::memory_allocation_function_pointer allocator_function,
		cs::core::memory_allocation_function_parameter allocator_parameter
	)
{
#if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	s_allocator					= allocator_function;
	s_allocator_parameter		= allocator_parameter;
#else // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	luabind::allocator			= allocator_function;
	luabind::allocator_parameter= allocator_parameter;
#endif // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
}

cs::core::memory_allocation_function_pointer cs::core::memory_allocator	()
{
#if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	return						s_allocator;
#else // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	return						luabind::allocator;
#endif // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
}

cs::core::memory_allocation_function_parameter cs::core::memory_allocator_parameter	()
{
#if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	return						s_allocator_parameter;
#else // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
	return						luabind::allocator_parameter;
#endif // #if CS_USE_MEMORY_MONITOR || defined(CS_NO_SCRIPTS)
}

#ifndef CATCH_MEMORY_LEAK
#	define add_address(a,b)
#	define remove_address(a)
	void dump_memory_leaks			()
	{
	}
#else // #ifndef CATCH_MEMORY_LEAK
	typedef std::map<pcvoid,u32>ADDRESSES;
	static ADDRESSES			addresses;
	static u32					total = 0;
#	include "os_include.h"

	void remove_address				(pcvoid buffer)
	{
		if (!buffer)
			return;

		string256				temp;
		ADDRESSES::iterator		I = addresses.find(buffer);
		ASSERT					(I != addresses.end());
		total					-= (*I).second;
		sprintf_s				(
			temp,
			"free    0x%08x [%7d][%7d][%d] (%3d active allocations)\n",
			*(u32*)&buffer,
			(*I).second,
			total,
			GetCurrentThreadId(),
			addresses.size() - 1
		);
		addresses.erase			(I);
		OUTPUT_DEBUG_STRING		(temp);
	}

	void add_address				(pcvoid result, u32 const size)
	{
		if (result == *(pcvoid*)&s_catch_address)
			__asm int 3;

		string256				temp;
		ADDRESSES::iterator		I = addresses.find(result);
		ASSERT					(I == addresses.end());
		addresses.insert		(std::make_pair(result,size));
		total					+= size;
		sprintf_s				(
			temp,
			"malloc  0x%08x [%7d][%7d][%d] (%d active allocations)\n",
			*(u32*)&result,
			size,
			total,
			GetCurrentThreadId(),
			addresses.size()
		);
		OUTPUT_DEBUG_STRING		(temp);
	}

	void dump_memory_leaks			()
	{
		string256				temp;
		sprintf_s				(temp, "total %d memory leaks\n", addresses.size());
		OUTPUT_DEBUG_STRING		(temp);

		ADDRESSES::iterator		I = addresses.begin();
		ADDRESSES::iterator		E = addresses.end();
		for ( ; I != E; ++I) {
			sprintf_s			(temp,sizeof(temp),"leak 0x%08x [%7d]\n",*(u32 const*)&(*I).first,(*I).second);
			OUTPUT_DEBUG_STRING	(temp);
		}
	}
#endif // #ifndef CATCH_MEMORY_LEAK

static pvoid CS_CALL library_allocation_function(
		cs::core::maf_parameter parameter,
		pcvoid buffer,
		size_t const new_buffer_size
	)
{
	CS_UNREFERENCED_PARAMETER	( parameter );

	mutex_guard					guard;

	if (!new_buffer_size) {
		remove_address			(buffer);
		library_free			((pvoid)buffer);
		return					0;
	}

	if (!buffer) {
		pvoid					result = library_malloc(new_buffer_size);
		add_address				(result,new_buffer_size);
		return					result;
	}

	pvoid						result = library_realloc((pvoid)buffer,new_buffer_size);
	remove_address				(buffer);
	add_address					(result,new_buffer_size);
	return						result;
}

#if CS_USE_MEMORY_MONITOR && !defined(CS_NO_SCRIPTS)
static pvoid CS_CALL allocator_wrapper	(
		cs::core::maf_parameter parameter,
		pcvoid buffer,
		size_t const new_buffer_size
	)
{
	CS_UNREFERENCED_PARAMETER	( parameter );

	pvoid result	= 
		s_allocator(
			s_allocator_parameter,
			buffer,
			new_buffer_size
		);

	if ( !new_buffer_size )
		memory_monitor::add_deallocation	( buffer );
	else {
		if ( !buffer )
			memory_monitor::add_allocation	( result, new_buffer_size, "luabind" );
		else {
			memory_monitor::add_deallocation( buffer );
			memory_monitor::add_allocation	( result, new_buffer_size, "luabind" );
		}
	}

	return					result ;
}
#endif // #if CS_USE_MEMORY_MONITOR && !defined(CS_NO_SCRIPTS)

void cs_memory::initialize			()
{
#if CS_USE_MEMORY_MONITOR
	memory_monitor::initialize	();

#	ifndef CS_NO_SCRIPTS
		luabind::allocator				= &allocator_wrapper;
		luabind::allocator_parameter	= 0;
#	endif // #ifndef CS_NO_SCRIPTS

#endif // #if CS_USE_MEMORY_MONITOR

	if (cs::core::memory_allocator())
		return;

	cs::core::memory_allocator	(&library_allocation_function, 0);
}

void cs_memory::switch_mt_alloc		()
{
	if (s_mutex)
		return;

	s_mutex						= cs_new<threading::mutex>();
}

void cs_memory::uninitialize		()
{
	if (!s_mutex) {
#if CS_USE_MEMORY_MONITOR
		memory_monitor::finalize();
#endif // #if CS_USE_MEMORY_MONITOR

		return;
	}

	threading::mutex*			temp = s_mutex;
	s_mutex						= 0;
	cs_delete					(temp);
#if CS_USE_MEMORY_MONITOR
	memory_monitor::finalize	();
#endif // #if CS_USE_MEMORY_MONITOR
}

size_t cs::core::memory_stats			()
{
#ifdef USE_DOUG_LEA_MEMORY_ALLOCATOR
	if (::cs::core::memory_allocator() != &library_allocation_function)
		return					0;

	return						(u32)dlmallinfo().uordblks;
#else // USE_DOUG_LEA_MEMORY_ALLOCATOR
	return						0;
#endif // #ifndef USE_DOUG_LEA_MEMORY_ALLOCATOR
}
