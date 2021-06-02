////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include <xray/resources.h>
#include <xray/resources_cook_classes.h>
#include <xray/resources_fs.h>
#include <xray/resources_fs_iterator.h>
#include <xray/fs_path_string.h>
#include "memory.h"
#include <xray/core_test_suite.h>
#include <xray/testing_macro.h>

#include "resources_manager.h"
#include "resources_impl.h"
#include <xray/managed_allocator.h>

#include "resources_test_managed.h"
#include "resources_test_inplace_managed.h"
#include "resources_test_unmanaged.h"
#include "resources_test_inplace_unmanaged.h"
#include "resources_test_translate_query.h"
#include "resources_test_configuration.h"
#include "game_resman.h"
#include <xray/vfs/pack_archive.h>

// If this code works, it was written by Lain. If not, I don't know who wrote it

XRAY_DECLARE_LINKAGE_ID(resources_test)

namespace xray {
namespace resources {

using namespace	fs;

fixed_vector<fs_new::native_path_string, 16>	s_created_files;

class resource_tester
{
public:
	resource_tester() : m_mount_completed(0), m_temp_files_to_delete_left(0) {}

	void   on_mount_completed			(mount_ptr result)
	{
		TEST_ASSERT							(result);
		m_mount_test					=	result;
	}

	void   on_mount_disk_completed		(mount_ptr result)
	{
		TEST_ASSERT							(result);
		m_mount_test_disk				=	result;
	}

	void   resources_callback (xray::resources::queries_result & queries)
	{
		for ( u32 i=0; i<queries.size(); ++i )
			TEST_CURE_ASSERT		(queries[i].is_successful(), return);
		TEST_CURE_ASSERT			(queries.is_successful(), return);
		LOGI_INFO					("resources:test", "callback called with success");
	}

	void   on_temp_file_deleted			(bool)
	{
		--m_temp_files_to_delete_left;
	}

	void   erase_temp_files				(vfs::vfs_iterator it)
	{
		R_ASSERT							(it);
		LOGI_INFO							("fs", "received fs_iterator to: %s", it.get_virtual_path().c_str());
		for ( vfs::vfs_iterator	it_child		=	it.children_begin(),
								it_child_end	=	it.children_end();
								it_child		!=	it_child_end;
								++it_child )
		{
			if ( it_child.is_archive() )
				continue;

			if ( strstr(it_child.get_name(), "this_file_dont_exist") != NULL )
			{
				fs_new::native_path_string const physical_path	=	it_child.get_physical_path();
				++m_temp_files_to_delete_left;
				query_erase_file			(physical_path, 
											 "", 
											 boost::bind(& resource_tester::on_temp_file_deleted, this, _1), 
											 & memory::g_mt_allocator);
			}

			erase_temp_files				(it_child);
		}
	}

	void   on_test_recursive_iterator_ready	(vfs::vfs_locked_iterator const & iterator)
	{	
		vfs::log_vfs_iterator				(iterator);
		erase_temp_files					(iterator);
	}

	void   query_mounts ()
	{
		fs_new::physical_path_info const info	=	resources::get_physical_path_info	(m_disk_path);
		TEST_ASSERT						(info.is_folder());

		query_mount_physical			("test/disk", m_disk_path.c_str(), "disk", fs_new::watcher_enabled_true,
										 boost::bind(&resource_tester::on_mount_disk_completed, this, _1), 
										 &::xray::memory::g_mt_allocator,
										 recursive_false);

		query_mount_physical			("test", m_folder_with_db.c_str(), "", fs_new::watcher_enabled_true,
										 boost::bind(&resource_tester::on_mount_completed, this, _1), 
										 &::xray::memory::g_mt_allocator,
										 recursive_false);
	}

	void   query_unmounts ()
	{
		LOGI_INFO							("resources:test", "releasing mount ptrs %d", m_test_iteration);
		R_ASSERT							(m_mount_test_disk && m_mount_test);
		m_mount_test_disk				=	NULL;
		m_mount_test					=	NULL;
	}

