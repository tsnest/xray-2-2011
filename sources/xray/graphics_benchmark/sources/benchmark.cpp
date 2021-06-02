////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "benchmark.h"
#include "benchmark_helpers.h"
#include "gpu_timer.h"
#include "gpu_counter.h"
#include "test_simple_shader_op.h"
#include <conio.h>

#include <xray\render\core\world.h>
#include <xray\render\core\device.h>
#include <xray\render\core\pix_event_wrapper.h>

#define REGISTER_TEST_CLASS(type) \
	this->register_test_class(type::creator, #type);

namespace xray { 
namespace graphics_benchmark {

namespace colorize {
	//12 - red, 14 - yellow, 10	- green, 7, 8  - gray, 15 - white
	void set_text_color(unsigned short color)
	{
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hCon,color);
	}
	void set_base_color	()	{ set_text_color(7); }
	void set_gray_color	()	{ set_text_color(8); }

} // namespace colorize

using namespace xray::render;

static pcstr s_window_id					= XRAY_ENGINE_ID " DX11 Renderer Window";
static pcstr s_window_class_id				= XRAY_ENGINE_ID " DX11 Renderer Window Class ID";
static WNDCLASSEX s_window_class;
static std::string const& BASE_test_name = "BASE";
static std::string const& settings_str = "SETTINGS";

static LRESULT APIENTRY message_processor	( HWND window_handle, UINT message_id, WPARAM w_param, LPARAM l_param )
{
	switch ( message_id ) {
	case WM_DESTROY: {
		PostQuitMessage	( 0 );

		return			( 0 );
					 }
	case WM_ACTIVATE: {

		while (	ShowCursor( TRUE ) < 0 );
		break;
					  }
	}

	return					( DefWindowProc( window_handle, message_id, w_param, l_param ) );
}

static void create_window			( HWND& result, u32 window_size_x, u32 window_size_y )
{
	WNDCLASSEX const temp	=
	{
		sizeof( WNDCLASSEX ),
		CS_CLASSDC,
		&message_processor,
		0L,
		0L, 
		GetModuleHandle( 0 ),
		NULL,
		NULL,
		NULL,
		NULL,
		s_window_class_id,
		NULL
	};

	s_window_class			= temp;
	RegisterClassEx			( &s_window_class );

	u32 const screen_size_x	= GetSystemMetrics( SM_CXSCREEN );
	u32 const screen_size_y	= GetSystemMetrics( SM_CYSCREEN );

	DWORD const	window_style = WS_POPUP;//WS_OVERLAPPEDWINDOW;//WS_CAPTION;

	RECT		window_size = { 0, 0, window_size_x, window_size_y };
	AdjustWindowRect		(&window_size, window_style, false);

	result					= 
		CreateWindow (
		s_window_class_id,
		s_window_id,
		window_style,
		( screen_size_x - window_size.right ) / 2,
		( screen_size_y - window_size.bottom ) / 2,
		window_size.right - window_size.left,
		window_size.bottom - window_size.top,
		GetDesktopWindow( ),
		0,
		s_window_class.hInstance,
		0
		);
}

HWND benchmark::get_new_window(u32 window_size_x, u32 window_size_y)
{
	HWND h_wnd;

	create_window(h_wnd,window_size_x,window_size_y);
	::ShowWindow(h_wnd,SW_SHOW);
	
	m_hwnd = h_wnd;

	return h_wnd;
}

bool benchmark::initialize_d3d()
{
	return true;
}

void benchmark::finalize_d3d()
{
	//XRAY_DELETE_IMPL(g_allocator, m_device);
}

ID3DDevice* benchmark::d3d_device() const
{
	return device::ref().d3d_device();
}

static void shader_compile_error_handler( xray::render::enum_shader_type /*shader_type*/, char const* /*name*/, char const* error_string)
{
	printf("\n%s\n",error_string);
}

benchmark::benchmark(char const* benchmark_file):
	m_benchmark_file_name( benchmark_file ),
	m_benchmark_loaded   ( execute_benchmark_file(benchmark_file)),
	m_counters_valid	 ( initialize())
{
	HWND window = get_new_window(m_window_size_x,m_window_size_y);
	render::initialize_core	( window, !m_is_full_screen);

	m_benchmark_renderer.initialize	( );

	if (m_benchmark_loaded)
		post_initialize();
	
	xray::render::resource_manager::ref().set_compile_error_handler(shader_compile_error_handler);
	
	m_first_class			= NULL;
	
	m_current_test_index	= 0;

}

benchmark::~benchmark()
{
	::ShowWindow(m_hwnd,SW_HIDE);
	
	m_groups.clear();

	pre_finalize();
	finalize_d3d();
	finalize();
	
	benchmark_class* next = NULL;
	for ( benchmark_class* it=m_first_class; it!=NULL; it = next)
	{
		next = it->next;
		XRAY_DELETE_IMPL(g_allocator,it);
	}
	
	::ShowWindow(m_hwnd,SW_HIDE);
	::ShowWindow(m_hwnd,SW_HIDE);
}

bool benchmark::initialize()
{
	if (m_benchmark_target==target_ati)		return initialize_ati();
	if (m_benchmark_target==target_nvidia)	return initialize_nvidia();
	if (m_benchmark_target==target_xbox)	return initialize_xbox();
	if (m_benchmark_target==target_ps3) 	return initialize_ps3();
	return true;
}

bool benchmark::post_initialize()
{
#if USE_HARDWARE_COUNTERS
	if (m_benchmark_target==target_ati && m_counters_valid)
	{
		GPA_Status result;
		
		result = GPA_OpenContext( m_device->d3d_device() );
		if (result)	printf("\nGPA_OpenContext failed (%d)!\n",result);

		result = GPA_EnableAllCounters();
		if (result)	printf("\nGPA_EnableAllCounters failed (%d)!\n",result);
	}
#endif
	// Here some initialization code will go into more general initialization stage 
	sampler_state_descriptor sampler_sim;

	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	
	sampler_sim.set_max_anisotropy(16);

	resource_manager::ref().register_sampler( "s_base", resource_manager::ref().create_sampler_state( sampler_sim));

	sampler_sim.set_max_anisotropy(8);

	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	resource_manager::ref().register_sampler( "s_base_hud", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set_max_anisotropy(1);
	
	sampler_sim.set( D3D_FILTER_ANISOTROPIC );
	resource_manager::ref().register_sampler( "s_detail", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.reset();

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP );
	resource_manager::ref().register_sampler( "s_position", resource_manager::ref().create_sampler_state( sampler_sim) );
	resource_manager::ref().register_sampler( "s_normal", resource_manager::ref().create_sampler_state( sampler_sim) );
	resource_manager::ref().register_sampler( "s_diffuse", resource_manager::ref().create_sampler_state( sampler_sim) );
	resource_manager::ref().register_sampler( "s_accumulator", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "s_material", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_CLAMP);
	resource_manager::ref().register_sampler( "s_material1", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "smp_nofilter", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "smp_rtlinear", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_LINEAR, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "smp_linear", resource_manager::ref().create_sampler_state( sampler_sim) );

	sampler_sim.set( D3D_FILTER_ANISOTROPIC, D3D_TEXTURE_ADDRESS_WRAP);
	resource_manager::ref().register_sampler( "smp_base", resource_manager::ref().create_sampler_state( sampler_sim) );


	// Read only first mip level.
	sampler_sim.set_mip( 0.f, 0.f, 0.f);

	sampler_sim.set( D3D_FILTER_MIN_MAG_MIP_POINT, D3D_TEXTURE_ADDRESS_WRAP);
	sampler_sim.set_mip( 0.f, 0.f, 0.f);
	resource_manager::ref().register_sampler( "s_first_mip", resource_manager::ref().create_sampler_state( sampler_sim) );
	
	//sampler_sim.reset();
	return true;
}

void benchmark::finalize()
{
	if (m_benchmark_target==target_ati)		finalize_ati();
	if (m_benchmark_target==target_nvidia)	finalize_nvidia();
	if (m_benchmark_target==target_xbox)	finalize_xbox();
	if (m_benchmark_target==target_ps3)		finalize_ps3();
}


bool benchmark::initialize_ati()
{
#if USE_HARDWARE_COUNTERS
	GPA_Status fail = GPA_Initialize();
	if (fail) 
	{
		printf("\nGPA_Initialize failed (%d)\n",fail);
		return false;
	}
	retun true;
#else
	printf("\nATI counters not implemented.\n");
	return false;
#endif // #if USE_HARDWARE_COUNTERS
}

#if USE_HARDWARE_COUNTERS
int MyEnumFunc(UINT unCounterIndex, char *pcCounterName)
{
	(void)&unCounterIndex;
	(void)&pcCounterName;

	char zString[200], zLine[400];
	unsigned int unLen;
	float fValue;
	(void)&fValue;

	unLen = 200;
	if(NVPMGetCounterDescription(unCounterIndex, zString, &unLen) == NVPM_OK) {
		sprintf(zLine, "Counter %d [%s] : ", unCounterIndex, zString);

		//unLen = 200;
		if(NVPMGetCounterName(unCounterIndex, zString, &unLen) == NVPM_OK)
			strcat(zLine, zString); // Append the short name
		else
			strcat(zLine, "Error retrieving short name");

		NVPMGetCounterClockRate(zString, &fValue);
		sprintf(zString, " %.2f\n", fValue);
		strcat(zLine, zString);
		printf(zLine);
		//OutputDebugStringA(zLine);
	}

	return(NVPM_OK);
}
#endif // #if USE_HARDWARE_COUNTERS

bool benchmark::initialize_nvidia()
{
#if USE_HARDWARE_COUNTERS
	NVPMRESULT fail = NVPMInit();

	if (fail) 
	{
		printf("\nNVPMInit faild (%d)\n",fail);
		return false;
	}
	fail = NVPMAddCounterByName("D3D driver time");
	fail = NVPMAddCounterByName("gpu_idle");

	//NVPMEnumCounters(MyEnumFunc);
	return true;
#else
	printf("\nNVIDIA counters not implemented.\n");
	return false;
#endif // #if USE_HARDWARE_COUNTERS
}

void benchmark::finalize_nvidia()
{
#if USE_HARDWARE_COUNTERS
	NVPMRESULT fail = NVPMShutdown();
	printf("\nNVPMShutdown (%d)\n",fail);
#endif // #if USE_HARDWARE_COUNTERS
}


// call before d3d device closed
void benchmark::pre_finalize()
{
#if USE_HARDWARE_COUNTERS
	if (m_benchmark_target==target_ati)
	{
		GPA_Status result = GPA_CloseContext();
		if (result) printf("\nGPA_CloseContext failed (%d)\n",result);
	}
#endif // #if USE_HARDWARE_COUNTERS
}

void benchmark::finalize_ati()
{
#if USE_HARDWARE_COUNTERS
	GPA_Status result = GPA_Destroy();
	if (result) printf("\nGPA_Destroy failed (%d)\n",result);
#endif // #if USE_HARDWARE_COUNTERS
}

void benchmark::register_test_class( creator_func_type creator_func, char const* class_name)
{
	for ( benchmark_class* it=m_first_class; it!=NULL; it = it->next)
	{
		if (it->name!=class_name)
			continue;

		it->creator = creator_func;
		return;
	}
	benchmark_class* new_class	= NEW(benchmark_class)();
	new_class->next				= m_first_class;
	m_first_class					= new_class;

	new_class->creator			= creator_func;
	new_class->name				= class_name;
}

std::string benchmark::target_to_string(benchmark_target_enum t)
{
	if (t==target_ati)		return "ati";
	if (t==target_nvidia)	return "nvidia";
	if (t==target_xbox)		return "xbox";
	if (t==target_ps3)		return "ps3";
	if (t==target_general)	return "general";
	
	return "unknown";
}

benchmark_target_enum benchmark::string_to_target(std::string const& t)
{
	if (t=="ati" )		return target_ati;
	if (t=="nvidia" )	return target_nvidia;
	if (t=="xbox" )		return target_xbox;
	if (t=="ps3" )		return target_ps3;
	if (t=="general" )	return target_general;
	
	return target_unknown;
}

bool benchmark::execute_test( group_to_execute& group, test_to_execute& test, gpu_timer& gtimer, bool preview)
{
	//if (preview) return true;
	PIX_EVENT(execute_test);
	
	backend::ref().set_render_output			( &device::ref().default_render_output());
	backend::ref().reset_depth_stencil_target	();

	math::rectangle<math::uint2> res_viewport;
	res_viewport = math::rectangle<math::uint2>( math::uint2(0, 0), math::uint2(backend::ref().target_width(), backend::ref().target_height()));

#pragma warning(push)
#pragma warning(disable:4244)
	D3D_VIEWPORT d3d_viewport	= { res_viewport.left, res_viewport.top, res_viewport.width(), res_viewport.height(), 0.f, 1.f};
#pragma warning(pop)
	backend::ref().set_viewport				( d3d_viewport);
	
	if ( !test.object )
		return false;
	
	//char const*						   test_name	= test.test_name.c_str();
	std::map<std::string,std::string>& parameters	= test.parameters;
	
	const u32 num_passes	 = preview ? 1 : test.num_passes,
			  num_tests		 = preview ? 1 : group.num_circles,
			  num_skip_tests = preview ? 0 : group.num_skipped_circles;
	
	// Set first 3 render targets
	const u32 num_rts = test.render_targets.size();
	//xray::render::backend::ref().reset_render_targets();
	if (num_rts)
		xray::render::backend::ref().set_render_targets(
		num_rts>0 ? &*test.render_targets[0].rt : 0,
		num_rts>1 ? &*test.render_targets[1].rt : 0,
		num_rts>2 ? &*test.render_targets[2].rt : 0,
		0);
	else
		//xray::render::backend::ref().set_render_targets(0,0,0,0);
		xray::render::backend::ref().reset_render_targets();
	
	if (test.depth_rt.is_valid)
	{
		//set_depth_stencil_target
	}
	
	// Try initialize test.
	if (!test.object->initialize(*this,group,test,parameters,test.test_comments))
	{
		test.object->finalize(*this);
		return false;
	}
	
	benchmark_test* test_object = test.object;
	
	backend::ref().clear_render_targets( math::color( 0.f, 0.f, 0.f, 1.0f));
	backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1, 0);
	
	double avrg			 = 0.0,
		   elapsed_time	 = 0.0;//,
		   //min_elapsed_time = 100000000000.0;
		   
	u64	   avrg_ticks	 = 0,
		   elapsed_ticks = 0,
		   frequency	 = 0;//,
		   //min_elapsed_ticks = 100000000000;
	
	if (m_benchmark_target==target_general)
	{
		for ( u32 ps=0; ps < num_tests + num_skip_tests; ps++)
		{
			
			if (m_show_preview_window)
			{
				xray::render::device::ref().begin_frame();
			}

			gtimer.start();
				for (u32 pass_index=0; pass_index < num_passes; pass_index++) test_object->execute(*this,pass_index+1,num_passes, test, gtimer);
			backend::ref().flush();

			if (m_show_preview_window) 
				xray::render::device::ref().d3d_context()->Flush();

			gtimer.stop();
			
			gtimer.get_elapsed_info(elapsed_time,elapsed_ticks,frequency);
			
			if (m_show_preview_window) xray::render::device::ref().end_frame();

			if (ps<num_skip_tests)
				continue;
			
			//if (elapsed_ticks<min_elapsed_ticks) min_elapsed_ticks = elapsed_ticks;
			//if (elapsed_time<min_elapsed_time) min_elapsed_time = elapsed_time;
			
			avrg	   += elapsed_time;
			avrg_ticks += elapsed_ticks;
		}
	}
#if USE_HARDWARE_COUNTERS
	else if (m_benchmark_target==target_ati)
	{

		static gpa_uint32 currentWaitSessionID = 1;
		gpa_uint32 sessionID;
		GPA_Status fail_res;
		fail_res = GPA_BeginSession( &sessionID );
		if (fail_res) printf("\nGPA_BeginSession failed (%d)",fail_res);

		gpa_uint32 numRequiredPasses;
		fail_res = GPA_GetPassCount( &numRequiredPasses );
		if (fail_res) 
		{
			printf("\nGPA_BeginSession failed (%d)",fail_res);
			return false;
		}

		//xray::render::device::ref().begin_frame();
		//test_object->execute(*this,1,1, gtimer);
		//xray::render::device::ref().end_frame();

		//xray::render::device::ref().begin_frame();

		for ( u32 ps=0; ps < num_tests + num_skip_tests; ps++)
		{
			for (u32 pass_index=0; pass_index < num_passes; pass_index++)
			{				
				for ( gpa_uint32 i = 0; i < numRequiredPasses; i++ )
				{
					GPA_BeginPass();
					GPA_BeginSample( 0 );

					test_object->execute(*this,pass_index+1,num_passes, gtimer);

					GPA_EndSample();
					GPA_EndPass();
				}
			}

			bool readyResult = false;
			if ( sessionID != currentWaitSessionID )
			{
				GPA_Status sessionStatus;
				sessionStatus = GPA_IsSessionReady( &readyResult,
					currentWaitSessionID );
				while ( sessionStatus == GPA_STATUS_ERROR_SESSION_NOT_FOUND )
				{
					// skipping a session which got overwritten
					currentWaitSessionID++;
					sessionStatus = GPA_IsSessionReady( &readyResult, 
						currentWaitSessionID );
				}
			}
			if ( readyResult )
			{
				gpa_uint32 counter_index = 0;
				GPA_Status fail = GPA_GetCounterIndex("GPUBusy",&counter_index);
				if (!fail)
				{
					GPA_Type type;
					GPA_GetCounterDataType( counter_index, &type );
					if ( type == GPA_TYPE_UINT32 )
					{
						gpa_uint32 value;
						GPA_GetSampleUInt32( currentWaitSessionID, 
							0, counter_index, &value );
						printf( "\nElapsed ms: %u,", value );
						elapsed_time = (double)value;
					}
					else if ( type == GPA_TYPE_UINT64 )
					{
						gpa_uint64 value;
						GPA_GetSampleUInt64( currentWaitSessionID, 
							0, counter_index, &value );
						printf( "%Elapsed ms: I64u,", value );
						elapsed_time = (double)value;
					}
					else if ( type == GPA_TYPE_FLOAT32 )
					{
						gpa_float32 value;
						GPA_GetSampleFloat32( currentWaitSessionID, 
							0, counter_index, &value );
						printf( "\nElapsed ms: %f,", value );
						elapsed_time = (double)value;
					}
					else if ( type == GPA_TYPE_FLOAT64 )
					{
						gpa_float64 value;
						GPA_GetSampleFloat64( currentWaitSessionID, 
							0, counter_index, &value );
						printf( "\nElapsed ms: %f,", value );
						elapsed_time = (double)value;
					}
				}
				currentWaitSessionID++;
			}

			if (ps<num_skip_tests)
				continue;

			avrg	   += elapsed_time;
			avrg_ticks += elapsed_ticks;
		}

		GPA_EndSession();
		//xray::render::device::ref().end_frame();
	}
	else if (m_benchmark_target==target_nvidia)
	{
		u64 start_time;
		
		int nNumPasses;
		NVPMBeginExperiment(&nNumPasses);
		
		u64 value, cycle = 0;
		
		NVPMRESULT res;
		//NVPMBeginExperiment(&nNumPasses);
		res = NVPMGetCounterValueByName("D3D driver time", 0, &start_time, &cycle);
		//NVPMEndExperiment();
		
		for ( u32 ps=0; ps < num_tests + num_skip_tests; ps++)
		{
			if (ps<num_skip_tests)
				continue;
			NVPMBeginExperiment(&nNumPasses);
			if (m_show_preview_window) xray::render::device::ref().begin_frame();
			for (u32 pass_index=0; pass_index < num_passes; pass_index++) 
			{
				for (s32 i=0; i<nNumPasses; i++)
				{
					NVPMBeginPass(i);
						test_object->execute(*this,pass_index+1,num_passes, gtimer);
					NVPMEndPass(i);
				}
			}
			if (m_show_preview_window) xray::render::device::ref().end_frame();
			NVPMEndExperiment();
			
			res = NVPMGetCounterValueByName("D3D driver time", 0, &value, &cycle);
			
			elapsed_time  = 0.0;
			elapsed_ticks = value - start_time;
			
			avrg	   += elapsed_time;
			avrg_ticks += elapsed_ticks;
		}
	}
#endif // USE_HARDWARE_COUNTERS

	avrg		/= num_tests?num_tests:1;
	avrg_ticks	/= num_tests?num_tests:1;
	
	//elapsed_time	= min_elapsed_time;
	//elapsed_ticks	= min_elapsed_ticks;
	
	elapsed_time	= avrg;
	elapsed_ticks	= avrg_ticks;
	
	if (num_passes)
	{
		if (elapsed_time)
		{
			test.result.elapsed_time	= (elapsed_time*1000.0*1000.0*1000.0)/(double)(num_passes) / (double)( m_window_size_x * m_window_size_y );
			test.result.fps				= 1.0f / ((float)elapsed_time/(float)(num_passes));
		}
		test.result.elapsed_ticks	= elapsed_ticks/num_passes;
		test.result.pixels_per_tick = ( (double)elapsed_ticks / (double)num_passes ) / (double)( m_window_size_x * m_window_size_y );
	}
	else
	{
		test.result.elapsed_time	= 0;
		test.result.fps				= 0;
		test.result.elapsed_ticks	= 0;
		test.result.pixels_per_tick = 0;
		test.result.frequency		= frequency;
	}
	
	test.result.triangles_count = 0; // get from nv/ati counters
	test.result.batch_count		= 0; // get from nv/ati counters
	test.result.frequency		= frequency;
	
	test.object->finalize(*this);
	
	m_current_test_index++;
	
	//m_results.insert(std::pair<std::string, counters_result>(test_name, test.result));
	
	return true;
	//::system("cls");
}

void benchmark::initialize_group_rts		( group_to_execute& group)
{
	for (u32 i=0; i<group.tests.size(); i++)
	{
		for (u32 j=0; j<group.tests[i].render_targets.size(); j++)
		{
			render_target& rt	= group.tests[i].render_targets[j];
			rt.rt				= resource_manager::ref().create_render_target( rt.name.c_str(), rt.width, rt.height , get_dxgi_format(rt.format), enum_rt_usage_render_target );
		}
	}
}

void benchmark::on_load_benchmark_file		( resources::queries_result & in_result )
{
	m_show_realtime_counter_results = true;
	m_show_preview_window			= false;

	if ( !in_result.is_successful() )
	{
		printf("Failed to load benchmark file!");
		m_is_valid_benchmark_file_name = false;
		return;
	}
	m_is_valid_benchmark_file_name = true;

	configs::lua_config_ptr config_ptr = static_cast_checked<configs::lua_config *>(in_result[0].get_unmanaged_resource().c_ptr());
	configs::lua_config_value const & config	=	config_ptr->get_root();
	
	configs::lua_config_value const & settings_config	=	config_ptr->get_root()["SETTINGS"];


	std::string log_file_name = "benchmark.log";

	// Read settings.
	for ( configs::lua_config::const_iterator it_s = settings_config.begin(); it_s != settings_config.end(); ++it_s )
	{
		configs::lua_config_value const & next_table_value	=	*it_s;
		fixed_string<260> next_table_name					= it_s.key();

		if (next_table_value.get_type()==configs::t_integer)
		{
			if (next_table_name=="resolution_x")
				m_window_size_x = (u32)next_table_value;
			else if (next_table_name=="resolution_y")
				m_window_size_y = (u32)next_table_value;
		}
		else if (next_table_value.get_type()==configs::t_string)
		{
			if (next_table_name=="log_file_name")
				log_file_name = (pcstr)next_table_value;
			else if (next_table_name=="target")
				m_benchmark_target = string_to_target( (pcstr)next_table_value );
		}
		else if (next_table_value.get_type()==configs::t_boolean)
		{
			if (next_table_name=="windowed")
				m_is_full_screen = !(bool)next_table_value;
			else if (next_table_name=="show_realtime_counter_results")
				m_show_realtime_counter_results = (bool)next_table_value;
			else if (next_table_name=="show_preview_window")
				m_show_preview_window = (bool)next_table_value;
			
		}
	}

	m_log_file_name = log_file_name;
	
	// Groups.
	for ( configs::lua_config_value::const_iterator it_g = config.begin(); it_g != config.end(); ++it_g )
	{
		configs::lua_config_value const & next_table_group	=	*it_g;
		fixed_string<260> next_table_name_group = it_g.key();
		
		if (next_table_name_group=="SETTINGS")
			continue;
		
		group_to_execute group;

		group.group_name			= next_table_name_group.c_str();
		group.num_circles			= 200;
		group.num_skipped_circles	= 5;
		group.order					= -1;
		group.has_base				= false;
		group.enabled				= true;

		std::string							default_test_class;
		u32									default_num_passes = 500;
		std::map<std::string, std::string>	default_parameters;
		bool								default_is_color_write_enable = false;
		bool								default_is_stencil_enable = false;
		
		// Get default values.
		for ( configs::lua_config_value::const_iterator	it_t = next_table_group.begin(); it_t != next_table_group.end(); ++it_t )
		{
			configs::lua_config_value const & group_field_value	=	*it_t;
			fixed_string<260> next_group_field_name = it_t.key();
			
			if ( group_field_value.get_type()==configs::t_integer)
			{
				if ( next_group_field_name=="num_passes")
					default_num_passes = (u32)group_field_value;
				else if ( next_group_field_name=="num_circles")
					group.num_circles = (u32)group_field_value;
				else if ( next_group_field_name=="num_skipped_circles")
					group.num_skipped_circles = (u32)group_field_value;
				else if ( next_group_field_name=="order")
					group.order = (u32)group_field_value;
			}
			else if ( group_field_value.get_type()==configs::t_string && next_group_field_name=="class")
				default_test_class = (pcstr)group_field_value;
			else if ( group_field_value.get_type()==configs::t_string)
				default_parameters[next_group_field_name.c_str()] = (pcstr)(pcstr)group_field_value;
			
			if ( group_field_value.get_type()==configs::t_boolean && next_group_field_name=="enabled")
				group.enabled = (bool)group_field_value;
			if ( group_field_value.get_type()==configs::t_boolean && next_group_field_name=="color_write_enable")
				default_is_color_write_enable = (bool)group_field_value;
			if ( group_field_value.get_type()==configs::t_boolean && next_group_field_name=="stencil_enable")
				default_is_stencil_enable = (bool)group_field_value;
			
		}
		
		// Tests, group fields.
		for ( configs::lua_config_value::const_iterator	it_t = next_table_group.begin(); it_t != next_table_group.end(); ++it_t )
		{
			configs::lua_config_value const & next_group_field	=	*it_t;
			fixed_string<260> next_group_field_name = it_t.key();
			
			configs::lua_config_value const & group_field_value	=	*it_t;
			
			if ( group_field_value.get_type()!=configs::t_table_named)
				continue;
			
			std::string test_class_name					  = default_test_class;
			std::string test_base_name					  = BASE_test_name;
			u32			test_num_passes					  = default_num_passes;
			s32			order							  = -1;
			bool		test_is_color_write_enable		  = default_is_color_write_enable;
			bool		test_is_stencil_enable			 = default_is_stencil_enable;
			
			std::map<std::string, std::string> parameters = default_parameters;
			
			// Test fields.
			for ( configs::lua_config_value::const_iterator	it_f = next_group_field.begin(); it_f !=	next_group_field.end(); ++it_f )
			{
				fixed_string<260> next_table_name_test = it_f.key();
				
				configs::lua_config_value const & field_value	=	*it_f;
				
				if ( field_value.get_type()==configs::t_integer )
				{
					if ( next_table_name_test=="order")
						order = (u32)field_value;
					else if ( next_table_name_test=="num_passes")
						test_num_passes = (u32)field_value;
					continue;
				}
				
				if ( field_value.get_type()==configs::t_string)
				{
					if ( next_table_name_test=="class")
						test_class_name							 = (pcstr)field_value;
					else if ( next_table_name_test=="base")
						test_base_name							 = (pcstr)field_value;
					else 
						parameters[next_table_name_test.c_str()] = (pcstr)field_value;
				}
				if ( field_value.get_type()==configs::t_boolean && next_table_name_test=="color_write_enable")
					test_is_color_write_enable = (bool)field_value;
				if ( field_value.get_type()==configs::t_boolean && next_table_name_test=="stencil_enable")
					test_is_stencil_enable = (bool)field_value;
				
			}
			
			test_to_execute test;
			
			test.depth_rt.is_valid = false;
			
			u32 const max_rt_count = 8;
			
			for (u32 i=0; i<max_rt_count; i++)
			{
				char buffer[128];
				::sprintf(buffer,"rt%d",i);
				
				if (parameters.find(buffer)!=parameters.end())
				{
					render_target rt;
					char buffer2[1024];
					::sprintf(buffer2,"$user$%s.%s.rt%d",group.group_name.c_str(),next_group_field_name.c_str(),i);
					
					rt.name				= buffer2;
					rt.format			= parameters[buffer].c_str();
					rt.width			= m_window_size_x;
					rt.height			= m_window_size_y;
					rt.is_valid			= true;
					
					test.render_targets.push_back( rt );
				}
				
				
				if (parameters.find("depth_rt")!=parameters.end())
				{
					render_target rt;
					char buffer2[1024];
					::sprintf(buffer2,"$user$%s.%s.depth_rt",group.group_name.c_str(),next_group_field_name.c_str());
					
					rt.name				= buffer2;
					rt.format			= parameters[buffer].c_str();
					rt.width			= m_window_size_x;
					rt.height			= m_window_size_y;
					
					test.depth_rt.is_valid = true;
					test.depth_rt = rt;
				}
			}
			
			test.class_name				= test_class_name;
			test.test_name				= next_group_field_name.c_str();
			test.parameters				= parameters;
			test.num_passes				= test_num_passes;
			test.object					= NULL;
			test.is_base				= (test.test_name==BASE_test_name);
			test.order					= order;
			test.group_name				= group.group_name;
			test.name_of_base			= test_base_name;
			test.is_color_write_enable	= test_is_color_write_enable;
			test.is_stencil_enable		= test_is_stencil_enable;
			
			if (test.is_base)
				group.has_base = true;
			
			group.tests.push_back( test);
		}
		m_groups.push_back( group);
	}
	
	
	// Sort groups and tests by name.
	std::sort(m_groups.begin(),m_groups.end());
	for (u32 i=0; i<m_groups.size(); i++)
		std::sort(m_groups[i].tests.begin(),m_groups[i].tests.end());
}

void benchmark::save_results()
{
	if (!m_is_valid_benchmark_file_name)
		return;
	
	FILE* output = fopen(m_log_file_name.c_str(),"a+");
	
	char buffer[2048];

	sprintf(buffer,"\n-------------------------------------------------------------------------------------\n");
	printf(buffer);
	fwrite(buffer,1,strlen(buffer),output);

	sprintf(buffer,"[%s, %s] Results for %S: (%dx%d)\n", 
		__DATE__, 
		__TIME__, 
		xray::render::device::ref().adapter_desc().Description, 
		m_window_size_x, 
		m_window_size_y
		);

	printf(buffer);
	fwrite(buffer,1,strlen(buffer),output);

	u32 max_name_len = 0;
	for ( u32 group_index=0; group_index<m_groups.size(); group_index++)
		for ( u32 test_index=0; test_index<m_groups[group_index].tests.size(); test_index++)
			if (max_name_len<m_groups[group_index].tests[test_index].test_name.length())
				max_name_len = m_groups[group_index].tests[test_index].test_name.length();

	for ( u32 group_index=0; group_index<m_groups.size(); group_index++)
	{
		group_to_execute& group = m_groups[group_index];

		if (!group.enabled)
			continue;

		std::string space_string;
		space_string.assign(max_name_len,' ');

		colorize::set_base_color();
		sprintf(buffer,"\n\n\n  %s (circles:%d, skipped circles:%d):\n\n         %snanosec/pixel    ns/p-base     ticks/frame        t/f-base     approx(t/f-base)\n\n", 
			m_groups[group_index].group_name.c_str(),
			m_groups[group_index].num_circles,
			m_groups[group_index].num_skipped_circles,
			space_string.c_str()
			);
		printf(buffer);
		fwrite(buffer,1,strlen(buffer),output);

		fflush(output);

		for ( u32 test_index=0; test_index<m_groups[group_index].tests.size(); test_index++)
		{
			test_to_execute& test = m_groups[group_index].tests[test_index];
			
			std::string space_string;
			space_string.assign(max_name_len-test.test_name.length(),' ');
			
			counters_result res;
			res = test.result;
			
			if ( test.test_name==BASE_test_name)
				m_groups[group_index].base_result = test.result;
			
			test_index%2==0?colorize::set_base_color():colorize::set_gray_color();
			
			sprintf(buffer,"    %4d. %s: %s%s          // %s\n", 
				test_index+1, 
				test.test_name.c_str(), 
				space_string.c_str(), 
				res.as_text(m_groups[group_index].base_result, test.is_base || !group.has_base, *this, group, test).c_str(),
				test.get_comments((u32)-1).c_str()
				);
			printf(buffer);
			fwrite(buffer,1,strlen(buffer),output);
		}
	}
	
	colorize::set_base_color();
	
	fclose(output);
}

void benchmark::start_execution()
{
	if (!m_is_valid_benchmark_file_name)
		return;
	printf("Benchmark '%s' started.\n", m_benchmark_file_name.c_str());
	printf("Execution...\n");
	
	timing::timer ctimer;
	FILE* output = NULL;

	if (m_show_realtime_counter_results)
		output = fopen(m_log_file_name.c_str(),"a+");
	
	char buffer[2048];
	u32 max_name_len = 0;

	if (m_show_realtime_counter_results)
	{
		sprintf(buffer,"\n-------------------------------------------------------------------------------------\n");
		printf(buffer);
		fwrite(buffer,1,strlen(buffer),output);

		sprintf(buffer,"[%s, %s] Results for %S: (%dx%d)\n", 
			__DATE__, 
			__TIME__, 
			xray::render::device::ref().adapter_desc().Description, 
			m_window_size_x, 
			m_window_size_y
			);

		printf(buffer);
		fwrite(buffer,1,strlen(buffer),output);
		fflush(output);

		
		for ( u32 group_index=0; group_index<m_groups.size(); group_index++)
			for ( u32 test_index=0; test_index<m_groups[group_index].tests.size(); test_index++)
				if (max_name_len<m_groups[group_index].tests[test_index].test_name.length())
					max_name_len = m_groups[group_index].tests[test_index].test_name.length();
	}

	// GPU timer.
	gpu_timer gtimer;

	ctimer.start();

	if (m_counters_valid)
	for ( u32 group_index=0; group_index<m_groups.size(); group_index++)
	{
		group_to_execute& group = m_groups[group_index];

		if (!group.enabled)
			continue;

		initialize_group_rts(group);

		for ( u32 test_index=0; test_index<group.tests.size(); test_index++)
		{
			test_to_execute& test = group.tests[test_index];
			benchmark_class* bmk_class = get_benchmark_class(test.class_name.c_str());
			if (bmk_class) 
				test.object = bmk_class->creator();
		}
	}

	if (!m_show_preview_window) xray::render::device::ref().begin_frame();

	// Execute groups.
	if (m_counters_valid)
	for ( u32 group_index=0; group_index<m_groups.size(); group_index++)
	{
		group_to_execute& group = m_groups[group_index];
		
		if (!group.enabled)
			continue;
		
		if (m_show_realtime_counter_results)
		{
			std::string space_string;
			space_string.assign(max_name_len,' ');
			
			colorize::set_base_color();
			sprintf(buffer,"\n\n\n  %s (circles:%d, skipped circles:%d):\n\n         %snanosec/pixel    ns/p-base     ticks/frame        t/f-base     approx(t/f-base)\n\n", 
				m_groups[group_index].group_name.c_str(),
				m_groups[group_index].num_circles,
				m_groups[group_index].num_skipped_circles,
				space_string.c_str()
				);
			printf(buffer);
			fwrite(buffer,1,strlen(buffer),output);
			
			fflush(output);
		}
		
		
		for ( u32 test_index=0; test_index<m_groups[group_index].tests.size(); test_index++)
		{
			test_to_execute& test = m_groups[group_index].tests[test_index];

			
			
			// Execute the test.
			bool valid_test = false;
			//for (u32 t=0; t<2; t++)
			{
				valid_test = execute_test(m_groups[group_index],test,gtimer,false);//t==0);
				//if (!valid_test)
				//	continue;
			}
			if (!valid_test)
			{
				printf("\nTest '%s.%s' failed\n",group.group_name.c_str(),test.test_name.c_str());
				continue;
			}
			

			if ( test.test_name==BASE_test_name)
				m_groups[group_index].base_result = test.result;

			bool use_other_base = false;
			counters_result base_result = m_groups[group_index].base_result;

			for ( u32 test_index_prev=0; test_index_prev<test_index; test_index_prev++)
			{
				test_to_execute& test_prev = m_groups[group_index].tests[test_index_prev];
				if (test_prev.test_name==test.name_of_base)
				{
					base_result = test_prev.result;
					use_other_base = true;
				}
			}

			
			
			backend::ref().reset();
			
			std::string space_string;
			
			if (m_show_realtime_counter_results)
				space_string.assign(max_name_len-test.test_name.length(),' ');

			if (m_show_realtime_counter_results)
			{
				test_index%2==0?colorize::set_base_color():colorize::set_gray_color();
				
				sprintf(buffer,"    %4d. %s: %s%s          // %s\n", 
					test_index+1, 
					test.test_name.c_str(), 
					space_string.c_str(), 
					test.result.as_text(base_result, test.is_base || !group.has_base, *this, group, test).c_str(),
					test.get_comments(500).c_str()
					);
				printf(buffer);
				fwrite(buffer,1,strlen(buffer),output);
				fflush(output);
			}
		}
	}

	//if (!m_show_preview_window) xray::render::device::ref().d3d_context()->Flush();
	if (!m_show_preview_window) xray::render::device::ref().end_frame();
	
	colorize::set_base_color();

	for ( u32 group_index=0; group_index<m_groups.size(); group_index++)
		for ( u32 test_index=0; test_index<m_groups[group_index].tests.size(); test_index++)
			if (  m_groups[group_index].tests[test_index].object )
				XRAY_DELETE_IMPL(g_allocator, m_groups[group_index].tests[test_index].object);
	
	if (m_show_realtime_counter_results)
		fclose(output);
	
	float sec = ctimer.get_elapsed_sec();
	
	u32	h = u32(sec/3600.f),
		m = h ? u32(sec-3600*h)/60 : u32(sec/60.f),
		s = m ? u32(sec-60.f*m-3600.f*h) : u32(sec);
	
	printf("\nDone! Benchmark time: %d h. %d m. %d s.", h, m, s);
	
	if (!m_show_realtime_counter_results)
		save_results();
}

bool benchmark::execute_benchmark_file		( char const* file_name)
{
//	char buff[512];
//	GetCurrentDirectory(512,buff);
	
	std::string new_file_name = "@";
	new_file_name += file_name;

	for (u32 i=0; i<new_file_name.length(); i++)
	{
		if (new_file_name[i]=='\\')
			new_file_name[i]='/';
	}
	
	m_window_size_x = 800;
	m_window_size_y = 800;
	m_is_full_screen = false;
	m_benchmark_target = target_general;
	
	new_file_name = file_name;//"resources/benchmark_temporary/benchmark.test";
	
	xray::resources::query_resource_and_wait(
		new_file_name.c_str(),
		xray::resources::lua_config_class,
		boost::bind( &benchmark::on_load_benchmark_file, this, _1 ),
		&g_allocator);
	xray::resources::wait_and_dispatch_callbacks(true);

	return m_is_valid_benchmark_file_name;
}


benchmark_class* 
benchmark::get_benchmark_class( char const* class_name) const
{
	benchmark_class* bmk_class = NULL;
	for ( benchmark_class* it=m_first_class; it!=NULL; it = it->next)
	{
		if (!it->creator)
		{
			printf("benchmark::execute(): one of class creators equal to 0!");
			continue;
		}
		if (it->name==class_name)
		{
			bmk_class = it;
			break;
		}
	}
	return bmk_class;
}

} // namespace graphics_benchmark
} // namespace xray