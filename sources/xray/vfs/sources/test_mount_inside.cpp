////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "test.h"
#include <xray/vfs/watcher.h>
#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include "mount_archive.h"
#include "find.h"
#include "branch_locks.h"

namespace xray {
namespace vfs {

using namespace		fs_new;

bool   vfs_test::test_inside_mount_impl	()
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
	int mount_order_iteration			=	value % 24;

	int mount_i0 = 0, mount_i1 = 0, mount_i2 = 0, mount_i3 = 0;
	int mount_o0 = 0, mount_o1 = 0, mount_o2 = 0, mount_o3 = 0;
	indexes_from_iteration					(mount_iteration, & mount_i0, & mount_i1, & mount_i2, & mount_i3);
	indexes_from_iteration					(mount_order_iteration, & mount_o0, & mount_o1, & mount_o2, & mount_o3);

	LOG_FORCED								(logging::info, "iteration %d", s_iteration);
	LOG_FORCED								(logging::info, "mount: %d, %d, %d, %d mount order: %d, %d, %d, %d",
											 mount_i0, mount_i1, mount_i2, mount_i3, mount_o0, mount_o1, mount_o2, mount_o3);

	memory::base_allocator * const allocator	=	& memory::g_mt_allocator;

	native_path_string mount_test_path	=	native_path_string::convert(m_suite->get_resources_path());
	char const separator				=	native_path_string::separator;
	mount_test_path.appendf					("%ctests%cvfs%cmount_inside", separator, separator, separator);

	native_path_string	paths[4];
	for ( int i=0; i<4; ++i )
		paths[i].assignf					("%s%cmount%d", mount_test_path.c_str(), separator, i);

	query_mount_arguments		mount_args[] = 
		{	query_mount_arguments::mount_physical_path
				(allocator, add_W ? "W" : "", paths[0], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true),
			query_mount_arguments::mount_physical_path
				(allocator, add_W ? "W/B" : "B", paths[1], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true),
			query_mount_arguments::mount_physical_path
				(allocator, add_W ? "W" : "", paths[2], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true),
			query_mount_arguments::mount_physical_path
				(allocator, add_W ? "W/B" : "B", paths[3], "", m_device.c_ptr(), NULL, & mount_callback, recursive_true)	};


	virtual_file_system						test_vfs;

	vfs_mount_ptr 	mount_ptr0			=	query_mount_and_wait(m_vfs, mount_args[mount_i0]).mount;
 	log_vfs_root							(m_vfs);
 	vfs_mount_ptr 	mount_ptr1			=	query_mount_and_wait(m_vfs, mount_args[mount_i1]).mount;
 	log_vfs_root							(m_vfs);
	vfs_mount_ptr 	mount_ptr2			=	query_mount_and_wait(m_vfs, mount_args[mount_i2]).mount;
 	log_vfs_root							(m_vfs);
	vfs_mount_ptr 	mount_ptr3			=	query_mount_and_wait(m_vfs, mount_args[mount_i3]).mount;
 	log_vfs_root							(m_vfs);

	mount_args[mount_i0].mount_id		=	1;
	mount_args[mount_i1].mount_id		=	2;
	mount_args[mount_i2].mount_id		=	3;
	mount_args[mount_i3].mount_id		=	4;

	LOG_INFO								("*****************************************************************");

	vfs_mount_ptr 	test_ptr0			=	query_mount_and_wait(test_vfs, mount_args[mount_o0]).mount;
 	log_vfs_root							(test_vfs);
 	vfs_mount_ptr 	test_ptr1			=	query_mount_and_wait(test_vfs, mount_args[mount_o1]).mount;
 	log_vfs_root							(test_vfs);
	vfs_mount_ptr 	test_ptr2			=	query_mount_and_wait(test_vfs, mount_args[mount_o2]).mount;
 	log_vfs_root							(test_vfs);
	vfs_mount_ptr 	test_ptr3			=	query_mount_and_wait(test_vfs, mount_args[mount_o3]).mount;
 	log_vfs_root							(test_vfs);

	m_vfs.assert_equal						(test_vfs);
 
	mount_ptr1							=	NULL;
   	log_vfs_root							(m_vfs);
	mount_ptr3							=	NULL;
   	log_vfs_root							(m_vfs);
	mount_ptr2							=	NULL;
  	log_vfs_root							(m_vfs);
	mount_ptr0							=	NULL;
  	log_vfs_root							(m_vfs, true);

	test_ptr0							=	NULL;
   	log_vfs_root							(test_vfs);
	test_ptr1							=	NULL;
   	log_vfs_root							(test_vfs);
	test_ptr2							=	NULL;
  	log_vfs_root							(test_vfs);
	test_ptr3							=	NULL;
  	log_vfs_root							(test_vfs, true);
 	
	++s_iteration;
	return									s_iteration < max_iteration;
}

void   vfs_test::test_inside_mount		()
{
   	while ( test_inside_mount_impl() )
	{;}
}

} // namespace vfs
} // namespace xray

