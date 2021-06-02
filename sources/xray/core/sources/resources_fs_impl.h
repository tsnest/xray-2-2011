////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_FS_IMPL_H_INCLUDED
#define RESOURCES_FS_IMPL_H_INCLUDED

#include <xray/resources_fs.h>
#include <xray/resources_fs_iterator.h>
#include <xray/fs_path.h>
#include <xray/fs_watcher.h>
#include <xray/intrusive_list.h>

#include "fs_file_system.h"

namespace xray {
namespace resources {

//----------------------------------------------------------
// mount task
//----------------------------------------------------------
	
class fs_task;
struct fs_task_base
{
	fs_task_base				() : next(NULL) {}
	fs_task *					next;
}; // struct fs_task_base

class fs_task : public fs_task_base
{
public:
	enum	type_enum		{	type_undefined, 
								type_fs_iterator_task,
								type_mount_operations_start,
								type_update_file_size_in_fat, 
								type_update_disk_node_name,
								type_erase_file,
								type_mount_disk, 
								type_mount_db, 
								type_mount_disk_node, 
								type_unmount_disk, 
								type_unmount_db,
								type_unmount_disk_node,
								type_mount_composite, // list of mount/unmount operations
								type_mount_operations_end,
								// take care to insert all mount operations between guards
							};

	type_enum					type;

#ifndef MASTER_GOLD
	fs::file_change_info		file_change_info;
#endif // #ifndef MASTER_GOLD

	fs::path_string 			logical_path;
	fs::path_string & 			fs_iterator_path;
	fs::path_string 			disk_dir_path;
	fs::path_string & 			physical_path;
	fs::path_string & 			fat_file_path;
	fs::path_string & 			old_physical_path;
	fs::path_string 			db_file_path;
	fs::path_string &			new_physical_path;
	bool						watch_folder;
	query_mount_callback		mount_callback;
	query_fs_iterator_callback	fs_iterator_callback;

	intrusive_list<fs_task_base, fs_task *, & fs_task_base::next>	children;
	memory::base_allocator *	allocator;
	query_result_for_cook *		parent;
	u32							thread_id;
	bool						result;
	bool						continue_mount_disk;
	bool  						need_replication;
	bool						reverse_byte_order;
	bool						fs_iterator_recursive;
	bool						fs_iterator_ready;
	bool						call_from_get_path_info;

	fs_task						(type_enum type);

	void						add_child			(fs_task * const child) { children.push_back(child); }
	bool						execute_may_destroy_this	();
	void						call_user_callback	();
	void						on_fs_iterator_ready();
	bool						parent_is_helper_query_for_mount () const { return parent && parent->is_helper_query_for_mount(); }

	bool						is_mount_task		() const
	{
		 return (type > type_mount_operations_start) && (type < type_mount_operations_end);
	}

private:
	void operator =				(fs_task const &) const;
	
	fs::find_results_struct		m_find_results;
};


} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_FS_IMPL_H_INCLUDED