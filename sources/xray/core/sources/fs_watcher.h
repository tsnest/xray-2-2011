////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WATCHER_H_INCLUDED
#define FS_WATCHER_H_INCLUDED

namespace xray {
namespace fs {

struct	file_change_info
{
	enum				change_type_enum	{	change_type_unset,
												change_type_added, 
												change_type_removed,
												change_type_modified,
												change_type_renamed,	};
	change_type_enum	type;
	path_string			file_path;
	path_string			renamed_old_file_path;
	path_string			renamed_new_file_path;
};

typedef fastdelegate::FastDelegate< void (file_change_info const & info) >	file_change_callback;

bool					watcher_enabled						();
void   					initialize_watcher					();
void   					finalize_watcher					();
void   					watch_physical_directory			(pcstr const physical_path);
void   					quit_watching_physical_directory	(pcstr const physical_path);
void					wakeup_watcher_thread				();
u32						watcher_thread_id					();

} // namespace fs
} // namespace xray

#endif // #ifndef FS_WATCHER_H_INCLUDED