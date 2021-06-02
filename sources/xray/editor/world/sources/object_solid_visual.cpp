////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_solid_visual.h"
#include "tool_solid_visual.h"
#include "project_items.h"
#include "level_editor.h"
#include "collision_object_dynamic.h"
#include "project.h"
#include "editor_world.h"
#include <xray/editor/base/collision_object_types.h>
#include "window_ide.h"

#pragma managed( push, off )
#	include <xray/collision/api.h>
#	include <xray/render/world.h>
#	include <xray/editor/world/engine.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/physics/world.h>
#pragma managed( pop )

namespace xray {
namespace editor {

object_solid_visual::object_solid_visual(tool_solid_visual^ t, render::scene_ptr const& scene ):
super				(t), 
m_tool_solid_visual	(t),
m_scene				( NEW (render::scene_ptr) (scene) ),
m_snap_to_terrain	( true ),
m_ghost_mode		( false ),
m_physics_model_type( physics::model_type_static ),
m_rigid_body		( NULL ),
m_physic_mode_now	( false ),
m_model_query		( NULL )
{
	m_model_instance	= NEW(render::static_model_ptr)();
	m_hq_collision		= NEW(collision::geometry_ptr)();
	image_index			= xray::editor_base::node_mesh;
	m_collision_shape	= NEW(physics::bt_collision_shape_ptr)();
}

object_solid_visual::~object_solid_visual()
{
	destroy_collision	( ) ;
	R_ASSERT			( !get_visible() );
	DELETE				( m_model_instance );
	DELETE				( m_hq_collision );
	DELETE				( m_scene );
	DELETE				( m_collision_shape );

	if(m_model_query)
		m_model_query->m_rejected = true;
}

void object_solid_visual::destroy_collision	( )
{
	if ( m_collision->initialized() )
		m_collision->destroy( g_allocator );
}

void object_solid_visual::load_props( configs::lua_config_value const& t )
{
	super::load_props				( t );

	if(t.value_exists("snap_to_terrain"))
		m_snap_to_terrain = t["snap_to_terrain"];

	if(t.value_exists("ghost_mode"))
		m_ghost_mode = t["ghost_mode"];

	if(t.value_exists("physics_model_type"))
	{
		int mode = (int)t["physics_model_type"];
		m_physics_model_type = (mode==0) ? physics::model_type_dynamic : physics::model_type_static;
	}
}

void object_solid_visual::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= m_physics_model_type==physics::model_type_dynamic ? "dynamic_visual" : "solid_visual";
	t["snap_to_terrain"]	= m_snap_to_terrain;
	t["ghost_mode"]			= m_ghost_mode;
	t["physics_model_type"] = m_physics_model_type;
}

void  object_solid_visual::ghost_mode::set(bool v)		
{ 
	m_ghost_mode=v; 
	if(get_visible() && (*m_model_instance).c_ptr())
	{
		get_editor_renderer().scene().set_model_ghost_mode( (*m_model_instance)->m_render_model, m_ghost_mode);
	}
}

void object_solid_visual::render( )
{
	if((*m_model_instance).c_ptr())
	{
		render::model_locator_item m;
		if( (*m_model_instance)->m_render_model->get_locator( "fire_point", m ) )
		{
			get_debug_renderer().draw_origin( get_editor_world().scene(), m.m_offset * *m_transform, 1.0f );
		}
	}
}

void object_solid_visual::set_visible(bool bvisible)
{
	ASSERT(bvisible!=get_visible());

	if((*m_model_instance).c_ptr())
	{
		if(bvisible)
		{
			get_editor_renderer().scene().add_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
			get_editor_renderer().select_model		( *m_scene, (*m_model_instance)->m_render_model, m_project_item->get_selected() );
			if(m_ghost_mode)
				get_editor_renderer().scene().set_model_ghost_mode( (*m_model_instance)->m_render_model, m_ghost_mode);
		}else
		{
			get_editor_renderer().select_model		( *m_scene, (*m_model_instance)->m_render_model, false );
			get_editor_renderer().scene().remove_model( *m_scene, (*m_model_instance)->m_render_model );
		}
	}

	super::set_visible(bvisible);
}

void object_solid_visual::set_transform			(float4x4 const& transform)
{
	super::set_transform			(transform);

	if(get_visible() && (*m_model_instance).c_ptr()) {
		get_editor_renderer().scene().update_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
		get_editor_renderer().select_model			( *m_scene, (*m_model_instance)->m_render_model, m_project_item->get_selected() );
	}
}

void object_solid_visual::on_selected(bool selected)
{
	super::on_selected(selected);

	if (get_visible() && (*m_model_instance).c_ptr()) {
		get_editor_renderer().scene().update_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
		get_editor_renderer().select_model			( *m_scene, (*m_model_instance)->m_render_model, selected );
	}
}



void object_solid_visual::set_library_name( System::String^ library_name )
{
	unload_contents		( true );
	m_lib_name			= check_valid_library_name(library_name);

	R_ASSERT			(m_lib_name!=nullptr);

	load_contents		( );
}

void object_solid_visual::on_model_loaded( resources::queries_result& data )
{
	if(!m_needed_quality_)
		return;
	
	m_model_query = NULL;

	R_ASSERT	((*m_model_instance).c_ptr()==NULL, data[0].get_requested_path());

	if( !data[0].is_successful() )
	{
		owner_tool()->ide()->error_message_out( System::String:: Format("model loading failed for {0} ({1})", get_library_name(), get_full_name()) );
		return;
	}else
	{
		*m_model_instance	= static_cast_resource_ptr<render::static_model_ptr>(data[0].get_unmanaged_resource());
		
		if(data[1].is_successful())
			*m_hq_collision		= static_cast_resource_ptr<collision::geometry_ptr>(data[1].get_unmanaged_resource());	
		else
			*m_hq_collision		= (*m_model_instance)->m_collision_geom;


		if(get_visible())
		{
			get_editor_renderer().scene().add_model	( *m_scene, (*m_model_instance)->m_render_model, *m_transform );
			get_editor_renderer().select_model		( *m_scene, (*m_model_instance)->m_render_model, m_project_item->get_selected() );
			if(m_ghost_mode)
				get_editor_renderer().scene().set_model_ghost_mode( (*m_model_instance)->m_render_model, m_ghost_mode);
		}
	
		initialize_collision	( );
	}
	
}

void object_solid_visual::initialize_collision	()
{
	m_aabbox->identity			( );
	ASSERT						( *m_model_instance );
	ASSERT						( (*m_hq_collision).c_ptr() );

	destroy_collision			( );

	m_collision->create_from_geometry	(
					false,
					this,
					&*collision::new_triangle_mesh_geometry_instance(g_allocator, float4x4().identity(), (*m_hq_collision).c_ptr() ),
					editor_base::collision_object_type_dynamic | editor_base::collision_object_type_touch
				);
	m_collision->insert				( m_transform );
}

void object_solid_visual::load_contents( )
{
	if(m_needed_quality_)
		return;

	m_needed_quality_		= true;
	if( (*m_model_instance).c_ptr() )
		return;

	m_model_query = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_solid_visual::on_model_loaded), g_allocator);
	
	unmanaged_string model_name(m_lib_name);

	fs_new::virtual_path_string hq_collision_path;
	hq_collision_path.assignf( "resources/models/%s.model/hq_collision", model_name.c_str() );
	resources::request	r[2]={
		{ model_name.c_str(), resources::static_model_instance_class },
		{ hq_collision_path.c_str(), resources::collision_geometry_class }
	};
	resources::query_resources	(
		r,
		2,
		boost::bind(&query_result_delegate::callback, m_model_query, _1),
		g_allocator, 0, 0, assert_on_fail_false
		);
}

