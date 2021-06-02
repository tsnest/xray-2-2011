////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////



#include "pch.h"
#include "properties_holder.h"
#include "property_value_lists_dictionary.h"

using namespace System::Linq;
using namespace System::Windows::Forms;

namespace xray {
namespace editor_base {

	void property_value_lists_dictionary::set_value(Object^ value)
	{
		String^ str_value = safe_cast<String^>(value);
		if (m_selected_value == str_value)
			return;

		if (m_selected_value != nullptr){
			configs::lua_config_iterator list_begin = (*m_defaults_config)[unmanaged_string(m_selected_value).c_str()].begin();
			configs::lua_config_iterator list_end	= (*m_defaults_config)[unmanaged_string(m_selected_value).c_str()].end();

			for(;list_begin != list_end;++list_begin){
				if ( dynamic_cast<editor::wpf_controls::property_container^>(m_container) != nullptr )
				dynamic_cast<editor::wpf_controls::property_container^>(m_container)->properties->remove( m_prop_category+ "/" + gcnew String(gcnew String(list_begin.key( ) ) ));
			}
		}
		
		m_selected_value = str_value;
		(*m_properties_config)["selected_value"] = (pcstr)unmanaged_string(m_selected_value).c_str();

		configs::lua_config_iterator list_begin	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].begin();
		configs::lua_config_iterator list_end	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].end();

		for(;list_begin != list_end;++list_begin){
			if((*list_begin).get_type() == configs::t_string){
				m_properties_holder->add_string(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,gcnew String(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
					false,
					m_property_path + str_value + "/");
			}
 			else if((*list_begin).get_type() == configs::t_float){
 				m_properties_holder->add_float(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(float)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
 					false,
 					m_property_path + str_value + "/");
 			}
 			else if((*list_begin).get_type() == configs::t_boolean){
 				m_properties_holder->add_bool(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(bool)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
 					false,
 					m_property_path + str_value + "/");
 			}
 			else if((*list_begin).get_type() == configs::t_integer){
 				m_properties_holder->add_integer(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(int)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
 					false,
 					m_property_path + str_value + "/");
 			}
			else if((*list_begin).get_type() == configs::t_float3){
				m_properties_holder->add_float3(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(float3)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
					false,
					m_property_path + str_value + "/");
			}
		}
		
		if (m_properties_holder->container->properties->count != 0)
		{
			editor::wpf_controls::property_editors::property^ prop = System::Linq::Enumerable::First<editor::wpf_controls::property_descriptor^>( m_properties_holder->container->properties )->owner_property;
			
			if ( prop != nullptr )
			{
				editor::wpf_controls::property_grid::property^ grid_property = dynamic_cast<editor::wpf_controls::property_grid::property^>( prop );
				if ( grid_property->owner_property_grid != nullptr )
					grid_property->owner_property_grid->reset();
			}			
		}


		on_property_value_changed( m_property_path, *m_properties_config );
		
// 		(*m_changes_config_value).clear();
// 		(*m_changes_config_value)["selected_value"] = unmanaged_string(m_selected_value).c_str();
// 		(*m_changes_config_value)[unmanaged_string(m_selected_value).c_str()].assign_lua_value((*m_properties_config)[unmanaged_string(m_selected_value).c_str()]);
		//	m_parent_node->parent_document->particle_system_property_changed(m_parent_node, *m_changes_config);
		

// 		if (m_parent_node->is_on_graph)
// 			m_parent_node->parent_editor->properties_panel->show_properties(m_parent_node->properties->container);
// 
// 		m_parent_node->parent_editor->reset_transform();
	}

} // namespace editor_base
} // namespace xray