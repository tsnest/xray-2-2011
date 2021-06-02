////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/vfs/watcher_notification.h>
#include <xray/vfs/mount_history.h>

namespace xray {
namespace vfs {

vfs_notification::vfs_notification	()	:	type				(type_unset), 
											next				(NULL), 
											virtual_path		(NULL), 
											virtual_new_path	(NULL), 
											virtual_old_path	(NULL),
											physical_path		(NULL), 
											physical_new_path	(NULL), 
											physical_old_path	(NULL),
											do_resource_deassociation	(false)
{

}

vfs_notification *	vfs_notification::create	(memory::base_allocator * const		allocator, 
												 vfs_notification const &			other)
{
	if ( other.type == type_renamed )
		return								create_rename(	allocator, 
															other.physical_old_path,
															other.virtual_old_path, 
															other.physical_new_path,
															other.virtual_new_path	);
	
	return									create(allocator, other.type, other.physical_path, other.virtual_path);
}

vfs_notification *	vfs_notification::create	(memory::base_allocator * const		allocator, 
												 mount_history_container * const	mount_history,
												 fs_new::fs_notification const &	other,
												 u32 const							index)
{
	if ( other.type == type_renamed )
		return								create_rename(	allocator, 
															mount_history, 
															other.old_path, 
															other.new_path,
															index	);
	
	return									create(allocator, mount_history, (type_enum)other.type, other.path, index);
}

vfs_notification *	 vfs_notification::create	(memory::base_allocator *				allocator, 
												 type_enum								type,
												 fs_new::native_path_string const &		physical_path,
												 fs_new::virtual_path_string const &	virtual_path)
{
	R_ASSERT								(physical_path.length());

	u32 const size						=	sizeof(vfs_notification) + 2 * (physical_path.length() + 1);
	vfs_notification * const notification	=	(vfs_notification *)XRAY_MALLOC_IMPL(allocator, size, "vfs_notification");
	new (notification)						vfs_notification();
	notification->type					=	type;

	pstr	buffer_pointer				=	(pstr)notification + sizeof(vfs_notification);
	notification->physical_path			=	buffer_pointer;
	buffer_pointer						+=	physical_path.length() + 1;
	notification->virtual_path			=	buffer_pointer;

	strings::copy							(notification->physical_path, physical_path.length() + 1, physical_path.c_str());
	strings::copy							(notification->virtual_path, virtual_path.length() + 1, virtual_path.c_str());

	return									notification;
}

vfs_notification *   vfs_notification::create	(memory::base_allocator * const			allocator, 
												 mount_history_container * const		mount_history,
												 type_enum								type,
												 fs_new::native_path_string const &		physical_path,
												 u32									index)
{
	fs_new::virtual_path_string				virtual_path;
	bool const convertion_result		=	convert_physical_to_virtual_path(* mount_history, & virtual_path, physical_path, index);
	if ( !convertion_result )
		return								NULL;

	return									create(allocator, type, physical_path, virtual_path);
}

vfs_notification *   vfs_notification::create_rename	(memory::base_allocator * const		allocator, 
														 mount_history_container * const	mount_history,
														 fs_new::native_path_string const & physical_old_path,
														 fs_new::native_path_string const & physical_new_path,
														 u32 const							index)
{
	fs_new::virtual_path_string				virtual_old_path;
	bool convertion_result				=	convert_physical_to_virtual_path(* mount_history, & virtual_old_path, physical_old_path, index);
	if ( !convertion_result )
		return								NULL;

	fs_new::virtual_path_string				virtual_new_path;
	convertion_result					=	convert_physical_to_virtual_path(* mount_history, & virtual_new_path, physical_new_path, index);
	if ( !convertion_result )
		return								NULL;

	return								create_rename(allocator, physical_old_path, virtual_old_path, physical_new_path, virtual_new_path);
}

vfs_notification *	 vfs_notification::create_rename	
										(memory::base_allocator *				allocator, 
										 fs_new::native_path_string const &		physical_old_path,
										 fs_new::virtual_path_string const &	virtual_old_path,
										 fs_new::native_path_string const &		physical_new_path,
										 fs_new::virtual_path_string const &	virtual_new_path)

{
	R_ASSERT								(physical_old_path.length());
	R_ASSERT								(physical_new_path.length());

	u32 const size						=	sizeof(vfs_notification) +	2 * (physical_new_path.length() + 1 + 
																			 physical_old_path.length() + 1);

	vfs_notification * const notification	=	(vfs_notification *)XRAY_MALLOC_IMPL(allocator, size, "vfs_notification");
	new (notification)						vfs_notification();
	notification->type					=	type_renamed;

	pstr	buffer_pointer				=	(pstr)notification + sizeof(vfs_notification);
	notification->physical_old_path		=	buffer_pointer;
	buffer_pointer						+=	physical_old_path.length() + 1;
	notification->physical_new_path		=	buffer_pointer;
	buffer_pointer						+=	physical_new_path.length() + 1;
	notification->virtual_old_path		=	buffer_pointer;
	buffer_pointer						+=	virtual_old_path.length() + 1;
	notification->virtual_new_path		=	buffer_pointer;
	buffer_pointer						+=	virtual_new_path.length() + 1;

	strings::copy							(notification->physical_old_path, physical_old_path.length() + 1, physical_old_path.c_str());
	strings::copy							(notification->physical_new_path, physical_new_path.length() + 1, physical_new_path.c_str());
	strings::copy							(notification->virtual_old_path, virtual_old_path.length() + 1, virtual_old_path.c_str());
	strings::copy							(notification->virtual_new_path, virtual_new_path.length() + 1, virtual_new_path.c_str());

	return									notification;
}

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED