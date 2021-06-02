////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

static xray::command_line::key		s_floating_point_control_disabled("floating_point_control_disabled", "", "math", "disables floating point control flags setup for each thread");

namespace xray {
namespace math {

void on_thread_spawn	( )
{
	if ( s_floating_point_control_disabled )
		return;

	errno_t		error;
	
	error		= _controlfp_s( 0, _PC_24, MCW_PC );
	R_ASSERT_U	( !error );

	error		= _controlfp_s( 0, _RC_NEAR, MCW_RC );
	R_ASSERT_U	( !error );

#if XRAY_PLATFORM_WINDOWS
	_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
#endif

	error		= _controlfp_s( 0, _IC_PROJECTIVE, _MCW_IC );
	R_ASSERT_U	( !error );

	error		= _controlfp_s( 0, _DN_FLUSH, _MCW_DN );
	R_ASSERT_U	( !error );

#if XRAY_PLATFORM_WINDOWS
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif
}

} // namespace math
} // namespace xray