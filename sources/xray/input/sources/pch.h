////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_INPUT_BUILDING

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_LOG_MODULE_INITIATOR	"input"
#include <xray/extensions.h>

#include "input_memory.h"

#endif // #ifndef PCH_H_INCLUDED