////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug.h"

namespace xray {
namespace core {
namespace debug {

xray::command_line::key		g_fpe_disabled("fpe_disabled", "", "math", "turns off fpu exceptions");

struct debug_preinitializer
{
	debug_preinitializer	( )
	{
		preinitialize		( );
	}
};

static debug_preinitializer	s_preinitializer;

} // namespace debug
} // namespace core
} // namespace xray