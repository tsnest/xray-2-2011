////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PLATFORM_EXTENSIONS_WIN_H_INCLUDED
#define PLATFORM_EXTENSIONS_WIN_H_INCLUDED

namespace xray {
namespace platform {

bool   XRAY_CORE_API	unload_delay_loaded_library			( pcstr dll_name );
u64		XRAY_CORE_API	get_local_video_memory_size			( );
u64		XRAY_CORE_API	get_minimal_local_video_memory_size	( );

} // namespace platform
} // namespace xray

#endif // #ifndef PLATFORM_EXTENSIONS_WIN_H_INCLUDED