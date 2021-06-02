////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/scheduled_to_unmount.h>

namespace xray {
namespace vfs {

void   dispatch_scheduled_to_unmount	(scheduled_to_unmount_container &	scheduled_to_unmount,
										 mount_history_container &			mount_history)
{
	if ( scheduled_to_unmount.empty() )
		return;

	mount_history_container::mutex_raii		mount_history_lock(mount_history.policy());

	// we need here a copy, because scheduled_to_unmount.clear
	// can trigger insertion to a itself while still in process of clearing
	scheduled_to_unmount_container			copy;
	scheduled_to_unmount.swap				(copy);

	while ( !copy.empty() )
		copy.pop_front						();
}

} // namespace vfs
} // namespace xray