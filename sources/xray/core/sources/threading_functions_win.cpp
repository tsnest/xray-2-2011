////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_include.h>

namespace xray {
namespace threading {

void set_current_thread_affinity_impl	(u32 const hardware_thread)
{
#pragma message(XRAY_TODO("Lain 2 Lain: dont forget to uncomment back after debugging"))
	//SetThreadAffinityMask			(GetCurrentThread(), 1 << hardware_thread);	
}

u32 actual_core_count ( )
{
	SYSTEM_INFO						system_info;
	GetSystemInfo					( &system_info );
	return							system_info.dwNumberOfProcessors;
}

} // namespace threading
} // namespace xray