////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_UTILS_H_INCLUDED
#define VFS_MOUNT_UTILS_H_INCLUDED

#include "nodes.h"

namespace xray {
namespace vfs {

typedef	intrusive_list< base_node<>, base_node<>::node_pointer, & base_node<>::m_next, threading::single_threading_policy >
	node_list;
// note: using m_next is essential, as node_list is used to populate children nodes

class	vfs_hashset;

void	relink_separated_children		(base_node<> * overlapper, u32 separator_mount_id, base_node<> * overlapped);
void	separate_folders_by_file_node	(vfs_hashset & hashset, pcstr path, u32 hash, 
										 base_node<> * last_overlapper, u32 separator_mount_id);//, base_node<> * overlapped);

u32		next_mount_id					();
u32		next_mount_operation_id			();

void	exchange_nodes					(fs_new::virtual_path_string const &	virtual_path,
										 u32									virtual_path_hash,
										 vfs_hashset & 							hashset,
										 base_node<> * 							erase_node, 
										 base_node<> * 							replace_node, 
										 base_node<> * 							overlapper);

void	replace_and_free_what_node		(fs_new::virtual_path_string const &	virtual_path,
										 u32 									virtual_path_hash,
										 virtual_file_system & 					file_system,
										 base_node<> * 							what_node, 
										 base_node<> * 							with_node, 
										 base_node<> * 							overlapper,
										 base_node<> * 							root_write_lock,
										 memory::base_allocator *				allocator);

base_node<> *   find_node_of_mount		(vfs_hashset &							hashset,
										 fs_new::virtual_path_string const &	virtual_path,
										 u32 const								virtual_path_hash,
										 u32 const								mount_id);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_UTILS_H_INCLUDED