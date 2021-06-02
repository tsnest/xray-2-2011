////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_IMPL_FINDING_H_INCLUDED
#define FS_IMPL_FINDING_H_INCLUDED

#include "fs_impl_finding_environment.h"

namespace xray {
namespace resources {
	void   query_continue_disk_mount (pcstr 					logical_path, 
									  pcstr 					disk_dir_path, 
									  bool						recursively,
									  query_mount_callback		callback,
									  memory::base_allocator *	allocator);
} // namespace resources
namespace fs {

void								query_recursive_mount_of_tree		(fat_node<> *			tree_root, 
																		 find_results_struct *	root_find_results,
																		 enum_flags<find_enum>	find_flags);

bool								can_traverse_tree_sync				(fat_node<> *	node, 
																		 enum_flags<find_enum> find_flags, 
																		 fat_node<> *	root_node);
void								find_async_across_link				(find_env &		env);
resources::fs_iterator				make_iterator						(fat_node<> *	node, 
																		 find_enum		find_flags);
fat_node<> *						try_sync_mounting					(fat_node<> *	node, 
																		 enum_flags<find_enum> find_flags, 
																		 bool			is_full_path,
																		 bool			traversing_leafs);

bool   								need_disk_mount						(fat_node<> *	node, 
																		 enum_flags<find_enum> find_flags, 
																		 bool			is_full_path, 
																		 bool			traversing_leafs);

void  								pin_sub_fats_of_ancestors			(int			change, 
																		 fat_node<> *	node);
void								pin_sub_fats_of_self_and_children	(int			change, 
																		 fat_node<> *	node, 
																		 find_enum		find_flags);
void								pin_sub_fats_of_node				(int			change, 
																		 fat_node<> *	node, 
																		 find_enum		find_flags);
void								pin_sub_fat_if_node_is_root_of_sub_fat	(int change, fat_node<> * node);

fat_node<> *						find_referenced_link_node			(fat_node<> *	node);

} // namespace fs
} // namespace xray

#endif // #ifndef FS_IMPL_FINDING_H_INCLUDED