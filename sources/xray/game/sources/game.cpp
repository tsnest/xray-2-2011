////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "game.h"
#include "game_exit_handler.h"
#include "npc_manipulation_handler.h"
#include "game_world.h"
#include "main_menu.h"
#include "stats.h"
#include "stats_graph.h"
#include "project_cooker.h"
#include "cell_cooker.h"
#include "object_cooker.h"
#include "sound_player_cook.h"
#include "human_npc_cook.h"
#include "human_npc.h"
#include "weapon_cook.h"
#include "building_object.h"
#include "composite_building.h"
#include "ai_sound_player.h"
#include <xray/input/world.h>
#include <xray/ui/world.h>
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>
#include <xray/sound/sound_debug_stats.h>
#include <xray/engine/console.h>
#include <xray/console_command.h>
#include <xray/console_command_processor.h>
#include <xray/physics/world.h>
#include <xray/animation/world.h>
#include <xray/rtp/world.h>
#include <xray/ai_navigation/world.h>
#include <xray/ai/world.h>
#include "collision_object_types.h"
#include <xray/collision/space_partitioning_tree.h>
#include <xray/game_test_suite.h>
#include "animated_model_instance_cook.h"
#include <xray/math_randoms_generator.h>
#include <xray/render/facade/scene_renderer.h>
#include "object_weapon.h"
#include "npc_stats.h"
#include <xray/input/keyboard.h>
#include <xray/input/mouse.h>
#include "free_fly_camera.h"
#include <xray/render/facade/debug_renderer.h>

using xray::console_commands::cc_delegate;
using xray::console_commands::cc_float3;

static bool s_draw_stats_value = true;
static console_commands::cc_bool s_draw_stats("draw_stats", s_draw_stats_value, true, xray::console_commands::command_type_user_specific);

static command_line::key		s_level_key			( "level", "", "", "level to load" );
static command_line::key		s_run_sound_tests	( "run_sound_test", "", "", "test for sound system" );

void cfg_save_user()
{
	xray::console_commands::save(NULL, xray::console_commands::command_type_engine_internal, memory::g_mt_allocator );
}
void cfg_save_system()
{
	xray::console_commands::save(NULL, xray::console_commands::command_type_user_specific, memory::g_mt_allocator );
}

static cc_delegate	cgf_save_system_cc( "cfg_save_system", boost::bind(&cfg_save_system), false );
static cc_delegate	cgf_save_user_cc( "cfg_save_user", boost::bind(&cfg_save_user), false );




static command_line::key		s_speedtree_key		( "speedtree", "", "", "speedtree test" );
//static bool is_speedtree_loaded	= false;




#ifndef MASTER_GOLD
static void	make_crash				( pcstr )
{
	FATAL					( "do not report, this console command is aimed for testing bug tracking" );
}
static console_commands::cc_delegate s_crash("crash", &make_crash, false);
#endif // #ifndef MASTER_GOLD

static u32 s_max_particles_value = 100;
static console_commands::cc_u32 s_max_particles("max_particles", s_max_particles_value, 0, 1000, true, xray::console_commands::command_type_engine_internal);

static u32 s_particle_lod_value = 0;
static console_commands::cc_u32 s_particle_lod("particle_lod", s_particle_lod_value, 0, 10, true, xray::console_commands::command_type_engine_internal);

namespace stalker2 {

// namespace temp
// {
// 	static xray::math::random32 randomizer(100);
// 	static u32 const random_max = 65535;
// 
// 	float random_float(float min_value, float max_value)
// 	{
// 		float alpha = float(randomizer.random(random_max)) / float(random_max);
// 		return min_value * (1.0f - alpha) + max_value * alpha;
// 	}
// 
// 	float3 random_float3(float3 const& min_value, float3 const& max_value)
// 	{
// 		return float3(random_float(min_value.x, max_value.x),
// 			random_float(min_value.y, max_value.y),
// 			random_float(min_value.z, max_value.z));
// 	}
// }

// static void speedtree_loaded(resources::queries_result& data, xray::render::game::renderer* r)
// {
// 	if (!data.is_successful())
// 		return;
// 	
// 	using namespace xray::math;
// 	
// 	float4x4 transform = create_translation( temp::random_float3(float3(-400, 0, -400), float3(400, 0, 400)));
// 	
// 	xray::render::speedtree_instance_ptr st_instance_ptr = static_cast_resource_ptr<xray::render::speedtree_instance_ptr>(data[0].get_unmanaged_resource());
// 	r->add_speedtree_instance(st_instance_ptr, transform);
// }
// 
// static void load_speedtree(xray::render::game::renderer* r)
// {
// 	if (is_speedtree_loaded || !s_speedtree_key.is_set())
// 		return;
// 	
// 	for (u32 i=0; i<5000; i++)
// 	{
// 		pcstr					tree_name = "acacia";
// 		if (i>5*333 && i<5*666)		tree_name = "white_birch"; // live_oak
// 		else if (i>=5*666)		tree_name = "banana_plant";
// 		
// 		resources::query_resource(
// 			tree_name,
// 			resources::speedtree_instance_class,
// 			boost::bind(&speedtree_loaded, _1, r),
// 			g_allocator
// 		);
// 	}
// 	
// 	is_speedtree_loaded = true;
// }






game::game					(
		xray::engine_user::engine& engine,
		xray::render::world& render_world,
		xray::sound::world& sound,
		xray::network::world& network
	) :
	m_engine				( engine ),
	m_render_world			( render_world ),
	m_renderer				( render_world.game_renderer() ),
	m_sound_world			( sound ),
	m_network_world			( network ),
	m_last_frame_time		( 0 ),
	m_active_scene			( 0 ),
	m_input_world			( 0 ),
	m_ui_world				( 0 ),
	m_physics_world			( 0 ),
	m_animation_world		( 0 ),
	m_rtp_world				( 0 ),
	m_ai_navigation_world	( 0 ),
	
	m_ai_world				( 0 ),
	m_spatial_tree			( 0 ),
	m_sound_test_allowed	( s_run_sound_tests.is_set() ),
	m_is_dictionary_created	( false ),
	m_is_npc_auto_creation_enabled( true ),
	m_selected_npc			( 0 ),
	m_npc_queries_count		( 0 ),
	m_active_npc_set		( false ),

	m_game_world			( NULL ),
	m_main_menu				( NULL ),
	m_last_frame_time_ms	( 0 ),
	m_enabled				( false ),
	m_initialized			( false ),
	m_is_active				( false ),
	m_sound_scene			( 0 )
{
	query_scene				( );
	query_sound_scene		( );
	
	static xray::console_commands::cc_delegate s_reload_shaders(
		"reload_shaders",
		boost::bind( &xray::render::scene_renderer::reload_shaders, &m_renderer.scene() ),
		false
	);
	m_timer.start			( );
	
	static xray::console_commands::cc_delegate s_reload_modified_textures(
		"reload_modified_textures",
		boost::bind( &xray::render::scene_renderer::reload_modified_textures, &m_renderer.scene() ),
		false
	);
}

game::~game( )
{	
	console_commands::save		( 0, xray::console_commands::command_type_engine_internal, *g_allocator );
	console_commands::save		( 0, xray::console_commands::command_type_user_specific, *g_allocator );

	if ( m_active_scene )
		m_active_scene->on_deactivate ( );

	DELETE						( m_main_menu );
	DELETE						( m_console );
	DELETE						( m_stats );
	DELETE						( m_active_npc_stats );
	DELETE						( m_fps_graph );
#ifndef MASTER_GOLD
	DELETE						( m_sound_stats	);
#endif //#ifndef MASTER_GOLD
	DELETE						( m_game_world );
	deinitialize_modules		( );
}

void game::on_scene_created		( xray::resources::queries_result& data )
{
	m_scene						= static_cast_resource_ptr< xray::render::scene_ptr >( data[0].get_unmanaged_resource() );
	m_scene_view				= static_cast_resource_ptr< xray::render::scene_view_ptr >( data[1].get_unmanaged_resource() );
	m_render_output_window		= static_cast_resource_ptr< xray::render::render_output_window_ptr >( data[2].get_unmanaged_resource() );

	m_timer.start				( );
	
	query_npc_dictionary		( );

	initialize_modules			( );
	register_cooks				( );

	m_initialized				= true;
	
	m_game_world				= NEW( game_world )( *this );
 	m_main_menu					= NEW( main_menu )( *this );
	m_console					= m_engine.create_game_console( ui_world(), input_world() );
	m_stats						= NEW( stats )( *m_ui_world );
	m_fps_graph					= NEW( stats_graph )( 1.f, math::infinity, 30.f, 60.f );
	m_active_npc_stats			= NEW( npc_stats )( *m_ui_world );


#ifndef MASTER_GOLD
	m_sound_stats				= NEW( xray::sound::sound_debug_stats )( g_allocator, m_sound_world.get_logic_world_user(), m_sound_scene, *m_ui_world );
#endif //#ifndef MASTER_GOLD

	register_console_commands	( );

	m_viewport.left				= 0.f;
	m_viewport.top				= 0.f;
	m_viewport.right			= 1.f;
	m_viewport.bottom			= 1.f;

	fixed_string512				project_path;
	bool const load_level		= s_level_key.is_set_as_string( &project_path );
	
	if ( load_level && !m_engine.command_line_editor() )
		load					( project_path.c_str() );
	else
		m_game_world->switch_to_free_fly_camera( );

	R_ASSERT					( m_rtp_world );
	rtp().set_scene				( m_scene );
	
	R_ASSERT					( m_animation_world );
	animation_world().set_test_scene( m_scene );

	enable						( m_enabled );

	if ( m_is_active )
	{
		m_is_active				= false;
		on_application_activate	( );
	}
}

void game::query_scene			( )
{
	xray::render::editor_renderer_configuration		render_configuration;
	render_configuration.m_create_terrain			= true;
	render_configuration.m_create_particle_world	= true;
	
	resources::user_data_variant temp_data;
	temp_data.set				( render_configuration );
	
	resources::user_data_variant output_window_data;
	HWND render_window_handle	= m_engine.get_render_window_handle();
	output_window_data.set		( *(HWND*)&render_window_handle );
	xray::resources::user_data_variant const* data[] = { &temp_data, 0, &output_window_data };
	
	xray::const_buffer			temp_buffer( "", 1 );
	xray::resources::creation_request requests[] = 
	{
		xray::resources::creation_request( "game_scene", temp_buffer, resources::scene_class ),
		xray::resources::creation_request( "game_scene_view", temp_buffer, resources::scene_view_class ),
		xray::resources::creation_request( "game_render_output_window", temp_buffer, resources::render_output_window_class )
	};
 	xray::resources::query_create_resources(
 		requests,
		boost::bind( &game::on_scene_created, this, _1 ),
 		stalker2::g_allocator,
 		data
 	);
}

void game::query_sound_scene		( )
{
	xray::const_buffer temp_buffer	( "", 1 );

	query_create_resource	
	( 
		"game_sound_scene",
		temp_buffer, 
		resources::sound_scene_class,
		boost::bind( &game::on_sound_scene_created, this, _1 ),
		stalker2::g_allocator 
	);
}

void game::on_sound_scene_created	( resources::queries_result& data )
{
	R_ASSERT			( data.is_successful( ) );
	m_sound_scene		= static_cast_resource_ptr< xray::sound::sound_scene_ptr >( data[0].get_unmanaged_resource() );

	m_sound_world.get_logic_world_user().set_active_sound_scene( m_sound_scene, 1000, 0 );

}


void game::enable					( bool value )
{
	m_enabled						= value;

	if ( !m_scene )
		return;

	if ( value )
	{
		m_input_world->acquire		( );

		// select active scene
		game_scene* scene_to_activate = m_game_world;
#if 0		
		if( m_game_world->empty() )
			scene_to_activate		= m_main_menu;
#endif
		switch_to_scene				( scene_to_activate );
	}
	else
	{
		m_input_world->unacquire	( );
	}
}

