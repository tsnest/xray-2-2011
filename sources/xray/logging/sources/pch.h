////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_LOGGING_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define	XRAY_LOG_MODULE_INITIATOR	"logging"

#include <xray/macro_platform.h>
#include <xray/type_extensions.h>
#include <xray/macro_extensions.h>
#include <xray/debug/extensions.h>
#include <xray/memory_override_operators.h>
#include <xray/memory_extensions.h>
#include <boost/noncopyable.hpp>
#include <xray/command_line_extensions.h>
#include <xray/threading_extensions.h>
#include <xray/containers_extensions.h>
#include <xray/uninitialized_reference.h>
#include <xray/type_enum_flags.h>
#include <xray/math_extensions.h>
#include <xray/stdlib_extensions.h>
#include <xray/strings_extensions.h>

#include "logging_memory.h"

#endif // #ifndef PCH_H_INCLUDED