void object_solid_visual::unload_contents( bool )
{
	if( !(*m_model_instance).c_ptr() )
		return;

	if(!m_needed_quality_)
		return;

	if(get_visible())
	{
		get_editor_renderer().select_model			( *m_scene, (*m_model_instance)->m_render_model, false );
		get_editor_renderer().scene().remove_model	( *m_scene, (*m_model_instance)->m_render_model );
	}

	destroy_collision		( );

	(*m_model_instance)		= 0;
	m_needed_quality_		= false;
	deactivate_physics_shell();
}

enum_terrain_interaction object_solid_visual::get_terrain_interaction( )
{
	if(m_snap_to_terrain)
		return enum_terrain_interaction::tr_vertical_snap;
	else
		return enum_terrain_interaction::tr_free;
}

void object_solid_visual::physic_mode::set( bool value )
{
	m_physic_mode_now = value;

	if(m_physic_mode_now)
		activate_physics_shell();
	else
		deactivate_physics_shell();
}

void object_solid_visual::tick( )
{
	float4x4 t				= m_rigid_body->get_transform();
	set_transform			( t );

	owner_tool()->get_level_editor()->add_onframe_delegate( gcnew execute_delegate(this, &object_solid_visual::tick), 1);
}

void object_solid_visual::on_physic_loaded( resources::queries_result& data )
{
	*m_collision_shape				= static_cast_resource_ptr<physics::bt_collision_shape_ptr>(data[0].get_unmanaged_resource());
	configs::binary_config_ptr cfg	= static_cast_resource_ptr<configs::binary_config_ptr>(data[1].get_unmanaged_resource());

	physics::bt_rigid_body_construction_info	info;
	info.m_collisionShape						= *m_collision_shape;

	if(cfg.c_ptr())
	{
		bool load_result = info.load			( cfg->get_root() );
		if(!load_result)
		{
			if( m_physics_model_type == physics::model_type_dynamic )		
				info.m_mass = 10.0f;
			else
				info.m_mass = 0.0f;
		}
	}

	if(m_physics_model_type != physics::model_type_dynamic )
		info.m_mass = 0.0f;

	m_rigid_body					= physics::create_rigid_body( info );
	m_rigid_body->set_transform		( create_rotation( get_rotation() ) * create_translation(get_position()) );
	
	get_editor_world().get_physics_world().add_rigid_body( m_rigid_body );
	owner_tool()->get_level_editor()->add_onframe_delegate( gcnew execute_delegate(this, &object_solid_visual::tick), 1);
}

