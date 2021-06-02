////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "vfs_pack.h"
#include "vfs_pack_application.h"
#include <xray/vfs/locked_iterator.h>

using namespace xray;
using namespace	fs_new;
using namespace	vfs;

namespace vfs_pack {

bool	application::unpack				()
{
	path_string								unpack_dir;
	if ( !unpack_command.is_set_as_string(& unpack_dir) )
	{
		printf								("you must set '%s' to point to directory where you want to unpack\n", 
											 unpack_command.full_name());
		return								false;
	}

	native_path_string						sources_string;
	if ( !read_sources_string(sources_command, & sources_string) )
		return								false;

	u32 const sources_count				=	sources_string.count_of(';') + 1;
	sources_array							sources(ALLOCA(sizeof(native_path_string) * sources_count), sources_count);
	parse_sources							(sources_string, & sources);

	mount_ptrs_array	mount_ptrs			((vfs_mount_ptr *)ALLOCA(sizeof(vfs_mount_ptr) * sources.size()), sources.size());
	virtual_file_system		vfs;
	mount_sources							(vfs, m_fs_devices->hdd_sync, sources, mount_ptrs, & g_vfs_pack_allocator, & m_log_format, m_log_flags);

	vfs_locked_iterator						iterator;
	result_enum const find_result		=	vfs.find_async("", & iterator, find_recursively, & g_vfs_pack_allocator, NULL);
	R_ASSERT								(find_result == result_success);

	unpack_callback callback			=	NULL;
	if ( m_debug_info != debug_info_disabled )
		callback						=	boost::bind(& db_callback, _1, _2, _3, _4, & m_log_format, m_log_flags, m_debug_info);
	
	ppmd_compressor		compressor			(& g_vfs_pack_allocator, 1);
	unpack_arguments	unpack_args			("", native_path_string::convert(unpack_dir.c_str()), 
											 m_fs_devices->hdd_sync,
											 compressor,
											 & g_vfs_pack_allocator,
											 callback,
											 & m_log_format, 
											 m_log_flags);

	bool const result					=	vfs.unpack(unpack_args);
	printf									("unpacking to '%s' %s\n", 
											 unpack_dir.c_str(), result ? "successfull" : "failed");

	iterator.clear							();
	mount_ptrs.clear						();

	vfs.user_finalize						();

	return									true;
}

} // namespace vfs_pack