	void   work_with_specific_cooker	(cook_base * cook, test_configuration_type const & test_configuration)
	{
		if ( !test_configuration.do_query_create_resource )
			query_mounts				();

		if ( test_configuration.wait_after_mount )
			threading::yield		(50);

		request normal_request[] = 
		{
    		{ "test/test.db/folder1/2-866.no_inline",		test_resource_class },
    		{ "test/test.db/folder1/2.txt",					test_resource_class },
       		{ "test/test.db/folder2/1-1260.no_inline",		test_resource_class },
      		{ "test/test.db/folder2/test_link/4.txt",		test_resource_class },
       		{ "test/test.db/test_link/4.txt",				test_resource_class },
       		{ "test/test.db/folder2/2-866.no_inline",		test_resource_class },
      		{ "test/test.db/folder1/1-80.data",				test_resource_class },
      		{ "test/test.db/1-80.data",						test_resource_class },
     		{ "test/test.db/2-119.data",					test_resource_class },
      		{ "test/test.db/3-1215.test",					test_resource_class },
    		{ "test/test.db/4-4.test",						test_resource_class },
      		{ "test/test.db/5-5.test",						test_resource_class },
      		{ "test/test.db/6-6.test",						test_resource_class },
  	  		{ "test/disk/folder1/2-866.no_inline",			test_resource_class },
  			{ "test/disk/folder2/1-1260.no_inline",			test_resource_class },
   			{ "test/disk/1-80.data",						test_resource_class },
   			{ "test/disk/2-119.data",						test_resource_class },
   			{ "test/disk/3-1215.test",						test_resource_class },
    		{ "test/disk/4-4.test",							test_resource_class },
    		{ "test/disk/5-5.test",							test_resource_class },
   			{ "test/disk/6-6.test",							test_resource_class },
		};

		request inplace_inline_request[] = 
		{
   			{ "test/inplace_inline.inplace_inline",		test_resource_class },
		};

		request request_for_file_that_dont_exist_reuse_true[]	=	
		{
 			{ "test/disk/this_file_dont_exist1",	test_resource_class },
  			{ "test/disk/this_file_dont_exist2",	test_resource_class },
 			{ "test/disk/this_file_dont_exist1",	test_resource_class },
		};

		request request_for_file_that_dont_exist_reuse_false[]	=
		{
			{ "test/disk/this_file_dont_exist1",	test_resource_class },
 			{ "test/disk/this_file_dont_exist2",	test_resource_class },
 			{ "test/disk/this_file_dont_exist3",	test_resource_class },
		};

		request request_for_translate_query[]	=
		{
			{ "translate_query_request1",			test_resource_class },
			{ "translate_query_request2",			test_resource_class },
			{ "translate_query_request1",			test_resource_class },
		};

		if ( test_configuration.do_query_create_resource )
		{
			u32	src_buffer_size				=	creation_data_size;
			u32 src_buffer_offs_to_raw_data	=	0;
			if ( test_configuration.inplace_in_creation_or_inline_data )
			{
				R_ASSERT					(test_configuration.cook_type == cook_type_inplace_unmanaged);
				src_buffer_size			+=	sizeof(test_unmanaged_resource);
				src_buffer_offs_to_raw_data	=	sizeof(test_unmanaged_resource);
			}

			pstr src_buffer_data		=	test_configuration.inplace_in_creation_or_inline_data ? 
											(pstr)MT_ALLOC(char, src_buffer_size) : (pstr)ALLOCA(src_buffer_size);

			for ( u32 i=0; i<creation_data_size; ++i )
				src_buffer_data[src_buffer_offs_to_raw_data + i]	=	(i % 2) ? '0' : '1';

			const_buffer	 src_buffer		(src_buffer_data, src_buffer_size);
			int const num_creation_requests	=	3;
			fixed_vector< creation_request, num_creation_requests >	creation_requests;

			creation_requests.push_back( creation_request( "create_resource_request1", src_buffer, test_resource_class ) );
			creation_requests.push_back( creation_request( "create_resource_request2", src_buffer, test_resource_class ) );
			creation_requests.push_back( creation_request( "create_resource_request1", src_buffer, test_resource_class ) );

			u32 const creation_requests_count	=	test_configuration.inplace_in_creation_or_inline_data ? 1 : creation_requests.size();

			query_create_resources_and_wait	(&*creation_requests.begin(), 
											 creation_requests_count,
											 boost::bind(&resource_tester::resources_callback, this, _1), 
											 & memory::g_mt_allocator);
		}
		else
		{
			request * requests			=	test_configuration.inplace_in_creation_or_inline_data ? 
											inplace_inline_request : normal_request;
			
			if ( test_configuration.inplace_in_creation_or_inline_data )
				R_ASSERT					(test_configuration.cook_type == cook_type_inplace_unmanaged);

			u32 requests_count			=	test_configuration.inplace_in_creation_or_inline_data ? 
											array_size(inplace_inline_request) : array_size(normal_request);

			if ( test_configuration.cook_type == cook_type_translate_query )
			{
				requests				=	request_for_translate_query;
				requests_count			=	array_size(request_for_translate_query);
			}

			if ( cook->does_create_resource_if_no_file() )
			{
				if ( test_configuration.reuse_type == cook_base::reuse_false )
				{
					requests			=	request_for_file_that_dont_exist_reuse_false;
					requests_count		=	array_size(request_for_file_that_dont_exist_reuse_false);
				}
				else
				{
					requests			=	request_for_file_that_dont_exist_reuse_true;
					requests_count		=	array_size(request_for_file_that_dont_exist_reuse_true);
				}
			}

			query_resources_and_wait		( requests, 
											  requests_count,
											  boost::bind(&resource_tester::resources_callback, this, _1), 
											  & memory::g_mt_allocator	);
		}
	
		//release_all_grm_resources	();
				
		if ( !test_configuration.do_query_create_resource )
			query_unmounts					();
	}

