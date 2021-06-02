////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/debug/debug.h>
#include <xray/debug/macros.h>
#include <xray/linkage_helper.h>
#include "utils.h"

#if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
#	pragma warning( push )
#	pragma warning( disable : 4074 )
#	pragma init_seg( compiler )
#	pragma warning( pop )
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360
#else // #elseif XRAY_PLATFORM_PS3
#	error please define your platform
#endif // #if XRAY_PLATFORM_WINDOWS || XRAY_PLATFORM_XBOX_360

XRAY_DECLARE_LINKAGE_ID(debug_initialize);

namespace xray {
namespace debug {

void			preinitialize		( );

struct debug_preinitializer
{
	debug_preinitializer	( )
	{
		preinitialize		( );
	}
};

static debug_preinitializer	s_preinitializer XRAY_INIT_PRIORITY(0);

} // namespace debug
} // namespace xray