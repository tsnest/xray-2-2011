////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_CORE_BUILDING	1

#ifndef XRAY_STATIC_LIBRARIES
#	define XRAY_ENGINE_BUILDING
#endif // #ifndef XRAY_STATIC_LIBRARIES

#define XRAY_IN_PCH			1
#include <xray/extensions.h>
#undef XRAY_IN_PCH

#endif // #ifndef PCH_H_INCLUDED