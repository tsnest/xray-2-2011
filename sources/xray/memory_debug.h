////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_DEBUG_H_INCLUDED
#define MEMORY_DEBUG_H_INCLUDED

#include <xray/platform_extensions.h>

namespace xray {
namespace memory {

template <typename T>
inline T	uninitialized_value						( )
{
#if XRAY_PLATFORM_64_BIT
	return horrible_cast<u64,T>( ::xray::platform::little_endian() ? 0xfdfdcdcdcdcdcdcd : 0xcdcdcdcdcdcdfdfd ).second;
#else // #if XRAY_PLATFORM_64_BIT
	return horrible_cast<u32,T>( ::xray::platform::little_endian() ? 0xfdfdcdcd : 0xcdcdfdfd ).second;
#endif // #if XRAY_PLATFORM_64_BIT
}

} // namespace memory
} // namespace xray

#endif // #ifndef MEMORY_DEBUG_H_INCLUDED