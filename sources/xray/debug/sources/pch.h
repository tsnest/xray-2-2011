////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_DEBUG_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#include <xray/macro_extensions.h>
#include <xray/type_extensions.h>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <xray/debug/log_callback.h>

#endif // #ifndef PCH_H_INCLUDED