void object_solid_visual::activate_physics_shell( )
{
	if(m_rigid_body)
		return;

	unmanaged_string model_name(m_lib_name);
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_solid_visual::on_physic_loaded), g_allocator);

	fs_new::virtual_path_string model_collision_path;
	model_collision_path.assignf( "resources/models/%s.model/collision", model_name.c_str() );

	fs_new::virtual_path_string model_config_path;
	model_config_path.assignf( "resources/models/%s.model/settings", model_name.c_str() );

	resources::class_id_enum classid = (m_physics_model_type==physics::model_type_dynamic) ? 
										resources::collision_bt_shape_class_dynamic : 
										resources::collision_bt_shape_class_static;

	resources::request	r[]={
		{ model_collision_path.c_str(), classid },
		{ model_config_path.c_str(), resources::binary_config_class }
	};

	resources::query_resources(
		r,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);
}

void object_solid_visual::deactivate_physics_shell( )
{
	if(!m_rigid_body)
		return;

	get_editor_world().get_physics_world().remove_rigid_body( m_rigid_body );
	destroy_rigid_body( m_rigid_body );
	m_rigid_body		= NULL;
	*m_collision_shape	= NULL;
	owner_tool()->get_level_editor()->remove_onframe_delegate( gcnew execute_delegate(this, &object_solid_visual::tick) );

}

void object_solid_visual::edit_in_model_assembler( button^ )
{
	lib_item^ li = m_tool_solid_visual->m_library->get_library_item_by_full_path(get_library_name(), false);
	if(li!=nullptr)
	{
		editor_base::tool_window^ ed = get_level_editor()->get_editor_world().create_model_editor();
		ed->Show( m_tool_solid_visual->item_type_to_str(li->m_item_type), li->full_path_name() );
	}
}

wpf_controls::property_container^ object_solid_visual::get_property_container( )
{
	wpf_controls::property_container^ cont = super::get_property_container();

	cont->properties["physics/type"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(3){
			"dynamic",
			"static",
		} ) 
	);

	button^ but = cont->properties["general/library name"]->add_button("->", 
																		"Open in model assembler", 
																		gcnew System::Action<button^>( this, &object_solid_visual::edit_in_model_assembler ) );
	but->width	= 20;

	return cont;
}

} // namespace editor
} // namespace xray