	void release_all_grm_resources ()
	{
		LOGI_INFO							("test", "scheduling release of all resources");
		schedule_release_all_resources		();
		while ( is_release_all_resources_scheduled() )
		{
			resources::dispatch_callbacks	();
			if ( threading::g_debug_single_thread )
				resources::tick				();
		}
	}
	
	void test_recursive_fs_iterator ()
	{
		query_mounts				();

		LOGI_INFO					("resources:test",	"RECURSIVE FS ITERATOR TEST");

		query_vfs_iterator_and_wait	("test", 
									 boost::bind(& resource_tester::on_test_recursive_iterator_ready, this, _1),
									 &::xray::memory::g_mt_allocator,
									 recursive_true);

		while ( m_temp_files_to_delete_left )
		{
			resources::dispatch_callbacks	();
			if ( threading::g_debug_single_thread )
				resources::tick		();
		}

		release_all_grm_resources	();
		query_unmounts				();
	}

	void unregister_cook (cook_base * cook)
	{
		u32 const time_for_cleanup	=	1000;

		timing::timer				timer;
		timer.start					();
		while ( cook->cook_users_count() && (timer.get_elapsed_msec() < time_for_cleanup || debug::is_debugger_present()) )
		{
			resources::dispatch_callbacks	();
			if ( threading::g_debug_single_thread )
				resources::tick		();
		}

		CURE_ASSERT					(timer.get_elapsed_msec() < time_for_cleanup || debug::is_debugger_present(), return, "cooker has failed to delete all resources/buffers, though it was given %d ms of time", time_for_cleanup);

		resources::unregister_cook	(cook->get_class_id());
	}

	void create_db						()
	{
		vfs::pack_archive_args	args		(resources::get_synchronous_device(), NULL, (logging::log_flags_enum)0);
		args.allocator					=	& memory::g_mt_allocator;
		args.archive_part_max_size		=	4096;
		args.fat_part_max_size			=	1024;
		args.fat_align					=	2048;
		args.sources					=	m_disk_path;
		args.platform					=	vfs::archive_platform_pc;

		args.config_file.assignf_with_conversion	("%s/inline.config", m_resource_path.c_str());

		fs_new::native_path_string	db_path	=	m_folder_with_db;
		db_path.append_path					("test.db");
		args.target_db					=	db_path;
		args.target_fat					=	db_path;

		bool const made_from_db			=	pack_archive(args);
		TEST_ASSERT							(made_from_db);
	}

