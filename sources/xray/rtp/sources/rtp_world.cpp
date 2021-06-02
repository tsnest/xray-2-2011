////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "rtp_world.h"
#include "resource_cooks.h"

#include <xray/rtp/engine.h>

//#include "animation_grasping_controller.h"
//#include "test_grasping_controller.h"

#include <xray/rtp/base_controller.h>

#include <xray/render/world.h>
#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/game_renderer.h>

#include <xray/animation/world.h>
#include <xray/animation/i_animation_controller_set.h>

#include <xray/configs.h>
#include <xray/console_command.h>
#include <xray/input/world.h>
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>

#include "testing.h"

#include "controller.h"

namespace xray {
namespace rtp {

struct command_bool: 
	public console_commands::cc_bool
{
	typedef console_commands::cc_bool			super;
						command_bool		(pcstr name, bool& value, bool serializable, console_commands::command_type const command_type ):
							super			( name, m_load_settings, serializable, command_type ),
							m_load_settings	( false ),
							m_value			( value )
							{

							}

	virtual void		execute					(pcstr args)
	{
		super::execute( args );
		if( m_load_settings )
			m_value = true;
	}

	bool		&m_value;
	bool 		m_load_settings;

}; // struct command_bool


command_line::key	rtp_learn_disable_tasks("rtp_learn_disable_tasks", "", "rtp", "");

command_line::key	g_learn_task_count( "rtp_learn_task_count", "rtp", "task count for learning process iteration", "" );

static bool b_render = true;
static console_commands::cc_bool rtp_test_render( "rtp_render", b_render, true, console_commands::command_type_user_specific );

static bool 	b_pause = false;
static console_commands::cc_bool rtp_test_pause( "rtp_pause", b_pause, true, console_commands::command_type_user_specific );

static bool 	b_learn = false;
static command_bool rtp_test_learn( "rtp_learn", b_learn, false, console_commands::command_type_user_specific );

static bool 	b_walk = true;
static console_commands::cc_bool rtp_test_walk( "rtp_walk", b_walk, true, console_commands::command_type_user_specific );

static bool 	b_save = false;
static console_commands::cc_bool rtp_test_save( "rtp_save", b_save, false, console_commands::command_type_user_specific );

static bool 	b_save_training = false;	
static console_commands::cc_bool rtp_test_save_training( "rtp_save_training", b_save_training, true, console_commands::command_type_user_specific );

static bool 	b_load_training = false;
static console_commands::cc_bool rtp_test_load_training( "rtp_load_training", b_load_training, true, console_commands::command_type_user_specific );

static bool 	b_load = false;
static command_bool rtp_test_load( "rtp_load", b_load, true, console_commands::command_type_user_specific );

bool b_learn_trees_frozen =false;
static console_commands::cc_bool learn_trees_frozen( "rtp_learn_trees_frozen", b_learn_trees_frozen, true, console_commands::command_type_user_specific );

bool b_learn_prune = true;
static console_commands::cc_bool learn_prune( "rtp_learn_prune", b_learn_prune, true, console_commands::command_type_user_specific );

bool b_render_learn = true;
static console_commands::cc_bool render_learn( "rtp_learn_render", b_render_learn, true, console_commands::command_type_user_specific );

bool b_camera_control = true;
static console_commands::cc_bool camera_control( "rtp_camera_control", b_camera_control, true, console_commands::command_type_user_specific );


float rtp_learn_max_residual = 70.f;
static console_commands::cc_float max_residual( "rtp_learn_max_residual", rtp_learn_max_residual, 0.f, 10000.f, true, console_commands::command_type_user_specific );

u32	rtp_learn_max_iterations = 25;
static console_commands::cc_u32 learn_max_iterations( "rtp_learn_max_iterations", rtp_learn_max_iterations, 0, 1000000, true, console_commands::command_type_user_specific );

string32 s_current_controller_set = "human_controller_set";

//static console_commands::cc_string	animation_controller_set( "animation_controller_set", s_current_controller_set, sizeof(s_current_controller_set), true, console_commands::command_type_user_specific );
//human_controller_set

static bool b_reset_controllers	= false;
static console_commands::cc_bool reset_controllers	( "rtp_reset_controllers", b_reset_controllers, false, console_commands::command_type_user_specific );

xray::math::random32_with_count	xray::rtp::rtp_world::m_random;

rtp_world::rtp_world(
		xray::rtp::engine& engine,
		xray::animation::world& animation,
		xray::render::scene_ptr const& scene,
		xray::render::game::renderer* renderer,
		pcstr const resources_xray_path
	) :
	m_renderer			( renderer ),
	m_animation_world	( &animation ),
	m_scene				( scene ),
	m_resource_path		( resources_xray_path ),
	
