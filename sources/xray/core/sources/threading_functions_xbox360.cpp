////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_preinclude.h>
#include <xray/os_include.h>

namespace xray {
namespace threading {

void set_current_thread_affinity_impl	(u32 const hardware_thread)
{
	XSetThreadProcessor				(GetCurrentThread(), hardware_thread);
}

u32 actual_core_count ( )
{
	return							6;
}

} // namespace threading
} // namespace xray