////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_composite_visual.h"

using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::control_containers;

namespace xray{
namespace model_editor{

ref class library_name_ui_type_editor : public editor::wpf_controls::property_editors::i_external_property_editor
{
public:
	virtual	void			run_editor	( editor::wpf_controls::property_editors::property^ prop );
}; // ref class library_name_ui_type_editor

void library_name_ui_type_editor::run_editor( xray::editor::wpf_controls::property_editors::property^ prop )
{
	editor::wpf_controls::property_container^ container = safe_cast<editor::wpf_controls::property_container^>(prop->property_owners[0]);
	composite_visual_item^ o = safe_cast<composite_visual_item^>(container->owner);
	
	System::String^ selected	= nullptr;
	if( editor_base::resource_chooser::choose( "solid_visual", 
												o->model_name, 
												nullptr, 
												selected,
												true)
		)
	{
		o->model_name	= selected;
	}
}


editor::wpf_controls::property_container^ edit_object_composite_visual::get_contents_property_container( )
{
	property_container^	result		= gcnew property_container;

	control_container^ container	= result->add_uniform_container( );

	container->add_button			( "Add visual", gcnew Action<button^>(this, &edit_object_composite_visual::add_visual_clicked) );

	int i=0;
	for each ( composite_visual_item^ item in m_contents )
	{
		property_container^	sub				= gcnew property_container;
		editor_base::object_properties::initialize_property_container( item, sub );
		
		String^	name						= String::Format("{0}", i++);
		property_descriptor^ desc			= result->properties->add_container	( name, "all", "no description", sub );
		sub->inner_value					= gcnew property_descriptor( name, item, "model_name" );
		sub->inner_value->set_external_editor_style( library_name_ui_type_editor::typeid );

		desc->add_button					( " õ ", "delete", gcnew Action<button^>( this, &edit_object_composite_visual::remove_item_clicked ) )->tag = item;
			
		desc->selection_changed		+= gcnew System::Action<System::Boolean>(item, &composite_visual_item::set_selected);
	}
	return result;
}

}
}

