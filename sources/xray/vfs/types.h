////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_VFS_TYPES_H_INCLUDED
#define XRAY_VFS_VFS_TYPES_H_INCLUDED

namespace xray {
namespace vfs {

enum	store_in_mount_history_bool	{ 	store_in_mount_history_false, store_in_mount_history_true	};
enum	mount_type_enum				{	mount_type_unknown, 
										mount_type_physical_path, 
										mount_type_archive			};
enum	submount_type_enum			{	submount_type_unset,
										submount_type_lazy, 
										submount_type_subfat, 
										submount_type_automatic_archive, 
										submount_type_hot_mount, 
										submount_type_hot_unmount,	};

enum recursive_bool						{	recursive_false, recursive_true	};
enum result_enum						{	result_fail, result_success, result_need_async, result_out_of_memory, result_cannot_lock	};

enum	lock_type_enum					{	lock_type_uninitialized, 
											lock_type_read, 
											lock_type_write,
											lock_type_soft_read, 
											lock_type_soft_write	};

enum	lock_operation_enum				{	lock_operation_lock, 
											lock_operation_try_lock, 
										};

typedef	boost::function< void (u32 const saved_count, u32 all_count, pcstr name, u32 flags) >
											save_archive_callback;

enum	archive_platform_enum			{	archive_platform_unset, 
											archive_platform_pc, 
											archive_platform_xbox360, 
											archive_platform_ps3,		};

typedef boost::function< bool (pcstr description, pcstr physical_path, pcstr virtual_path) >	convertion_filter;

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_VFS_VFS_TYPES_H_INCLUDED