////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_IMPL_SAVING_H_INCLUDED
#define FS_FILE_SYSTEM_IMPL_SAVING_H_INCLUDED

#include "fs_impl_saving_info_tree.h"

namespace xray {
namespace fs {

template <platform_pointer_enum target_pointer_size>
class save_db_impl
{
public:
	static fat_node<target_pointer_size> * 
						save_nodes						(saving_environment &	env, 
														 fat_node<> *			cur, 
														 fat_node_info *		info_cur,
														 file_size_type			prev_offs, 
														 file_size_type			parent_offs);

	static void				save_children_nodes			(saving_environment &	env, 
							 							 fat_node<> *			node, 
														 fat_node_info *		node_info,
														 fat_folder_node<target_pointer_size> * temp_folder,
														 file_size_type			new_parent_offs);
};

class save_db_helper
{
public:
	static void				save_node_file				(saving_environment &	env, 
								     					 fat_node_info * *		out_duplicate,
									 					 file_size_type *		out_pos,
									 					 bool *					out_is_compressed,
									 					 mutable_buffer *		out_inlined_data,
									 					 u32 *					out_compressed_size,
									 					 u32 *					out_file_hash,
									 					 fat_node<> *			node, 
 									 					 fat_node_info * 		node_info,
									 					 pcstr					sub_fat_path,
									 					 u32					sub_fat_size);

	static void				save_sub_fat				(saving_environment &	env, 
														 path_string *			out_sub_fat_path,
									 					 u32 *					out_sub_fat_size,
									 					 fat_node<> *			cur, 
									 					 fat_node_info *		info_cur);

	static fat_node_info *	find_duplicate_folder		(saving_environment & env, fat_node_info * node_info);

}; 

} // namespace fs
} // namespace xray

#endif // #ifndef FS_FILE_SYSTEM_IMPL_SAVING_H_INCLUDED