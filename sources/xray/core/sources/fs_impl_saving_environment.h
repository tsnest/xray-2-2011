////////////////////////////////////////////////////////////////////////////
//	Created		: 16.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_SAVING_ENVIRONMENT_H_INCLUDED
#define FS_SAVING_ENVIRONMENT_H_INCLUDED

#include "fs_impl_saving_info_tree.h"

namespace xray {
namespace fs {

struct	saving_environment 
{
// should be filled when passing to save_db
	file_system_impl *						fs;
	path_string	 							fat_disk_path;
	path_string	 							logical_path;
	file_system::db_callback				callback;
	file_type *		 						fat_file;
	file_type *								db_file;
	fat_node_info *							info_root;
	u32										fat_alignment;
	u32										fat_part_max_size;
	bool		 							no_duplicates;
	file_system::db_target_platform_enum	db_format;
	memory::base_allocator *				alloc;
	compressor *							compressor;
	float									compress_smallest_rate;
	fat_inline_data *						inline_data;
	file_size_type							offset_for_file_positions;

// are filled automatically
	char *									dest_buffer;
	fat_node_info_set *						node_info_set;
	file_size_type			    			dest_buffer_size;
	u32										cur_offs;
	u32										node_index;
	u32										num_nodes;
	u32										result_fat_size;

	saving_environment () 
		: fs(NULL), fat_file(NULL), db_file(NULL), info_root(NULL), fat_alignment(0),
		fat_part_max_size(u32(-1)), no_duplicates(false), db_format(file_system::db_target_platform_unset),
		alloc(NULL), compressor(NULL), compress_smallest_rate(0.f), inline_data(NULL),
		offset_for_file_positions(0), dest_buffer(NULL), node_info_set(NULL),
		dest_buffer_size(0), cur_offs(0), node_index(0), num_nodes(0), result_fat_size(0) {}
};

} // namespace fs
} // namespace xray

#endif // #ifndef FS_SAVING_ENVIRONMENT_H_INCLUDED