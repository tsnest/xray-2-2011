#include "pch.h"
#include "rms_generator_application.h"
#include "rms_generator_memory.h"
#include "xray/buffer_string.h"
#include <xray/core/core.h>
#include <xray/os_include.h>
#include "rms_evaluator.h"
#include <xray/core/simple_engine.h>
#include <xray/math_curve.h>
#include <xray/sound/sound_spl.h>
#include <xray/resources_classes.h>
#include "xray/sound/sources/sound_spl_cook.h"

using namespace xray;

static xray::command_line::key				s_input_filename	("input_file_name",		"i", "RMS Generator", "set input file name");

namespace rms_generator
{

	xray::memory::doug_lea_allocator_type	g_rms_generator_allocator;
	static xray::core::simple_engine		s_core_engine;

	void application::initialize	( u32 argc, pstr const* const argv )
	{
		m_argv								=	argv;
		m_argc								=	argc;

		xray::core::preinitialize			(
			&s_core_engine,
			core::create_log, 
			GetCommandLine(), 
			command_line::contains_application_true,
			"rms_generator",
			__DATE__
		);

		g_rms_generator_allocator.do_register			(64*1024*1024,	"rms_generator");
		g_rms_generator_allocator.user_current_thread_id();

		xray::memory::allocate_region		( );
		xray::core::initialize				( "../../resources/sources/", "rms_generator", core::perform_debug_initialization );
	}

	void   application::finalize	()
	{
		xray::core::finalize				( );
	}

	void   application::execute		()
	{
		m_exit_code							=	0;

		//if(!s_input_filename.is_set())
		//{
		//	printf							("Input file name is not typed! Please type input file name for process.");
		//	return;
		//}
		//char buffer[256];
		//xray::buffer_string input_file_name(&buffer[0], array_size(buffer));

		//s_input_filename.is_set_as_string	(&input_file_name);
		//rms_generator::process_file			(input_file_name.c_str());

//		sound::sound_spl_cook				s_sound_spl_cook;
//		register_cook						( &s_sound_spl_cook );

		vectora< math::curve_point<float> > vec( g_rms_generator_allocator );
		int i = 0;
		float La = 0.0f;
		float spl = 60.0f;


		do
		{
			math::curve_point<float> pt;
			float dist = 1.0f  * i;
			// Inverse Square Law
			float coeff = 0.0f;
			if ( math::is_zero( dist ))
			{
				coeff = 1.0f;
			}
			else
			{
				coeff = 1.0f / ( dist * dist );
			}
			La = 20 * log10( coeff ) + spl;

			if ( La > 0.0f )
			{
				pt.time							= static_cast<float>( i );
				pt.upper_value					= La;
				pt.lower_value					= La;
				vec.push_back					( pt );
			}

			i += 10;
		} while ( La > 0.0f );	


		configs::lua_config_ptr cfg				= configs::create_lua_config( "../../../../resources/sources/sounds/test.spl" );
		configs::lua_config_value val			= cfg->get_root( )["spl"];

		for ( u32 i = 0; i < vec.size( ); ++i )
		{

			xray::fixed_string<8> key_name;
			key_name.assignf				("key%d", i);
			pcstr str						= key_name.c_str();

			val[str]["position"]		= math::float2 ( vec[i].time, vec[i].upper_value );
			if ( i == 0 )
			{
				val[str]["key_type"]		= 0;
				val[str]["right_tangent"]	= math::float2( vec[i + 1].time, vec[i + 1].upper_value ).normalize();
				val[str]["left_tangent"]	= math::float2( 0.0f, 0.0f );
			}
			else if ( i == vec.size( ) - 1 )
			{
				val[str]["key_type"]		= 0;
				val[str]["left_tangent"]	= math::float2( vec[i - 1].time, vec[i - 1].upper_value );
				val[str]["right_tangent"]	= math::float2( 0.0f, 0.0f );
			}
			else
			{
				val[str]["key_type"]		= 0;
				math::float2 tg_left		( vec[i + 1].time - vec[i - 1].time, vec[i + 1].upper_value - vec[i - 1].upper_value );
				tg_left.normalize			( );
				val[str]["left_tangent"]	= math::float2( vec[i].time - tg_left[0], vec[i].upper_value - tg_left[1] );
				val[str]["right_tangent"]	= math::float2( vec[i].time + tg_left[0], vec[i].upper_value + tg_left[1] );
			}

		}

		cfg->save				( );

		{


			math::float_curve curve;
//			configs::lua_config_ptr cfg				= configs::create_lua_config( "z:/test.spl" );
			curve.load								( val );
			//curve.calc_tangents						( );
			for ( float i = 0.0f; i < curve.curve_time_max; i+= 0.4f )
			{
				LOG_DEBUG			( "time = %f,\t val = %f", i, curve.evaluate( i, 0.0f ) );
			}
		}
		m_exit_code							=	s_core_engine.get_exit_code();
	}

	void application::on_config_loaded	( resources::queries_result& queries )
	{
		configs::lua_config_ptr config					= static_cast_resource_ptr<configs::lua_config_ptr>( queries[0].get_unmanaged_resource() );

		math::float_curve curve;
		curve.load										( config->get_root( )["spl"] );
		for ( float i = 0.0f; i < curve.curve_time_max; i+= 0.2f )
		{
			LOG_DEBUG			( "val = %f", i );
		}
	}

} // namespace rms_generator