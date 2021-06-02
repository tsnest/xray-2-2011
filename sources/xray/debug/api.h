////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_API_H_INCLUDED
#define XRAY_DEBUG_API_H_INCLUDED

#ifndef XRAY_DEBUG_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_DEBUG_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_DEBUG_BUILDING
#			define XRAY_DEBUG_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_DEBUG_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_DEBUG_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_DEBUG_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_DEBUG_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_DEBUG_API

#endif // #ifndef XRAY_DEBUG_API_H_INCLUDED