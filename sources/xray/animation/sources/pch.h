////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_ANIMATION_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"animation"
#include <xray/extensions.h>

#if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)
#	define	XRAY_USE_MAYA_ANIMATION	1
#else // #if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)
#	define	XRAY_USE_MAYA_ANIMATION	0
#endif // #if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)

#include "animation_memory.h"

namespace xray {
namespace animation {

template < typename T >
T* get_shift_ptr( T* ptr, u32 byte_shift )
{
	return (T*) ( ( pbyte )( ptr ) + byte_shift );
}

template < typename T >
T const* get_shift_ptr( T const* ptr, u32 byte_shift )
{
	return (T*) ( ( pbyte )( ptr ) + byte_shift );
}

template< typename T1, typename T2 >
size_t bytes_dist( T1 const* from, T2 const* to )
{
	ASSERT( pbyte( from ) > pbyte( to ));
	return pbyte( from ) - pbyte( to );
}

enum {
	integration_interval_length_in_ms	= 10,
};

} // namespace animation
} // namespace xray

#endif // #ifndef PCH_H_INCLUDED