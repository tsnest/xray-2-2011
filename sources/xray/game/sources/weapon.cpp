////////////////////////////////////////////////////////////////////////////
//	Created		: 15.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "weapon.h"
#include "game_world.h"
#include "game.h"
#include <xray/render/facade/scene_renderer.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include <xray/animation/mixing_math.h>

namespace stalker2{

weapon_part::weapon_part( )
:m_visible		( false )
{}

void weapon_part::load( configs::binary_config_value const& config )
{
	XRAY_UNREFERENCED_PARAMETER(config);
}

weapon_part_visual::weapon_part_visual( )
{}

void weapon_part_visual::load( configs::binary_config_value const& config )
{
	super::load			( config );
	pcstr attach_point	= config["attach_point"];

	if(strings::length(attach_point))
	{
		string32						base_name;
		pcstr attach_point_name			= strings::get_token(attach_point, base_name, '/');

		weapon_part_visual* parent_part = m_parent->get_part(base_name);
		visual_attach_desc				desc;
		desc.m_item						= this;
		strings::copy					( desc.m_attach_point.m_name, attach_point_name );
		desc.m_attach_point.m_bone		= u16(-1);
		parent_part->get_locator		( attach_point_name, desc.m_attach_point );
		parent_part->m_childs.push_back	( desc );
	}
}

void weapon_part_visual::update_childs_transform( )
{
	childs::iterator it		= m_childs.begin();
	childs::iterator it_e	= m_childs.end();
	for(; it!=it_e; ++it)
	{
		visual_attach_desc& desc = *it;
		float4x4 m;
		calculate_locator			( desc.m_attach_point, m );
		desc.m_item->set_transform	( m );
	}
}

void weapon_part_visual::set_transform( float4x4 const& transform )
{
	m_transform				= transform;
	update_childs_transform	( );
}

void weapon_part_visual::show( float4x4 const& initial_transform )
{
	m_visible	= true;
	m_transform = initial_transform;

	childs::iterator it		= m_childs.begin();
	childs::iterator it_e	= m_childs.end();
	for(; it!=it_e; ++it)
	{
		visual_attach_desc& desc = *it;
		float4x4 m;
		calculate_locator	( desc.m_attach_point, m );
		desc.m_item->show	( m );
	}
}

void weapon_part_visual::hide( )
{
	m_visible				= false;
	childs::iterator it		= m_childs.begin();
	childs::iterator it_e	= m_childs.end();
	for(; it!=it_e; ++it)
	{
		visual_attach_desc& desc = *it;
		desc.m_item->hide	( );
	}
}

bool weapon_part_visual::calculate_locator( render::model_locator_item const& locator, 
										   float4x4& result )
{
	result = locator.m_offset * m_transform;
	return true;
}

weapon_part_solid_visual::weapon_part_solid_visual( )
{}

void weapon_part_solid_visual::load( configs::binary_config_value const& config, render::static_model_ptr model )
{
	super::load		( config );

	m_model			= model;
}

bool weapon_part_solid_visual::get_locator( pcstr locator_name, render::model_locator_item& locator )
{
	bool res = m_model->m_render_model->get_locator( locator_name, locator );
	R_ASSERT(res, "locator not found %s", locator_name );
	return res;
}

void weapon_part_solid_visual::set_transform( float4x4 const& transform )
{
	super::set_transform( transform );
	if(m_visible)
	{
		render::scene_ptr scene		= m_parent->get_game_world().get_game().get_render_scene();
		render::game::renderer& r	= m_parent->get_game_world().get_game().renderer();

		r.scene().update_model			( scene, m_model->m_render_model, m_transform );
	}
}

void weapon_part_solid_visual::show( float4x4 const& initial_transform )
{
	super::show	( initial_transform );

	{
		render::scene_ptr scene		= m_parent->get_game_world().get_game().get_render_scene();
		render::game::renderer& r	= m_parent->get_game_world().get_game().renderer();

		r.scene().add_model			( scene, m_model->m_render_model, m_transform );
	}
}

void weapon_part_solid_visual::hide( )
{
	super::hide	( );

	{
		render::scene_ptr scene		= m_parent->get_game_world().get_game().get_render_scene();
		render::game::renderer& r	= m_parent->get_game_world().get_game().renderer();

		r.scene().remove_model			( scene, m_model->m_render_model );
	}
}

weapon_part_skinned_visual::weapon_part_skinned_visual( )
:m_matrices(NULL)
{}

void weapon_part_skinned_visual::load( configs::binary_config_value const& config,
										render::skeleton_model_ptr model,
										animation::skeleton_animation_ptr idle_anim,
										animation::skeleton_animation_ptr reload_anim,
										animation::skeleton_animation_ptr shoot_anim )
{
	super::load			( config );
	m_model		= model;
	m_idle		= idle_anim;
	m_reload	= reload_anim;
	m_shoot		= shoot_anim;
	u32 const non_root_bones_count	= m_model->m_skeleton->get_non_root_bones_count( );
	m_matrices		= ALLOC(float4x4, non_root_bones_count);
	float4x4 inentity = float4x4().identity();
	for(u32 i=0; i<non_root_bones_count; ++i)
		m_matrices[i] = inentity;
}

bool weapon_part_skinned_visual::get_locator( pcstr locator_name, render::model_locator_item& locator )
{
	bool res = m_model->m_render_model->get_locator( locator_name, locator );
	R_ASSERT(res, "locator not found %s", locator_name );
	return res;
}

weapon_part_skinned_visual::~weapon_part_skinned_visual( )
{
	FREE( m_matrices );
}

void weapon_part_skinned_visual::tick( animation::animation_player*& animation_player )
{
	u32 const non_root_bones_count	= m_model->m_skeleton->get_non_root_bones_count( );

	animation_player->compute_bones_matrices ( *m_model->m_skeleton, 
												m_matrices, 
												m_matrices + non_root_bones_count );

	render::game::renderer& r		= m_parent->get_game_world().get_game().renderer();

	r.scene().update_skeleton	(	m_model->m_render_model,
									m_matrices,
									non_root_bones_count );

	update_childs_transform		( );
}

void weapon_part_skinned_visual::set_transform( float4x4 const& transform )
{
	super::set_transform			( transform );
	if(m_visible)
	{
		render::scene_ptr scene		= m_parent->get_game_world().get_game().get_render_scene();
		render::game::renderer& r	= m_parent->get_game_world().get_game().renderer();

		r.scene().update_model			( scene, m_model->m_render_model, m_transform );
	}
}

void weapon_part_skinned_visual::show( float4x4 const& initial_transform )
{
	super::show	( initial_transform );

	{
		render::scene_ptr scene		= m_parent->get_game_world().get_game().get_render_scene();
		render::game::renderer& r	= m_parent->get_game_world().get_game().renderer();

		r.scene().add_model			( scene, m_model->m_render_model, m_transform );
//		if(m_current_state!=-1)
//			select_animation		( );
	}
}

void weapon_part_skinned_visual::hide( )
{
	super::hide	( );

	{
		render::scene_ptr scene		= m_parent->get_game_world().get_game().get_render_scene();
		render::game::renderer& r	= m_parent->get_game_world().get_game().renderer();

		r.scene().remove_model		( scene, m_model->m_render_model );
	}
}

xray::animation::mixing::animation_lexeme weapon_part_skinned_visual::select_animation( mutable_buffer& buffer )
{
	animation::skeleton_animation_ptr animation = NULL;

	switch(m_parent->m_current_state)
	{ //idle
	case 0:
		animation = m_idle;
		break;

	case 1:
		animation = m_shoot;
		break;
	case 2:
		animation = m_reload;
		break;
	default:
		NODEFAULT();
	}

	animation::mixing::animation_lexeme	lexeme(
		animation::mixing::animation_lexeme_parameters(
			buffer,
			"some animation",
			animation
			)
			.synchronization_group_id(999)
			.additivity_priority(1)
	);
	return lexeme;
}

bool weapon_part_skinned_visual::calculate_locator( render::model_locator_item const& locator, 
										   float4x4& result )
{
	if( locator.m_bone==u16(-1) )
		result	= locator.m_offset * m_transform;
	else
		result	= locator.m_offset * m_matrices[locator.m_bone] * m_transform;

	return	true;
}

weapon::weapon( )
:m_game_world( NULL ),
m_current_state( -1 )

{
	m_base				= NEW(weapon_part_skinned_visual)();
	m_base->m_parent	= this;

	m_cover				= NEW(weapon_part_solid_visual)();
	m_cover->m_parent	= this;

	m_stock				= NEW(weapon_part_solid_visual)();
	m_stock->m_parent	= this;

	m_handle			= NEW(weapon_part_solid_visual)();
	m_handle->m_parent	= this;

	m_ammo				= NEW(weapon_part_solid_visual)();
	m_ammo->m_parent	= this;

	m_bolt				= NEW(weapon_part_solid_visual)();
	m_bolt->m_parent	= this;

	m_fore_end			= NEW(weapon_part_solid_visual)();
	m_fore_end->m_parent= this;

	m_barrel			= NEW(weapon_part_solid_visual)();
	m_barrel->m_parent	= this;

	m_barrel_end		= NEW(weapon_part_solid_visual)();
	m_barrel_end->m_parent	= this;
}

void weapon::load( configs::binary_config_value const& config )
{
	XRAY_UNREFERENCED_PARAMETERS(config);
	//m_base->load		( config["base"] );

	//m_cover->load		( config["cover"] );
	//m_stock->load		( config["stock"] );
	//m_handle->load		( config["handle"] );
	//m_ammo->load		( config["ammo"] );
	//m_bolt->load		( config["bolt"] );
	//m_fore_end->load	( config["fore_end"] );
	//m_barrel->load		( config["barrel"] );
	//m_barrel_end->load	( config["barrel_end"] );
}

void weapon::tick( animation::animation_player*& animation_player )
{
	m_base->tick			( animation_player );
	m_base->set_transform	( m_transform );
}

void weapon::set_transform( float4x4 const& transform )
{
	m_transform				= transform;
	m_base->set_transform	(transform);
}

void weapon::show( float4x4 const& initial_transform )
{
	m_transform				= initial_transform;
	m_base->show			(initial_transform);
}

void weapon::hide( )
{
	m_base->hide( );
}

void weapon::action( int id )
{
	m_current_state				= id;
	m_current_state_start_time	= get_game_world().game_time_ms();
}

xray::animation::mixing::animation_lexeme weapon::select_animation( mutable_buffer& buffer )
{
	return m_base->select_animation( buffer );
}

weapon_part_visual* weapon::get_part( pcstr name )
{
	if(strings::equal("base", name))
		return m_base;
	else
	if(strings::equal("cover", name))
		return m_cover;
	else
	if(strings::equal("stock", name))
		return m_stock;
	else
	if(strings::equal("handle", name))
		return m_handle;
	else
	if(strings::equal("ammo", name))
		return m_ammo;
	else
	if(strings::equal("bolt", name))
		return m_bolt;
	else
	if(strings::equal("fore_end", name))
		return m_fore_end;
	else
	if(strings::equal("barrel", name))
		return m_barrel;
	else
	if(strings::equal("barrel_end", name))
		return m_barrel_end;
	else
		return NULL;
}

}