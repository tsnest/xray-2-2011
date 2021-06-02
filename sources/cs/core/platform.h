////////////////////////////////////////////////////////////////////////////
//	Module 		: platform.h
//	Created 	: 10.03.2008
//  Modified 	: 10.03.2008
//	Author		: Dmitriy Iassenev
//	Description : platform specific
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_PLATFORM_H_INCLUDED
#define CS_CORE_PLATFORM_H_INCLUDED

#include <cs/core/os_functions.h>

#if CS_PLATFORM_WINDOWS
#	include <intrin.h>
#	pragma intrinsic(__rdtsc)
#endif // #if CS_PLATFORM_WINDOWS

namespace cs {
namespace core {

enum platform_enum {
	platform_PC 		= (unsigned int)(0),
	platform_XBox_360	= (unsigned int)(1),
	platform_PS3 		= (unsigned int)(2),
}; // enum platform_enum

class CS_CORE_API platform {
public:
	u64					qpc_per_second;

public:
						platform		();
	inline u64			get_clocks		()
	{
#if CS_PLATFORM_WINDOWS
		return			__rdtsc();
#else // #if CS_PLATFORM_WINDOWS
		return			get_QPC();
#endif // #if CS_PLATFORM_WINDOWS
	}
	
	u64					get_QPC			();

	static inline bool	little_endian	()
	{
		u16	const word	= 0x0001;
		return			!!*(u8 const*)&word;
	}

	static inline u32	alignment		()
	{
		struct dummy {};
		return			sizeof(dummy);
	}
};

CS_CORE_API extern platform	g_platform;

} // namespace core
} // namespace cs

#endif // #ifndef CS_CORE_PLATFORM_H_INCLUDED