 void on_config_loaded				( resources::queries_result& data )
 {
 	if( !data.is_successful() )
 	{
 		LOG_ERROR					( "config file loading FAILED" );
 		return;
 	}
 
 	resources::pinned_ptr_const<u8> pinned_data( data[0].get_managed_resource() );
 	memory::reader F				( pinned_data.c_ptr(), pinned_data.size() );
	console_commands::load			( F, console_commands::execution_filter_general );
 }
 
 void load_config_query				( pcstr cfg_name )
 {
 	resources::query_resource		(
 		cfg_name,
 		xray::resources::raw_data_class,
 		boost::bind( &on_config_loaded, _1 ),
 		g_allocator, 
		0, 
		0, 
		assert_on_fail_false		
 	);
 }

void game::register_console_commands( )
{
	static exit_handler				game_exit_handler( *this );
	static npc_manipulation_handler npc_management_handler( *this );
	
	m_input_world->add_handler		( game_exit_handler );
	m_input_world->add_handler		( npc_management_handler );

	static cc_delegate				game_exit_cc( "quit", boost::bind( &game::exit, this, _1 ), false );
 	static cc_delegate				cfg_load_cc( "cfg_load", boost::bind( &load_config_query, _1 ), true );
 	cfg_load_cc.execute				( "resources/startup.cfg" );
 	cfg_load_cc.execute				( "user_data/user.cfg" );

 	static cc_delegate				cfg_load_level( "level_load",   boost::bind( &game::load_cmd, this, _1 ), true );
 	static cc_delegate				cfg_unload_level( "level_unload", boost::bind( &game::unload_cmd, this, _1 ), false );
}

void game::switch_to_scene			( game_scene* scene )
{
	R_ASSERT						( scene );
//	ASSERT			( m_active_scene != scene );

	if( m_active_scene )
		m_active_scene->on_deactivate( );
	
	m_active_scene	= scene;
	m_active_scene->on_activate		( );
}

u32 game::time_ms					( )
{
	return							m_timer.get_elapsed_msec();
}

//void game::apply_camera( game_camera const* c )
//{
//	m_inverted_view					= c->get_inverted_view_matrix();
//	m_projection					= c->get_projection_matrix();
//	m_sound_world.get_logic_world_user().set_listener_properties( m_inverted_view );
//}

void game::toggle_console			( )
{
	if ( m_console->get_active() )
		m_console->on_deactivate	( );
	else
		m_console->on_activate		( );
}

void game::exit						( pcstr str )
{
	unload							( str, true );
	
	if ( m_engine.command_line_editor() )
		m_engine.enter_editor_mode	( );
	else
		m_engine.exit				( 0 );
}

void game::tick						( u32 const current_frame_id )
{
	static bool game_test_run		= false;
	if ( !game_test_run )
	{
		game_test_suite::run_tests	( );
		game_test_run				= true;
	}
	
	if ( !m_scene )
	{
		m_renderer.end_frame		( );
		return;
	}

	if ( m_active_scene )
	{
 		m_input_world->tick			( );
		m_active_scene->tick		( );	
	}

	test							( );

	m_ui_world->tick				( );
	m_physics_world->tick			( );

	m_rtp_world->tick				( );
	m_animation_world->tick			( );

	m_ai_navigation_world->tick		( );

	m_sound_world.get_logic_world_user().set_listener_properties_interlocked(
		m_sound_scene,
		m_inverted_view_matrix.c.xyz(),
		m_inverted_view_matrix.k.xyz(),
		m_inverted_view_matrix.j.xyz());

	if ( m_is_dictionary_created )
		run_ai_tests				( current_frame_id );

	m_ai_world->tick				( );

#ifdef DEBUG
	if ( m_game_world->get_camera_director()->get_active_camera() )
	{
		float3 const& position				= m_game_world->get_camera_director()->get_active_camera()->get_inverted_view_matrix().c.xyz();
		float3 const& direction				= m_game_world->get_camera_director()->get_active_camera()->get_inverted_view_matrix().k.xyz();
		float4x4 const& projection_matrix	= m_game_world->get_camera_director()->get_active_camera()->get_projection_matrix();
		float4x4 const& view_matrix			= math::invert4x3( m_game_world->get_camera_director()->get_active_camera()->get_inverted_view_matrix() );
		m_ai_navigation_world->debug_render	( *m_input_world, position, direction,	math::frustum( mul4x4( view_matrix, projection_matrix ) ) );
	}
#endif // #ifdef DEBUG

	if( m_console->get_active() )
		m_console->tick				( );

	// because of out of memory at the start
	update_stats					( current_frame_id );

	m_renderer.draw_scene			( m_scene, m_scene_view, m_render_output_window, m_viewport );
	m_renderer.end_frame			( );
}

void game::apply_camera				( camera_director_ptr cd )
{
	m_inverted_view_matrix			= cd->get_inverted_view_matrix();
	m_projection_matrix				= cd->get_projection_matrix();

	m_renderer.scene().set_view_matrix		( m_scene_view, math::invert4x3( m_inverted_view_matrix ) );
	m_renderer.scene().set_projection_matrix( m_scene_view, m_projection_matrix );
}

void game::update_stats				( u32 const current_frame_id )
{
	float const last_frame_time		= float( m_timer.get_elapsed_msec() - m_last_frame_time_ms ) / 1000.f;
	m_fps_graph->add_value			( last_frame_time, math::is_zero( last_frame_time - m_last_frame_time ) ? math::infinity : 1.f / ( last_frame_time - m_last_frame_time ) );
	m_last_frame_time				= last_frame_time;

	update_npc_stats				( );

	if ( s_draw_stats_value && !m_active_npc_set )
	{
		m_stats->set_fps_stats		( m_fps_graph->average_value() );
		m_stats->set_camera_stats	( m_inverted_view_matrix.c.xyz(), m_inverted_view_matrix.k.xyz() );
		string64					buff;
		xray::sprintf				( buff, "Q: %d", xray::resources::pending_queries_count() );
		m_stats->set_resources_stats( buff );

		m_stats->draw				( ui_world().get_renderer() );
	}


	//if( true ) {
	//	string4096 s = "";
	//	m_rtp_world->get_controllers_dump( s );
	//	m_stats->set_rtp_controllers_dump( s );
	//}
		
	static bool draw_stats_graph	= false;
	static console_commands::cc_bool	fps_graph( "draw_fps_graph", draw_stats_graph, false, console_commands::command_type_user_specific );

	if ( draw_stats_graph && !m_active_npc_set )
	{
		m_fps_graph->set_time_interval	( 5.f );
		m_fps_graph->render				( ui_world().get_renderer(), ui_world().get_scene_view(), current_frame_id, 10, 768 - 128 - 2, 1024 - 2*10, 128 );
	}
	else
	{
		m_fps_graph->stop_rendering		( );
		m_fps_graph->set_time_interval	( 1.f );
	}

#ifndef MASTER_GOLD
	if ( m_sound_stats && m_sound_stats->is_stats_available( ) )
		m_sound_stats->draw				( m_scene );
#endif //#ifndef MASTER_GOLD

}

static void delete_weapons				( human_npc_ptr& owner )
{
	while ( object_weapon* weapon = owner->pop_weapon() )
		DELETE							( weapon );
}

static void kill_npc					( human_npc_ptr& condemned )
{
	delete_weapons						( condemned );
	condemned->clear_resources			( );
}

void game::clear_resources				( )
{
	for ( human_npc_ptr it_npc = m_npcs.front(); it_npc; it_npc = m_npcs.get_next_of_object( it_npc ) )
		kill_npc						( it_npc );

	m_npcs.clear						( );

	R_ASSERT							( m_spatial_tree );
	xray::collision::delete_space_partitioning_tree( m_spatial_tree );
	m_spatial_tree						= 0;

#ifndef MASTER_GOLD
	m_sound_stats->clear_resources		( m_sound_world.get_logic_world_user() );
#endif //#ifndef MASTER_GOLD

	m_sound_world.get_logic_world_user().remove_sound_scene	( m_sound_scene );
	//m_render_world.destroy( *g_allocator, m_renderer);

	m_physics_world->clear_resources	( );
	m_input_world->clear_resources		( );
	m_ui_world->clear_resources			( );
	m_ai_navigation_world->clear_resources( );
	m_ai_world->clear_resources			( );
	m_animation_world->clear_resources	( );
	m_rtp_world->clear_resources		( );

	if( !m_game_world->empty() )
		m_game_world->unload			( );
}

void game::load_cmd						( pcstr project_name )
{
	load								( project_name );
}

void game::unload_cmd					( pcstr s )
{
	unload								( s, false );
}

void game::load							( pcstr project_resource_name, pcstr project_resource_path )
{
	m_game_world->load		( project_resource_name, project_resource_path );
	
	m_ai_navigation_world->load_navmesh( project_resource_path ? project_resource_path : project_resource_name );

	switch_to_scene			( m_game_world );

	m_sound_world.get_logic_world_user().set_active_sound_scene( m_sound_scene, 0, 0 );
}

void game::unload						( pcstr , bool destroying )
{
	ASSERT								( m_game_world );

	m_game_world->unload				( );
	
	for ( human_npc_ptr it_npc = m_npcs.front(); it_npc; it_npc = m_npcs.get_next_of_object( it_npc ) )
	{
		kill_npc( it_npc );
	}
	m_selected_npc		= NULL;
	m_active_npc_set	= false;
	m_npc_queries_count	= 0;
	m_npcs.clear();

	if(!destroying)
		switch_to_scene					( m_main_menu );
}

void game::scene_close_query			( )
{
	if ( m_active_scene == m_main_menu )
		switch_to_scene					( m_game_world );
	else
		switch_to_scene					( m_main_menu );
}


void game::register_cooks				( )
{
	static project_cooker				s_project_cook( engine().command_line_editor() );
	static cell_cooker					s_cell_cook;
	static object_cooker				s_object_cook( *this );
	static object_scene_cooker			s_object_scene_cook( *this );

	static sound_player_cook			s_logic_sound_player_cook		( m_sound_scene, &m_sound_world, m_input_world, resources::sound_player_logic_class );
	static sound_player_cook			s_editor_sound_player_cook		( m_sound_scene, &m_sound_world, m_input_world, resources::sound_player_editor_class );
	static human_npc_cook				s_human_npc_cook				( *this );
	static animated_model_instance_cook	s_animated_model_instance_cook;
	static weapon_cook					s_weapon_cook					( *this );

	register_cook						( &s_project_cook );
	register_cook						( &s_cell_cook );
	register_cook						( &s_object_cook );
	register_cook						( &s_object_scene_cook );
	register_cook						( &s_logic_sound_player_cook );
	register_cook						( &s_editor_sound_player_cook );
	register_cook						( &s_human_npc_cook );
	register_cook						( &s_animated_model_instance_cook );
	register_cook						( &s_weapon_cook );
}

#if 0
static void on_lua_config_loaded	( resources::queries_result& data )
{
	XRAY_UNREFERENCED_PARAMETER		( data );
	R_ASSERT						( !data.is_failed() );

	{
		configs::binary_config_ptr	config	= configs::create_lua_config("test.lua");
		configs::binary_config_value root	= (*config)["skeleton"];
		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"]["head"] = 1;
		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"]["head"].create_table();
		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"]["head2"] = 1;
		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"].add_super_table( root["hips"] );
		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"].remove_super_table( root["hips"] );
		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"].add_super_table( "scripts/test_config", "new_table.mega_table" );
		configs::binary_config_value value = root["hips"]["spine"]["spine_1"]["spine_2"]["neck"]["mega_value"];
		pcstr const mega_value			= value;
		debug::printf( "%s%c", mega_value, '\n' );
//		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"].remove_super_table( "test", "hips.spine"  );
//		root["hips"]["spine"]["spine_1"]["spine_2"]["neck"]["head"] = "ooops";
		root["hips"]["spine"]["spine_1"]["spine_2"]["left_collar"]["left_arm"]["left_fore_arm"]["left_hand_roll"].create_table();
		config->save					( );
	}
	{
		configs::binary_config_ptr config_ptr = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().get());
		configs::lua_config& config		= *config_ptr;
		u32 const value					= config["atest_table"]["value"];
		u32 const value2				= config["atest_table"]["value2"];
		config.save_as					( "d:/test.lua" );
	}

	{
		configs::binary_config_ptr config	= configs::create_lua_config();
		configs::binary_config_value params = *config;
		params["test"][0]["a"] 			= "a";
		params["test"][0]["b"] 			= "b";
		params["test"][1]["a"] 			= "a";
		params["test"][1]["b"] 			= "b";
		config->save_as					( "d:/test.lua" );
		configs::create_lua_config( config->get_binary_config() )->save_as("d:/test2.lua");
	}

	configs::binary_config_ptr config_ptr = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().get());
	configs::lua_config& config		= *config_ptr;
	u32 u32_value					= config["test_table"]["u32_value"];
	s32 s32_value					= config["test_table"]["s32_value"];
	float float_value				= config["test_table"]["float_value"];
	pcstr string_value				= config["test_table"]["string_value"];
	math::float2 const float2_value	= config["test_table"]["float2_value"];
	math::float3 float3_value		= config["test_table"]["float3_value"];
	bool bool_value					= config["test_table"]["one_more_table"]["bool_value"];
	bool const value_exist0			= config["test_table"]["one_more_table"].value_exists("bool_value");
	bool const value_exist1			= config["test_table"]["one_more_table"].value_exists("bool_value_");
	bool const value_exist2			= config.value_exists("test_table.one_more_table.bool_value");

	config["test_table"]["u32_value"]		= 128;
	config["test_table"]["s32_value"]		= -128;
	config["test_table"]["float_value"]		= 128.1f;
	config["test_table"]["string_value"]	= "string_value";
	config["test_table"]["float2_value"]	= math::float2( 0.f, 1.f );
	config["test_table"]["float3_value"]	= math::float3( 0.f, 1.f, 2.f ).normalize();
	config["test_table"]["new_table"]["float3_value"]	= math::float3( 0.f, 128.f, 2.f ).normalize();
	config["new_table"]["new_table"]["float3_value"]	= math::float3( 2.f, 128.f, 2.f ).normalize();
	config["test_table"]["float3_value"]["new_value"]	= math::float3( 0.f, 1.f, 2.f ).normalize();

	config["test_table"]["new_table2"]["float3_value"].value_exists("float4_value");
	config["new_table2"]["new_table"]["float3_value"].value_exists("float4_value");

	config["expreimaental"]["test_super_value"]		= "super_value";

	config["test_table"]["__a"].remove_super_table	( "scripts/test_config", "test_table" );
	config["test_table"]["__a"].remove_super_table	( "scripts/test_config", "new_table" );
	config["test_table"]["__a"].add_super_table		( "scripts/test_config", "expreimaental" );
	pcstr const super_value			= config["test_table"]["__a"]["test_super_value"];
	config["test_table"]["__a"]["test_super_value"]	= "super_value";
//	config["test_table"]["__a"].add_super_table	( "scripts/test_config", "test_table" );
//	float float_value2				= config["test_table"]["__a"]["float_value"];
//	config["test_table"]["__a"].remove_super_table	( "scripts/test_config", "test_table" );
//	float_value2					= config["test_table"]["__a"]["float_value"];
//	config["test_table"]["__a"].add_super_table	( config[new_table] );
//	float3_value					= config["test_table"]["__a"]["new_table"]["float3_value"];
//	config["test_table"]["new_table"].remove_super_table( config["new_table"] );
//	float3_value					= config["test_table"]["new_table"]["new_table"]["float3_value"];

	configs::binary_config_value value	= config.get_root( );
	value							= value["test_table"];
	value							= value["float2_value"];
	value							= value["new_value"];
	value							= math::float2( 3, 1 );

	config.save_as					( "c:\\test_config.lua" );
//	config_ptr						= configs::create_lua_config( );
//	configs::lua_config& config		= *config_ptr;
//	config["test_table"]["u32_value"]		= 1;
//	config["test_table"]["string_value"]	= "aga!";
//	u32 const u32_value				= (*bc)["test_table"]["u32_value"];
//	pcstr const string_value		= (*bc)["test_table"]["string_value"];

	{
		configs::binary_config_ptr bc	= config.get_binary_config( );
		u32_value						= (*bc)["test_table"]["u32_value"];
		s32_value						= (*bc)["test_table"]["s32_value"];
		float_value						= (*bc)["test_table"]["float_value"];
		string_value					= (*bc)["test_table"]["string_value"];
		bool_value						= (*bc)["test_table"]["one_more_table"]["bool_value"];
		math::float3 const float3_value	= (*bc)["test_table"]["float3_value"]["new_value"];
	}
}
#endif // #if 0

void game::on_application_activate		( )
{
	threading::mutex_raii guard			( m_application_activation );

	R_ASSERT							( !m_is_active );

	if ( m_input_world )
		m_input_world->on_activate		( );

	m_is_active							= true;
}

void game::on_application_deactivate	( )
{
	if ( !m_input_world )
		return;

	threading::mutex_raii guard			( m_application_activation );

//?	R_ASSERT	( m_is_active );
	m_input_world->on_deactivate		( );
	m_is_active							= false;
}

void game::get_colliding_objects	( math::aabb const& query_aabb, xray::ai::ai_objects_type& results )
{
	xray::collision::objects_type	objects( g_allocator );
	m_spatial_tree->aabb_query		( collision_object_type_ai, query_aabb, objects );
	for ( xray::collision::objects_type::const_iterator iter = objects.begin(); iter != objects.end(); ++iter )
	{
		ai_collision_object const* const ai_object = static_cast_checked< ai_collision_object const* >( *iter );
		results.push_back			( &ai_object->get_game_object() );
	}
}

void game::get_visible_objects		( math::cuboid const& cuboid, xray::ai::update_frustum_callback_type const& update_callback )
{
	xray::collision::results_callback_type result_callback( boost::bind( &game::get_frustum_objects_callback, this, &update_callback, _1 ) );
	m_spatial_tree->cuboid_query	( collision_object_type_ai, cuboid, result_callback );
}

void game::get_frustum_objects_callback	(
		xray::ai::update_frustum_callback_type const* update_callback,
		xray::collision::object const& frustum_object
	)
{
	ai_collision_object const& ai_object = static_cast_checked< ai_collision_object const& >( frustum_object );
	( *update_callback )			( ai_object.get_game_object() );
}

struct ray_query_predicate : private boost::noncopyable
{
	inline ray_query_predicate		(
			float& value,
			collision::object const* const object_of_interest,
			collision::object const* const object_to_ignore,
			float const threshold
		) :
		visibility_value			( value ),
		requested_object			( object_of_interest ),
		object_to_ignore			( object_to_ignore ),
		transparency_threshold		( threshold ),
		requested_object_was_found	( false )
	{
	}

	inline bool predicate			( xray::collision::ray_triangle_result const& triangle )
	{
		if ( !triangle.object->user_data() )
			return					false;

		if ( triangle.object == object_to_ignore )
			return					false;

		game_material_visibility_parameters const* const parameters = static_cast_checked< game_material_visibility_parameters const* >( triangle.object->user_data() );
		float const transparency	= parameters->get_transparency_value();
		requested_object_was_found	= triangle.object == requested_object;
		visibility_value			-= ( 1.f - ( requested_object_was_found ? 1.f : transparency ) );
		if ( requested_object_was_found || visibility_value <= transparency_threshold || transparency == 0 )
			return					true;
		
		return						false;
	}

	float&							visibility_value;
	collision::object const* const	requested_object;
	collision::object const* const	object_to_ignore;
	float const						transparency_threshold;
	bool							requested_object_was_found;
}; // class ray_query_predicate

bool game::ray_query				(
		ai::collision_object const* const object_to_pick,
		ai::collision_object const* const object_to_ignore,
		float3 const& origin,
		float3 const& direction,
		float const max_distance,
		float const transparency_threshold,
		float& visibility_value
	) const
{
	collision::ray_triangles_type game_objects = collision::ray_triangles_type( g_allocator );
	ray_query_predicate query_predicate = ray_query_predicate(
		visibility_value,
		static_cast_checked< ai_collision_object const* >( object_to_pick ),
		static_cast_checked< ai_collision_object const* >( object_to_ignore ),
		transparency_threshold
	);
	m_spatial_tree->ray_query( collision_object_type_ai, origin, direction, max_distance, game_objects, collision::triangles_predicate_type( &query_predicate, &ray_query_predicate::predicate ) );
	return query_predicate.requested_object_was_found;
}

void game::draw_ray	( float3 const& start_point, float3 const& end_point, bool sees_something ) const
{
	m_renderer.debug().draw_arrow( m_scene, start_point, end_point, sees_something ? math::color( 255, 0, 0 ) : math::color( 0, 255, 255 ) );
}

void game::draw_frustum					(
		float fov_in_radians,
		float far_plane_distance,
		float aspect_ratio,
		float3 const& position,
		float3 const& direction,
		math::color color
	) const
{
	m_renderer.debug().draw_frustum		(
		m_scene,
		fov_in_radians,
		0.f,
		far_plane_distance,
		aspect_ratio,
		position,
		direction,
		float3( 0.f, 1.f, 0.f ),
		color
	);
}

u32 game::get_node_by_name	( pcstr node_name ) const
{
// 	game_objects_type::const_iterator it		= m_game_objects.begin();
// 	game_objects_type::const_iterator it_end	= m_game_objects.end();
// 
// 	for ( ; it != it_end; ++it )
// 		if ( strings::equal( (*it)->get_name(), node_name ) )
// 			return (*it)->get_id();

	LOG_ERROR( "node with name %s wasn't found", node_name );
	return u32(-1);
}

void game::get_available_weapons( ai::npc* owner, ai::weapons_list& list_to_be_filled ) const
{
 	human_npc* const npc_owner	= static_cast_checked< human_npc* >( owner );
	npc_owner->get_available_weapons( list_to_be_filled );
}

void game::run_ai_tests		( u32 const current_frame_id )
{
	for ( human_npc_ptr it_npc = m_npcs.front(); it_npc; it_npc = m_npcs.get_next_of_object( it_npc ) )
		it_npc->tick		( current_frame_id );
}

void game::check_selected_npc			( )
{
	m_active_npc_set					= !m_active_npc_set;
	
	if ( m_active_npc_set )
		m_selected_npc					= find_npc_in_camera_direction();
	else
		m_selected_npc					= 0;

	if ( m_selected_npc == 0 )
		m_active_npc_set				= false;
}

