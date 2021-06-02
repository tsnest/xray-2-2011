////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_environment.h"
#include "editor_world.h"

using namespace xray::editor::wpf_controls;
typedef xray::editor::wpf_controls::select_behavior select_behavior;

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

using xray::editor::wpf_controls::property_editors::attributes::external_editor_attribute;

void object_environment::material_name::set( System::String^ value )
{
	if(m_material_name	!= value)
	{
		m_material_name				= value;
	}
	requery_material				( );
}


wpf_controls::property_container^ object_environment::get_property_container( )
{
	wpf_controls::property_container^ container = super::get_property_container();

	container->properties->remove( "general/position" );
	container->properties->remove( "general/rotation" );
	container->properties->remove( "general/scale" );
	container->properties->remove( "general/library name" );
	
	container->properties["general/post effect"]->dynamic_attributes->add( gcnew external_editor_attribute( material_instance_ui_type_editor::typeid ) );
	
	return container;
}

Float3 object_environment::camera_position::get( )
{
	float3 p,d;
	get_editor_world().view_window()->get_camera_props( p, d );
	return Float3( p );
}

void object_environment::camera_position::set( Float3 value )
{
	float3 p,d;
	get_editor_world().view_window()->get_camera_props( p, d );
	p = value;
	get_editor_world().view_window()->set_camera_props( p, d );
}

Float3 object_environment::camera_direction::get( )
{
	float3 p,d;
	get_editor_world().view_window()->get_camera_props( p, d );
	return Float3( d );
}

void object_environment::camera_direction::set( Float3 value )
{
	float3 p, d, original;
	get_editor_world().view_window()->get_camera_props( p, original );
	
	d = value;
	d.normalize_safe( original );
	get_editor_world().view_window()->set_camera_props( p, d );
}

} //namespace editor
} //namespace xray