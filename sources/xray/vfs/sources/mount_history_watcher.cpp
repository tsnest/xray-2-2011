////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/vfs/mount_history.h>
#include "mount_root_node_functions.h"
#include <xray/vfs/watcher.h>

namespace xray {
namespace vfs {

using namespace fs_new;

template <class history_predicate>
void   foreach_history_item 			(mount_history_container &	history,
										 history_predicate const	action)
{
}

enum	action_enum	{ action_quit_watching, action_watch };

struct action
{
	action(vfs_watcher * watcher, action_enum const action) : m_watcher(watcher), m_action(action) {}

	void	operator () (vfs_mount * mount) const
	{
		mount_root_node_base<> * const root_node	=	mount->get_mount_root();

		if ( root_node->watcher_enabled != watcher_enabled_true )
			return;

		if ( root_node->mount_type != mount_type_physical_path )
			return;

		if ( m_action == action_quit_watching )
			m_watcher->quit_watching_physical_directory	(root_node->physical_path);
		else
			m_watcher->watch_physical_directory			(root_node->physical_path);
	}
	
	vfs_watcher *							m_watcher;
	action_enum								m_action;
}; // remove_folder_watchers_predicate

void   quit_watching_history			(vfs_watcher * watcher, mount_history_container &	history)
{
	if ( !watcher )
		return;
	history.for_each						(action(watcher, action_quit_watching));
}

void   watch_history					(vfs_watcher * watcher, mount_history_container & history)
{
	if ( !watcher )
		return;
	history.for_each						(action(watcher, action_watch));
}

} // namespace vfs
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED
