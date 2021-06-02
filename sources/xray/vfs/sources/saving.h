////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SAVING_H_INCLUDED
#define VFS_SAVING_H_INCLUDED

#include <xray/vfs/saving_args.h>
#include "saving_info_tree.h"
#include "saving_environment.h"
#include <xray/vfs/saving_fat_inline_data.h>
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

class archive_saver : private core::noncopyable
{
public:
	archive_saver 						(save_archive_args &		args, 
										 vfs_hashset &				hashset);
	archive_saver 						(save_archive_args &		args, 
										 vfs_hashset &				hashset,
										 saving_environment &		environment);

	bool	finished_successfully		() const { return m_finished_successfully; }

private:
	bool			save_db				();

	void			save_node_file		(fat_node_info * *				out_duplicate,
							 			 file_size_type *				out_pos,
							 			 bool *							out_is_compressed,
							 			 mutable_buffer *				out_inlined_data,
							 			 u32 *							out_compressed_size,
							 			 u32 *							out_file_hash,
							 			 base_node<> *					node, 
 							 			 fat_node_info * 				node_info,
							 			 fs_new::native_path_string const & sub_fat_path,
							 			 u32							sub_fat_size);

	void			save_sub_fat		(fs_new::native_path_string *	out_sub_fat_path,
							 			 u32 *							out_sub_fat_size,
							 			 base_node<> *					cur, 
							 			 fat_node_info *				info_cur);
	void			save_archive_part	(fs_new::native_path_string *	out_archive_part_name,
										 u32 *							out_archive_fat_size,
										 base_node<> *					node, 
										 fat_node_info *				node_info);

	fat_node_info *	find_duplicate_folder	(fat_node_info * node_info);
	fat_node_info *   find_duplicate_folder_for_patch	(base_node<> * node);
	fat_node_info *		find_node_info_by_path			(fs_new::virtual_path_string const & node_path);
	fat_node_info * find_duplicate_file		(fat_node_info * node_info);

	template <platform_pointer_enum T>
	base_node<T> *		save_nodes		(base_node<> *			node, 
									     fat_node_info *		node_info,
						 			     file_size_type			parent_offs);
	template <platform_pointer_enum T>
	void		save_children_nodes		(base_node<> *			node, 
										 fat_node_info *		node_info,
										 base_folder_node<T> *	temp_folder,
										 file_size_type			new_parent_offs);

	bool								saving_patch() const { return m_args.to_vfs != NULL; }

private:
	saving_environment const &			env() const { return m_env; }

private:
	save_archive_args &					m_args;
	saving_environment					m_env;
	vfs_hashset &						m_hashset;
	file_size_type						m_mount_root_offs;
	bool								m_finished_successfully;
	bool								m_do_reverse_bytes;
	bool								m_root_save_db; // false for sub-fats

	friend void instantiator ();
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SAVING_H_INCLUDED