	void test (core_test_suite * suite)
	{
		//threading::yield				(10000);
		m_resource_path					=	suite->get_resources_path();

		//u32 const path_parts_to_trim_count	=	0;
		//for ( u32 path_part=0; path_part<path_parts_to_trim_count; ++path_part )
		//{
		//	u32 const last_slash_pos	=	m_resource_path.rfind('/');
		//	TEST_ASSERT						(last_slash_pos != m_resource_path.npos, "wrong resource path '%s'", m_resource_path.c_str());
		//	m_resource_path.set_length		(last_slash_pos);
		//}

		m_resource_path.append_with_conversion		("/tests/resources_manager");
		m_folder_with_db				=	m_resource_path;
		m_folder_with_db.appendf_with_conversion	("/db-%s", platform::big_endian() ? "be" : "le");

		m_disk_path						=	m_resource_path;
		m_disk_path.append_with_conversion	("/disk");

		create_db();

		if ( debug::is_debugger_present() )
		{
			logging::push_filter				( "", logging::warning, & memory::g_mt_allocator );
			logging::push_filter				( "core:resources:test", logging::trace, & memory::g_mt_allocator );
			logging::push_filter				( "vfs", logging::info, & memory::g_mt_allocator );
			//logging::push_filter				( "vfs", logging::debug, & memory::g_mt_allocator );
//			logging::push_filter				( "core:resources:allocator", logging::silent, & memory::g_mt_allocator );
// 			logging::push_filter				( "core:resources:device_manager", logging::trace, & memory::g_mt_allocator );
// 			logging::push_filter				( "core:resources", logging::trace, & memory::g_mt_allocator );
		}

		test_recursive_fs_iterator			();

		m_test_iteration				=	1;
		test_configuration_type				test_configuration(1);
		u32 const tests_to_skip			=	0;

		
		do {

			if ( m_test_iteration <= tests_to_skip )
			{
				++m_test_iteration;
				continue;
			}

			cook_base * cook			=	NULL;
			if ( test_configuration.cook_type == cook_type_managed )
			{ 
				cook					=	MT_NEW(test_managed_cook)
											(test_configuration.reuse_type,
											 test_configuration.create_in_current_thread,
											 test_configuration.generate_if_no_file_test);
			}
			else if ( test_configuration.cook_type == cook_type_inplace_managed )
			{
				cook					=	MT_NEW(test_inplace_managed_cook)
											(test_configuration.reuse_type,
											 test_configuration.create_in_current_thread,
											 test_configuration.generate_if_no_file_test);
			}
			else if ( test_configuration.cook_type == cook_type_unmanaged )
			{
				cook					=	MT_NEW(test_unmanaged_cook)
											(test_configuration.reuse_type,
											 test_configuration.create_in_current_thread,
											 test_configuration.allocate_in_current_thread,
											 test_configuration.generate_if_no_file_test);
			}
			else if ( test_configuration.cook_type == cook_type_inplace_unmanaged )
			{
				cook					=	MT_NEW(test_inplace_unmanaged_cook)
											(test_configuration.reuse_type,
											 test_configuration.create_in_current_thread,
											 test_configuration.allocate_in_current_thread || test_configuration.inplace_in_creation_or_inline_data,
											 test_configuration.generate_if_no_file_test,
											 test_configuration.inplace_in_creation_or_inline_data);
			}
			else if ( test_configuration.cook_type == cook_type_translate_query )
			{
				cook					=	MT_NEW(test_translate_query_cook)
											(test_configuration.reuse_type,
											 test_configuration.translate_in_current_thread);
			}
			else
				NOT_IMPLEMENTED();

			register_cook					(cook);

			fixed_string512					cook_string;
			cook->to_string					(& cook_string);

			if ( test_configuration.generate_if_no_file_test == generate_if_no_file_test_requery )
				cook_string				+=	"+requery";
			else if ( test_configuration.generate_if_no_file_test == generate_if_no_file_test_save_generated )
				cook_string				+=	"+save_generated";
			else if ( test_configuration.do_query_create_resource )
				cook_string				+=	"+query_create_resource";

			if ( test_configuration.inplace_in_creation_or_inline_data )
				cook_string				+=	"+inplace_inline";

			LOGI_INFO("resources:test",	"TEST ITERATION %d | cook: %s, cook_reuse: %s, create_in_current_thread: %d, allocate_in_current_thread: %d, sleep_after_mount: %d", 
										m_test_iteration, cook_string.c_str(), 
										convert_cook_reuse_value_to_string(test_configuration.reuse_type), 
										test_configuration.create_in_current_thread, 
										test_configuration.allocate_in_current_thread, 
										test_configuration.wait_after_mount);

			bool skip_test				=	false;
#ifdef MASTER_GOLD
			if ( test_configuration.generate_if_no_file_test == generate_if_no_file_test_save_generated )
				skip_test				=	true;
#endif // #ifdef MASTER_GOLD
			
			if ( skip_test )
				LOGI_INFO					("resources:test",	"this test iteration is skipped in Master Gold because it uses DEBUG allocator");
			else
				work_with_specific_cooker	(cook, test_configuration);

			unregister_cook					(cook);

			delete_files_created_by_cook	(test_configuration, cook_string.c_str());		

			MT_DELETE						(cook);

			++m_test_iteration;

		} while ( test_configuration.move_next() );
		


		for ( u32 i=0; i<2; ++i )
			logging::pop_filter				();

		//threading::yield					(100);
		wait_and_dispatch_callbacks			(true);
	}

