////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_base.h"
#include "property_grid_panel.h"
#include "model_editor.h"

#pragma managed(push, off)
#include <xray/collision/primitives.h>
#pragma managed(pop)

using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::control_containers;

namespace xray {
namespace model_editor {

void edit_object_mesh::on_collision_settings_ready( resources::queries_result& data )
{
	if( data.is_successful() )
	{
		(*m_collision_cfg)			= static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	}else
		(*m_collision_cfg)			= configs::create_lua_config();

	bool load_result				= load_collision( );

	if(!load_result)
		set_modified	( );
}

bool edit_object_mesh::load_collision( )
{
	for each ( collision_primitive_item^ p in m_collision_primitives ) delete p;
	m_collision_primitives.Clear();

	bool result = true;

	configs::lua_config_value const& root = (*m_collision_cfg)->get_root();

	configs::lua_config_value const& composite	= root["composite"];
	configs::lua_config_iterator it				= composite.begin();
	configs::lua_config_iterator it_e			= composite.end();
	
	float3 const mass_center = m_physics_settings->mass_center;
	for( ;it!=it_e; ++it)
	{
		configs::lua_config_value const& current = (*it);
		collision_primitive_item^ itm		= gcnew collision_primitive_item(this);
		itm->load							( current, mass_center );
		m_collision_primitives.Add			( itm );
	}

	m_collision_panel->set_property_container( get_collision_property_container() );
	
	set_edit_mode					( edit_mode::edit_none );
	
	return							result;
}

void edit_object_mesh::save_collision( )
{
	if(m_collision_primitives.Count)
	{
		// save impl..
		configs::lua_config_ptr config_ptr	= configs::create_lua_config( );
		configs::lua_config_value root		= config_ptr->get_root()["composite"];

		float3 const mass_center = m_physics_settings->mass_center;
		int i=0;
		for each ( collision_primitive_item^ prim in m_collision_primitives )
		{
			configs::lua_config_value current	= root[i];
			prim->save							( current, mass_center );
			++i;
		}	

		(*m_collision_cfg)->get_root().assign_lua_value	( config_ptr->get_root() );
		fs_new::virtual_path_string			config_name;
		config_name.assignf		( "resources/models/%s%s/collision/primitives", unmanaged_string(m_full_name).c_str(), model_file_extension() );
		(*m_collision_cfg)->save_as( config_name.c_str(), configs::target_sources );
	}else
	{
		System::String^ config_name		= System::String::Format("{0}/sources/models/{1}{2}/collision/primitives", m_model_editor->get_resources_path(), m_full_name, gcnew System::String(model_file_extension()) );
		editor_base::fs_helper::remove_to_recycle_bin( gcnew System::String(config_name) );
	}
}

void edit_object_mesh::duplicate_collision_primitive_clicked( button^ button )
{
	collision_primitive_item^ prim					= safe_cast<collision_primitive_item^>(button->tag);
	

	collision_primitive_item^ prim_copy	= gcnew collision_primitive_item(this);
	prim_copy->type						= prim->type;
	prim_copy->position					= prim->position;
	prim_copy->rotation					= prim->rotation;
	prim_copy->scale					= prim->scale;
	m_collision_primitives.Add			( prim_copy );

	prim_copy->activate					( true );

	m_collision_panel->set_property_container( get_collision_property_container() );

	prim_copy->set_selected				( true );
	set_modified						( );
}

void edit_object_mesh::remove_collision_primitive_clicked( button^ button )
{
	collision_primitive_item^ prim					= safe_cast<collision_primitive_item^>(button->tag);
	m_collision_primitives.Remove					( prim );

	prim->set_selected								( false );

	delete											prim;
	
	set_modified									( );
	m_collision_panel->set_property_container		( get_collision_property_container() );
}

void edit_object_mesh::add_sphere_collision_primitive_clicked( button^ )
{
	collision_primitive_item^ prim		= gcnew collision_primitive_item(this);
	prim->type						= (int)collision::primitive_sphere;
	prim->position					= Float3(float3(0,0,0));
	prim->rotation					= Float3(float3(0,0,0));
	prim->scale						= Float3(float3(1,1,1));
	m_collision_primitives.Add		( prim );
	set_modified					( );
	m_collision_panel->set_property_container( get_collision_property_container() );
	prim->activate					( true );
}

void edit_object_mesh::add_box_collision_primitive_clicked ( button^ )
{
	collision_primitive_item^ prim		= gcnew collision_primitive_item(this);
	prim->type						= (int)collision::primitive_box;
	prim->position					= Float3(float3(0,0,0));
	prim->rotation					= Float3(float3(0,0,0));
	prim->scale						= Float3(float3(1,1,1));
	m_collision_primitives.Add		( prim );
	set_modified					( );
	m_collision_panel->set_property_container( get_collision_property_container() );
	prim->activate					( true );
}

void edit_object_mesh::add_cylinder_collision_primitive_clicked ( button^ )
{
	collision_primitive_item^ prim		= gcnew collision_primitive_item(this);
	prim->type						= (int)collision::primitive_cylinder;
	prim->position					= Float3(float3(0,0,0));
	prim->rotation					= Float3(float3(0,0,0));
	prim->scale						= Float3(float3(1,1,1));
	m_collision_primitives.Add		( prim );
	set_modified					( );
	m_collision_panel->set_property_container( get_collision_property_container() );
	prim->activate					( true );
}


property_container^ edit_object_mesh::get_collision_property_container	( )
{
	property_container^	result		= gcnew property_container;

	control_container^ container	= result->add_dock_container( false );
	container->category				= "General";
	button^ but = container->add_button( "Switch to edit", gcnew Action<button^>( this, &edit_object_mesh::switch_to_collision_clicked ) );
	but->width	= 100;

	container						= result->add_dock_container( false );
	container->category				= "Add new item";

	but		= container->add_button( "sphere", gcnew Action<button^>( this, &edit_object_mesh::add_sphere_collision_primitive_clicked ) );
	but->width	= 50;
	but		= container->add_button	( "box", gcnew Action<button^>( this, &edit_object_mesh::add_box_collision_primitive_clicked ) );
	but->width	= 40;
	but		= container->add_button	( "cyliner", gcnew Action<button^>( this, &edit_object_mesh::add_cylinder_collision_primitive_clicked ) );
	but->width	= 50;

	int i=0;
	for each ( collision_primitive_item^ prim in m_collision_primitives )
	{
		property_container^	sub		= gcnew property_container;
		editor_base::object_properties::initialize_property_container( prim, sub );
		
		System::String^ name				= System::String::Format("{0}         ", i++);
		prim->m_current_desc				= result->properties->add_container	( name, "all", "no description", sub );
		sub->inner_value					= gcnew property_descriptor( name, prim, "type" );

		sub->inner_value->set_compo_box_style(
			gcnew cli::array<System::String^, 1>(3){
				"sphere", 
				"box", 
				"cylinder", 
			}
		);

		prim->m_current_desc->add_button( " x ", "", gcnew Action<button^>( this, &edit_object_mesh::remove_collision_primitive_clicked ) )->tag = prim;
		prim->m_current_desc->add_button( "duplicate", "", gcnew Action<button^>( this, &edit_object_mesh::duplicate_collision_primitive_clicked ) )->tag = prim;
		
		prim->m_current_desc->selection_changed		+= gcnew System::Action<System::Boolean>(prim, &collision_primitive_item::set_selected);
	}

	editor_base::object_properties::initialize_property_container( m_physics_settings, result );
	result->properties["Rigid Body Settings/Mass center"]->selection_changed += 
		gcnew System::Action<System::Boolean>(m_mass_center_item, &mass_center_item::set_selected);

	return result;
}

} // namespace model_editor
} // namespace xray

