////////////////////////////////////////////////////////////////////////////
//	Module 		: defines.h
//	Created 	: 23.04.2008
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : important macro definitions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_DEFINES_H_INCLUDED
#define CS_DEFINES_H_INCLUDED

////////////////////////////////////////////////////////////////////////////
// STATIC_CHECK
////////////////////////////////////////////////////////////////////////////
#ifndef STATIC_CHECK
#	define STATIC_CHECK(expr, msg)			typedef char ERROR_##msg[1][2*(expr)-1]
#endif // #ifndef STATIC_CHECK

////////////////////////////////////////////////////////////////////////////
// DECLSPEC_NOVTABLE
////////////////////////////////////////////////////////////////////////////
#ifndef DECLSPEC_NOVTABLE
#	if (_MSC_VER >= 1100) && defined(__cplusplus)
#		define DECLSPEC_NOVTABLE			__declspec(novtable)
#	else // #if (_MSC_VER >= 1100) && defined(__cplusplus)
#		define DECLSPEC_NOVTABLE
#	endif // #if (_MSC_VER >= 1100) && defined(__cplusplus)
#endif // #ifndef DECLSPEC_NOVTABLE

////////////////////////////////////////////////////////////////////////////
// CS_STRING_CONCAT + CS_STRING_CONCAT_HELPER
////////////////////////////////////////////////////////////////////////////
#if defined(CS_STRING_CONCAT) || defined(CS_STRING_CONCAT_HELPER)
#	error do not define CS_STRING_CONCAT and CS_STRING_CONCAT_HELPER macros
#endif // #if defined(CS_STRING_CONCAT) || defined(CS_STRING_CONCAT_HELPER)

#define CS_STRING_CONCAT_HELPER(a,b)		a##b
#define CS_STRING_CONCAT(a,b)				CS_STRING_CONCAT_HELPER(a,b)

////////////////////////////////////////////////////////////////////////////
// CS_MAKE_STRING
////////////////////////////////////////////////////////////////////////////
#if defined(CS_MAKE_STRING) || defined(CS_MAKE_STRING_HELPER)
#	error do not define CS_MAKE_STRING and CS_MAKE_STRING_HELPER macros
#endif // #if defined(CS_MAKE_STRING) || defined(CS_MAKE_STRING_HELPER)

#define CS_MAKE_STRING_HELPER(a)			#a
#define CS_MAKE_STRING(a)					CS_MAKE_STRING_HELPER(a)

#endif // #ifndef CS_DEFINES_H_INCLUDED