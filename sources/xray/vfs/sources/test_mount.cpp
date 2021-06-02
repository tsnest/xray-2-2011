////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include "test.h"
#include <xray/vfs/watcher.h>
#include "mount_archive.h"
#include "find.h"
#include "branch_locks.h"

namespace xray {
namespace vfs {

using namespace fs_new;

bool   vfs_test::test_mount_unmount_impl	()
{
	int max_iteration					=	24 * 24 * 2;
	static int s_iteration				=	0;
	int value							=	s_iteration;
	bool add_W							=	false;
	if ( value >= 24*24 )
	{
		add_W							=	true;
		value							-=	24*24;
	}

	int mount_iteration					=	value / 24;
	int unmount_iteration				=	value % 24;

	int mount_i[4] = {0,0,0,0};
	
	int unmount_i[4] = {0,0,0,0};
	indexes_from_iteration					(mount_iteration, & mount_i[0], & mount_i[1], & mount_i[2], & mount_i[3]);
	indexes_from_iteration					(unmount_iteration, & unmount_i[0], & unmount_i[1], & unmount_i[2], & unmount_i[3]);

	LOG_FORCED								(logging::info, "iteration %d", s_iteration);
	LOG_FORCED								(logging::info, "mount order: %d, %d, %d, %d unmount order: %d, %d, %d, %d (adding W:%d)",
											 mount_i[0], mount_i[1], mount_i[2], mount_i[3], unmount_i[0], unmount_i[1], unmount_i[2], unmount_i[3], add_W);

	memory::base_allocator * const allocator	=	& memory::g_mt_allocator;

	native_path_string mount_test_path	=	native_path_string::convert(m_suite->get_resources_path());
	char const separator				=	native_path_string::separator;
	mount_test_path.appendf					("%ctests%cvfs%cmount", separator, separator, separator);

	native_path_string	paths[4];
	for ( int i=0; i<4; ++i )
		paths[i].assignf					("%s%cmount%d", mount_test_path.c_str(), separator, i);

 	query_mount_arguments		mount_args[] = {	
		query_mount_arguments::mount_physical_path(allocator, 
		add_W ? "W" : "", paths[0], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true),													

		query_mount_arguments::mount_physical_path(allocator, 
		add_W ? "W/B" : "B", paths[1], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true),

		query_mount_arguments::mount_physical_path(allocator, 
		add_W ? "W" : "", paths[2], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true),
		
		query_mount_arguments::mount_physical_path(allocator, 
		add_W ? "W/B" : "B", paths[3], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true)	};

	vfs_mount_ptr	mount_ptrs[4];
	mount_ptrs[0]						=	query_mount_and_wait(m_vfs, mount_args[mount_i[0]]).mount;
 	log_vfs_root							(m_vfs);
 	mount_ptrs[1]						=	query_mount_and_wait(m_vfs, mount_args[mount_i[1]]).mount;
 	log_vfs_root							(m_vfs);
	mount_ptrs[2]						=	query_mount_and_wait(m_vfs, mount_args[mount_i[2]]).mount;
 	log_vfs_root							(m_vfs);
	mount_ptrs[3]						=	query_mount_and_wait(m_vfs, mount_args[mount_i[3]]).mount;
 	log_vfs_root							(m_vfs);

	mount_ptrs[unmount_i[0]]			=	NULL;
   	log_vfs_root							(m_vfs);
	mount_ptrs[unmount_i[1]]			=	NULL;
   	log_vfs_root							(m_vfs);
	mount_ptrs[unmount_i[2]]			=	NULL;
  	log_vfs_root							(m_vfs);
	mount_ptrs[unmount_i[3]]			=	NULL;
  	log_vfs_root							(m_vfs, true);
 	
	++s_iteration;
	return									s_iteration < max_iteration;
}

void   vfs_test::test_mount_unmount		()
{
	while ( test_mount_unmount_impl() )
	{;}
}

} // namespace vfs
} // namespace xray