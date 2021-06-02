////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "testing.h"

#include <xray/animation/engine.h>
#include <xray/rtp/engine.h>
#include <xray/rtp/base_controller.h>
#include <xray/resources_fs.h>

#include "rtp_world.h"

#include <xray/game_test_suite.h>
#include <xray/testing_macro.h>

#include <xray/macro_library_name.h>

XRAY_DECLARE_LINKAGE_ID( rtp_tester );

namespace xray {
namespace rtp {

command_line::key	cmd_save_test( "rtp_save_test", "", "rtp", "" );


testing::testing( rtp_world& w ): 
m_rtp_world( w ), 
m_test_controller( 0 ),
m_controller( 0 )
{

}
testing::~testing()
{
	m_rtp_world.destroy_controller( m_test_controller );
	m_rtp_world.destroy_controller( m_controller );
}

void testing::load( )
{
	pcstr controller_set_path = "resources/tests/rtp/navgation_controller/animation/controller_set.lua";
	m_controller = m_rtp_world.create_navigation_controller( "test", controller_set_path );
	
	LOG_WARNING( "loading controller for calculation.." );

	for ( ;; ) {
		if ( threading::g_debug_single_thread ) 
			xray::resources::tick ( );

		xray::resources::dispatch_callbacks	( );
		xray::threading::yield	( 10 );
		
		if( m_controller->is_loaded() )
			break;

	}
	LOG_WARNING( "loaded controller for calculation" );

}

void	testing::on_resources_loaded( resources::queries_result& data )
{
	//LOG_WARNING( "saved controller on_resources_loaded" );
	
	R_ASSERT( data.is_successful() );

	m_settings = static_cast_resource_ptr< configs::binary_config_ptr >( data[0].get_unmanaged_resource() );
	configs::binary_config_value cfg = m_settings->get_root();

	m_test_controller =m_rtp_world.create_navigation_controller( "test", cfg );
}

static pcstr platform_name()
{
#if XRAY_PLATFORM_WINDOWS_64
	return "x64";
#else // #if XRAY_PLATFORM_WINDOWS_64
	return "win32";
#endif // #if XRAY_PLATFORM_WINDOWS_64
}



pcstr	testing::value_test_dir				( )const
{
	return "resources/tests/rtp/navgation_controller/value";
}

pcstr	testing::controller_name				( )const
{
	return XRAY_LIBRARY_NAME( navigation_0, controller );
	//return "navigation_0.controller";
}


fs_new::virtual_path_string	testing::value_controller_path		()const
{
	fs_new::virtual_path_string ret;
	ret.append( value_test_dir		( ) );	ret.append( "/" );
	ret.append( platform_name		( ) );	ret.append( "/" );
	//ret.append(	configuration_name	( ) );	ret.append( "/" );
	ret.append( controller_name		( ) );

	return ret;

}
#ifndef MASTER_GOLD
void test_float( xray::configs::lua_config_value const &v_cfg, u32 size, u32 level )
{
	
	for( u32 i = 0; i < size; ++i  )
	{	
		xray::configs::lua_config_value v_data = v_cfg["data"];
		v_data["test_float3"] = float3(1.03f,2.004f,0.000f);
		//v_data["test_float2"] = float2(1.03,2.004);
		//v_data["test_float"] = 1.03f;
		//v_data["test_name"] = "test";
	}
	if( level == 0 )
		return;
	test_float( v_cfg["level"] , size, level-1 );
}
#endif // MASTER_GOLD

void	testing::save_test_controller( rtp::base_controller	*ctrl  )
{
	
#ifndef MASTER_GOLD

		xray::configs::lua_config_ptr cfg		= xray::configs::create_lua_config();
		xray::configs::lua_config_value v_cfg	=*cfg;

		R_ASSERT_U( ctrl );

	//	ctrl->save( v_cfg, false );

		

		//test_float( v_cfg, 1, 59  );
		test_float( v_cfg, 1, 240  );

		fs_new::virtual_path_string name	= value_controller_path		();

		fs_new::native_path_string		physical_path;
		if ( !resources::convert_virtual_to_physical_path(& physical_path, name.c_str(), resources::sources_mount) )
			FATAL		( "Cannot convert logical path '%s' to physical", name.c_str() );

		
		cfg->save_as	(  physical_path.c_str(), configs::target_sources );
#else
	XRAY_UNREFERENCED_PARAMETER					( ctrl );

	NODEFAULT();

#endif

}

void	testing::load_test_controller( )
{

		fs_new::virtual_path_string const & name = value_controller_path();
		
		resources::request	resources[]	= {
			{ name.c_str(),	xray::resources::binary_config_class },
		};

		LOG_WARNING( "saved controller query path %s", name.c_str() );

		xray::resources::query_resources(
			resources,
			array_size(resources),
			boost::bind(&testing::on_resources_loaded, this, _1),
			xray::rtp::g_allocator
		);
		
		LOG_WARNING( "loading saved controller.." );

		for ( ;; ) {
			xray::resources::dispatch_callbacks	( );
			xray::threading::yield	( 10 );
			
			if( m_test_controller && m_test_controller->is_loaded() )
				break;

		}
		
		LOG_WARNING( "saved controller loaded" );

		m_settings = 0;
}

void	testing::calculate( )
{
	do {
		xray::resources::dispatch_callbacks	( );
	} while (
		m_controller->learn_step( 70.f, 100 )
	);
}
void		testing::test						( )
{
	
	load( );
	
	load_test_controller( );

	calculate( );

	TEST_ASSERT ( m_controller->compare_value( *m_test_controller ) ) ;

}
void		testing::save_test					( )
{
	
	//load( );

	//calculate( );

	save_test_controller( m_controller );
}

struct rtp_tester
{
	rtp_tester( )
	{
	}

	void test ( game_test_suite const * const suite )
	{
		TEST_ASSERT( suite->rtp_world() );

		if( cmd_save_test )
		{
			suite->rtp_world()->save_testing();
			return;
		}

		suite->rtp_world()->testing();

	}

}; //struct rtp_tester

//REGISTER_TEST_CLASS( rtp_tester, game_test_suite );



} // namespace rtp

} // namespace xray