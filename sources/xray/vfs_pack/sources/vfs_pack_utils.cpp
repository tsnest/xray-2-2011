////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "vfs_pack.h"

using namespace xray;
using namespace	xray::fs_new;
using namespace	xray::vfs;
using namespace	xray::resources;

namespace vfs_pack {

command_line::key	debug_info_command				("debug_info", "", "vfs_pack", "outputs debug info", "normal|detail");
command_line::key	fat_command						("fat", "", "vfs_pack", "fat-dest file");
command_line::key	db_command						("db", "", "vfs_pack", "db-dest file");
command_line::key	unpack_command					("unpack", "", "vfs_pack", "unpack", "unpack dest dir");
command_line::key	pack_command					("pack", "", "vfs_pack", "pack");
command_line::key	target_platform					("target_platform", "", "vfs_pack", "fat file compatible with specific platform", "pc|xbox360|ps3");
command_line::key	align_fat_command				("align_fat", "", "vfs_pack", "", "fat-size alignment");
command_line::key	archive_rate_command			("archive_rate", "", "vfs_pack", "", "archiving rate which is small enough to archive");
command_line::key	fat_part_max_size_command		("fat_part_max_size", "", "vfs_pack", "", "subdivision of fat into parts criteria");
command_line::key	archive_part_max_size_command	("archive_part_max_size", "", "vfs_pack", "", "subdivision of archive into parts criteria");
command_line::key	sources_command					("sources", "src", "vfs_pack", "", "paths to sources separated with ';'");
command_line::key	patch_from_command				("patch_from", "", "vfs_pack", "", "paths separated with ';'");
command_line::key	patch_to_command				("patch_to", "", "vfs_pack", "", "paths separated with ';'");
command_line::key	patch_command					("patch", "", "vfs_pack", "", "path to output patch archive");

command_line::key	inline_config_command			("inline_config", "", "vfs_pack", "", "path to inline config");

bool   read_sources_string					(xray::command_line::key & key, native_path_string * const out_sources_string)
{
	fixed_string4096	paths;
	if ( !key.is_set_as_string(& paths) )
	{
		printf								("you must set '%s' key", key.full_name());
		return								false;
	}

	out_sources_string->assign_with_conversion	(paths.c_str());

	if ( !out_sources_string->length() )
	{
		printf								("you must assign '%s' key", key.full_name());
		return								false;
	}

	return									true;
}

} // namespace vfs_pack