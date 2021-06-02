////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_PLATFORM_CONFIGURATION_H_INCLUDED
#define XRAY_FS_PLATFORM_CONFIGURATION_H_INCLUDED

#include <xray/macro_platform.h>
#include <xray/platform_pointer.h>

namespace xray {

#if (XRAY_PLATFORM_WINDOWS == 1) && !defined(MASTER_GOLD)
#	define XRAY_FS_NEW_WATCHER_ENABLED	1
#else
#	define XRAY_FS_NEW_WATCHER_ENABLED	0
#endif

#if XRAY_PLATFORM_WINDOWS
typedef		u64			file_size_type;
#else // #if XRAY_PLATFORM_WINDOWS
typedef		u32			file_size_type;
#endif // #if XRAY_PLATFORM_WINDOWS

namespace fs_new {

enum { max_path_length = 260 };

template <platform_pointer_enum pointer_for_fat_size>
struct platform_to_file_size_type
{
	typedef		u32					file_size_type;
};

template <>
struct platform_to_file_size_type<platform_pointer_64bit>
{
	typedef		u64					file_size_type;
};

} // namespace fs_new

} // namespace xray

#endif // #ifndef XRAY_FS_PLATFORM_CONFIGURATION_H_INCLUDED