void game::setup_movement_target		( )
{
	collision::ray_objects_type			objects( g_allocator );
	m_game_world->get_collision_tree()->ray_query(
		collision_object_type_terrain,
		get_camera_position(),
		m_inverted_view_matrix.k.xyz(),
		1000.f,
		objects,
		collision::objects_predicate_type()
	);

	if ( !objects.empty() )
		m_movement_target				= get_camera_position() + m_inverted_view_matrix.k.xyz() * objects.front().distance;
}

void game::toggle_npc_creation_mode		( )
{
	m_is_npc_auto_creation_enabled		= !m_is_npc_auto_creation_enabled;
}

struct get_first_npc_in_camera_direction_predicate : private boost::noncopyable
{
	inline get_first_npc_in_camera_direction_predicate	( ) :
		first_npc										( 0 )
	{
	}

	inline bool predicate					( xray::collision::ray_triangle_result const& triangle )
	{
		collision::object const* object_at_direction	= triangle.object;
		ai_collision_object const* ai_collision			= static_cast_checked< ai_collision_object const* >( object_at_direction );
		R_ASSERT							( ai_collision );
		first_npc							= ai_collision->get_game_object().cast_npc();
		return								first_npc != 0;
	}

	ai::npc* 								first_npc;
}; // struct get_first_npc_in_camera_direction_predicate

human_npc* game::find_npc_in_camera_direction	( ) const
{
	collision::ray_triangles_type game_objects	( g_allocator );
	get_first_npc_in_camera_direction_predicate query_predicate;
	m_spatial_tree->ray_query				(
		collision_object_type_ai,
		get_camera_position(),
		m_inverted_view_matrix.k.xyz(),
		50000.f,
		game_objects,
		collision::triangles_predicate_type( &query_predicate, &get_first_npc_in_camera_direction_predicate::predicate )
	);
	return query_predicate.first_npc ? static_cast_checked< human_npc* >( query_predicate.first_npc ) : 0;
}

void game::update_npc_stats				( )
{
	if ( m_active_npc_set && m_selected_npc )
	{
		m_active_npc_stats->set_stats	( m_selected_npc.c_ptr() );
		m_active_npc_stats->draw		( ui_world().get_renderer(), m_scene_view );
	}
}

void game::set_navmesh_info				( pcstr text ) const
{
	m_stats->set_navmesh_info			( text );
}

void game::query_npc_dictionary			( )
{
	query_resource	(
		"resources/brain_units/filters/available_filter_names.dictionary", 
		resources::binary_config_class, 
		boost::bind( &game::on_npc_dictionary_created, this, _1 ), 
		g_allocator
	);
}

void game::on_npc_dictionary_created	( resources::queries_result& data )
{
	if ( !data.is_successful() )
	{
		R_ASSERT						( data.is_successful(), "couldn't retrieve npc dictionary options" );
		return;
	}
		
	configs::binary_config_ptr config	= static_cast_resource_ptr< configs::binary_config_ptr >( data[0].get_unmanaged_resource() );
	configs::binary_config_value const& dictionary = config->get_root();
	m_ai_world->fill_objects_dictionary	( dictionary );
	
	m_is_dictionary_created				= true;
}

void game::query_npc					( human_npc_behaviour_type_enum const behaviour_type, float3 const& initial_position )
{
	if ( !m_is_dictionary_created )
	{
		LOG_WARNING						( "dictionary wasn't created yet, so npc couldn't be created as well" );
		return;
	}
	
	if ( m_npc_queries_count == m_ai_world->get_characters_count() )
	{
		LOG_WARNING						( "limit of test characters is reached" );
		return;
	}

	pcstr config_name					= behaviour_type == human_npc_aggressive ? "aggressive" : "neutral";
	
	ai::brain_unit_cook_params			brain_unit_params;
	brain_unit_params.world_user_type	= resources::sound_player_logic_class;
		
	npc_cook_params						human_npc_params;
	human_npc_params.brain_unit_params	= brain_unit_params;
	human_npc_params.scene				= m_scene;
	human_npc_params.sound_scene		= m_sound_scene;
	human_npc_params.physics_world		= m_physics_world;
	resources::user_data_variant		npc_data;
	npc_data.set						( human_npc_params );

	query_resource						(
		config_name, 
		resources::human_npc_class, 
		boost::bind( &game::on_npc_created, this, _1, initial_position ), 
		g_allocator,
		&npc_data,
		0
	);

	++m_npc_queries_count;
}

bool game::is_npc_id_available			( u32 const npc_id ) const
{
	for ( human_npc_ptr it_npc = m_npcs.front(); it_npc; it_npc = m_npcs.get_next_of_object( it_npc ) )
		if ( it_npc->get_id() == npc_id )
			return						false;

	return								true;
}

static void generate_weapons			(
		math::random32& randomizer,
		human_npc::npc_game_attributes& attributes,
		ai::world const& world,
		ai::weapon_types_enum weapon_type
	)
{
	u32 const random_id					= randomizer.random( world.get_weapons_count( weapon_type ) );
	attributes.weapons.push_back		( NEW( object_weapon )( weapon_type, world.get_weapon_name_by_id( weapon_type, random_id ), random_id ) );
}

