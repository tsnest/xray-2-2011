////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "object_human_npc.h"
#include "tool_misc.h"
#include "project_items.h"
#include <xray/collision/collision_object.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/collision/animated_object.h>
#include <xray/animation/mixing_expression.h>
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include <xray/render/facade/debug_renderer.h>
#include "editor_world.h"

#pragma managed( push, off )
#	include <xray/render/world.h>
#	include <xray/editor/world/engine.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

/*
void model_name_ui_type_editor::run_editor( xray::editor::wpf_controls::property_editors::property^ prop )
{
	wpf_controls::property_container^ container = safe_cast<wpf_controls::property_container^>( prop->property_owners[0] );
	object_human_npc^ npc = safe_cast<object_human_npc^>( container->owner );
	
	System::String^ selected = nullptr;
	if ( editor_base::resource_chooser::choose( "solid_visual", npc->model, nullptr, selected, true ) )
	{
		npc->model = selected;
	}
}
*/

void animation_name_ui_type_editor::run_editor( xray::editor::wpf_controls::property_editors::property^ prop )
{
	wpf_controls::property_container^ container = safe_cast< wpf_controls::property_container^ >( prop->property_owners[0] );
	object_human_npc^ npc			= safe_cast< object_human_npc^ >( container->owner );
	
	System::String^ selected		= nullptr;
	
	if ( editor_base::resource_chooser::choose( "animations_list", npc->animation_idle, "single/human", selected, true ) )
		npc->animation_idle			= "resources/animations/" + selected;
}

object_human_npc::object_human_npc	( tool_misc^ tool, render::scene_ptr const& scene ) :
	super							( tool ),
	m_parent_tool					( tool ),
	m_model_instance				( NEW( animated_model_instance_ptr ) ),
	m_animation						( NEW( animation::skeleton_animation_ptr ) ),
	m_scene							( NEW( render::scene_ptr )( scene ) )
{
	m_onframe_delegate				= gcnew execute_delegate( this, &object_human_npc::play_animation );
	image_index						= editor_base::node_mesh;
}

object_human_npc::~object_human_npc	( )
{
	R_ASSERT						( !get_visible() );
	
	m_owner_tool->get_level_editor()->remove_onframe_delegate( m_onframe_delegate );
	
	DELETE							( m_animation );
	DELETE							( m_model_instance );
	DELETE							( m_scene );
}

void object_human_npc::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy		( g_allocator );
}

void object_human_npc::set_visible	( bool is_visible )
{
	ASSERT							( is_visible != get_visible() );

	if ( m_model_instance->c_ptr() )
	{
		if ( is_visible )
		{
			get_editor_renderer().scene().add_model	( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, *m_transform );
			get_editor_renderer().select_model		( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, m_project_item->get_selected() );
		}
		else
			get_editor_renderer().scene().remove_model( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model );
	}

	super::set_visible				( is_visible );
}

void object_human_npc::set_transform( float4x4 const& transform )
{
	super::set_transform			( transform );

	if ( get_visible() && m_model_instance->c_ptr() )
	{
		get_editor_renderer().scene().update_model	( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, *m_transform );
		get_editor_renderer().select_model			( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, m_project_item->get_selected() );
	}
}

void object_human_npc::on_selected	( bool selected )
{
	super::on_selected				( selected );

	if ( get_visible() && m_model_instance->c_ptr() )
	{
		get_editor_renderer().scene().update_model	( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, *m_transform );
		get_editor_renderer().select_model			( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, selected );
	}
}

void object_human_npc::save			( configs::lua_config_value config )
{
	super::save						( config );
	config["game_object_type"]		= "human_npc";
	config["model"]					= unmanaged_string( m_animated_model_name/*m_model_name*/ ).c_str();
	config["animation_idle"]		= unmanaged_string( m_animation_name ).c_str();

	config["brain"]					= "resources/npc/human/default.npc";

	configs::lua_config_value attributes_config = config["attributes"].create_table();
	
	attributes_config.add_super_table		( "npc/human/game_attributes/for_manual_creation.attributes", "attributes" );
	attributes_config["initial_position"]	= get_position();
	attributes_config["initial_rotation"]	= get_rotation();
	attributes_config["initial_scale"]		= get_scale();
}

void object_human_npc::load_props	( configs::lua_config_value const& config )
{
	super::load_props				( config );
	
	if ( config.value_exists( "model" ) )
	{
		pcstr model_name			= config["model"];
		animated_model_filename		= gcnew System::String( model_name );
	}
	if ( config.value_exists( "animation_idle" ) )
	{
		pcstr animation_name		= config["animation_idle"];
		animation_idle				= gcnew System::String( animation_name );
	}
}

void object_human_npc::load_contents( )
{
}

void object_human_npc::unload_contents( bool )
{
	if ( !m_model_instance->c_ptr() )
		return;

	if ( get_visible() )
		get_editor_renderer().scene().remove_model	( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model );

	destroy_collision		( );

	*m_model_instance		= 0;
	*m_animation			= 0;
}

void object_human_npc::on_model_loaded	( resources::queries_result& data )
{
	if ( data.is_successful() )
	{
		if ( m_model_instance->c_ptr() )
			get_editor_renderer().scene().remove_model( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model );

		*m_model_instance		= static_cast_resource_ptr< animated_model_instance_ptr >( data[0].get_unmanaged_resource() );
		get_editor_renderer().scene().add_model	( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, *m_transform );
		m_model_instance->c_ptr()->m_animation_player->set_object_transform( *m_transform );
	
		destroy_collision		( );
		initialize_collision	( );
	}
	else
		LOG_ERROR				( "model loading failed for %s (%s)", unmanaged_string( m_animated_model_name/*m_model_name*/ ).c_str(), unmanaged_string( get_library_name() ).c_str() );
}

