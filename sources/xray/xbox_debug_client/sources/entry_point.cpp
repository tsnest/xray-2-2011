////////////////////////////////////////////////////////////////////////////
//	Created		: 15.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core/simple_engine.h>
#include <xray/core/core.h>
#include "debug_client.h"

xray::core::simple_engine				core_engine;
xray::memory::doug_lea_allocator_type	g_allocator;

void error_handler(pcstr const error_descr = "unknown error")
{
	printf						("\nERROR: %s\n", error_descr);
}

static xray::command_line::key		s_debug_output	("debug_output", "d", "main", "allows printing debug output to stdout");
static xray::command_line::key		s_image_name	("image_name", "i", "main", "image name to run on xbox");
static xray::command_line::key		s_work_dir		("work_dir", "", "main", "work dir for image to run on xbox");
static xray::command_line::key		s_xbox_name		("xbox_name", "x", "main", "xbox machine name");
static xray::command_line::key		s_testing_mode	("run_tests", "t", "main", "sets --run_tests_and_exit command line parameter to image and waits for results");
static xray::command_line::key		s_cmd_params	("cmd_params", "c", "main", "sets command line parameter to image");
static xray::command_line::key		s_wait_params	("wait_time", "w", "main", "waits a number of seconds");


int main(int argc, char ** argv)
{
	XRAY_UNREFERENCED_PARAMETERS			( argc, argv );

	using namespace xray;
	xray::core::preinitialize				(&core_engine, 
											 core::no_log,
											 GetCommandLine(), 
											 command_line::contains_application_true,
											 "xbox_debug_client",
											 __DATE__);
	g_allocator.user_current_thread_id		();
	g_allocator.do_register					(64*1024*1024, "g_allocator");
	xray::memory::allocate_region			();
	xray::core::initialize					("xbox_debug_client", core::perform_debug_initialization);

	xray::fixed_string512	xbox_name_store;
	pcstr xbox_name							= NULL;
	if (s_xbox_name.is_set_as_string(&xbox_name_store))
	{
		xbox_name = xbox_name_store.c_str();
	}
	debug_client							dclient(xbox_name, s_debug_output);
	if (dclient.failed())
	{
		error_handler(dclient.error_descr());
		xray::core::finalize();
		return EXIT_FAILURE;
	}

	
	xray::fixed_string512	image_name;
	if (!s_image_name.is_set_as_string(&image_name))
	{
		printf("ERROR: image name not set. Please set image name with key --%s",
			s_image_name.full_name());
		xray::core::finalize();
		return EXIT_FAILURE;
	}

	xray::fixed_string512	work_dir;
	if (!s_work_dir.is_set_as_string(&work_dir))
	{
		printf("ERROR: work dir is not set. Please set work dir with key --%s",
			s_work_dir.full_name());
		xray::core::finalize();
		return EXIT_FAILURE;
	}	
	
	xray::fixed_string512	cmd_line;
	if (!s_cmd_params.is_set_as_string(&cmd_line))
	{
		cmd_line = "";
	}

	if (s_testing_mode)
	{
		if (!s_cmd_params.is_set())
		{
			printf("ERROR: command line parameters not set. Please set it using key --%s",
				s_cmd_params.full_name());
			xray::core::finalize();
			return EXIT_FAILURE;
		}
	}
	
	if (!dclient.run_image(image_name.c_str(), work_dir.c_str(), cmd_line.c_str()))
	{
		error_handler			(dclient.error_descr());
		xray::core::finalize	();
		return EXIT_FAILURE;
	}

	u32		wait_number = 5; //5 seconds by default
	float	wait_float	= 0.f;
	if (s_wait_params.is_set_as_number(&wait_float))
	{
		wait_number = static_cast<u32>(wait_float);
	}
	
	if (!s_testing_mode)
	{
		printf("\nwaiting for results...");
		dclient.wait_title_finish(wait_number);
		xray::core::finalize();

		if (dclient.is_title_crashed())
		{
			error_handler("title has crashed :(");
			return EXIT_FAILURE;
		} else if (!dclient.is_title_finished())
		{
			error_handler("wait time out :(");
			return EXIT_FAILURE;
		}
		printf("\nSUCCESS: image has run successfully !");
		return EXIT_SUCCESS;
	}
	
	printf("\ntesting mode: waiting for hello...");
	if (!dclient.wait_hello(wait_number))
	{
		if (dclient.is_title_crashed())
		{
			error_handler("title has crashed :(");
		} else
		{
			error_handler("wait timeout :(");
		}
		xray::core::finalize	();
		return EXIT_FAILURE;
	}

	int ret_code = dclient.wait_tests_result(wait_number);
	if (ret_code == 0)
	{
		printf("SUCCESS: test has completed successfully !");
		xray::core::finalize();
		return EXIT_SUCCESS;
	}
	printf("\nERROR: test has FAILED: %d", ret_code);
	xray::core::finalize();
	return ret_code;
}