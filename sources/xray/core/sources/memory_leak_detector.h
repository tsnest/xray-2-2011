////////////////////////////////////////////////////////////////////////////
//	Created		: 30.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_LEAK_DETECTOR_H_INCLUDED
#define MEMORY_LEAK_DETECTOR_H_INCLUDED

#if defined(NDEBUG) || !defined(_MSC_VER)
#	define XRAY_USE_MEMORY_LEAK_DETECTOR	0
#else // #if defined(NDEBUG) || !defined(_MSC_VER)
#	define XRAY_USE_MEMORY_LEAK_DETECTOR	1
#endif // #if defined(NDEBUG) || !defined(_MSC_VER)

#if XRAY_USE_MEMORY_LEAK_DETECTOR

namespace xray {
namespace memory {
namespace leak_detector {

void	preinitialize			( );
void	initialize				( );
void	finalize				( );

void	on_alloc				( pvoid& pointer, size_t& size, size_t previous_size, pcstr description );
void	on_free					( pvoid& pointer, bool can_clear = true );
pvoid	get_real_pointer		( pvoid pointer );

size_t	get_house_keeping_size	( );

} // namespace leak_detector
} // namespace memory
} // namespace xray

#endif // #if XRAY_USE_MEMORY_LEAK_DETECTOR

#endif // #ifndef MEMORY_LEAK_DETECTOR_H_INCLUDED