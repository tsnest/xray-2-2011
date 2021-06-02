////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_base.h"

using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::control_containers;
using namespace System;

typedef xray::editor::wpf_controls::control_containers::button button;

namespace xray {
namespace model_editor {

ref class material_instance_ui_type_editor : public xray::editor::wpf_controls::property_editors::i_external_property_editor
{
	typedef xray::editor::wpf_controls::property_descriptor property_descriptor;
public:
	virtual	void run_editor( xray::editor::wpf_controls::property_editors::property^ prop );
}; // ref class material_instance_ui_type_editor

void material_instance_ui_type_editor::run_editor( property_editors::property^ prop )
{
	System::String^ current = nullptr;
	System::String^ filter = nullptr;
	System::String^ selected = nullptr;
	
	if(prop->value)
		current = safe_cast<System::String^>(prop->value);

	property_descriptor^ pd		= safe_cast<property_descriptor^>(prop->descriptor);
	edit_surface^ s				= safe_cast<edit_surface^>(pd->tag);

	if(s->m_base_texture_name && s->m_base_texture_name->Length>0)
		xray::editor_base::resource_chooser::set_shared_context("diffuse_texture", s->m_base_texture_name);
	
	if( xray::editor_base::resource_chooser::choose( "material_instance", current, filter, selected, false ) )
	{
		prop->value = selected;
	}
};


using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;

property_container^ edit_object_mesh::get_surfaces_property_container( )
{
	property_container^	result = gcnew property_container;
	u32 tri_count=0;
	u32 v_count=0;
	u32 i_count=0;

	for each ( edit_surface^ s in m_surfaces.Values )
	{
		tri_count	+= s->Triangles;
		v_count		+= s->Vertices;
		i_count		+= s->Indices;

		property_container^	sub		= gcnew property_container;
		sub->inner_value			= gcnew property_descriptor( "Material", s, "material_name" );
		sub->inner_value->tag		= s;
		sub->inner_value->set_external_editor_style( material_instance_ui_type_editor::typeid, true );

		property_descriptor^ desc;

		desc						= gcnew property_descriptor( "Base texture", s, "m_base_texture_name" );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add		( desc );
		
		desc						= gcnew property_descriptor( "Triangles", s, "Triangles" );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add		( desc );

		desc						= gcnew property_descriptor( "Vertices", s, "Vertices" );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add		( desc );
		
		desc						= gcnew property_descriptor( "Indices", s, "Indices" );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add		( desc );

		desc						= result->properties->add_container( s->name, "all surfaces", "no description", sub );	
		desc->selection_changed		+= gcnew System::Action<System::Boolean>(s, &edit_surface::on_selected);
	}
	property_container^	total = gcnew property_container;

	property_descriptor^ desc = gcnew property_descriptor( 
									"Triangles", 
									gcnew object_property_value<int>(tri_count)
									);
	desc->is_read_only			= true;
	total->properties->add		( desc );

	desc = gcnew property_descriptor( 
									"Vertices", 
									gcnew object_property_value<int>(v_count)
									);
	desc->is_read_only			= true;
	total->properties->add		( desc );
	
	desc = gcnew property_descriptor( 
									"Indices", 
									gcnew object_property_value<int>(i_count)
									);
	desc->is_read_only			= true;
	total->properties->add		( desc );


	desc = result->properties->add_container( "Total:", "Summary", "no description", total );	


	return result;
}

property_container^ edit_object_mesh::get_portals_edit_property_container( )
{
	property_container^	result = gcnew property_container;

	control_container^ container	= result->add_dock_container( false );
	container->category				= "General";
	button^ button_instance = container->add_button( "Switch to edit", gcnew Action<button^>( this, &edit_object_mesh::switch_to_portals_clicked ) );
	button_instance->width	= 80;
	
	button_instance = container->add_button( "Make coplanar", gcnew Action<button^>( this, &edit_object_mesh::make_portals_coplanar_clicked ) );
	button_instance->width	= 80;

	button_instance = container->add_button( "New portal", gcnew Action<button^>( this, &edit_object_mesh::add_portal_clicked ) );
	button_instance->width	= 80;

	button_instance = container->add_button( "Delete portal", gcnew Action<button^>( this, &edit_object_mesh::delete_selected_portals_clicked ) );
	button_instance->width	= 80;

	for each ( edit_portal^ portal in m_portals )
	{
		portal->set_selected( false );
		property_container^	sub		= gcnew property_container;
		property_descriptor^ desc	= 
			gcnew property_descriptor( "Name", gcnew property_property_value( portal, "Name" ) );
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add		( desc );
		desc = result->properties->add_container( portal->Name, "all portals", "no description", sub );
		desc->selection_changed += gcnew System::Action<System::Boolean>(portal, &edit_portal::set_selected);
	}
	return result;
}

xray::editor::wpf_controls::property_container^	edit_object_mesh::get_portals_generation_property_container( )
{
	property_container^	result = gcnew property_container;

	control_container^ container	= result->add_dock_container( false );
	container->category				= "General";
	button^ button_instance			= nullptr;

	button_instance = container->add_button( "Build sectors", gcnew Action<button^>( this, &edit_object_mesh::generate_sectors_clicked ) );
	button_instance->width	= 80;

	button_instance = container->add_button( "Generate portals", gcnew Action<button^>( this, &edit_object_mesh::generate_portals_clicked ) );
	button_instance->width	= 80;

	button_instance = container->add_button( "Build BSP-tree", gcnew Action<button^>( this, &edit_object_mesh::generate_bsp_tree_clicked ) );
	button_instance->width	= 80;

	return result;
}


} // namespace model_editor
} // namespace xray
