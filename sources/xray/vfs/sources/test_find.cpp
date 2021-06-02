////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include <xray/testing_macro.h>

#include "test.h"
#include <xray/vfs/watcher.h>
#include "mount_archive.h"
#include "find.h"
#include "branch_locks.h"
#include <xray/math_permutations.h>

namespace xray {
namespace vfs {

using namespace		fs_new;

void    vfs_find_callback				(vfs_locked_iterator const &	it, 
										 result_enum					result, 
										 bool *							found,
										 virtual_file_system *			vfs)
{
	R_ASSERT_U								(it && result == result_success);
	LOG_TRACE								("find callback received! node:(%s)", it.get_name());
	log_vfs_root							(* vfs);
	* found								=	true;
}

bool   make_archive						(native_path_string const &			source_folder_path,
										 native_path_string const &			result_archive_path,
										 virtual_file_system &				vfs,
										 asynchronous_device_interface &	device)
{
	memory::base_allocator * const allocator	=	& memory::g_mt_allocator;
	query_mount_arguments	mount_args	=	query_mount_arguments::mount_physical_path
															(allocator, "", source_folder_path, "",
															 & device, NULL, & mount_callback, recursive_true);

	vfs_mount_ptr const mount_ptr		=	query_mount_and_wait(vfs, mount_args).mount;
	fs_new::synchronous_device_interface	synchronous_device	(& device, allocator);
	
	fixed_vector<fat_inline_data::item, 1024>	inline_container;
	fat_inline_data	inline_data				(& inline_container);

	save_archive_args	save_args			(synchronous_device,
											 result_archive_path,
											 result_archive_path,
											 (save_flags_enum)0, // flags
											 2048, // fat_align
											 allocator,
											 NULL, // compressor
											 0, // smallest_compress_rate
											 archive_platform_pc,
											 inline_data,
									 		 512, // fat_part_max_size
											 u32(-1),//512, // u32(-1), // archive_part_max_size
											 NULL);

	bool const result					=	vfs.save_archive(save_args);
	return									result;
}

void   vfs_test::test_find_impl			()
{
	memory::base_allocator * const allocator	=	& memory::g_mt_allocator;

	bool found							=	false;
	m_vfs.try_find_async					("", boost::bind(& vfs_find_callback, _1, _2, & found, & m_vfs),
  											 find_recursively, allocator);
  
  	while ( !found )
  	{
  		m_device->dispatch_callbacks		();
  		m_vfs.dispatch_callbacks			();
  	}
 
	LOG_TRACE								("OUT OF FIND CALLBACK");
 	log_vfs_root							(m_vfs);
}

void   vfs_test::test_find				()
{
	memory::base_allocator * const allocator	=	& memory::g_mt_allocator;

 	query_mount_arguments	mount_args[] = 
 	{	
		query_mount_arguments::mount_archive		(allocator, "", "bla", "", "", m_device.c_ptr(), NULL, & mount_callback),	
  		query_mount_arguments::mount_physical_path	(allocator, "", "bla", "", m_device.c_ptr(), NULL, & mount_callback, recursive_false),
 	};

	native_path_string find_test_path	=	native_path_string::convert(m_suite->get_resources_path());
	char const separator				=	native_path_string::separator;
	find_test_path.appendf					("%ctests%cvfs%cfind", separator, separator, separator);

	native_path_string						folder_path[4], archive_path[4];

	for ( int i=0; i<4; ++i )
	{
		folder_path[i].assignf				("%s%croot%d", find_test_path.c_str(), separator, i + 1);
		archive_path[i].assignf				("%s%cdb%d.le", find_test_path.c_str(), separator, i + 1);

		bool const archive_packed		=	make_archive(folder_path[i], archive_path[i], m_vfs, * m_device);
		TEST_ASSERT							(archive_packed);
	}
  
	vfs_mount_ptr mount_ptrs[4];
	int	permutation[4];
	math::first_permutation					(permutation, 8, 4);
	
	int iteration						=	0;

	int start_from_iteration			=	0;
	do 
	{
		if ( iteration < start_from_iteration )
		{
			++iteration;
			continue;
		}

		LOG_FORCED							(logging::info, "VFS:FIND TEST ITERATION %d", iteration);
			
		for ( int i=0; i<4; ++i )
		{
			int index					=	permutation[i] - 1;

			if ( index < 4 )
			{
				mount_args[1].physical_path		=	folder_path[index];
   				mount_ptrs[i]			=	query_mount_and_wait(m_vfs, mount_args[1]).mount;
			}
			else
			{
				mount_args[0].archive_physical_path	=	archive_path[index - 4];
   				mount_ptrs[i]			=	query_mount_and_wait(m_vfs, mount_args[0]).mount;
			}
		}

		test_find_impl						();

		for ( int i=0; i<4; ++i )
		{
	 		mount_ptrs[i]				=	NULL; // causes unmount
	 		log_vfs_root					(m_vfs);
		}

		LOG_TRACE							("LEAVED MOUNT POINTERS");
		if ( debug::is_debugger_present() )
 			log_vfs_root					(m_vfs, true);

		++iteration;
	} while ( math::next_permutation(permutation, 8, 4) );
}

} // namespace vfs
} // namespace xray