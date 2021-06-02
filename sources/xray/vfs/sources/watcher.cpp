////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/intrusive_double_linked_list.h>
#include <xray/threading_functions_tls.h>
#include <xray/memory_stack_allocator.h>
#include <xray/type_predicates.h>
#include <xray/vfs/watcher.h>
#include <xray/fs/watcher_utils.h>

namespace xray {
namespace vfs {

using namespace fs_new;

#if XRAY_FS_NEW_WATCHER_ENABLED

vfs_watcher::vfs_watcher				(watcher_type_enum	type, 
										 mount_history_container *	mount_history, 
										 vfs_notification_callback	callback)
	:	m_fs_watcher					(type, NULL),
		m_mount_history					(mount_history), 
		m_callback						(callback)
{
	m_fs_watcher.set_callback			(boost::bind(& vfs_watcher::on_fs_watcher_notification, this, _1));
}

void   vfs_watcher::on_fs_watcher_notification (fs_notification const & fs_notification)
{
	vfs_notification * notification		=	0;
	u32 index							=	0;
	do {
		memory::stack_allocator				stack_allocator;
		u32 const arena_size			=	fs_notification.size() * 2;
		pstr const arena				=	(pstr)ALLOCA(arena_size);
		stack_allocator.initialize			(arena, arena_size, "temp-alloc");

		notification					=	vfs_notification::create(& stack_allocator, m_mount_history, fs_notification, index);
		if ( !notification )
			continue;

		if ( notification->type == vfs_notification::type_added || 
			 notification->type == vfs_notification::type_modified  ||
			 notification->type == vfs_notification::type_renamed )
		{
			native_path_string const & path	=	(notification->type == vfs_notification::type_renamed) ?
													notification->physical_old_path : notification->physical_path;
			notification->do_resource_deassociation	=	!try_remove_deassociation_notifications(path);
		}

		m_callback							(* notification);
		XRAY_FREE_IMPL						(stack_allocator, notification);

		++index;
	} while ( notification );
}

vfs_watcher::~vfs_watcher				()
{
	m_subscribers.for_each					(delete_predicate(& debug::g_mt_allocator));
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED