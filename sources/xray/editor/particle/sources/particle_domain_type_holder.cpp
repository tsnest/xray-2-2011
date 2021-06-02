////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_domain_type_holder.h"

namespace xray{
namespace particle_editor{

particle_domain_type_holder::particle_domain_type_holder()
{



}	

void particle_domain_type_holder::set_value( System::Object^ value )
{
// 	System::String^ string_value = ( System::String^ )value;
// 
// 	m_parent_node->
// 
// 
// 
// 	String^ str_value = safe_cast<String^>(value);
// 	if (m_selected_value == str_value)
// 		return;
// 
// 	if (m_parent_node->is_on_graph)
// 		m_parent_node->parent_editor->properties_panel->show_properties(nullptr);
// 
// 	if (m_selected_value != nullptr){
// 		configs::lua_config_iterator list_begin = (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].begin();
// 		configs::lua_config_iterator list_end	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].end();
// 
// 		for(;list_begin != list_end;++list_begin){
// 			m_parent_node->properties->container->properties->remove(m_prop_category+ "/" + gcnew String(gcnew String(list_begin.key()) ));
// 		}
// 	}
// 
// 	m_selected_value = str_value;
// 	(*m_properties_config)["selected_value"] = (pcstr)unmanaged_string(m_selected_value).c_str();
// 
// 	configs::lua_config_iterator list_begin	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].begin();
// 	configs::lua_config_iterator list_end	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].end();
// 
// 	for(;list_begin != list_end;++list_begin){
// 		if((*list_begin).get_type() == configs::t_string){
// 			//(*m_list_config)[(pcstr)(list_begin.key())] = (pcstr)(*list_begin);
// 			m_parent_node->properties->add_string(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,gcnew String(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
// 				false,
// 				m_property_path + str_value + "/");
// 		}
// 		else if((*list_begin).get_type() == configs::t_float){
// 			//(*m_list_config)[(pcstr)(list_begin.key())] = (float)(*list_begin);
// 			m_parent_node->properties->add_float(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(float)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
// 				false,
// 				m_property_path + str_value + "/");
// 		}
// 		else if((*list_begin).get_type() == configs::t_boolean){
// 			//(*m_list_config)[(pcstr)(list_begin.key())] = (bool)(*list_begin);
// 			m_parent_node->properties->add_bool(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(bool)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
// 				false,
// 				m_property_path + str_value + "/");
// 		}
// 		else if((*list_begin).get_type() == configs::t_integer){
// 			//(*m_list_config)[(pcstr)(list_begin.key())] = (int)(*list_begin);
// 			m_parent_node->properties->add_integer(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(int)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
// 				false,
// 				m_property_path + str_value + "/");
// 		}
// 		else if((*list_begin).get_type() == configs::t_float3){
// 			//(*m_list_config)[(pcstr)(list_begin.key())] = (int)(*list_begin);
// 			m_parent_node->properties->add_float3(m_container, gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(float3)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
// 				false,
// 				m_property_path + str_value + "/");
// 		}
// 	}
// 	if (m_parent_node->is_on_graph){
// 		(*m_changes_config_value).clear();
// 		(*m_changes_config_value)["selected_value"] = unmanaged_string(m_selected_value).c_str();
// 		(*m_changes_config_value)[unmanaged_string(m_selected_value).c_str()].assign_lua_value((*m_properties_config)[unmanaged_string(m_selected_value).c_str()]);
// 		m_parent_node->parent_document->particle_system_property_changed(m_parent_node, *m_changes_config);
// 	}
// 
// 	if (m_parent_node->is_on_graph)
// 		m_parent_node->parent_editor->properties_panel->show_properties(m_parent_node->properties->container);
// 
// 	m_parent_node->parent_editor->reset_transform();
}
	
} //namespace particle_editor
} //namespace xray
