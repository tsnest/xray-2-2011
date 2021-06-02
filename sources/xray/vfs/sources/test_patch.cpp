////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test.h"
#include "patch.h"
#include <xray/testing_macro.h>
#include <xray/fs/device_utils.h>
#include <xray/fs/file_type_pointer.h>

namespace xray {
namespace vfs {

using namespace fs_new;

u32	s_archive_part_from					=	0;
u32	s_fat_part_from						=	0;
u32	s_archive_part_patch				=	0;
u32	s_fat_part_patch					=	0;


class test_patch_impl : core::noncopyable
{
public:
	test_patch_impl::test_patch_impl	(native_path_string const &			from_path,
										 native_path_string const &			to_path,
										 native_path_string const &			patch_path,
										 synchronous_device_interface & 	synchronous_device,
										 memory::base_allocator * const		allocator)
	:	m_from_path				(from_path), 
		m_to_path				(to_path), 
		m_patch_path			(patch_path), 
		m_synchronous_device	(synchronous_device), 
		m_allocator				(allocator)
{
	s_archive_part_from					=	4096 + 4096 * (rand() % 2);
	s_fat_part_from						=	512 + 512 * (rand() % 3);
	s_archive_part_patch				=	4096 + 4096 * (rand() % 2);
	s_fat_part_patch					=	512 + 512 * (rand() % 3);

	create_random_patch_resources			();
 
	create_from_db							();

 	create_test_patch						();

	query_mount_arguments patch_mount_args	=	
 		query_mount_arguments::mount_archive	(m_allocator, "", m_patch_path, "", "", NULL, & m_synchronous_device, & mount_callback);

	query_mount_arguments from_mount_args	=	
	 	query_mount_arguments::mount_archive	(m_allocator, "", m_from_db, "", "", NULL, & m_synchronous_device, & mount_callback);

	query_mount_arguments to_mount_args	=	
  		query_mount_arguments::mount_physical_path	(m_allocator, "", m_to_path, "", NULL, & m_synchronous_device, & mount_callback, recursive_false);

	virtual_file_system						original_vfs;
	vfs_mount_ptr to_mount				=	query_mount_and_wait(original_vfs, to_mount_args).mount;

	virtual_file_system						patched_vfs;
	vfs_mount_ptr from_mount			=	query_mount_and_wait(patched_vfs, from_mount_args).mount;
	vfs_mount_ptr patch_mount			=	query_mount_and_wait(patched_vfs, patch_mount_args).mount;

	vfs_locked_iterator		original_iterator;
	result_enum const original_result	=	original_vfs.find_async("", & original_iterator, find_recursively, m_allocator, NULL);
	R_ASSERT_U								(original_result == result_success);

	vfs_locked_iterator		patch_iterator;
	result_enum const patch_result		=	patched_vfs.find_async("", & patch_iterator, find_recursively, m_allocator, NULL);
	R_ASSERT_U								(patch_result == result_success);

	LOGI_INFO								("test", "PATCHED VFS:");
	log_vfs_root							(patched_vfs);

	// just for debug:
	//create_to_db							();

	original_vfs.assert_equal				(patched_vfs, false);

	patch_iterator.clear					();
	original_iterator.clear					();

	from_mount							=	NULL;
	patch_mount							=	NULL;
	to_mount							=	NULL;

	original_vfs.user_finalize				();
	patched_vfs.user_finalize				();
}

void   create_from_db					()
{
	pack_archive_args	args				(m_synchronous_device, NULL, (logging::log_flags_enum)0);
	args.allocator						=	m_allocator;
	args.archive_part_max_size			=	s_archive_part_from;
	args.fat_part_max_size				=	s_fat_part_from;
	args.fat_align						=	2048;
	args.sources						=	m_from_path;

	m_from_db							=	m_from_path;
	m_from_db							+=	".db";

	args.platform						=	archive_platform_pc;
	args.target_db						=	m_from_db;
	args.target_fat						=	m_from_db;

	bool const made_from_db				=	pack_archive(args);
	TEST_ASSERT								(made_from_db);
}

void   create_to_db						()
{
	pack_archive_args	args				(m_synchronous_device, NULL, (logging::log_flags_enum)0);
	args.allocator						=	m_allocator;
	args.archive_part_max_size			=	s_archive_part_patch;
	args.fat_part_max_size				=	s_fat_part_patch;
	args.fat_align						=	2048;
	args.sources						=	m_to_path;

	native_path_string to_db			=	m_to_path;
	to_db								+=	".db";

	args.platform						=	archive_platform_pc;
	args.target_db						=	to_db;
	args.target_fat						=	to_db;

	bool const made_to_db				=	pack_archive(args);
	TEST_ASSERT								(made_to_db);

	query_mount_arguments to_mount_args	=	
	 	query_mount_arguments::mount_archive	(m_allocator, "", to_db, "", "", NULL, & m_synchronous_device, & mount_callback);

	virtual_file_system	* to_vfs		=	XRAY_NEW_IMPL(m_allocator, virtual_file_system);
	vfs_mount_ptr to_mount				=	query_mount_and_wait(* to_vfs, to_mount_args).mount;

	vfs_locked_iterator		to_iterator;
	result_enum const find_result		=	to_vfs->find_async("", & to_iterator, find_recursively, m_allocator, NULL);
	R_ASSERT_U								(find_result == result_success);

	LOGI_INFO								("test", "TO.DB VFS:");
	log_vfs_root							(* to_vfs);

	to_iterator.clear						();
	to_mount							=	NULL;
	to_vfs->user_finalize					();

	XRAY_DELETE_IMPL						(m_allocator, to_vfs);
}

bool   create_file						(native_path_string const & path, u32 const file_index)
{
	if ( !create_folder_r(m_synchronous_device, path, false) )
		return								false;

	mutable_buffer const data			=	m_files_data[file_index];

	file_type_pointer	file				(path, m_synchronous_device, file_mode::create_always, file_access::write);
	if ( !file )
		return								false;

	m_synchronous_device->write				(file, data.c_ptr(), data.size());
	return									true;
}

bool   create_equal_files				(native_path_string const & child_from, 
										 native_path_string const & child_to,
										 u32 const					file_index)
{
	if ( !create_file(child_from, file_index) )
		return								false;
	if ( !create_file(child_to, file_index) )
		return								false;

	return									true;
}

bool   create_random_file				(native_path_string const & path, u32 const file_index)
{
	if ( !create_file(path, file_index) )
		return								false;

	return									true;
}

void   create_random_patch_resources	()
{
	for ( int i=0; i<files_count; ++i )
	{
		u32 const file_size				=	rand() % 1000;
		m_files_data[i].assign				(file_size, m_allocator, "file_data");
		for ( u32 j=0; j<file_size; ++j )
			m_files_data[i][j]			=	'a' + (rand() % 20);

		m_files_names[i]				=	'a' + (rand() % 20);
	}

	erase_r									(m_synchronous_device, m_from_path);
	erase_r									(m_synchronous_device, m_to_path);
	
	bool created_folder					=	false;
	created_folder						=	create_folder_r(m_synchronous_device, m_from_path, true);
	R_ASSERT								(created_folder);
	created_folder						=	create_folder_r(m_synchronous_device, m_to_path, true);
	R_ASSERT								(created_folder);
	
	u32 const files_total				=	1 + rand() % 500;
	for ( u32 i=0; i<files_total; ++i )
	{
		u32 const depth					=	1 + (rand() % 5);
		native_path_string	path;
		u32 const from_file_index		=	rand() % files_count;
		u32 const to_file_index			=	rand() % files_count;
		for ( u32 i=0; i<depth; ++i )
		{
			path						+=	native_path_string::separator;
			char const name				=	(i == depth - 1) ? m_files_names[from_file_index] : ('a' + (rand() % 20));
			path						+=	name;
		}

		native_path_string  child_from	=	m_from_path;
		child_from						+=	path;
		native_path_string  child_to	=	m_to_path;
		child_to						+=	path;

		if ( m_synchronous_device->get_physical_path_info(child_from).exists() )
			continue;
		if ( m_synchronous_device->get_physical_path_info(child_to).exists() )
			continue;

		switch ( rand() % 3 )
		{
		case 0:	
			if ( rand() % 2 )
			{
				if ( rand() % 2 )
					create_equal_files		(child_from, child_to, from_file_index);
				else
				{
					create_random_file		(child_from, from_file_index);
					create_random_file		(child_to, to_file_index);
				}
			}
			else
			{
				switch ( rand() % 3 )
				{
				case 0:
					create_random_file		(child_from, from_file_index);
					create_folder_r			(m_synchronous_device, child_to, true);
					break;
				case 1:
					create_folder_r			(m_synchronous_device, child_from, true);
					create_random_file		(child_to, to_file_index);
					break;
				case 2:
					create_folder_r			(m_synchronous_device, child_from, true);
					create_folder_r			(m_synchronous_device, child_to, true);
					break;
				}
			}
			break;
		case 1:
			create_random_file				(child_from, from_file_index);
			break;

		case 2:
			create_random_file				(child_to, to_file_index);
			break;
		}
	}
}

void   create_test_patch				()
{
	patch_args								args(m_synchronous_device, NULL, (logging::log_flags_enum)0);
	args.allocator						=	m_allocator;
	args.archive_part_max_size			=	s_archive_part_patch;
	args.fat_part_max_size				=	s_fat_part_patch;
	args.fat_align						=	2048;
	args.from_sources					=	m_from_db;
	args.to_sources						=	m_to_path;
	args.platform						=	archive_platform_pc;
	args.target_db						=	m_patch_path;
	args.target_fat						=	m_patch_path;

	bool const made_patch				=	make_patch(args);
	TEST_ASSERT								(made_patch);
}

private:
	native_path_string const &			m_from_path;
	native_path_string					m_from_db;
	native_path_string const &			m_to_path;
	native_path_string const &			m_patch_path;
	synchronous_device_interface &		m_synchronous_device;
	memory::base_allocator * const		m_allocator;
	enum { files_count	=	15	};
	allocated_buffer					m_files_data[files_count];
	char								m_files_names[files_count];

}; // class test_patch_impl


void   vfs_test::test_patch				()
{
	memory::base_allocator * const allocator	=	& memory::g_mt_allocator;

	native_path_string patch_test_path	=	native_path_string::convert(m_suite->get_resources_path());
	char const separator				=	native_path_string::separator;
	patch_test_path.appendf					("%ctests%cvfs%cpatch", separator, separator, separator);
	
	native_path_string		from_path;
	from_path.assignf						("%s%cfrom", patch_test_path.c_str(), separator);
	native_path_string		to_path;
	to_path.assignf							("%s%cto", patch_test_path.c_str(), separator);
	native_path_string		patch_path;
	patch_path.assignf						("%s%cpatch.db", patch_test_path.c_str(), separator);

	fs_new::synchronous_device_interface	synchronous_device	(m_device.c_ptr(), allocator);

	srand									(1);
	u32 const test_iterations_count		=	10;
	for ( int i=0; i<test_iterations_count; ++i )
	{
		LOG_FORCED							(logging::info, "VFS:PATCH TEST ITERATION %d", i);
		test_patch_impl						(from_path, to_path, patch_path, synchronous_device, allocator);
	}
}

} // namespace vfs
} // namespace xray
