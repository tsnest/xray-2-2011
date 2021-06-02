////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "vfs_pack.h"
#include "vfs_pack_application.h"

using namespace xray;
using namespace	fs_new;
using namespace	vfs;
using namespace	resources;

namespace vfs_pack {

bool   application::fill_pack_arguments	(pack_archive_args & args)
{
	if ( !args.vfs )
	{
		if ( !read_sources_string(sources_command, & args.sources) )
			return							false;
	}

	args.allocator						=	& g_vfs_pack_allocator;

	path_string								config_file;
	inline_config_command.is_set_as_string	(& config_file);
	args.config_file					=	native_path_string::convert(config_file.c_str());

	fat_part_max_size_command.is_set_as_number		(& args.fat_part_max_size);
	archive_part_max_size_command.is_set_as_number	(& args.archive_part_max_size);
	archive_rate_command.is_set_as_number			(& args.compression_rate);
	align_fat_command.is_set_as_number				(& args.fat_align);

	path_string								fat_file;
	path_string								db_file;
	fat_command.is_set_as_string			(& fat_file);
	db_command.is_set_as_string				(& db_file);
	if ( !db_file.length() )
		db_file							=	"vfs_pack_default_output.db";
	if ( !fat_file.length() )
		fat_file						=	db_file;

	args.target_db						=	native_path_string::convert(db_file.c_str());
	args.target_fat						=	native_path_string::convert(fat_file.c_str());

	args.platform						=	m_platform;

	return									true;
}

bool   application::pack				()
{
	pack_archive_args						args(m_fs_devices->hdd_sync, & m_log_format, m_log_flags);

	if ( !fill_pack_arguments(args) )
		return								false;

	if ( !pack_archive(args) )
		return								false;

	return									true;
}

} // namespace vfs_pack