////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
/*
#include "test_grasping_controller.h"
#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>

using xray::rtp::test_grasping_controller;
using xray::rtp::test_grasping_world_params;
using xray::rtp::grasping_space_params;



	test_grasping_controller::test_grasping_controller():
	learning_controller< test_grasping_action >( "test_grasping", m_position ),
	m_target_position( float2(0,0) ),
	m_position ( world_space_param_type( float2(1,0) ,float2(0,0) ) ),
	m_current_walk_step( 0 )
{
	create_actions();
	learn_init();
	dbg_set_random_target();
}
void	test_grasping_controller::create_actions()
{
		//math::random32 rnd;

	//for( u32 i= 0; i < num_moving_actions; ++i )
	//{
	//	float rotation	= -2 * math::pi_d3 + float( 4 * math::pi_d3 * random( rnd ) );
	//	float forward	= - float ( 0.3f *  random( rnd ) );
	//	float strafe	= 0;//-0.025f + float(  0.05f * random( rnd ) ); 
	//	float2 translate( forward, strafe );
	//	NEW( test_grasping_action )
	//	(
	//		m_controller.base(),
	//		rotation,
	//		translate,
	//		false,
	//		grasping_space_params	()
	//	);
	//}
//0




NEW( test_grasping_action )
		(
			base(),
			- math::pi/10,-math::pi/20,
			float2( 0, 0 ),
			float2( 0.03f, 0.01f ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);
//1
NEW( test_grasping_action )
		(
			base(),
			 math::pi/23,math::pi/4,
			float2( 0.0, 0.0 ), float2( 0.0, 0.0 ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);
/////////////////////////////////////////////////////////////////////////////////////////////
//2
NEW( test_grasping_action )
		(
			base(),
			 -math::pi/15.f,-math::pi/4.5f,
			float2( 0.01f, 0.0f ),float2( 0.02f, 0.0f ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);

//3
NEW( test_grasping_action )
		(
			base(),
			 math::pi/4.3f,math::pi/1.7f,
			float2( 0.01f, 0.0f ),
			float2( 0.0f, 0.0f ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);
//////////////////////////////////////////////////////////////
//4
NEW( test_grasping_action )
		(
			base(),
			-math::pi/5.f, -math::pi/1.5f,
			float2( 0.01f, 0.0f ),float2( 0.01f, 0.0f ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);



//5
NEW( test_grasping_action )
		(
			base(),
			math::pi/2.f, math::pi/1.2f,
			float2( 0, 0 ),float2( 0, 0 ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//6
NEW( test_grasping_action )
		(
			base(),
			0, 0,
			float2( 0.1f, 0 ),float2( 0.3f, 0 ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

//7
NEW( test_grasping_action )
		(
			base(),
			0,0,
			float2( 0, 0 ),float2( 0, 0 ),
			5.f,
			true,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);
//8
NEW( test_grasping_action )
		(
			base(),
			- math::pi/15,math::pi/19,
			
			float2( 0, 0 ),
			float2( 0.0f, 0.001f ),
			1.f,
			false,
			grasping_space_params	( float2( -0.4f, 0 ) )
		);	
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
}


//float	test_grasping_controller::learn_step( )
//{
//	return learning_controller<test_grasping_action>::learn_step( );
//}

u32 max_walk_steps = 15;

void test_grasping_controller::on_new_walk_action( world_space_param_type& w, space_param_type	&params )
{
	XRAY_UNREFERENCED_PARAMETERS	( &w, &params );

	++m_current_walk_step;
	if( m_current_walk_step >max_walk_steps ) 
	{
		dbg_set_random_target();
		
	}
}

bool	test_grasping_controller::dbg_update_walk( float dt )
{
	
	if( update( m_position.current_position(), dt ) && action_time() >= current_action()->duration()   )
	{
		  dbg_set_random_target();
	
	}
	
	return false;		
}

void	test_grasping_controller::debug_render( xray::render::debug::renderer& renderer ) const 
{
	float3 target = render_pos( m_target_position );

	renderer.draw_sphere( target, 0.1f, math::color(  255, 0, 0 ) );
	//renderer.draw_sphere( target, 0.2f, math::color_argb( 60 ,0, 255, 0 ) );
	//renderer.draw_sphere( target, 0.6f, math::color_argb( 50 ,0, 255, 0 ) );
	//renderer.draw_sphere( target, 0.4f, math::color_argb( 40 ,255, 255, 0 ) );

	float3 pos = render_pos( m_position.m_current_position.pos );
	//renderer.draw_sphere( pos, 0.1f, math::color_xrgb( 0, 0, 255 ) );

	current_action()->render( m_position.m_current_position, renderer );
	float3 to = pos +   -float3( 0, m_position.m_current_position.dir.x, m_position.m_current_position.dir.y ) * 0.4f ;
	renderer.draw_arrow( pos, to, math::color( 0, 0, 255 ) );
	grasping_space_params p;
	from_world					( m_position.m_current_position, p );
	if(current_action()->target(p))
		renderer.draw_sphere( to, 0.2f, math::color(  255, 0, 0 ) );
}	

void	test_grasping_controller::dbg_set_random_target( )
{
	//m_position;
	//m_direction;

	grasping_space_params	rng; rng.set_random();

	m_target_position	= m_position.m_current_position.pos + rng.target_position();
	
	walk_init( 6, 0, m_position.m_current_position );

	m_current_walk_step=0;
}

void	test_grasping_controller::from_world( const test_grasping_world_params& w, grasping_space_params &p )const
{
	float2 target = m_target_position - w.pos;

	float2 right_dir = float2( -w.dir.y, w.dir.x );

	p = grasping_space_params( float2( target|w.dir, target|right_dir ) );
}
*/

/*


void test_grasping_controller::save( xray::configs::lua_config_value cfg, bool training_trees )const
{
	controller<test_grasping_action>::save( cfg ,training_trees );
}

void test_grasping_controller::load					( const xray::configs::lua_config_value &cfg )
{
	controller<test_grasping_action>::load( cfg );
}

void	test_grasping_controller::save_training_sets( xray::configs::lua_config_value cfg )const
{
	controller<test_grasping_action>::save_training_sets( cfg );
}

void	test_grasping_controller::load_training_sets( const xray::configs::lua_config_value &cfg )
{
	controller<test_grasping_action>::load_training_sets( cfg );
}

*/