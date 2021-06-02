////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SAVING_ARGS_H_INCLUDED
#define VFS_SAVING_ARGS_H_INCLUDED

#include <xray/vfs/saving_fat_inline_data.h>
#include <xray/vfs/types.h>
#include <xray/fs/native_path_string.h>
#include <xray/compressor.h>
#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace vfs {

class virtual_file_system;
struct fat_node_info;

enum save_flags_enum { save_flag_forbid_folder_links,
					   save_flag_forbid_empty_files	};

struct save_archive_args : private core::noncopyable
{
	fs_new::synchronous_device_interface &	device;
	fs_new::native_path_string const &		fat_path;
	fs_new::native_path_string const &		archive_path;
	save_flags_enum							flags;
	u32										fat_alignment;
	memory::base_allocator *				allocator;
	xray::compressor *						compressor;
	float									compress_smallest_rate;
	archive_platform_enum					archive_platform;
	fat_inline_data &						inline_data;
	u32										fat_part_max_size;
	u32										archive_part_max_size;
	save_archive_callback					callback;
	logging::log_flags_enum					log_flags;
	logging::log_format *					log_format;
	virtual_file_system *					from_vfs;
	fat_node_info *							from_vfs_info_tree;
	virtual_file_system *					to_vfs;
	fat_node_info *							to_vfs_info_tree;


	save_archive_args					(fs_new::synchronous_device_interface &	device,
										 fs_new::native_path_string const &		fat_path,
										 fs_new::native_path_string const &		archive_path,
										 save_flags_enum						flags,
										 u32									fat_alignment,
										 memory::base_allocator *				allocator,
										 xray::compressor *						compressor,
										 float									compress_smallest_rate,
										 archive_platform_enum					archive_platform,
										 fat_inline_data &						inline_data,
										 u32									fat_part_max_size,
										 u32									archive_part_max_size,
										 save_archive_callback					callback,
										 logging::log_format *					log_format			=	NULL,
										 logging::log_flags_enum				log_flags			=	(logging::log_flags_enum)0,
										 virtual_file_system *					from_vfs			=	NULL,
										 fat_node_info *						from_vfs_info_tree	=	NULL,
										 virtual_file_system *					to_vfs				=	NULL,
										 fat_node_info *						to_vfs_info_tree	=	NULL)
	: device(device),
	  fat_path(fat_path),
	  archive_path(archive_path),
	  flags(flags),
	  fat_alignment(fat_alignment),
	  allocator(allocator),
	  compressor(compressor),
	  compress_smallest_rate(compress_smallest_rate),
	  archive_platform(archive_platform),
	  inline_data(inline_data),
	  fat_part_max_size(fat_part_max_size),
	  archive_part_max_size(archive_part_max_size),
	  callback(callback),
	  from_vfs(from_vfs),
	  from_vfs_info_tree(from_vfs_info_tree),
	  to_vfs(to_vfs),
	  to_vfs_info_tree(to_vfs_info_tree),
	  log_flags(log_flags),
	  log_format(log_format)	{}
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SAVING_ARGS_H_INCLUDED