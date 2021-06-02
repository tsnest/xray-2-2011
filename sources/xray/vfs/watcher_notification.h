////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_WATCHER_NOTIFICATION_H_INCLUDED
#define VFS_WATCHER_NOTIFICATION_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/vfs/api.h>
#include <xray/fs/native_path_string.h>
#include <xray/vfs/mount_history.h>
#include <xray/fs/watcher_notification.h>

namespace xray {
namespace vfs {

struct XRAY_VFS_API vfs_notification
{
	enum			type_enum			{	type_unset,
											type_added, 
											type_removed,
											type_modified,
											type_renamed,	};

	vfs_notification						();

	// factory functions:
	static vfs_notification *	create	(memory::base_allocator *				allocator, 
										 mount_history_container *				mount_history,
										 type_enum								type,
										 fs_new::native_path_string const &		physical_path,
										 u32									index);

	static vfs_notification *	create	(memory::base_allocator *				allocator, 
										 type_enum								type,
										 fs_new::native_path_string const &		physical_path,
										 fs_new::virtual_path_string const &	virtual_path);

	static vfs_notification *	create	(memory::base_allocator *				allocator, 
										 vfs_notification const &				other);

	static vfs_notification *	create	(memory::base_allocator *				allocator, 
										 mount_history_container *				mount_history,
										 fs_new::fs_notification const &		other,
										 u32									index);

	static vfs_notification *	create_rename	
										(memory::base_allocator *				allocator, 
										 mount_history_container *				mount_history,
										 fs_new::native_path_string const &		old_path,
										 fs_new::native_path_string const &		new_path,
										 u32									index);

	static vfs_notification *	create_rename	
										(memory::base_allocator *				allocator, 
										 fs_new::native_path_string const &		physical_old_path,
										 fs_new::virtual_path_string const &	virtual_old_path,
										 fs_new::native_path_string const &		new_path,
										 fs_new::virtual_path_string const &	virtual_new_path);

public:
	type_enum							type;
	pstr								physical_path;
	pstr								physical_old_path;
	pstr								physical_new_path;
	pstr								virtual_path;
	pstr								virtual_old_path;
	pstr								virtual_new_path;
	bool								do_resource_deassociation;

	vfs_notification *					next;
};

typedef boost::function< void (vfs_notification const & info) >	vfs_notification_callback;

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef FS_WATCHER_NOTIFICATION_H_INCLUDED