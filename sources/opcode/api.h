////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OPCODE_API_H_INCLUDED
#define OPCODE_API_H_INCLUDED

#ifndef ICE_NO_DLL
#	ifdef XRAY_STATIC_LIBRARIES
#		define	ICE_NO_DLL
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#else // #ifndef ICE_NO_DLL
#	ifdef XRAY_STATIC_LIBRARIES
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef ICE_NO_DLL

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#include <boost/noncopyable.hpp>
#include <xray/macro_extensions.h>
#include <xray/optimization_extensions.h>
#include <xray/type_extensions.h>
#include <xray/debug/extensions.h>
#include <xray/detail_noncopyable.h>
#include <xray/memory_extensions.h>

#undef BOOL
#undef ASSERT
#include "opcode.h"

#endif // #ifndef OPCODE_API_H_INCLUDED