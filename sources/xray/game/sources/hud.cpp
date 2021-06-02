////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "hud.h"
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/mixing_animation_lexeme.h>
#include <xray/animation/mixing_math.h>
#include <xray/animation/skeleton_animation.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include "game_world.h"
#include "game.h"
#include <xray/collision/space_partitioning_tree.h>
#include <xray/physics/character_controller.h>
#include <xray/physics/world.h>
#include <xray/physics/rigid_body.h>
#include "collision_object_types.h"
#include <xray/input/mouse.h>
#include <xray/animation/cubic_spline_skeleton_animation.h>

#include "actor_controller.h"
#include "weapon.h"

namespace stalker2{

hud::hud( game_world& w )
:super				( w ),
m_animation_player	( NULL ),
m_camera_director	( w.get_camera_director() ),
m_is_active			( false ),
m_model_added		( false ),
m_crouch			( false )
{
	m_character_transform.identity	( );
	m_frame_events.m_last_frame_time_ms= 0;
	m_frame_events.m_mouse_move.set	( 0,0,0 );
}

hud::~hud( )
{
	unload	();
}

void hud::load( )
{
	resources::request r[] ={
		{ "character/human/neutral/neutral_01/neutral_01",		resources::skeleton_model_instance_class },
		{ "resources/animations/single/human/hud/stand_idle",	resources::animation_class },
		{ "resources/animations/single/human/hud/stand_add",	resources::animation_class },
//.		{ "item/weapon/ak74/ak74_4tir3",						resources::static_model_instance_class },
		{ "ak_74",												resources::weapon_class },
	};

	resources::query_resources(
		r,
		boost::bind( &hud::on_resources_loaded, this, _1 ),
		g_allocator
	);
}

void hud::unload( )
{
	if(!m_animation_player)
		return;

	m_actor_controller->deactivate	( );
	DELETE					( m_actor_controller );
	m_actor_controller		= NULL;
	remove_models_from_scene( );

	m_character_model		= NULL;
//	m_weapon_model			= NULL;

	m_idle_stand_animation	= NULL;
	m_look_animation_add	= NULL;
	DELETE					( m_animation_player );
	m_animation_player		= NULL;
}


void hud::on_resources_loaded( resources::queries_result& data )
{
	if(!data.is_successful())
		return;

	m_character_model			= static_cast_resource_ptr<render::skeleton_model_ptr>(data[0].get_unmanaged_resource());
	m_animation_player			= NEW(animation::animation_player)( );
	m_animation_player->set_no_delete();// ??

	m_idle_stand_animation			= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[1].get_managed_resource());
	m_look_animation_add			= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[2].get_managed_resource());

	m_anim_timer.start		( );

	m_head_bone_idx			= m_character_model->m_skeleton->get_bone_index("Head")-1;
	m_weapon_bone_idx		= m_character_model->m_skeleton->get_bone_index("Weapon")-1;

//	m_weapon_model			= static_cast_resource_ptr<render::static_model_ptr>(data[3].get_unmanaged_resource());

	m_weapon				= static_cast_resource_ptr<weapon_ptr>(data[3].get_unmanaged_resource());
	m_weapon->m_game_world	= &m_game_world;
	
	m_actor_controller	= xray::physics::create_character_controller(*g_allocator, m_game_world.get_physics_world() );
	m_actor_controller->initialize( );

	// make y axis up
	float3 i						= float3(1,0,0);
	float3 j						= float3(0, 1, 0);
	float3 k						= float3(0,0,1);//( i ^ j );
	m_character_transform.i.xyz() 	= i;//j ^ k;
	m_character_transform.j.xyz() 	= j;
	m_character_transform.k.xyz() 	= k;
	m_character_transform.c.xyz()	= m_camera_inv_view.c.xyz();


	m_weapon->action				( 0 );//idle


	m_actor_controller->activate				( m_character_transform );
	m_animation_player->set_object_transform	( m_character_transform );


	if(m_is_active)
		add_models_to_scene	( );
}

void hud::on_activate( camera_director* cd )
{
	super::on_activate				( cd );

	m_frame_events.m_last_frame_time_ms= m_game_world.get_game().time_ms();

	m_camera_inv_view				= m_camera_director->get_inverted_view_matrix( );
	m_is_active						= true;
	load		( );
}

void hud::on_deactivate( )
{
	super::on_deactivate( );
	m_is_active			= false;
	unload				( );
}

