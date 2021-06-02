////////////////////////////////////////////////////////////////////////////
//	Created		: 24.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_anim_object.h"
#include "game_world.h"
#include "game.h"
#include <xray/render/facade/scene_renderer.h>
#include <xray/animation/mixing_animation_lexeme.h>
#include <xray/animation/mixing_math.h>
#include "weapon.h"

namespace stalker2{

test_anim_object::test_anim_object( game_world& w)
:m_game_world			( w ),
m_test_animation_player	( NULL )
{
	load();
}
float3 test_object_position(27,2,40);
void test_anim_object::load( )
{
	m_test_animation_player		= NEW(animation::animation_player)( );
	m_test_animation_player->set_no_delete();// ??
	m_test_animation_player->set_object_transform(math::create_translation(test_object_position));
	m_test_timer.start			( );

	resources::query_resource(
		"ak_74",
		resources::weapon_class,
		boost::bind(&test_anim_object::on_weapon_loaded, this, _1 ),
		g_allocator
		);
//.	m_weapon = NEW	(weapon)(m_game_world, "resources/weapons/ak_74.options");
//.	m_weapon->show	( math::create_translation(float3(113,2,5)) );
//	m_weapon->action ( 0 );//idle
//.	m_weapon->action ( 1 );//shoot
//	m_weapon->action ( 2 );//reload
	//resources::request r_test[] ={
	//	{ "test_object/animaxis",										resources::skeleton_model_instance_class},
	//	{ "resources/animations/single/test_object/animaxis_base",		resources::animation_class },
	//	{ "resources/animations/single/test_object/animaxis_add",		resources::animation_class },
	//};

	////resources::request r_test[] ={
	////	{ "character/human/neutral/neutral_01/neutral_01",			resources::skeleton_model_instance_class},
	////	{ "resources/animations/single/human/hud/stand_idle",		resources::animation_class },
	////	{ "resources/animations/single/human/hud/stand_add",		resources::animation_class },
	////	{ "resources/animations/single/human/hud/stand_add_full",	resources::animation_class },
	////};

	//resources::query_resources(
	//	r_test,
	//	boost::bind( &test_anim_object::on_resources_loaded, this, _1 ),
	//	g_allocator
	//);
}

void test_anim_object::on_weapon_loaded( resources::queries_result& data )
{
	if(!data.is_successful())
		return;

	m_weapon = static_cast_resource_ptr<weapon_ptr>(data[0].get_unmanaged_resource());
	m_weapon->m_game_world	= &m_game_world;
	m_weapon->show			( math::create_translation(test_object_position) );
	
	m_weapon->action		( 1 );// shoot
}

void test_anim_object::on_resources_loaded( resources::queries_result& data )
{
	if(!data.is_successful())
		return;

	//m_test_model	= static_cast_resource_ptr<render::skeleton_model_ptr>(data[0].get_unmanaged_resource());

	//m_test_base		= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[1].get_managed_resource());
	//m_test_add		= static_cast_resource_ptr<animation::skeleton_animation_ptr>(data[2].get_managed_resource());

	//float4x4 test_object_transform = math::create_translation(float3(113,2,5) );
	//m_test_animation_player->set_object_transform(test_object_transform);

	//render::scene_ptr scene		= m_game_world.get_game().get_render_scene();
	//render::game::renderer& r	= m_game_world.get_game().renderer();

	//r.scene().add_model			( scene, m_test_model->m_render_model, test_object_transform );
}

void test_anim_object::tick()
{
	if(m_weapon.c_ptr())
	{
		mutable_buffer buffer				( ALLOCA( animation::animation_player::stack_buffer_size ), animation::animation_player::stack_buffer_size );
		xray::animation::mixing::animation_lexeme target = m_weapon->select_animation( buffer );
		m_test_animation_player->set_target_and_tick( target, m_test_timer.get_elapsed_msec() );
	}
//.		m_test_animation_player->tick(m_test_timer.get_elapsed_msec());

	if(m_weapon.c_ptr())
		m_weapon->tick( m_test_animation_player );

	//u32 const current_time			= m_test_timer.get_elapsed_ms();
	//mutable_buffer buffer			( ALLOCA( animation::animation_player::stack_buffer_size ), animation::animation_player::stack_buffer_size );

	//animation::mixing::animation_lexeme	test_idle_lexeme(
	//	animation::mixing::animation_lexeme_parameters(
	//		buffer, 
	//		"test base",
	//		m_test_base
	//	)
	//);

	//static bool first_time = true;

	//animation::mixing::animation_lexeme	test_additive_lexeme( 
	//	animation::mixing::animation_lexeme_parameters(
	//		buffer, 
	//		"test additive",
	//		m_test_add
	//	)
	//);

	//m_test_animation_player->set_target_and_tick	( test_idle_lexeme /*+ test_additive_lexeme*/, current_time );

	//render::scene_ptr scene			= m_game_world.get_game().get_render_scene();
	//render::game::renderer& r		= m_game_world.get_game().renderer();

	//u32 const non_root_bones_count	= m_test_model->m_skeleton->get_non_root_bones_count( );
	//float4x4* const matrices		= static_cast<float4x4*>( ALLOCA(non_root_bones_count*sizeof(float4x4)) );
	//m_test_animation_player->compute_bones_matrices( *m_test_model->m_skeleton, matrices, matrices + non_root_bones_count );


	//r.scene().update_skeleton	(	m_test_model->m_render_model,
	//								matrices,
	//								non_root_bones_count );
}

}