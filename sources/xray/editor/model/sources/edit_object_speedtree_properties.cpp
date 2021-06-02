////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_speedtree.h"

namespace xray {
namespace model_editor {

using namespace xray::editor::wpf_controls;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;

ref class tree_material_ui_type_editor : public xray::editor::wpf_controls::property_editors::i_external_property_editor
{
	typedef xray::editor::wpf_controls::property_descriptor property_descriptor;
public:
	virtual	void run_editor( xray::editor::wpf_controls::property_editors::property^ prop );
};

void tree_material_ui_type_editor::run_editor( property_editors::property^ prop )
{
	System::String^ current = nullptr;
	System::String^ filter = nullptr;
	System::String^ selected = nullptr;
	
	if(prop->value)
		current = safe_cast<System::String^>(prop->value);

	if( xray::editor_base::resource_chooser::choose( "material_instance", current, filter, selected, false ) )
		prop->value = selected;
};


property_container^ edit_object_speedtree::get_surfaces_property_container( )
{
	property_container^	result = gcnew property_container;

	for each ( speedtree_surface^ s in m_surfaces.Values )
	{
		property_descriptor^ desc	= gcnew property_descriptor( s->name, s, "material_name" );

		desc->tag					= s;
		desc->selection_changed		+= gcnew System::Action<System::Boolean>(s, &speedtree_surface::on_selected);
		desc->set_external_editor_style( tree_material_ui_type_editor::typeid, true );
/* 
		property_descriptor^ chk	= gcnew property_descriptor( "Visible", gcnew property_property_value(s, "Visible") );
		desc->inner_properties->Add	( "asd", chk );
*/
		result->properties->add( desc );
	}

	return result;
}


} // namespace model_editor
} // namespace xray

