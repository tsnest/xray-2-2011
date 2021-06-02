////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WATCHER_INTERNAL_H_INCLUDED
#define FS_WATCHER_INTERNAL_H_INCLUDED

#include <xray/fs_watcher.h>

namespace xray {
namespace fs {

struct watcher_notification
{
	watcher_notification	(file_change_info const & info, file_change_callback callback) : info(info), callback(callback) {}
	file_change_info		info;
	file_change_callback	callback;
	watcher_notification *	next;
};

bool					is_watcher_enabled					();
bool					is_watcher_initialized				();
void   					initialize_watcher					();
void   					finalize_watcher					();
void   					watch_physical_directory			(pcstr physical_path);
void   					quit_watching_physical_directory	(pcstr physical_path);
void					wakeup_watcher_thread				();
u32						watcher_thread_id					();

// when the file is created/modified by the program
// we rarely need fs watcher callback to break association 
// between the fat-node and associated resource
// so this functionality is used to track files opened by the program
// and not to break associations with resources
void					skip_deassociation_notifications		(file_type* physical_path, u32 notifications_to_skip);
void					skip_deassociation_notifications		(pcstr physical_path, u32 notifications_to_skip);
bool					try_remove_deassociation_notifications	(pcstr physical_path, u32 notifications_to_skip = 1);
FILE *					file_type_to_FILE						(file_type * file);
u32						open_file_notifications_count			(pcstr const portable_path, enum_flags<open_file_enum> const open_flags);

} // namespace fs
} // namespace xray

#endif // #ifndef FS_WATCHER_INTERNAL_H_INCLUDED
