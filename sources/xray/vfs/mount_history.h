////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MOUNT_HISTORY_H_INCLUDED
#define MOUNT_HISTORY_H_INCLUDED

#include <xray/fs/native_path_string.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/vfs/types.h>
#include <xray/vfs/mount_ptr.h>
#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace vfs {

typedef	intrusive_double_linked_list	<	vfs_mount, 
											vfs_mount *, 
											& vfs_mount::prev_mount_history, 
											& vfs_mount::next_mount_history,
											threading::simple_lock	>	mount_history_container;

class		query_mount_arguments;

class		virtual_file_system;
void		add_to_mount_history		(vfs_mount * new_item, virtual_file_system & file_system);
void  		erase_from_mount_history	(vfs_mount * item, virtual_file_system & file_system);

vfs_mount_ptr    find_in_mount_history	(fs_new::virtual_path_string const &	virtual_path, 
										 fs_new::native_path_string const &		fat_physical_path, 
										 mount_history_container &				history);
vfs_mount_ptr	 find_in_mount_history	(convertion_filter const &				filter,
										 mount_history_container &				history);


vfs_mount_ptr    find_sub_fat_parent	(fs_new::virtual_path_string const &	virtual_path, 
										 fs_new::native_path_string const &		fat_physical_path, 
										 mount_history_container &				history);

bool  convert_physical_to_virtual_path	(mount_history_container &				mount_history,
										 fs_new::virtual_path_string *			out_path, 
										 fs_new::native_path_string const &		path,
										 convertion_filter const &				filter = NULL,
										 vfs_mount_ptr *						out_mount_ptr = NULL);

bool  convert_physical_to_virtual_path	(mount_history_container &				mount_history,
										 fs_new::virtual_path_string *			out_path, 
										 fs_new::native_path_string const &		path,
										 u32 const								index,
										 vfs_mount_ptr *						out_mount_ptr = NULL);

bool  convert_virtual_to_physical_path	(mount_history_container &				mount_history,
										 fs_new::native_path_string *			out_path, 
										 fs_new::virtual_path_string const &	path,
										 convertion_filter const &				filter = NULL,
										 vfs_mount_ptr *						out_mount_ptr = NULL);

bool  convert_virtual_to_physical_path	(mount_history_container &				mount_history,
										 fs_new::native_path_string *			out_path, 
										 fs_new::virtual_path_string const &	path,
										 u32 const								index,
										 vfs_mount_ptr *						out_mount_ptr = NULL);


} // namespace vfs
} // namespace xray

#endif // #ifndef MOUNT_HISTORY_H_INCLUDED