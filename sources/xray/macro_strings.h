////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_STRINGS_H_INCLUDED
#define XRAY_MACRO_STRINGS_H_INCLUDED

// XRAY_STRING_CONCAT macro
#if defined(XRAY_STRING_CONCAT) || defined(XRAY_STRING_CONCAT_HELPER)
#	error please do not define XRAY_STRING_CONCAT or XRAY_STRING_CONCAT_HELPER macros
#endif // #if defined(XRAY_STRING_CONCAT) || defined(XRAY_STRING_CONCAT_HELPER)

#define XRAY_STRING_CONCAT_HELPER(a,b)	a##b
#define XRAY_STRING_CONCAT(a,b)			XRAY_STRING_CONCAT_HELPER(a,b)

// XRAY_MAKE_STRING macro
#if defined(XRAY_MAKE_STRING) || defined(XRAY_MAKE_STRING_HELPER)
#	error please do not define XRAY_MAKE_STRING or XRAY_MAKE_STRING_HELPER macros
#endif // #if defined(XRAY_MAKE_STRING) || defined(XRAY_MAKE_STRING_HELPER)

#define XRAY_MAKE_STRING_HELPER(a)		#a
#define XRAY_MAKE_STRING(a)				XRAY_MAKE_STRING_HELPER(a)

#endif // #ifndef XRAY_MACRO_STRINGS_H_INCLUDED