void object_human_npc::on_animation_loaded	( resources::queries_result& data )
{
	if ( data.is_successful() )
	{
		*m_animation = static_cast_resource_ptr< animation::skeleton_animation_ptr >( data[0].get_managed_resource() );
		m_owner_tool->get_level_editor()->add_onframe_delegate( gcnew execute_delegate( this, &object_human_npc::play_animation ), 1 );
	}
	else
		LOG_ERROR				( "animation loading failed for %s (%s)", unmanaged_string( m_animation_name ).c_str(), unmanaged_string( get_library_name() ).c_str() );
}

void object_human_npc::load_model	( System::String^ model_name )
{
	query_result_delegate* rq		= NEW( query_result_delegate )( gcnew query_result_delegate::Delegate( this, &object_human_npc::on_model_loaded ), g_allocator );
	resources::query_resource		(
		unmanaged_string( model_name ).c_str(),
		resources::editor_animated_model_instance_class,
		boost::bind( &query_result_delegate::callback, rq, _1 ),
		g_allocator
	);
}

void object_human_npc::load_animation	( System::String^ animation_name )
{
	query_result_delegate* rq			= NEW( query_result_delegate )( gcnew query_result_delegate::Delegate( this, &object_human_npc::on_animation_loaded ), g_allocator );
	resources::query_resource			(
		unmanaged_string( animation_name ).c_str(),
		resources::animation_class,
		boost::bind( &query_result_delegate::callback, rq, _1 ),
		g_allocator
	);
}

void object_human_npc::initialize_collision( )
{
	ASSERT								( !m_collision->initialized() );
	
	non_null<collision::geometry_instance>::ptr	instance	= collision::new_composite_geometry_instance( g_allocator, float4x4().identity(), m_model_instance->c_ptr()->m_damage_collision->get_geometry() );
	m_collision->create_from_geometry	( false, this, &*instance, editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch );
	m_collision->insert					( m_transform );
}

void object_human_npc::play_animation	( )
{
	using 								xray::animation::animation_player;
	using 								xray::animation::instant_interpolator;
	using 								xray::animation::mixing::animation_clip;
	using 								xray::animation::mixing::animation_lexeme;
	
	m_owner_tool->get_level_editor()->add_onframe_delegate( m_onframe_delegate, 1 );

	if ( !m_model_instance->c_ptr() || !m_animation->c_ptr() )
		return;

	mutable_buffer buffer				( ALLOCA( animation_player::stack_buffer_size ), animation_player::stack_buffer_size );
	animation_lexeme a					(
		animation::mixing::animation_lexeme_parameters(
			buffer,
			unmanaged_string( m_animation_name ).c_str(),
			*m_animation
		).weight_interpolator( instant_interpolator() )
	);
	
	u32 const current_frame_id			= m_owner_tool->get_level_editor()->get_editor_world().frame_id();
	m_model_instance->c_ptr()->m_animation_player->set_target_and_tick	( a, current_frame_id );
	m_model_instance->c_ptr()->m_animation_player->set_object_transform	( *m_transform );
	m_model_instance->c_ptr()->m_animation_player->tick					( current_frame_id );
}

void object_human_npc::render			( )
{
	if ( !m_model_instance->c_ptr() || !m_animation->c_ptr() )
		return;

	animation::animation_player* player	= m_model_instance->c_ptr()->m_animation_player;
	animation::skeleton_ptr	skeleton	= m_model_instance->c_ptr()->m_physics_model->m_skeleton;

	u32 const bone_matrices_count		= skeleton->get_non_root_bones_count();
	float4x4* const bone_matrices		= static_cast< float4x4* >( ALLOCA( bone_matrices_count * sizeof( float4x4 ) ) );
	player->compute_bones_matrices		( *skeleton, bone_matrices, bone_matrices + bone_matrices_count );

	*m_transform						= player->get_object_transform();
	get_editor_renderer().scene().update_model		( *m_scene, m_model_instance->c_ptr()->m_render_model->m_model, *m_transform );
	get_editor_renderer().scene().update_skeleton	(
		m_model_instance->c_ptr()->m_render_model->m_model,
		bone_matrices,
		bone_matrices_count
	);

	m_model_instance->c_ptr()->m_render_model->m_bounding_collision->update	( bone_matrices, bone_matrices + bone_matrices_count );
	m_model_instance->c_ptr()->m_physics_model->m_collision->update			( bone_matrices, bone_matrices + bone_matrices_count );
	m_model_instance->c_ptr()->m_damage_collision->update					( bone_matrices, bone_matrices + bone_matrices_count );

	get_editor_renderer().select_model	(
		*m_scene,
		m_model_instance->c_ptr()->m_render_model->m_model,
		m_project_item->get_selected()
	);
}

wpf_controls::property_container^ object_human_npc::get_property_container	( )
{
	wpf_controls::property_container^ container		= super::get_property_container();

	container->properties["visual/animated model instance"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( m_parent_tool->animated_model_instances_names()->ToArray() )
	);
	
	return											container;
}

} // namespace editor
} // namespace xray