void game::fill_npc_attributes_randomly	( human_npc_ptr owner, float3 const& initial_position )
{
	human_npc::npc_game_attributes		attributes;
	math::random32 randomizer			( m_ai_world->get_current_time_in_ms() );
	attributes.group_id					= randomizer.random( m_ai_world->get_groups_count() );
	attributes.outfit_id				= randomizer.random( m_ai_world->get_outfits_count() );
	attributes.debug_draw_color			= math::color( randomizer.random( 255 ), randomizer.random( 255 ), randomizer.random( 255 ) );
	attributes.initial_velocity			= randomizer.random_f( 16 );
	attributes.initial_position			= initial_position;
	u32 const weapons_count				= randomizer.random( 6 );
	for ( u32 i = 0; i < weapons_count; ++i )
	{
		ai::weapon_types_enum const weapon_type	= (ai::weapon_types_enum)randomizer.random( ai::weapon_types_count );
		generate_weapons				( randomizer, attributes, *m_ai_world, weapon_type );
	}

	u32 const characters_count			= m_ai_world->get_characters_count();
	u32 character_index					= randomizer.random( characters_count );
	ai::character_type name_id			= m_ai_world->get_character_attributes_by_index( character_index );
	while ( !is_npc_id_available( name_id.second ) )
	{
		++character_index				%= characters_count;
		name_id							= m_ai_world->get_character_attributes_by_index( character_index );
	}

	attributes.name						= name_id.first;
	attributes.id						= name_id.second;

	finish_npc_creation					( owner, attributes );
}

void game::fill_npc_attributes_manually	( human_npc_ptr owner )
{
	query_resource						(
		"resources/npc/human/game_attributes/for_manual_creation.attributes", 
		resources::binary_config_class, 
		boost::bind( &game::on_queried_npc_attributes_received, this, _1, owner ), 
		g_allocator
	);
}

void game::on_npc_attributes_received	( configs::binary_config_value const& attributes_config, human_npc_ptr owner )
{

	human_npc::npc_game_attributes		attributes;
	attributes.group_id					= attributes_config["group_id"];
	attributes.class_id					= attributes_config["class_id"];
	attributes.outfit_id				= attributes_config["outfit_id"];
	float3 color						= (float3)attributes_config["debug_draw_color"];
	attributes.debug_draw_color			= math::color( (u32)color.x, (u32)color.y, (u32)color.z );
	attributes.initial_velocity			= attributes_config["initial_velocity"];
	attributes.initial_luminosity		= attributes_config["initial_luminosity"];
	attributes.description				= attributes_config["description"];
	attributes.initial_position			= (float3)attributes_config["initial_position"];
	attributes.initial_rotation			= (float3)attributes_config["initial_rotation"];
	attributes.initial_scale			= (float3)attributes_config["initial_scale"];
	attributes.name						= attributes_config["name"];
	attributes.id						= attributes_config["id"];
	configs::binary_config_value const& weapons = attributes_config["weapons"];

	configs::binary_config_value::const_iterator it		= weapons.begin();
	configs::binary_config_value::const_iterator it_end = weapons.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& gun		= *it;
		u32 const type								= gun["type"];
		ai::weapon_types_enum const weapon_type		= (ai::weapon_types_enum)type;
		u32 const weapon_id							= gun["id"];
		attributes.weapons.push_back				( NEW( object_weapon )( weapon_type, m_ai_world->get_weapon_name_by_id( weapon_type, weapon_id ), weapon_id ) );
	}

	finish_npc_creation					( owner, attributes );
}

void game::on_queried_npc_attributes_received	( resources::queries_result& data, human_npc_ptr owner )
{
	if ( !data.is_successful() )
	{
		R_ASSERT						( data.is_successful(), "couldn't retrieve npc attributes for manual creation" );
		return;
	}
		
	configs::binary_config_ptr config	= static_cast_resource_ptr< configs::binary_config_ptr >( data[0].get_unmanaged_resource() );
	configs::binary_config_value const& config_root = config->get_root();

	on_npc_attributes_received			( config_root, owner );
}

void game::on_npc_created				( resources::queries_result& data, float3 const camera_position )
{
	R_ASSERT							( data.is_successful() );

	human_npc_ptr new_npc				= static_cast_resource_ptr< human_npc_ptr >( data[0].get_unmanaged_resource() );

	if ( m_is_npc_auto_creation_enabled )
		fill_npc_attributes_randomly	( new_npc, camera_position );
	else
		fill_npc_attributes_manually	( new_npc );

	if ( m_sound_test_allowed )
		new_npc->set_sound_dbg_mode		( true );
}

void game::finish_npc_creation			( human_npc_ptr& new_npc, human_npc::npc_game_attributes& attributes )
{
	new_npc->set_attributes				( attributes );
	new_npc->enable						( );
	m_npcs.push_back					( new_npc );
}

void game::rotate_selected_npc			( float const y_angle )
{
	if ( m_selected_npc )
	{
		float const y_angle_rad			= math::deg2rad( y_angle );
		float4x4 const& rotation		= math::create_rotation( m_selected_npc->get_rotation_angles() );
		float4x4 const& new_rotation	= math::create_rotation( float3( 0.f, y_angle_rad, 0.f ) );
		m_selected_npc->set_rotation	( rotation * new_rotation );
	}
}

void game::delete_selected_npc			( )
{
	if ( m_selected_npc )
	{
		m_npcs.erase					( m_selected_npc );
		kill_npc						( m_selected_npc );
		m_selected_npc					= 0;
		m_active_npc_set				= false;
		--m_npc_queries_count;
	}
}

void game::assign_behaviour				( )
{
	if ( m_selected_npc )
	{
		xray::ai::behaviour_cook_params	behaviour_params;
		resources::user_data_variant	new_params;
		new_params.set					( behaviour_params );
		
		query_resource					(
			"test",
			resources::behaviour_class,
			boost::bind( &game::on_behaviour_created, this, _1 ), 
			g_allocator,
			&new_params,
			0
		);
	}
}

void game::on_behaviour_created			( resources::queries_result& data )
{
	R_ASSERT							( data.is_successful() );

	resources::unmanaged_resource_ptr new_behaviour = data[0].get_unmanaged_resource();
	
	if ( m_selected_npc )
		m_selected_npc->set_behaviour	( new_behaviour );
}

} // namespace stalker2
