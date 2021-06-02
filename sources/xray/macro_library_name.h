////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_LIBRARY_NAME_H_INCLUDED
#define XRAY_MACRO_LIBRARY_NAME_H_INCLUDED

#include <xray/macro_extensions.h>

#ifdef XRAY_LIBRARY_PREFIX
#	error please do not define XRAY_LIBRARY_PREFIX macro
#endif // #ifdef XRAY_LIBRARY_PREFIX

#ifdef _MSC_VER
#	define XRAY_LIBRARY_PREFIX		xray_
#else // #ifdef _MSC_VER
#	define XRAY_LIBRARY_PREFIX		/../xray_
#endif // #ifdef _MSC_VER

#ifdef XRAY_STATIC_LIBRARIES
#	ifdef MASTER_GOLD
#		define XRAY_STATIC_ID		-static-gold
#	else // #ifdef MASTER_GOLD
#		define XRAY_STATIC_ID		-static
#	endif // #ifdef MASTER_GOLD
#else // #ifdef XRAY_STATIC_LIBRARIES
#	define XRAY_STATIC_ID
#endif // #ifdef XRAY_STATIC_LIBRARIES

#ifdef DEBUG
#	define XRAY_DEBUG_LIBRARIES
#	define XRAY_CONFIGURATION_ID	-debug
#else // #ifdef DEBUG
#	define XRAY_CONFIGURATION_ID
#endif // #ifdef DEBUG

#ifdef _MSC_VER
#	define XRAY_LIBRARY_NAME(library, extension)	\
		XRAY_MAKE_STRING(\
			XRAY_STRING_CONCAT(\
				XRAY_LIBRARY_PREFIX,\
				XRAY_STRING_CONCAT(\
					library,\
					XRAY_STRING_CONCAT(\
						XRAY_STATIC_ID,\
						XRAY_STRING_CONCAT(\
							XRAY_CONFIGURATION_ID,\
							XRAY_STRING_CONCAT(\
								.,\
								extension\
							)\
						)\
					)\
				)\
			)\
		)
#else // #ifdef _MSC_VER
#	if defined(__SNC__)
#		define XRAY_COMPILER_ID	-snc
#	elif defined(__GCC__) // #if defined(__SNC__)
#		define XRAY_COMPILER_ID	-gcc
#	else // #elif defined(__GCC__)
#		error define your XRAY_COMPILER_ID here
#	endif // #if defined(__SNC__)

#	define XRAY_LIBRARY_NAME(library, extension)	\
		XRAY_MAKE_STRING(\
			XRAY_STRING_CONCAT(\
				XRAY_LIBRARY_PREFIX,\
				XRAY_STRING_CONCAT(\
					library,\
					XRAY_STRING_CONCAT(\
						XRAY_COMPILER_ID,\
						XRAY_STRING_CONCAT(\
							XRAY_STATIC_ID,\
							XRAY_CONFIGURATION_ID\
						)\
					)\
				)\
			)\
		)
#endif // #ifdef _MSC_VER

#endif // #ifndef XRAY_MACRO_LIBRARY_NAME_H_INCLUDED