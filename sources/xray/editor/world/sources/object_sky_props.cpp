////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_sky.h"
#include "editor_world.h"

using namespace xray::editor::wpf_controls;
typedef xray::editor::wpf_controls::select_behavior select_behavior;

using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;
using xray::editor::wpf_controls::property_editors::attributes::external_editor_event_handler;

namespace xray {
namespace editor {


public ref class material_instance_ui_type_editor : public property_editors::i_external_property_editor
{
	typedef xray::editor::wpf_controls::property_descriptor property_descriptor;

public:
	virtual	void run_editor( property_editors::property^ prop )
	{
		System::String^ current = nullptr;
		System::String^ filter = nullptr;
		System::String^ selected = nullptr;
		
		if(prop->value)
			current = safe_cast<System::String^>(prop->value);
		
		if( xray::editor_base::resource_chooser::choose( "material_instance", current, filter, selected, false ) )
		{
			prop->value = selected;
		}
	};
}; // ref class material_instance_ui_type_editor

wpf_controls::property_container^ object_sky::get_property_container( )
{
	wpf_controls::property_container^ container = super::get_property_container();

	container->properties->remove( "general/position" );
	container->properties->remove( "general/rotation" );
	container->properties->remove( "general/scale" );
	container->properties->remove( "general/library name" );
	
	container->properties["general/sky material"]->dynamic_attributes->add( gcnew external_editor_attribute( material_instance_ui_type_editor::typeid ) );
	
	return container;
}

} //namespace editor
} //namespace xray