void hud::add_models_to_scene( )
{
	render::scene_ptr scene		= m_game_world.get_game().get_render_scene();
	render::game::renderer& r	= m_game_world.get_game().renderer();

	r.scene().add_model			( scene, m_character_model->m_render_model, m_character_transform );
//. 	r.scene().add_model			( scene, m_weapon_model->m_render_model, float4x4().identity() );
	m_weapon->show				( m_character_transform );
	m_model_added				= true;
}

void hud::remove_models_from_scene( )
{
	render::scene_ptr scene		= m_game_world.get_game().get_render_scene();
	render::game::renderer& r	= m_game_world.get_game().renderer();

	r.scene().remove_model		( scene, m_character_model->m_render_model );
//. 	r.scene().remove_model		( scene, m_weapon_model->m_render_model );
	m_weapon->hide				( );

	m_model_added				= false;
}

collision::geometry_instance& hud::get_caracter_capsule( )
{
	return m_actor_controller->get_capsule( );
}

void hud::tick( )
{
	super::tick						( );
	input_tick						( );

	if(!m_animation_player || !m_game_world.get_collision_tree())
		return;

	m_character_transform = m_actor_controller->get_transform();

	float const angle_factor		= 0.5f;

	// apply rotation without physic simulation
	float3 const angles_zxy			= m_character_transform.get_angles( math::rotation_zxy );
	float3 const new_angles_zxy		= float3(	/*angle_factor*raw_angles.x + */angles_zxy.x, 
												angle_factor*m_frame_events.m_onframe_turn_y + angles_zxy.y, 
												angles_zxy.z );

	float4x4 rotation				= math::create_rotation( new_angles_zxy, math::rotation_zxy );
	float4x4 const translation		= math::create_translation( m_character_transform.c.xyz() );

	m_character_transform			= rotation * translation;

	// apply moving
	m_actor_controller->set_transform( m_character_transform );

	float3 walk_direction(0,0,0);

	float const frame_time_sec		= m_frame_events.m_last_frame_time_delta/1000.0f;

	if(	m_frame_events.m_onframe_shift	&& !m_frame_events.m_onframe_back)  
	{
		float const run_delta_fw		= frame_time_sec * 1.66f *6; //6km/h
		float const run_delta_right		= frame_time_sec * 0.83f *6; //3km/h

		walk_direction	= m_character_transform.k.xyz()*m_frame_events.m_onframe_run_fwd * run_delta_fw;
		walk_direction	+= m_character_transform.i.xyz()*m_frame_events.m_onframe_run_right * run_delta_right;
	} else {
		float const move_delta_fw		= frame_time_sec * 1.66f *3.5; //3.5km/h
		float const move_delta_right	= frame_time_sec * 0.83f *3.5;

		walk_direction	= m_character_transform.k.xyz()*m_frame_events.m_onframe_move_fwd * move_delta_fw;
		walk_direction	+= m_character_transform.i.xyz()*m_frame_events.m_onframe_move_right * move_delta_right;
	}

	m_actor_controller->set_walk_direction(walk_direction  );
	
	if(m_frame_events.m_onframe_jump)
		m_actor_controller->jump();

	static bool spectator = false;

	// caclulate additive animation coefficient, based on camera X angle
	float k = m_camera_inv_view.k.xyz() | float3(0.0f, 1.0f, 0.0f);
	//1.0-up....-1.0f-down
	k			= 1.0f - (k+1.0f)/2.0f; // normalized to 0..1.0f

	mutable_buffer buffer				( ALLOCA( animation::animation_player::stack_buffer_size ), animation::animation_player::stack_buffer_size );

	//m_crouch = false;

	animation::skeleton_animation_ptr current_idle_animation		= m_idle_stand_animation;
	animation::skeleton_animation_ptr current_additive_animation	= m_look_animation_add;

	float additive_current_anim_time		= animation::cubic_spline_skeleton_animation_pinned( current_additive_animation ).c_ptr()->length_in_frames() / animation::default_fps * k;

	if(m_frame_events.m_onframe_shoot) {
		m_frame_events.m_onframe_shoot = false;
		m_weapon->action				( 1 );
	}

	if(m_frame_events.m_onframe_reload) {
		m_frame_events.m_onframe_reload = false;
		m_weapon->action				( 2 );
	}

if(!m_crouch) {
	animation::mixing::animation_lexeme	current_idle_lexeme(
		animation::mixing::animation_lexeme_parameters(
			buffer, 
			"idle",
			current_idle_animation
		).time_scale( 0.f )
	);

	animation::mixing::animation_lexeme	current_additive_lexeme(
		animation::mixing::animation_lexeme_parameters(
			buffer, 
			"additive",
			current_additive_animation
		)
		.time_scale( 0.f )
		.start_animation_interval_time( additive_current_anim_time )
		.override_existing_animation( true )
		.additivity_priority( 1 )
	);

	u32 current_time				= m_anim_timer.get_elapsed_msec();
	
	animation::mixing::animation_lexeme weapon_target = m_weapon->select_animation( buffer );

	m_animation_player->set_target_and_tick	( 
						current_idle_lexeme
						+ current_additive_lexeme
						+ weapon_target /*+ current_additive_lexeme_1*/
						,current_time );
	} else {


	//animation::skeleton_animation_ptr current_crouch_animation		= m_crouch_animation;

	animation::mixing::animation_lexeme	current_idle_lexeme(
		animation::mixing::animation_lexeme_parameters(
			buffer, 
			"crouch",
			current_idle_animation
		).time_scale( 0.f )
	);

	animation::mixing::animation_lexeme	current_additive_lexeme(
		animation::mixing::animation_lexeme_parameters(
			buffer, 
			"additive",
			current_additive_animation
		)
		.time_scale( 0.f )
		.start_animation_interval_time( additive_current_anim_time )
		.override_existing_animation( true )
		.additivity_priority( 1 )
	);

	u32 current_time				= m_anim_timer.get_elapsed_msec();
	
	animation::mixing::animation_lexeme weapon_target = m_weapon->select_animation( buffer );

	m_animation_player->set_target_and_tick	( 
						current_idle_lexeme
						+ current_additive_lexeme
						+ weapon_target
						,current_time );

		}

	render::scene_ptr scene			= m_game_world.get_game().get_render_scene();
	render::game::renderer& r		= m_game_world.get_game().renderer();

	
	float4x4 character_render_transform		= create_rotation(float3(0.0f, math::pi, 0.0f)) * m_character_transform;
	
	if(!spectator)
		r.scene().update_model			( scene, m_character_model->m_render_model, character_render_transform );

	u32 const non_root_bones_count	= m_character_model->m_skeleton->get_non_root_bones_count( );
	float4x4* const matrices		= static_cast<float4x4*>( ALLOCA(non_root_bones_count*sizeof(float4x4)) );
	m_animation_player->compute_bones_matrices( *m_character_model->m_skeleton, matrices, matrices + non_root_bones_count );

	if(!spectator)
	{
		r.scene().update_skeleton	(	m_character_model->m_render_model,
										matrices,
										non_root_bones_count );

	
		float4x4 weapon				= weapon_matrix( matrices );
		m_weapon->set_transform		( weapon );
		m_weapon->tick				( m_animation_player );
	}

//.	r.scene().update_model		( scene, m_weapon_model->m_render_model, weapon);


	// setup camera matrix
	m_inverted_view_matrix		= m_camera_inv_view;
	
	if(!spectator)
	{
		float4x4 head				= head_matrix( matrices );
		m_inverted_view_matrix		= head;
	}

	{
		float3 ray_from		= m_inverted_view_matrix.c.xyz();
		float3 ray_dir		= m_inverted_view_matrix.k.xyz();
		float ray_length	= 100.0f;

		render::debug::renderer& d	= r.debug();

		physics::closest_ray_result result = m_game_world.get_physics_world()->ray_test( ray_from, ray_dir, ray_length );
		
		if(result.m_object)
		{
			d.draw_aabb( scene, result.m_hit_point_world ,float3(0.01f,0.01f,0.01f), math::color(0,255,0,255));

			if(m_frame_events.mouse_event_present(input::mouse_button_left))
				shot( result.m_object, ray_dir, result.m_hit_point_world );
		}
	}

	m_frame_events.reset		( )	;
}

float4x4 hud::head_matrix( float4x4* const matrices ) const
{
	float4x4 character_render_transform		= create_rotation(float3(0.0f, math::pi, 0.0f)) * m_character_transform;
	float4x4 result							= ( create_rotation(float3(0,0,math::pi_d2)) * 
											matrices[m_head_bone_idx] * 
											character_render_transform );

	result.c.xyz()			+= result.j.xyz()*0.1f;
	return result;
}

float4x4 hud::weapon_matrix( float4x4* const matrices ) const
{
	float4x4 character_render_transform		= create_rotation(float3(0.0f, math::pi, 0.0f)) * m_character_transform;
	float4x4 result							= matrices[m_weapon_bone_idx] * 
											character_render_transform;
	return result;
}

void hud::shot( physics::bt_rigid_body* body, float3 const& ray_dir, float3 const& point )
{
	if(!body->is_static_or_kinematic_object())
	{
		float const impulse_strength	= 100.f;
		body->apply_impulse				( ray_dir*impulse_strength, point );
	}
}

}
