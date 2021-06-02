////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SAVING_ENVIRONMENT_H_INCLUDED
#define VFS_SAVING_ENVIRONMENT_H_INCLUDED

#include <xray/compressor.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/file_type.h>
#include "saving_info_tree.h"

namespace xray {
namespace vfs {

inline 
bool   is_big_endian_platform	(archive_platform_enum const platform)
{ 
	return platform == archive_platform_xbox360 || platform == archive_platform_ps3; 
};

struct	saving_environment
{
// should be filled when passing to save_db
	fs_new::virtual_path_string				virtual_path;
	base_node<> *							save_root; // virtual_path ^ save_root == 1 

	fs_new::file_type *		 				fat_file;
	fs_new::file_type *						db_file;
	fat_node_info *							info_root;

// are filled automatically
	pstr									dest_buffer;
	fat_node_info_set *						node_info_set;
	file_size_type			    			dest_buffer_size;
	u32										cur_offs;
	u32										node_index;
	u32										num_nodes;
	u32										result_fat_size;
	u32										next_archive_part;
	u32										archive_parts_count;
	u32										temp_name_index;
	fs_new::synchronous_device_interface *	device;

	saving_environment (fs_new::synchronous_device_interface & device)
		: fat_file(NULL), db_file(NULL), info_root(NULL), next_archive_part(1), 
		archive_parts_count(1), save_root(NULL), dest_buffer(NULL), 
		node_info_set(NULL), dest_buffer_size(0), cur_offs(0), temp_name_index(u32(-1)),
		node_index(0), num_nodes(0), result_fat_size(0), device(& device) {}
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SAVING_ENVIRONMENT_H_INCLUDED