////////////////////////////////////////////////////////////////////////////
//	Module 		: library_names.h
//	Created 	: 19.03.2009
//  Modified 	: 19.03.2009
//	Author		: Dmitriy Iassenev
//	Description : library names macro definitions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_LIBRARY_NAMES_H_INCLUDED
#define CS_LIBRARY_NAMES_H_INCLUDED

#include <cs/config.h>

#ifdef CS_STATIC_LIBRARIES
#	define CS_STATIC_ID			[static]

#	ifdef _DLL
#		define CS_CRT_ID		[crt_dynamic]
#	else // #ifdef _DLL
#		define CS_CRT_ID		[crt_static]
#	endif // #ifdef _DLL

#	ifdef CS_NO_SCRIPTS
#		define CS_SCRIPT_ID		[no_scripts]
#	else // #ifdef CS_NO_SCRIPTS
#		define CS_SCRIPT_ID
#	endif // #ifdef CS_NO_SCRIPTS
#else // #ifdef CS_STATIC_LIBRARIES

#	ifdef CS_SINGLE_DLL
#		define CS_STATIC_ID		[single_dll]
#	else // #ifdef CS_SINGLE_DLL
#		define CS_STATIC_ID
#	endif // #ifdef CS_SINGLE_DLL

#	define CS_CRT_ID
#	define CS_SCRIPT_ID
#	ifndef _DLL
#		error Please use only release CRT DLL
#	endif // #ifdef _DLL
#endif // #ifdef CS_STATIC_LIBRARIES

#if defined(__SNC__)
#	define	CS_COMPILER_ID		[snc]
#elif defined(__GCC__)
#	define	CS_COMPILER_ID		[gcc]
#else // #elif defined(__GCC__)
#	define	CS_COMPILER_ID
#endif // #if defined(__SNC__)

#if CS_DEBUG_LIBRARIES
#	define CS_CONFIGURATION_ID	(debug)
#else // #if CS_DEBUG_LIBRARIES
#	define CS_CONFIGURATION_ID
#endif // #if CS_DEBUG_LIBRARIES

#ifdef _MSC_VER
#	define CS_LIBRARY_PREFIX	cs.
#else // #ifdef _MSC_VER
#	define CS_LIBRARY_PREFIX	/../cs.
#endif // #ifdef _MSC_VER

#ifdef _MSC_VER
#	define CS_LIBRARY_NAME(library, extension)	\
		CS_MAKE_STRING(\
			CS_STRING_CONCAT(\
				CS_LIBRARY_PREFIX,\
				CS_STRING_CONCAT(\
					library,\
					CS_STRING_CONCAT(\
						CS_COMPILER_ID,\
						CS_STRING_CONCAT(\
							CS_STATIC_ID,\
							CS_STRING_CONCAT(\
								CS_CRT_ID,\
								CS_STRING_CONCAT(\
									CS_SCRIPT_ID,\
									CS_STRING_CONCAT(\
										CS_CONFIGURATION_ID,\
										CS_STRING_CONCAT(\
											.,\
											extension\
										)\
									)\
								)\
							)\
						)\
					)\
				)\
			)\
		)
#else // #ifdef _MSC_VER
#	define CS_LIBRARY_NAME(library, extension)	\
		CS_MAKE_STRING(\
			CS_STRING_CONCAT(\
				CS_LIBRARY_PREFIX,\
				CS_STRING_CONCAT(\
					library,\
					CS_STRING_CONCAT(\
						CS_COMPILER_ID,\
						CS_STRING_CONCAT(\
							CS_STATIC_ID,\
							CS_STRING_CONCAT(\
								CS_CRT_ID,\
								CS_STRING_CONCAT(\
									CS_SCRIPT_ID,\
									CS_CONFIGURATION_ID\
								)\
							)\
						)\
					)\
				)\
			)\
		)
#endif // #ifdef _MSC_VER

#endif // #ifndef CS_LIBRARY_NAMES_H_INCLUDED