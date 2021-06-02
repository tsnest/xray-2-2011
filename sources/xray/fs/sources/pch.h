////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_FS_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"fs"
#include <xray/macro_extensions.h>
#include <xray/optimization_extensions.h>
#include <xray/type_extensions.h>
#include <xray/platform_extensions.h>
#include <xray/detail_noncopyable.h>
#include <xray/debug/extensions.h>
#include <xray/stdlib_extensions.h>
#include <xray/debug/static_cast_checked.h>
#include <xray/debug/pointer_cast.h>
#include <xray/memory_extensions.h>
#include <xray/command_line_extensions.h>
#include <xray/os_extensions.h>
#include <xray/timing_extensions.h>
#include <xray/threading_extensions.h>
#include <xray/uninitialized_reference.h>
#include <xray/type_enum_flags.h>
#include <xray/memory_debug_allocator.h>
#include <xray/strings_extensions.h>
#include <xray/debug/check_no_multithread.h>
#include <xray/logging/extensions.h>
#include <xray/pointer_extensions.h>
#include <xray/debug/debug_pointers.h>
#include <xray/containers_extensions.h>
#include <xray/math_extensions.h>
#include <xray/memory_chunk_reader.h>
#include <xray/memory_writer.h>

#include "memory.h"

#endif // #ifndef PCH_H_INCLUDED