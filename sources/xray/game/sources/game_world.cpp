////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_world.h"
#include "game.h"
#include "camera_director.h"
#include "cell_manager.h"
#include "bullet_manager.h"
#include <xray/console_command.h>
#include "event_manager.h"
#include "object_scene.h"
#include "hud.h"
#include "free_fly_camera.h"
#include <xray/physics/api.h>
#include <xray/physics/world.h>

#include "test_anim_object.h"

namespace stalker2{

#ifdef XRAY_STATIC_LIBRARIES
extern xray::uninitialized_reference< xray::memory::doug_lea_allocator_type >	g_physics_allocator;
#endif // #ifdef XRAY_STATIC_LIBRARIES

extern pcstr  editor_project_ext;
extern pcstr  resources_converted_path;
extern pcstr  resources_path;


extern bool	b_rtp_dbg_input;
static console_commands::cc_bool rtp_dbg_input( "rtp_dbg_input", b_rtp_dbg_input, true, console_commands::command_type_user_specific );

game_world::game_world( game& game )
:m_game					( game ),
m_cell_manager			( NEW (cell_manager) ( game ) ),
m_bullet_manager		( NEW (bullet_manager) ( game ) ),
m_collision_tree		( NULL ),
m_game_time_ms			( 0 ),
m_game_time_sec			( 0 ),
m_last_frame_time_ms	( 0 ),
m_last_frame_time_sec	( 0 ),
m_physics_world			( NULL ),
m_test_anim_object		( NULL )
{
	m_camera_director					= NEW(camera_director)	( *this );
	m_camera_director->set_no_delete	( );
	turn_rtp_debug						( NULL );
	rtp_dbg_input.subscribe_on_change( boost::bind( &game_world::turn_rtp_debug, this, _1 ) );

	init_physics		( );
	m_hud				= NEW(hud)( *this );

	m_free_fly_camera	= NEW(free_fly_camera)(*this);

//	m_test_anim_object	= NEW(test_anim_object)(*this);
}

void game_world::init_physics( )
{
#ifdef XRAY_STATIC_LIBRARIES
	m_physics_world					= physics::create_world_bt( stalker2::g_physics_allocator.c_ptr(), *this );
#else // #ifdef XRAY_STATIC_LIBRARIES
	m_physics_world					= physics::create_world_bt( memory::g_crt_allocator, *this );
#endif // #ifdef XRAY_STATIC_LIBRARIES
	m_physics_world->initialize		( );
}

bool g_physics_enabled = true;
void game_world::test_physics1( )
{
	g_physics_enabled = !g_physics_enabled;
	LOG_INFO("physics is %s now.", g_physics_enabled?"enabled":"disabled");
//	if(!m_dynamicsWorld)
//		return;
//
//	if(body_added)
//		m_dynamicsWorld->removeRigidBody( m_dynamic_body );
//
//
//	btTransform			trans;
//	trans.setRotation	( start_rotation );
//	trans.setOrigin		( start_point );
//
//	m_dynamic_body->setWorldTransform	( trans );
////	m_dynamic_body->setAngularVelocity	( btVector3(math::pi_d2, math::pi_d2,0) );
//	m_dynamicsWorld->addRigidBody	( m_dynamic_body );
//	m_dynamic_body->activate(true);
////.	m_dynamic_body->setGravity			( btVector3(0,-0.05f,0) );
//	body_added = true;
}

void game_world::test_physics2( )
{
}

void game_world::test_physics3( )
{}

game_world::~game_world( )
{
	DELETE				( m_free_fly_camera );
	DELETE				( m_hud );
	camera_director* cd = m_camera_director.c_ptr(); 
	m_camera_director	= NULL;
	DELETE				( cd );
	DELETE				( m_test_anim_object );
}

void game_world::time_update( )
{
	u64 elapsed_ticks					= m_timer.get_elapsed_ticks( );
	float new_game_time_sec				= ( float( double(elapsed_ticks) / timing::g_qpc_per_second ) );
	u32 new_game_time_ms				= ( u32( (elapsed_ticks*u64( 1000 ) / timing::g_qpc_per_second) & u32(-1) ) );

	m_last_frame_time_sec				= new_game_time_sec - m_game_time_sec;
	m_game_time_sec						= new_game_time_sec;
	m_last_frame_time_ms				= new_game_time_ms - m_game_time_ms;
	m_game_time_ms						= new_game_time_ms;
}

void game_world::tick( )
{
	time_update					( );

	if(m_test_anim_object)
		m_test_anim_object->tick();

//	g_physics_enabled = false;
	if(g_physics_enabled)
		m_physics_world->tick			( );

	scenes_list::iterator it = m_active_scenes.begin();
	scenes_list::iterator it_e = m_active_scenes.end();
	for( ;it!=it_e; ++it)
		(*it)->tick			( );

	m_camera_director->tick				( );
	m_cell_manager->set_inv_view_matrix	( m_camera_director->get_inverted_view_matrix( ) );
	m_cell_manager->tick				( );
	m_bullet_manager->tick				( m_game_time_sec );
}

u32 game_world::game_time_ms	( )
{
	return m_game_time_ms;
}
float game_world::game_time_sec	( )
{
	return m_game_time_sec;
}

u32 game_world::last_frame_time_ms ( )
{
	return m_last_frame_time_ms;
}

float game_world::last_frame_time_sec ( )
{
	return m_last_frame_time_sec;
}

void game_world::unload( )
{
	scenes_list::iterator it = m_active_scenes.begin();
	scenes_list::iterator it_e = m_active_scenes.end();
	for( ;it!=it_e; ++it)
	{
		(*it)->stop			( true );
	}

	m_active_scenes.clear				( );
	m_camera_director->switch_to_camera	( NULL, "null" );
	m_cell_manager->unload				( );
	ASSERT								( empty() );
	xray::collision::delete_space_partitioning_tree( m_collision_tree );
	m_collision_tree					= NULL;
	m_game_project						= NULL;
}

void game_world::switch_to_hud_camera( )
{
	m_camera_director->switch_to_camera	( m_hud, "hud game camera" );
}

void game_world::switch_to_free_fly_camera( )
{
	m_camera_director->switch_to_camera	( m_free_fly_camera, "free fly camera" );
}

bool game_world::empty( )
{
	return		m_cell_manager->empty();
}

void game_world::load( pcstr project_resource_name, pcstr project_resource_path )
{
	if ( !empty() )
		unload();

	m_collision_tree = &( *( collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 ) ) );

