////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_performance_test_application.h"
#include "resources_performance_test_memory.h"
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <xray/os_include.h>		// for GetCommandLine
#include <xray/fs/physical_path_iterator.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/fs/device_utils.h>

namespace xray {

namespace resources_performance_test {

memory::doug_lea_allocator_type	resources_performance_test::g_allocator;

struct core_engine_type : public core::simple_engine
{
	virtual	pcstr	get_mounts_path			( ) const	{ return "..\\..\\mounts"; }	
};

static uninitialized_reference< core_engine_type >	s_core_engine;

resources::mount_ptr						s_mount_ptr;

void on_mounted (resources::mount_ptr mount)
{
	s_mount_ptr							=	mount;
}

void application::initialize( )
{
	m_exit_code							=	0;

	XRAY_CONSTRUCT_REFERENCE				( s_core_engine, core_engine_type );

	core::preinitialize		(
		&*s_core_engine,
		logging::create_log,
		GetCommandLine(), 
		command_line::contains_application_true,
		"test",
		__DATE__ 
	);

	g_allocator.do_register					( 4*1024*1024, "test" );

	memory::allocate_region					( );

	core::initialize						( "../../resources/sources", "test", core::perform_debug_initialization );

	XRAY_CONSTRUCT_REFERENCE				( m_fs_devices, fs_new::simple_asynchronous_interface )(fs_new::watcher_enabled_false);
	//XRAY_CONSTRUCT_REFERENCE				( m_fs_devices, fs_new::simple_synchronous_interface )(fs_new::watcher_enabled_false);

	core::initialize_resources				( m_fs_devices->hdd_async, m_fs_devices->dvd_async, resources::enable_fs_watcher_false );
	resources::start_resources_threads		( );
}

void application::finalize	( )
{
	xray::resources::finish_resources_threads	( );
	xray::core::finalize_resources			( );
	XRAY_DESTROY_REFERENCE					( m_fs_devices );
	core::finalize							( );

	XRAY_DESTROY_REFERENCE					( s_core_engine );
}

u32 mega_hash							( pcstr data, u32 size )
{
	u32 hash							=	0;
	for ( u32 i=0; i<1; ++i )
		hash							^=	fs_new::crc32(data, size);
	return									hash;
}

u32 read_file_and_calculate_hash		( fs_new::synchronous_device_interface &	device, 
										  fs_new::native_path_string const &		path,
										  u32										size )
{
	using namespace fs_new;
	file_type_pointer	file				(path, device, file_mode::open_existing, file_access::read);
	R_ASSERT								(file);

	pstr buffer							=	(pstr)ALLOC(char, (u32)size);
	file_size_type const read_bytes		=	device->read(file, buffer, size);	
	R_ASSERT_CMP							(read_bytes, ==, size);

	u32 const hash						=	mega_hash(buffer, (u32)size);

	FREE									(buffer);
	return									hash;
}

u32 application::test_without_resources_manager	(u32 * out_hash)
{
	timing::timer	timer;
	fs_new::synchronous_device_interface	device(& m_fs_devices->hdd_async, & g_allocator);
	timer.start();

	u32 hash							=	0;
	for ( u32 i=0; i<m_files.size(); ++i )
	{
		u32 const file_hash				=	read_file_and_calculate_hash(device, m_files[i].physical_path, m_files[i].size);
		hash							^=	file_hash;
	}

	u32 elapsed_ms						=	timer.get_elapsed_msec();
	* out_hash							=	hash;
	return									elapsed_ms;
}

void fill_files							(fs_new::physical_path_info const & info, 
										 files_array *						out_files,
										 u32 *								files_size)
{
	if ( !info.exists() )
		return;

	if ( info.is_file() )
	{
		* files_size					+=	(u32)info.get_file_size();
		path_pair path;
		path.physical_path				=	info.get_full_path();
		path.size						=	(u32)info.get_file_size();
		bool const converted			=	resources::convert_physical_to_virtual_path	
												(& path.virtual_path, path.physical_path, "exported");
		R_ASSERT							(converted);
		out_files->push_back				(path);
		return;
	}

	for ( fs_new::physical_path_iterator	it		=	info.children_begin(),
											end_it	=	info.children_end();
											it		!=	end_it;
											++it )
	{
		fill_files							(it, out_files, files_size);
	}
}

u32  calc_hash							(resources::managed_resource_ptr & ptr)
{
	pcbyte data							=	ptr->pin();
	u32 hash							=	mega_hash((pcstr)data, ptr->get_size());
	ptr->unpin								(data);
	return									hash;
}

void on_file_received					(u32 * out_hash, u32 * files_received, resources::queries_result & result)
{
	R_ASSERT								(result.is_successful());
	resources::managed_resource_ptr resource	=	result[0].get_managed_resource();
	* out_hash							^=	calc_hash(resource);
	++ * files_received;
}

void on_all_files_received				(u32 * out_hash, resources::queries_result & result)
{
	R_ASSERT								(result.is_successful());
	u32 hash							=	0;
	for ( u32 i=0; i<result.size(); ++i )
	{
		resources::managed_resource_ptr resource	=	result[i].get_managed_resource();
		hash							^=	calc_hash(resource);
	}

	* out_hash							=	hash;
}

u32 application::test_with_resources_manager	(u32 * out_hash)
{
	timing::timer	timer;
	timer.start();

	u32 files_received					=	0;
	u32 hash							=	0;

	//resources::request * requests		=	(resources::request *)ALLOCA(sizeof(resources::request) * m_files.size());
	//for ( u32 i=0; i<m_files.size(); ++i )
	//{
	//	requests[i].set						(m_files[i].virtual_path.c_str(), resources::raw_data_class_no_reuse);
	//}

	//resources::query_resource_params	params(
	//	requests,
	//	NULL, 
	//	m_files.size(), 
	//	boost::bind(on_all_files_received, & hash, _1),
	//	& g_allocator
	//);

	//resources::query_resources				(params);
	
	for ( u32 i=0; i<m_files.size(); ++i )
	{
		query_resource						(
			m_files[i].virtual_path.c_str(), 
			resources::raw_data_class, 
			boost::bind(on_file_received, & hash, & files_received, _1), 
			& g_allocator
		);
	}
	//LOG_INFO(logging::log_to_console, "querying took: %d", timer.get_elapsed_msec());

	while ( files_received != m_files.size() )
	{
		resources::dispatch_callbacks		();
		threading::yield					();
	}

	u32 elapsed_ms						=	timer.get_elapsed_msec();
	* out_hash							=	hash;
	return									elapsed_ms;
}

void application::create_test_files		(fs_new::native_path_string				folder,
										 u32 const								files_count, 
										 u32 const								average_file_size,
										 fs_new::synchronous_device_interface & device)
{
	using namespace fs_new;
	for ( u32 i=0; i<files_count; ++i )
	{
		fs_new::native_path_string			file_path;
		file_path.assignf					("%s\\test.%d", folder.c_str(), i + 1);
		
		file_type_pointer	file			(file_path, device, file_mode::create_always, file_access::write);

		u32 const ten_percent			=	average_file_size / 10;
		u32 size						=	average_file_size;
		if ( ten_percent != 0 )
		{
			size						+=	ten_percent - (rand() % (2 * ten_percent));
			if ( size <= 0 )
				size					=	1;
		}

		char c[2] = {0,0};
		for ( u32 i=0; i<size; ++i )
		{
			c[0]						=	'A' + (rand() % 20);
			device->write					(file, c, 1);
		}
	}
}

void application::test					(u32 const								files_count, 
										 u32 const								average_file_size)
{
	LOG_INFO								(logging::log_to_console, "testing %d files, avg file size: %d", 
											 files_count, average_file_size);
	pcstr const resources_path			=	"..\\..\\resources\\tests\\temp";
	{
		//fs_new::synchronous_device_interface	device	(& m_fs_devices->hdd_async, & g_allocator);
		//create_test_files						(resources_path, files_count, average_file_size, device);
	}

	resources::query_mount_physical			( "test", resources_path, "", fs_new::watcher_enabled_false, & on_mounted, & g_allocator, resources::recursive_true );
	while ( !s_mount_ptr )
		resources::dispatch_callbacks		();

	u32 files_size						=	0;
	{
		fs_new::synchronous_device_interface	device	(& m_fs_devices->hdd_async, & g_allocator);
		fs_new::physical_path_info	root_info	=	device->get_physical_path_info(resources_path);
		m_files.clear							();
		fill_files								(root_info, & m_files, & files_size);
	}

	fixed_string512	results1;
	fixed_string512	results2;
	u32 ms1_total						=	0;
	u32 ms2_total						=	0;
	u32 const iterations_count			=	10;
	for ( u32 i=0; i<iterations_count; ++i )
	{
		u32 hash1 = 0; 
		u32 ms1							=	test_without_resources_manager	(& hash1);
		u32 hash2 = 0; 
		u32 ms2							=	test_with_resources_manager		(& hash2);
		if ( i != 0 )
		{
			if ( results1.length() < 100 )
				results1.appendf			("%d ", ms1);
			if ( results2.length() < 100 )
				results2.appendf			("%d ", ms2);

			ms1_total					+=	ms1;
			ms2_total					+=	ms2;
		}
		R_ASSERT							(hash1 == hash2);
	}
	LOG_INFO(logging::log_to_console, "without  rm: %d", ms1_total / (iterations_count-1));
	LOG_INFO(logging::log_to_console, "with     rm: %d", ms2_total / (iterations_count-1));

	LOG_INFO(logging::log_to_console, "without  rm: %s", results1);
	LOG_INFO(logging::log_to_console, "with     rm: %s", results2);

	s_mount_ptr							=	NULL;
	{
		fs_new::synchronous_device_interface	device	(& m_fs_devices->hdd_async, & g_allocator);
		//erase_r									(device, resources_path);
	}	
}

void application::execute				( )
{
	SetPriorityClass						(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
 																	//	REALTIME_PRIORITY_CLASS);
	SetThreadPriority						(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
																//	THREAD_PRIORITY_TIME_CRITICAL);
		
		

	logging::set_format						(logging::format_message);

	test									(1000, 1000);

	/*for ( u32 file_count=100; file_count<=1000; file_count*=10 )
		for ( u32 file_size=100; file_size<=1000; file_size*=10 )
		{
			test							(file_count, file_size);
		}*/

	m_exit_code							=	s_core_engine->get_exit_code();
}

} // namespace resources_performance_test
} // namespace xray