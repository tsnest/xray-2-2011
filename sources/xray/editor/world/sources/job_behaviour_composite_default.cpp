////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_composite_default.h"
#include "object_composite.h"
#include "object_scene.h"
#include "project_items.h"
#include "object_job.h"
#include "object_values_storage.h"
#include "project.h"

using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {

job_behaviour_composite_default::job_behaviour_composite_default( object_job^ parent_job )
:job_behaviour_base(parent_job)
{
	m_value_storage_properties_list = gcnew value_storage_properties_list();
}

void job_behaviour_composite_default::initialize( )
{
	object_composite^ selected_resource = safe_cast< object_composite^ >( parent_job->selected_job_resource->get_object() );
	for each( composite_object_item^ item in selected_resource->m_objects )
	{
		if ( item->m_object->GetType() == object_values_storage::typeid )
		{
			object_values_storage^ parent_obj = safe_cast< object_values_storage^ >(  item->m_object );
			value_storage_property_value^ prop_value = gcnew value_storage_property_value( parent_obj );
			prop_value->set_value( parent_obj->selected_value );
			m_value_storage_properties_list->Add(prop_value);
		}
	}
}

void job_behaviour_composite_default::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);


	configs::lua_config_iterator it		= config["values"].begin();
	configs::lua_config_iterator it_e	= config["values"].end();
	for ( ;it != it_e; ++it )
	{
		value_storage_property_value^ prop_value = gcnew value_storage_property_value( nullptr );
		prop_value->set_value( gcnew System::String( *it ) );
		m_value_storage_properties_list->Add(prop_value);
		prop_value->parent_name = gcnew System::String( it.key() );
		parent_job->get_project()->query_project_item( prop_value->parent_name, gcnew queried_object_loaded( prop_value, &value_storage_property_value::on_parent_loaded ) );
	}

}
void job_behaviour_composite_default::save			( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	configs::lua_config_value values_list_config = config["values"];

	for each( value_storage_property_value^ val in m_value_storage_properties_list )
	{
		values_list_config[ unmanaged_string( val->parent->get_full_name( ) ).c_str( ) ] = unmanaged_string( val->get_value( )->ToString( ) ).c_str( );
	}
}

void job_behaviour_composite_default::get_properties	( wpf_property_container^ to_container )
{
	for each( value_storage_property_value^ prop_value in m_value_storage_properties_list )
	{
		property_descriptor^ descr = gcnew property_descriptor(prop_value->parent_name, "", "", prop_value->get_value(), prop_value);
		descr->dynamic_attributes->add( gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( "strings", gcnew System::Func<System::String^, System::Collections::Generic::IEnumerable<System::String^>^ >( prop_value, &value_storage_property_value::get_values_list ) ) );
		to_container->properties->add( descr );
	}

	job_behaviour_base::get_properties( to_container );
}

System::Collections::Generic::List<System::String^>^  job_behaviour_composite_default::get_available_events_list( )
{
	System::Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list( );

	if (  parent_job->selected_job_resource == nullptr )
		return ret_lst;

	object_composite^ selected_resource = safe_cast< object_composite^ >( parent_job->selected_job_resource->get_object() );

	if ( selected_resource != nullptr && selected_resource->logic_scene != nullptr )
	{
		ret_lst->AddRange( selected_resource->logic_scene->get_event_names_list( ) )	;
	}

	return ret_lst;
}


void			value_storage_property_value::set_value		( System::Object^ value )
{
	m_value = value->ToString();
}
System::Object^ value_storage_property_value::get_value		()
{
	return m_value;
}

System::Type^	value_storage_property_value::value_type::get()
{
	return System::String::typeid;
}

void value_storage_property_value::on_parent_loaded( project_item_base^ parent )
{
	m_parent = safe_cast< object_values_storage^ >( parent->get_object( ) );
}

System::Collections::Generic::IEnumerable<System::String^>^ value_storage_property_value::get_values_list( System::String^ type )
{
	if ( m_parent == nullptr )
		return gcnew System::Collections::Generic::List<System::String^>( );

	return m_parent->get_values_by_type( type );
}


} // namespace editor
} // namespace xray