	struct on_test_file_erased_predicate_type
	{
		on_test_file_erased_predicate_type () : files_left(0) {}
		void   callback					(bool result)
		{
			TEST_ASSERT						(result);
			R_ASSERT						(files_left);
			--files_left;
		}

		u32									files_left;
	};

	void   delete_files_created_by_cook (test_configuration_type const & test_configuration, pcstr const cook_string)
	{
		on_test_file_erased_predicate_type on_test_file_erased_predicate;

		for ( u32 i=0; i<s_created_files.size(); ++i )
		{
			if ( !s_created_files[i].length() )
				continue;

			++on_test_file_erased_predicate.files_left;

			resources::query_erase_file		(s_created_files[i],
											 "",
											 boost::bind(& on_test_file_erased_predicate_type::callback, & on_test_file_erased_predicate, _1),
											 & memory::g_mt_allocator);
		}

		u32 const time_for_cleanup		=	5000;
		timing::timer	timer;
		timer.start							();
		while ( on_test_file_erased_predicate.files_left && 
			  (timer.get_elapsed_msec() < time_for_cleanup || debug::is_debugger_present()) )
		{
			if ( threading::g_debug_single_thread )
				resources::tick				();

			resources::dispatch_callbacks	();
		}

		CURE_ASSERT		   (!on_test_file_erased_predicate.files_left || debug::is_debugger_present(), 
							return,
							"TEST ITERATION %d FAILED TO CLEAN FILES | cook: %s, cook_reuse: %s, create_in_current_thread: %d, allocate_in_current_thread: %d, sleep_after_mount: %d", 
							m_test_iteration, cook_string, 
							convert_cook_reuse_value_to_string(test_configuration.reuse_type), 
							test_configuration.create_in_current_thread, 
							test_configuration.allocate_in_current_thread, 
							test_configuration.wait_after_mount);

		s_created_files.clear		();
	}

private:
	fs_new::native_path_string				m_resource_path;
	fs_new::native_path_string				m_folder_with_db;
	fs_new::native_path_string				m_disk_path;

	u32										m_test_iteration;
	bool									m_stop_dispatch;
	u32										m_mount_completed;
	u32										m_temp_files_to_delete_left;

	mount_ptr								m_mount_test_disk;
	mount_ptr								m_mount_test;
};

REGISTER_TEST_CLASS							(resource_tester, core_test_suite);

} // namespace xray
} // namespace resources


 







































/*@                                    /\  /\
 * @                                  /  \/  \                        ----- |   | ----      |---\ |    | /--\  --- |   |  ---- /--\ /--\
 *  @                                /        --                        |   |   | |         |   / |    | |      |  |\  |  |    |    |
 *   \---\                          /           \                       |   |---| ----      |--/  |    |  \     |  | \ |  ----  \    \
 *    |   \------*****!!!!******---/       /-\    \                     |   |   | |         |  \  |    |   -\   |  |  \|  |      -\   -\
 *    |                                    \-/     \                    |   |   | ----      |---/  \--/  \--/  --- |   \  ---- \--/ \--/
 *     \                                             ------O
 *      \                                                 /                 --- |   | ----  /--\        |--\   /--\   /--\
 *       |    |                    |    |                /                   |  |\  | |    |    |       |   | |    | |
 *       |    |                    |    |-----    -------                    |  | \ | ---- |    |       |   | |    | | /-\
 *       |    |\                  /|    |     \  WWWWWW/                     |  |  \| |    |    |       |   | |    | |    |
 *       |    | \                / |    |      \-------                     --- |   \ |     \--/        |--/   \--/   \--/
 *       |    |  \--------------/  |    |
 *      /     |                   /     |
 *      \      \                  \      \
 *       \-----/                   \-----/
 */

