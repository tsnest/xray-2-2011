////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_PACK_ARCHIVE_H_INCLUDED
#define VFS_PACK_ARCHIVE_H_INCLUDED

#include <xray/fs/synchronous_device_interface.h>
#include <xray/vfs/mount_ptr.h>
#include <xray/vfs/types.h>
#include <xray/vfs/saving_args.h>

namespace xray {
namespace vfs {

enum   debug_info_enum					{ debug_info_disabled, debug_info_normal, debug_info_detail };

typedef	buffer_vector<fs_new::native_path_string>	sources_array;
typedef	xray::buffer_vector<vfs_mount_ptr>	mount_ptrs_array;

class virtual_file_system;
struct fat_node_info;

struct pack_archive_args : private core::noncopyable
{
	virtual_file_system *					vfs;
	fs_new::native_path_string				sources;
	fs_new::native_path_string				config_file;
	fs_new::native_path_string				target_db;
	fs_new::native_path_string				target_fat;
	fs_new::synchronous_device_interface &	synchronous_device;
	debug_info_enum							debug_info;
	memory::base_allocator *				allocator;
	logging::log_flags_enum					log_flags;
	logging::log_format *					log_format;
	u32										fat_part_max_size;
	u32										archive_part_max_size;
	float									compression_rate;
	u32										fat_align;
	save_flags_enum							flags;
	archive_platform_enum					platform;
	bool									clean_temp_files;
	// for patch:
	virtual_file_system *					from_vfs;	
	fat_node_info *							from_vfs_info_tree;
	virtual_file_system *					to_vfs;	
	fat_node_info *							to_vfs_info_tree;

	pack_archive_args					(fs_new::synchronous_device_interface &		synchronous_device,
										 logging::log_format * const				log_format,
										 logging::log_flags_enum const				log_flags)
		:	vfs								(NULL),
			synchronous_device				(synchronous_device),
			flags							((save_flags_enum)0),
			fat_part_max_size				(u32(-1)),
			archive_part_max_size			(u32(-1)),
			compression_rate				(0),
			fat_align						(2048),
			platform						(archive_platform_unset),
			allocator						(NULL),
			log_flags						(log_flags),
			log_format						(log_format),
			from_vfs						(NULL),
			from_vfs_info_tree				(NULL),
			to_vfs							(NULL),
			to_vfs_info_tree				(NULL),
			clean_temp_files				(true)
	{
	}
};

bool   pack_archive						(pack_archive_args & args);

void   parse_sources					(fs_new::native_path_string const &		sources_string, 
										 sources_array * const					out_sources);

bool   mount_sources					(virtual_file_system &					vfs,
										 fs_new::synchronous_device_interface & synchronous_device, 
										 sources_array &						sources, 
										 mount_ptrs_array &						mount_ptrs,
										 memory::base_allocator *				allocator,
										 logging::log_format *					log_format,
										 logging::log_flags_enum				log_flags);

void   db_callback						(u32									num_nodes, 
										 u32									whole_nodes, 
										 pcstr									name, 
										 u32									flags, 
										 logging::log_format *					log_format,
										 logging::log_flags_enum				log_flags,
										 debug_info_enum						debug_info);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_PACK_ARCHIVE_H_INCLUDED