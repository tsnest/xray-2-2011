////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_value_editable_collection.h"
#include "properties_holder.h"

using namespace System::Windows::Forms;

namespace xray {
	namespace editor_base {
		property_value_editable_collection::property_value_editable_collection( properties_holder^ holder,
			xray::editor::wpf_controls::property_container_base^ container,
			configs::lua_config_value const& object_properties_config,
			configs::lua_config_value const& properties_config,
			configs::lua_config_value const& defaults_config,
			System::String^ property_path)
		{
			m_container					= container;
			m_properties_holder			= holder;
			m_node_properties_config	= CRT_NEW(configs::lua_config_value)(object_properties_config);
			m_properties_config			= CRT_NEW(configs::lua_config_value)(properties_config);
			m_defaults_config			= CRT_NEW(configs::lua_config_value)(defaults_config);
			m_property_path				= property_path;
			
			int i = 0;			
			System::String^ item_name_template = dynamic_cast<editor::wpf_controls::property_container_collection^>(m_container)->item_name_template;
			while ((*m_node_properties_config).value_exists(unmanaged_string(System::String::Format(item_name_template, i)).c_str()))
			{	
				m_properties_holder->fill_properties_table( m_container, *m_node_properties_config, *m_properties_config, *m_defaults_config, m_property_path);
				i++;
			}
			//if ( i > 0 ){
			//	m_particle_node->notify_render_on_property_changed(gcnew cli::array<String^>(1){property_path});
			//}
		}

		bool property_value_editable_collection::add_property(){ 
			m_properties_holder->fill_properties_table( m_container, *m_node_properties_config, *m_properties_config, *m_defaults_config, m_property_path);

			on_property_value_changed( m_property_path, *m_node_properties_config);
// 			m_particle_node->notify_render_on_property_changed(gcnew cli::array<String^>(1){m_property_path});
// 
// 			m_particle_node->parent_editor->reset_transform();

			return true;
		}
		bool property_value_editable_collection::remove_property(Object^ property_id){
			int count = safe_cast<editor::wpf_controls::property_container_collection^>(m_container)->properties->Count;
			
			safe_cast<editor::wpf_controls::property_container_collection^>(m_container)->properties->remove((System::Int32)property_id);
			
			count = safe_cast<editor::wpf_controls::property_container_collection^>(m_container)->properties->Count;
			
			System::String^ item_name_template = dynamic_cast<editor::wpf_controls::property_container_collection^>(m_container)->item_name_template;
			
			for(int i = (System::Int32)property_id; i<=count-1;i++)
			{
				System::String^ string_field_id			= System::String::Format(item_name_template, i );
				System::String^ string_next_field_id	= System::String::Format(item_name_template, i + 1);
				(*m_node_properties_config)[unmanaged_string(string_field_id).c_str()].assign_lua_value((*m_node_properties_config)[unmanaged_string(string_next_field_id).c_str()]);
			}		
			System::String^ string_last_field_id	= System::String::Format(item_name_template, count);
			(*m_node_properties_config).erase(unmanaged_string(string_last_field_id).c_str());

			on_property_value_changed( m_property_path, *m_node_properties_config);

		//	m_particle_node->notify_render_on_property_changed(gcnew cli::array<String^>(1){m_property_path});
			
		//	m_particle_node->parent_editor->reset_transform();

			return true;
		}

		bool property_value_editable_collection::move_property	( System::Int32 property_id, System::String^ order )
		{
			editor::wpf_controls::property_container_collection::property_collection^ prop_collection = safe_cast<editor::wpf_controls::property_container_collection^>(m_container)->properties;
			editor::wpf_controls::property_descriptor^ descr = prop_collection[property_id];
			
			System::String^ item_name_template = dynamic_cast<editor::wpf_controls::property_container_collection^>(m_container)->item_name_template;

			System::Int32 prop_1 = property_id;
			System::Int32 prop_2;

			if (order == "up" && property_id != 0)
			{
				prop_collection->Remove(descr);
				prop_collection->Insert(property_id - 1, descr);
				prop_2 = property_id - 1;
			}
			else if(order == "down" && property_id != prop_collection->Count - 1)
			{
				prop_collection->Remove(descr);
				prop_collection->Insert(property_id + 1, descr);
				prop_2 = property_id + 1;
			}
			else
				return false;
			
			configs::lua_config_value lua_prop_value = (*m_node_properties_config)[unmanaged_string(System::String::Format(item_name_template, prop_1 )).c_str()];
			(*m_node_properties_config)[unmanaged_string(System::String::Format(item_name_template, prop_1 )).c_str()].assign_lua_value((*m_node_properties_config)[unmanaged_string(System::String::Format(item_name_template, prop_2 )).c_str()]);
			(*m_node_properties_config)[unmanaged_string(System::String::Format(item_name_template, prop_2 )).c_str()].assign_lua_value(lua_prop_value);

//			m_particle_node->notify_render_on_property_changed(gcnew cli::array<String^>(1){m_property_path});

//			m_particle_node->parent_editor->reset_transform();

			on_property_value_changed( m_property_path, *m_node_properties_config);

			return true;
		}


	} // namespace editor_base
} // namespace xray