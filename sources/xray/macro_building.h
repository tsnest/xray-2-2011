////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_BUILDING_H_INCLUDED
#define XRAY_MACRO_BUILDING_H_INCLUDED

#ifndef XRAY_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_CORE_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		if XRAY_CORE_BUILDING
#			define XRAY_CORE_API				XRAY_DLL_EXPORT
#		else // #if XRAY_CORE_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_CORE_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_CORE_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #if XRAY_CORE_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_API

#endif // #ifndef XRAY_MACRO_BUILDING_H_INCLUDED