/////////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_monitor.h
//	Created 	: 14.06.2008
//  Modified 	: 14.06.2008
//	Author		: Dmitriy Iassenev
//	Description : memory monitor client code
/////////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_MONITOR_H_INCLUDED
#define MEMORY_MONITOR_H_INCLUDED

#if CS_DEBUG_LIBRARIES
#	define CS_USE_MEMORY_MONITOR	0
#else // #if CS_DEBUG_LIBRARIES
#	define CS_USE_MEMORY_MONITOR	0
#endif // #if CS_DEBUG_LIBRARIES

#if CS_USE_MEMORY_MONITOR

namespace memory_monitor {

void initialize			( );
void finalize			( );
void add_allocation		( pcvoid pointer, size_t size, pcstr description );
void add_deallocation	( pcvoid pointer );
void add_snapshot		( pcstr description );

} // namespace memory_monitor

#endif // #if CS_USE_MEMORY_MONITOR

#endif // #ifndef MEMORY_MONITOR_H_INCLUDED