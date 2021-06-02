////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_values_storage.h"
#include "object_values_storage_edit_form.h"
#include "tool_misc.h"

namespace xray {
namespace editor {

using xray::editor::wpf_controls::control_containers::button;

object_values_storage::object_values_storage( tool_misc^ tool )
	:super			( tool )
{
	image_index	= xray::editor_base::misc_tree_icon;
	m_values_list = gcnew System::Collections::Generic::List< System::String^ >();
}

void object_values_storage::load_props( configs::lua_config_value const& t )
{
	super::load_props( t );

	selected_value = gcnew System::String( t["selected_value"] );

	if ( !m_values_table_loaded ){
		configs::lua_config_iterator it		= t["values"].begin();
		configs::lua_config_iterator it_e	= t["values"].end();
		for( ;it != it_e; ++it )
		{
			m_values_list->Add( gcnew System::String( *it ) );
		}
		m_values_table_loaded = true;
	}
	
}

void object_values_storage::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "values_storage";
	
	t["selected_value"] = unmanaged_string( selected_value ).c_str();

	for each( System::String^ value in m_values_list )
	{
		t["values"][ m_values_list->IndexOf( value ) ] = unmanaged_string( value ).c_str( );
	}
	//	t["registry_name"]		= unmanaged_string( m_project_item->get_full_name()).c_str();
}

property_container^ object_values_storage::get_property_container	( )
{
	property_container^ prop_container = super::get_property_container( );
	
	property_descriptor^ prop_descriptor = gcnew property_descriptor( this, "selected_value");
	prop_descriptor->dynamic_attributes->add( gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( "strings", gcnew System::Func<System::String^, System::Collections::Generic::IEnumerable<System::String^>^ >( this, &object_values_storage::get_values_by_type ) ) );
	prop_descriptor->add_button( "...", "Edit values", gcnew Action<button^>( this, &object_values_storage::on_edit_vaues_button_clicked ) )->tag = prop_descriptor;
	prop_container->properties->add( prop_descriptor );

	return prop_container;
}

void object_values_storage::add_value( System::String^ value )
{
	m_values_list->Add( value );
}

void object_values_storage::remove_value	( System::String^ value )
{
	if ( selected_value == value )
		selected_value = "";
	m_values_list->Remove( value );
}

void	object_values_storage::on_edit_vaues_button_clicked( button^ btn )
{
	object_values_storage_edit_form^ frm = gcnew object_values_storage_edit_form( this );
	frm->ShowDialog( );
	property_descriptor^ descr = safe_cast< property_descriptor^ >( btn->tag );
	descr->update();
}

System::Collections::Generic::IEnumerable<System::String^>^	object_values_storage::get_values_by_type		( System::String^ )
{
	return m_values_list;
}

} // namespace editor
} // namespace xray