	m_is_loaded			( false ),
	m_previous_ms		( 0 )
{
	XRAY_UNREFERENCED_PARAMETER( engine );
	m_resources					= NEW ( resource_cooks )( *this );
	m_timer.start		( );
}

rtp_world::~rtp_world	( )
{
	clear_controllers( );
	DELETE( m_resources );
}

void	rtp_world::reset_controllers( )
{
	clear_controllers( );
	create_test_controllers( );
	b_reset_controllers = false;
}


void rtp_world::clear_resources	( )
{
	clear_controllers();
}


void	rtp_world::clear_controllers( )
{
	//vector<animation_controller_ptr>::iterator i = m_controllers.begin(), e = m_controllers.end();

	//for( ; i != e  ; ++i )
	//	DELETE( (*i) );
	m_controllers.clear();
}

void	rtp_world::destroy_controller					( base_controller* &ctrl )
{
	DELETE( ctrl );
}

base_controller	*rtp_world::create_navigation_controller( pcstr name, pcstr animation_set_path )
{
	return rtp::create_navigation_controller( name, *m_animation_world, animation_set_path );
}

base_controller	*rtp_world::create_navigation_controller( pcstr name, const xray::configs::binary_config_ptr &cfg )
{
	return rtp::create_navigation_controller( name, *m_animation_world, cfg );
}

bool check_loaded( const vector< animation_controller_ptr > & controllers )
{
	vector< animation_controller_ptr>::const_iterator i = controllers.begin(), e =	controllers.end();

	for( ;i!=e; ++i )
		if( !(*i)->is_loaded( ) )
			return false;
	return true;


}


void	rtp_world::render( ) const 
{
	if( !b_render )
		return;

	vector< animation_controller_ptr >::const_iterator i = m_controllers.begin(), e = m_controllers.end();
	for( ; i != e  ; ++i )
		(*i)->render( m_scene, m_renderer->scene(), m_renderer->debug(), (*i)->model(), true );
		
}

void	rtp_world::dbg_move_control( float4x4 &view_inverted,  input::world& input_world )
{
	if( dbg_input_commands( input_world ) )
		return;
	
	vector< animation_controller_ptr >::const_iterator i = m_controllers.begin(), e = m_controllers.end();

	for( ; i != e  ; ++i )
		(*i)->dbg_move_control( view_inverted, input_world );
}

extern u32 g_dbg_trajectory_render_diapason_id;
extern u32 g_dbg_trajectory_render_diapason_size;

bool rtp_world::dbg_input_commands ( input::world&  input )
{
	static u32 pressed_keys = 0;
	u32 new_pressed_keys = 0;
	const u32 mask0 = ( 1<<0 ), mask1 = ( 1<<1 ),   mask2 = ( 1<<2 ), mask3 = (1<<3), mask4 = (1<<4) ; 

	non_null<input::keyboard const>::ptr const keyboard	= input.get_keyboard();

	if (	keyboard->is_key_down( input::key_lcontrol ) 
			&&
			keyboard->is_key_down( input::key_s ) 
		) 
	{
			learn_single_step();
			return true;

	} else if(	!keyboard->is_key_down( input::key_lcontrol ) &&
				keyboard->is_key_down( input::key_n )  
			) {

		new_pressed_keys = new_pressed_keys | mask0;
		if( !( pressed_keys & mask0 ) )
				++g_dbg_trajectory_render_diapason_id;
		
	} else if( keyboard->is_key_down( input::key_n )  &&
			   keyboard->is_key_down( input::key_lcontrol )  
		) 
	{
		new_pressed_keys = new_pressed_keys | mask3;
		if( !( pressed_keys & mask3 ) && g_dbg_trajectory_render_diapason_id > 0 )
				--g_dbg_trajectory_render_diapason_id;
	}
	else if(  keyboard->is_key_down( input::key_lcontrol ) 
				&&
			  keyboard->is_key_down( input::key_i ) 
			) 
	{
		new_pressed_keys = new_pressed_keys | mask1;
		if( !( pressed_keys & mask1 ) )
			++g_dbg_trajectory_render_diapason_size;
		
	} else if(
				keyboard->is_key_down( input::key_lcontrol ) 
				&&
				keyboard->is_key_down( input::key_u ) 
				&&
				g_dbg_trajectory_render_diapason_size > 1
			) 
	{
		new_pressed_keys = new_pressed_keys | mask2;
		if(!( pressed_keys & mask2 ))
			--g_dbg_trajectory_render_diapason_size;
		
	}	else if(  keyboard->is_key_down( input::key_lcontrol ) 
					&&
				keyboard->is_key_down( input::key_b ) 
			) 
	{
		new_pressed_keys = new_pressed_keys | mask4;
		if(!( pressed_keys & mask4 ) )
			b_learn = false;
			
	}
	pressed_keys = new_pressed_keys;
	return ( pressed_keys != 0 );
}

void	rtp_world::on_controllers_loaded( resources::queries_result& data )
{

	if( !data.is_successful() )
	{
		LOG_ERROR( "controller : %s not found" , s_current_controller_set );
		m_is_loaded = true;
		return;
	}

	ASSERT( data.is_successful() );

	const u32 sz = data.size();
	
	for( u32 i = 0; i<sz; ++i )
	{
		//m_load_temp_settings = static_cast_resource_ptr< configs::binary_config_ptr >( data[i].get_unmanaged_resource() );
		//configs::binary_config_value cfg = m_load_temp_settings->get_root();

		//base_controller *ctrl =  create_controller( s_current_controller_set, m_load_temp_settings, *m_animation_world );

		animation_controller_ptr	ctrl			= static_cast_resource_ptr<animation_controller_ptr>(data[0].get_unmanaged_resource());

		m_load_temp_settings = 0;

		m_controllers.push_back	( ctrl ) ;
		ctrl->on_attach			( );

	}
}

void rtp_world::load( )
{

	b_load = false;
	m_is_loaded = false;

	clear_controllers( );
	u32 queried_controllers_count = query_learned_controllers( );

	for ( ;; ) {
		if ( threading::g_debug_single_thread )
			resources::tick					( );

		xray::resources::dispatch_callbacks	( );
		xray::threading::yield	( 10 );
		
		if( m_is_loaded || queried_controllers_count == m_controllers.size( ) && check_loaded( m_controllers ) )
			break;

	}

	m_timer_continious.start();
	m_is_loaded = true;


}
const	float normal_frame_rate = 30.f;

static void walk_step_controllers(  vector< animation_controller_ptr > &controllers, float dt )
{
		const float time_scale =  normal_frame_rate;
		
		vector< animation_controller_ptr>::const_iterator b = controllers.begin(), e = controllers.end();
		vector< animation_controller_ptr>::const_iterator  i = b;	
		for( ; i != e  ; ++i )
				(*i)->dbg_update_walk( dt * time_scale );
}

void	rtp_world::tick( )
{
	
	m_random.set_current_thread_id();

#ifndef MASTER_GOLD
	if( b_save )
	{
		save_controllers( m_controllers, b_save_training );
		b_save = false;
	}
#endif // #ifndef MASTER_GOLD
	if( b_reset_controllers )
		reset_controllers();

	if( b_load )
		load();

	render( );

	if( b_pause )
		return;

	(void)b_learn_trees_frozen;
	bool was_prune = b_learn_prune;


	if( b_learn  )
		b_learn = learn_step( rtp_learn_max_residual, rtp_learn_max_iterations );
	else if( !b_learn )
		learn_stop_controllers( m_controllers );
	
	if( was_prune && !b_learn_prune )
		b_save  = true;



	if( b_walk )
	{
		u32 const new_ms		= m_timer.get_elapsed_msec();
		float const time_delta	= float(new_ms - m_previous_ms)/1000.f;
		m_previous_ms			= new_ms;
		walk_step_controllers	( m_controllers, time_delta );
	}
}

void	rtp_world::create_test_controllers	( )
{

	//m_controllers.push_back( rtp::create_navigation_controller( *m_animation_world, "resources/tests/rtp/navgation_controller/animation/controller_set.lua" ) );
	
	//m_controllers.push_back( rtp::create_navigation_controller( *m_animation_world, "resources/test/animations/dummy_controller_set.lua" ) );
	//m_controllers.push_back( rtp::create_navigation_controller( *m_animation_world, "resources/test/animations/human_controller_set.lua" ) );

	//pcstr controller_set = 0;
	//STR_JOINA( controller_set, "resources/animations/controllers/", s_current_controller_set, ".lua" );
	//m_controllers.push_back( rtp::create_navigation_controller( s_current_controller_set, *m_animation_world, controller_set ) );
	////s_current_controller_set
	//for ( ;; ) {
	//		xray::resources::dispatch_callbacks	( );
	//		xray::threading::yield	( 100 );
	//		
	//		if( check_loaded( m_controllers ) )
	//			break;

	//}


	query_controller_sets();

	for ( ;; ) {
			xray::resources::dispatch_callbacks	( );
			xray::threading::yield	( 100 );
			
			if( check_loaded( m_controllers ) )
				break;

	}
}

u32		rtp_world::query_controller_sets			( )
{
		pcstr controller_set = 0;
		STR_JOINA( controller_set, "resources/animations/controllers/", s_current_controller_set );

		resources::request	resources[]	= {
					{ controller_set,	xray::resources::animation_controller_class },
		};

		u32 count_queried = array_size( resources );
		
		resources::user_data_variant v;
		controller_resource_user_data data;
		data.empty_set = true;
		data.name = s_current_controller_set;
		
		//v.set( pcstr(s_current_controller_set) );
		v.set( data );
		resources::user_data_variant const * pv = &v;

		xray::resources::query_resources(
			resources,
			count_queried,
			boost::bind(&rtp_world::on_controllers_loaded, this, _1),
			xray::rtp::g_allocator,
			& pv
		);
		return count_queried;
}

u32	rtp_world::query_learned_controllers( )
{

		pcstr controller_set = 0;
		
		STR_JOINA( controller_set, "resources/rtp/controllers/", s_current_controller_set );

		resources::request	resources[]	= {
					//{ controller_set,	xray::resources::binary_config_class },
					{ controller_set,	xray::resources::animation_controller_class },
		};
		
		u32 count_queried = array_size( resources );

		//resources::user_data_variant* v = create_variant( true );

		//resources::user_data_variant v;
		//resources::user_data_variant const* pv = &v;
		//controller_resource_user_data data;
		//data.animation_allocator = g_allocator;

		//v.set<controller_resource_user_data>(data);

		xray::resources::query_resources(
			resources,
			count_queried,
			boost::bind(&rtp_world::on_controllers_loaded, this, _1),
			xray::rtp::g_allocator
			//,&pv
		);
		
		//delete_variant		( v );

		return count_queried;

}

void rtp_world::save_controllers( const vector< animation_controller_ptr > &controllers, bool training )
{
#ifndef	MASTER_GOLD
	vector< animation_controller_ptr>::const_iterator b = controllers.begin(), e = controllers.end();
	vector< animation_controller_ptr>::const_iterator  i = b;
	for( ; i != e  ; ++i )
	{
		xray::configs::lua_config_ptr cfg		= xray::configs::create_lua_config();
		xray::configs::lua_config_value v_cfg	=*cfg;
		( *i )->save(v_cfg, training );
		//pcstr tp = ( *i )->type();
		//string16 n;
		//xray::sprintf(n,"%d",u32( i - b ));
		//pcstr name = 0;
		
		//STR_JOINA		( name, m_resource_path, "sources/rtp/controllers/", tp, "_", n, ".controller"  );
		
		pcstr name		= 0;
		STR_JOINA		( name, "resources/rtp/controllers/", (*i)->name(), ".controller"  );

		cfg->save_as	( name, configs::target_sources );
	}
#else // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETERS( &controllers, training );
	NODEFAULT();
#endif // #ifndef MASTER_GOLD
}


void rtp_world::learn_single_step		( )
{
		learn_step( 0, u32(-1) );
}

bool	rtp_world::learn_step( float stop_residual, u32 stop_iterations )
{

		if( m_controllers.empty() )
			create_test_controllers( );
	
		vector< animation_controller_ptr>::const_iterator b = m_controllers.begin(), e = m_controllers.end();
		vector< animation_controller_ptr>::const_iterator  i = b;	
		bool ret = false;
		for( ; i != e  ; ++i )
			ret =	(*i)->learn_step( stop_residual, stop_iterations ) || ret;

		return ret;
}

bool	rtp_world::learning( )
{

		vector< animation_controller_ptr>::const_iterator b = m_controllers.begin(), e = m_controllers.end();
		vector< animation_controller_ptr>::const_iterator  i = b;	
		for( ; i != e  ; ++i )
				if( (*i)->learn_initialized	( ) )
					return true;
		return false;

}

void	rtp_world::testing( )
{
	m_random = math::random32_with_count( 0 );
	
	rtp::testing *tests = NEW( rtp::testing )( *this );


	//xray::memory::doug_lea_allocator* 
	//	allocator = static_cast_checked<xray::memory::doug_lea_allocator*> ( g_allocator );
	//u32 id_allocator_current = allocator->user_thread_id		( );
	//allocator->user_current_thread_id();	

	tests->test	( );

	//allocator->user_thread_id( id_allocator_current );

	DELETE( tests );
}

void	rtp_world::save_testing			( )
{
#ifndef MASTER_GOLD
	
	m_random = math::random32_with_count( 0 );

	rtp::testing *tests = NEW( rtp::testing )( *this );
	
	tests->save_test( );

	DELETE( tests );
#endif // #ifndef MASTER_GOLD
}

#ifndef MASTER_GOLD
	void	rtp_world::get_controllers_dump( string4096 &s ) const
	{
		if( m_controllers.empty() )
			return;
		if( !m_controllers[0]->get_controller( ) )
			return;

		m_controllers[0]->get_controller( )->get_dump_string( s );
	}
#endif


void	rtp_world::set_scene( render::scene_ptr const& scene )
{
	ASSERT( scene );
	m_scene = scene;
}


} // namespace rtp
} // namespace xray
