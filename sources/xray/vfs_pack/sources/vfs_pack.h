////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_PACK_H_INCLUDED
#define VFS_PACK_H_INCLUDED

#include <xray/command_line_extensions.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
//#include "../../core/sources/vfs_mount_archive.h"
#include <xray/vfs/virtual_file_system.h>
#include <xray/fs/portable_path_string.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/physical_path_iterator.h>
#include <xray/threading_reader_writer_lock.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/physical_path_info.h>
#include "vfs_pack_memory.h"

namespace vfs_pack {

extern	xray::command_line::key				debug_info_command;
extern	xray::command_line::key				fat_command;
extern	xray::command_line::key				db_command;
extern	xray::command_line::key				unpack_command;
extern	xray::command_line::key				pack_command;
extern	xray::command_line::key				target_platform;
extern	xray::command_line::key				align_fat_command;
extern	xray::command_line::key				archive_rate_command;
extern	xray::command_line::key				fat_part_max_size_command;
extern	xray::command_line::key				archive_part_max_size_command;
extern	xray::command_line::key				sources_command;
extern	xray::command_line::key				patch_from_command;
extern	xray::command_line::key				patch_to_command;
extern	xray::command_line::key				patch_command;
extern	xray::command_line::key				inline_config_command;

typedef	xray::fixed_string< 260 >		path_string;

bool   read_sources_string				(xray::command_line::key &					key, 
										 xray::fs_new::native_path_string * const	out_sources_string);

} // namespace vfs_pack

#endif // #ifndef VFS_PACK_H_INCLUDED