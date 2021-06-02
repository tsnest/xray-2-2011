////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/fs/watcher_notification.h>

namespace xray {
namespace fs_new {

fs_notification::fs_notification	()	:	type(type_unset), 
											next(NULL), 
											path(NULL), 
											new_path(NULL), 
											old_path(NULL)
{

}

fs_notification *	fs_notification::create	(memory::base_allocator *		allocator, 
											 fs_notification const &		other)
{
	if ( other.type == type_renamed )
		return								create_rename(allocator, other.old_path, other.new_path);
	
	return									create(allocator, other.type, other.path);
}

fs_notification *   fs_notification::create	(memory::base_allocator * const			allocator, 
											 type_enum const						type,
											 fs_new::native_path_string const &		path)
{
	R_ASSERT								(path);

	u32 const size						=	sizeof(fs_notification) + path.length() + 1;
	fs_notification * const notification	=	(fs_notification *)XRAY_MALLOC_IMPL(allocator, size, "fs_notification");
	new (notification)						fs_notification();
	notification->type					=	type;

	notification->path					=	(pstr)notification + sizeof(fs_notification);
	strings::copy							(notification->path, path.length() + 1, path.c_str());

	return									notification;
}

fs_notification *   fs_notification::create_rename	(memory::base_allocator *			allocator, 
													 fs_new::native_path_string const & old_path,
													 fs_new::native_path_string const & new_path)
{
	R_ASSERT								(old_path);
	R_ASSERT								(new_path);

	u32 const size						=	sizeof(fs_notification) +	new_path.length() + 1 + 
																		old_path.length() + 1;

	fs_notification * const notification	=	(fs_notification *)XRAY_MALLOC_IMPL(allocator, size, "fs_notification");
	new (notification)						fs_notification();

	notification->old_path		=	(pstr)notification + sizeof(fs_notification);
		strings::copy						(notification->old_path, 
											 old_path.length() + 1, 
											 old_path.c_str());

	notification->new_path		=	notification->old_path + old_path.length() + 1;
		strings::copy						(notification->new_path, 
											 new_path.length() + 1, 
											 new_path.c_str());

	return									notification;
}

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED