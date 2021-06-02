////////////////////////////////////////////////////////////////////////////
//	Created		: 17.10.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/simple_synchronous_interface.h>

namespace xray { 
namespace fs_new {

simple_synchronous_interface::simple_synchronous_interface	(watcher_enabled_bool watcher_enabled)
	:	hdd_sync(& hdd_device, watcher_enabled),
		dvd_sync(& dvd_device, watcher_enabled),
		hdd_async(& hdd_sync),
		dvd_async(& dvd_sync)
{

}

} // namespace fs_new
} // namespace xray