////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SCHEDULED_TO_UNMOUNT_H_INCLUDED
#define VFS_SCHEDULED_TO_UNMOUNT_H_INCLUDED

#include <xray/vfs/mount_ptr.h>
#include <xray/vfs/mount_history.h>
#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace vfs {

class virtual_file_system;
typedef	intrusive_list	<	vfs_mount, 
							vfs_mount_ptr, 
							& vfs_mount::next_scheduled_to_unmount,
							threading::simple_lock	>	scheduled_to_unmount_container;

void   dispatch_scheduled_to_unmount	(scheduled_to_unmount_container &	scheduled_to_unmount,
										 mount_history_container &			mount_history);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SCHEDULED_TO_UNMOUNT_H_INCLUDED