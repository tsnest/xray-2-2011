////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_DX9_RENDER_INCLUDE_H_INCLUDED
#define XRAY_RENDER_DX9_RENDER_INCLUDE_H_INCLUDED

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOGDI
#undef NOTEXTMETRIC
#include <xray/os_include.h>

#pragma warning(push)
#pragma warning(disable:4995)
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning(pop)


#define R_CHK( expression, ... )											\
	if ( xray::identity(true) ) {											\
	static bool ignore_always			= false;						\
	if ( !ignore_always && FAILED( xray::identity(expression) ) ) {		\
	bool do_debug_break = false;									\
	::xray::debug::on_error	(										\
	& do_debug_break,											\
	xray::process_error_true,									\
	&ignore_always,												\
	xray::assert_untyped,										\
	"assertion_failed",											\
	XRAY_MAKE_STRING( expression ),								\
	__FILE__,													\
	__FUNCTION__,												\
	__LINE__,													\
	__VA_ARGS__);												\
	if ( do_debug_break )											\
	DEBUG_BREAK();												\
	}																	\
	} else (void)0


//	Include this before any renderer-specific includes
//#include <xray/render/common/sources/namespace_wrapper.h>

#include <bitset>


#endif // #ifndef XRAY_RENDER_DX9_RENDER_INCLUDE_H_INCLUDED