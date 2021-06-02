////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MACRO_STATIC_CHECK_H_INCLUDED
#define XRAY_MACRO_STATIC_CHECK_H_INCLUDED

#ifdef COMPILE_ASSERT
#	error do not define COMPILE_ASSERT macro
#endif // #if defined(WINDOWS_32) && defined(XBOX_360)

#define COMPILE_ASSERT(expr, msg)	typedef char ERROR_##msg[1][(expr)]

#endif // #ifndef XRAY_MACRO_STATIC_CHECK_H_INCLUDED