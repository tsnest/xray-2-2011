////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graphics_benchmark_application.h"
#include "graphics_benchmark_memory.h"
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <xray/os_include.h>		// for GetCommandLine

#include <conio.h>

#include "benchmark.h"
#include "test_clear_render_target_depth_stencil.h"
#include "test_2d_shader_op.h"
#include "test_3d_shader_op.h"

using namespace xray::graphics_benchmark;

using xray::graphics_benchmark::application;

xray::memory::doug_lea_allocator_type	xray::graphics_benchmark::g_allocator;
xray::render::render_allocator_type*	xray::render::g_allocator = &xray::graphics_benchmark::g_allocator;

class graphics_benchmark_core_engine : public xray::core::simple_engine
{
public:
	virtual	pcstr	get_mounts_path		( ) const	{ return "../../mounts"; }
	virtual	pcstr	get_resources_xray_path	( ) const			{ return "../../resources/sources"; }
};

static xray::uninitialized_reference< graphics_benchmark_core_engine >	s_core_engine;

namespace xray {
namespace render {

	bool register_texture_cook();
	void unregister_texture_cook();

}
}
namespace xray {
namespace graphics_benchmark {

xray::uninitialized_reference< xray::fs_new::simple_synchronous_interface >	s_fs_devices;
static xray::resources::mount_ptr	s_resources_mount;

static void on_resources_mounted		( xray::resources::mount_ptr result )
{
	CHECK_OR_EXIT						( result, "Cannot mount resources" );
	s_resources_mount					= result;
}

void application::initialize( )
{
	m_exit_code				= 0;

	XRAY_CONSTRUCT_REFERENCE( s_core_engine, graphics_benchmark_core_engine );

	pstr new_command_line	= 0;
	STR_JOINA				( new_command_line, GetCommandLine(), " -enable_crt_memory_allocator" );

	core::preinitialize		(
		&*s_core_engine,
		core::create_log, 
		new_command_line,
		command_line::contains_application_true,
		"graphics_benchmark",
		__DATE__ 
	);

	g_allocator.do_register	( 16*1024*1024, "graphics_benchmark" );

	memory::allocate_region	( );

	core::initialize		( "graphics_benchmark", core::perform_debug_initialization );

	XRAY_CONSTRUCT_REFERENCE	( s_fs_devices, xray::fs_new::simple_synchronous_interface ) 
									( xray::fs_new::watcher_enabled_true );

	xray::core::initialize_resources	( s_fs_devices->hdd_async, s_fs_devices->dvd_async, resources::enable_fs_watcher_false );
	xray::resources::start_resources_threads		( );

	resources::query_mount	( "resources", & on_resources_mounted, &g_allocator );
}

void application::finalize	( )
{
	s_resources_mount		= NULL;

	resources::finish_resources_threads	( );
	core::finalize_resources( );
	core::finalize			( );

	XRAY_DESTROY_REFERENCE	( s_fs_devices );
	XRAY_DESTROY_REFERENCE	( s_core_engine );
}

void application::execute	( )
{
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
	
	std::string benchmark_file_name = "resources/benchmark_temporary/benchmark.test";
	//std::string benchmark_file_name = "../../sources/xray/graphics_benchmark/tests/benchmark.test";
	
/*
	benchmark_file_name.clear();
	std::string command_line = GetCommandLine();
	printf("\n%s",command_line.c_str());
	// application name
	s32 l_pos  = command_line.find("\"");
	s32	r_pos		= command_line.find("\"",l_pos+1);
	
	// benchmark file name
	//s32 l_pos2  = command_line.find("\"",l_pos+1);
	//if (l_pos2!=-1)
	//{
	//	r_pos= command_line.find("\"",l_pos2+1);
	//}

	for (s32 i=l_pos+1; i<r_pos; i++)
		benchmark_file_name.push_back(command_line[i]);
*/

	xray::render::register_texture_cook();
	//{
		benchmark benchmark( benchmark_file_name.c_str() );
		
		// Add pointer to class creator function and class name or alias.
		benchmark.register_test_class ( test_clear_render_target::creator, "test_clear_render_target" );
		benchmark.register_test_class ( test_clear_depth_stencil::creator, "test_clear_depth_stencil" );
		benchmark.register_test_class ( test_2d_shader_op::creator, "test_2d_shader_op" );
		benchmark.register_test_class ( test_3d_shader_op::creator, "test_3d_shader_op" );
		
		benchmark.start_execution();
	//}
	xray::render::unregister_texture_cook();

	_getch();
	
	m_exit_code				= s_core_engine->get_exit_code();
}

} // namespace graphics_benchmark
} // namespace xray