	if ( project_resource_path )
		m_project_resource_path				= project_resource_path;
	else
		m_project_resource_path				= project_resource_name;


	LOG_INFO( "game_world::load : %s", project_resource_name );
	resources::query_resource(
		project_resource_name,
		resources::game_project_class,
		boost::bind( &game_world::on_project_loaded, this, _1 ),
		g_allocator
	);
}

void game_world::on_project_loaded( resources::queries_result& data )
{
	R_ASSERT						( data.is_successful() );

	m_game_project					= static_cast_resource_ptr<game_project_ptr>(data[0].get_unmanaged_resource());
	m_cell_manager->load			( m_game_project );

	math::float3 camera_position	= (*m_game_project->m_config)["camera"]["position"];
	math::float3 camera_direction	= (*m_game_project->m_config)["camera"]["direction"];
	m_camera_director->set_position_direction( camera_position, camera_direction );
	switch_to_free_fly_camera		( );
}

void game_world::on_activate( )
{
	super::on_activate			( );
	m_camera_director->on_focus	( true );
}

void game_world::on_deactivate( )
{
	super::on_deactivate		( );
	m_camera_director->on_focus	( false );
}

game_project_ptr const game_world::get_project( ) const
{
	return m_cell_manager->get_project( );
}

game_object_ptr_ game_world::get_object_by_name( pcstr object_id ) const
{
	return m_cell_manager->get_object_by_name( object_id );
}

void game_world::query_object_by_name	( pcstr object_id, object_loaded_callback_type const& callback ) const
{
	return m_cell_manager->query_object_by_name( object_id, callback );
}

void game_world::start_game( )
{
	m_timer.start			( );
	time_update				( );

//.	m_test_anim_object	= NEW(test_anim_object)(*this);

	camera_director_ptr object_ptr			= get_camera_director();
	m_cell_manager->m_named_registry["camera_director"]		= object_ptr.c_ptr();
	
	configs::binary_config_value scenes_to_start = (*m_game_project->m_config)["start"]["scenes_to_start"];
	for ( u32 i = 0; i < scenes_to_start.size(); i++ )
	{
		pcstr start_scene		= (pcstr)scenes_to_start[i];
		game_object_ptr_ s		= get_object_by_name(start_scene);
		object_scene_ptr scene	= static_cast_resource_ptr<object_scene_ptr>(s);
		scene->start			( );
	}
	
// 	pcstr start_scene		= (*m_game_project->m_config)["start"]["scene_name"];
// 	game_object_ptr_ s		= get_object_by_name(start_scene);
// 	object_scene_ptr scene	= static_cast_resource_ptr<object_scene_ptr>(s);
// 	scene->start			( );
}

void game_world::on_scene_start( object_scene_ptr scene )
{
	m_active_scenes.push_back	( scene );
}

void game_world::on_scene_stop( object_scene_ptr scene )
{
	scenes_list::iterator it	= std::find(m_active_scenes.begin(), m_active_scenes.end(), scene );
	R_ASSERT					( it!=m_active_scenes.end() );
	m_active_scenes.erase		( it );
}

} // namespace stalker2
