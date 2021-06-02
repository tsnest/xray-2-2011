////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_node_property_holders.h"
#include "particle_graph_node.h"
#include "particle_graph_document.h"
#include "property_container.h"
#include "particle_data_source_nodes.h"

namespace xray {
namespace editor {

//
// class particle_node_time_parameters_holder
//
void particle_node_time_parameters_holder::set_start_time_impl(float value){
	float time_limit = 600.f;
	if (m_owner->is_root_node()|| m_owner->is_particle_group_node() || m_owner->parent_node == nullptr){
		(*m_config)["Delay"] = value;
		on_property_changed(m_owner);
		return;
	}
	else
		time_limit = 600.f;//m_owner->parent_node->properties->length_time;
	int v = (int)value;
	math::clamp(v, 0 , (int)time_limit - (int)length_time);
	value = (float)v;
	if (start_time == value)
		return;
	(*m_config)["Delay"] = value;
	m_length_time_pos = start_time + length_time;
	if (m_length_time_pos > time_limit){
		m_length_time_pos = time_limit;
		(*m_config)["Delay"] = m_length_time_pos - length_time;
	}	
	on_property_changed(m_owner);
}

float particle_node_time_parameters_holder::get_start_time_impl(){
	//m_config->save("d:/lua.tst");
	return (float)(Single)(*m_config)["Delay"];
}

void particle_node_time_parameters_holder::set_length_time_impl(float value){
	float time_limit = 600.f;
	if (m_owner->is_root_node() || m_owner->is_particle_group_node() || m_owner->parent_node == nullptr){
		(*m_config)["LifeTime"] = value;
		on_property_changed(m_owner);
		return;
	}
	else
		time_limit = 600.f;//m_owner->parent_node->properties->length_time;
	
	int v = (int)value;
	math::clamp(v,16,(int)time_limit);
	value = (float)v;
	if (length_time == value)
		return;
	(*m_config)["LifeTime"] = value;
	m_length_time_pos = start_time + length_time;
	if (m_length_time_pos > time_limit){
		m_length_time_pos = time_limit;
		(*m_config)["LifeTime"] = m_length_time_pos - start_time;
	}
		on_property_changed(m_owner);
}

float particle_node_time_parameters_holder::get_length_time_impl(){
	return (float)(Single)(*m_config)["LifeTime"];
}

void particle_node_time_parameters_holder::set_start_length_offsets(float offset){
	float	new_start_value = start_time + offset;
	float old_start_value = start_time;
	int new_s_val = (int)new_start_value;
	math::clamp(new_s_val, 0 , (int)m_length_time_pos - 16);

	new_start_value = (float)new_s_val;

	offset = new_start_value - old_start_value;
	if (offset == 0)
		return;

	(*m_config)["Delay"] = start_time + offset;
	(*m_config)["LifeTime"] = length_time - offset;
		on_property_changed(m_owner);
}
//
// end of class particle_node_time_parameters_holder
//	
	
particle_float_curve_data_source_holder::particle_float_curve_data_source_holder(property_holder_ptr holder,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, float def_value)
:m_default_value(def_value)
{
	m_parent_node			= node;
	m_property_name			= property_name;
	m_property_description	= property_description;
	m_property_category		= property_category;
	m_selected_source		= "Const";
	m_holder				= NEW(property_holder_ptr)(holder);;
	m_config				= NEW(xray::configs::lua_config_value)(config);

}
	
void	particle_float_curve_data_source_holder::set_data_source_impl(String^ name){
	
	
	::property_holder*			real_value = dynamic_cast<::property_holder*>(&**m_holder);
	
	/*m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(nullptr);*/
	if (name == "Const"){
		real_value->remove_property(m_property_name);
			(*m_config)[(pcstr)unmanaged_string(m_property_name).c_str()] = m_last_float_value;
		real_value->add_float		(m_property_name, m_property_category, m_property_description, m_default_value, *m_config, 
			property_holder::property_read_write,
			property_holder::do_not_notify_parent_on_change,
			property_holder::no_password_char,
			property_holder::do_not_refresh_grid_on_change);

	
		m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(m_parent_node->prop_holder->container());
		m_selected_source = name;
		return;
	}
	if (m_selected_source == "Const")
		m_last_float_value = float(real_value->get_value(m_property_name));

	m_selected_source = name;
	real_value->remove_property(m_property_name);
	for each(Control^ c in m_parent_node->parent_editor->data_sources_panel->fc_page->Controls){
		if (c->Text == name){
			particle_data_source_fc_node^ node = safe_cast<particle_data_source_fc_node^>(c);

			dynamic_cast<::property_holder*>(real_value)->add_property(node->fc, m_property_name, m_property_category, m_property_description, property_holder::property_read_write,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);


			
	(*m_config)[(pcstr)unmanaged_string(m_property_name).c_str()] = (pcstr)unmanaged_string(m_selected_source).c_str();
			break;
		}
	}	
	m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(m_parent_node->prop_holder->container());
}	


particle_color_curve_data_source_holder::particle_color_curve_data_source_holder(property_holder_ptr holder,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, u32 def_value)
:m_default_value(def_value)
{
	m_parent_node			= node;
	m_property_name			= property_name;
	m_property_description	= property_description;
	m_property_category		= property_category;
	m_selected_source		= "Const";
	m_holder				= NEW(property_holder_ptr)(holder);;
	m_config				= NEW(xray::configs::lua_config_value)(config);

}

void	particle_color_curve_data_source_holder::set_data_source_impl(String^ name){

	::property_holder*			real_holder = dynamic_cast<::property_holder*>(&**m_holder);

	/*m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(nullptr);*/
	if (name == "Const"){
		real_holder->remove_property(m_property_name);
		(*m_config)[(pcstr)unmanaged_string(m_property_name).c_str()] = m_last_color_value;
		real_holder->add_color		(m_property_name, m_property_category, m_property_description, color(m_default_value), *m_config, 
			property_holder::property_read_write,
			property_holder::do_not_notify_parent_on_change,
			property_holder::no_password_char,
			property_holder::do_not_refresh_grid_on_change);

		
		m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(m_parent_node->prop_holder->container());
		m_selected_source = name;
		return;
	}
	if (m_selected_source == "Const"){
		//Color last_color = Color(m_parent_node->prop_holder->get_value(m_property_name));
		m_last_color_value = Drawing::Color::FromArgb(255,255,255,255).ToArgb();
	}

	m_selected_source = name;
	real_holder->remove_property(m_property_name);
	for each(Control^ c in m_parent_node->parent_editor->data_sources_panel->cc_page->Controls){
		if (c->Text == name){
			particle_data_source_cc_node^ node = safe_cast<particle_data_source_cc_node^>(c);

			dynamic_cast<::property_holder*>(real_holder)->add_property(node->cc, m_property_name, m_property_category, m_property_description, property_holder::property_read_write,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);



			(*m_config)[(pcstr)unmanaged_string(m_property_name).c_str()] = (pcstr)unmanaged_string(m_selected_source).c_str();
			break;
		}
	}	
	m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(m_parent_node->prop_holder->container());
}	
	





void particle_object_list_holder::set_value(String^ value){

	if (m_parent_node->is_on_graph)
		m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(nullptr);

	if (m_selected_value != nullptr){
		configs::lua_config_iterator list_begin = (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].begin();
		configs::lua_config_iterator list_end	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].end();

		for(;list_begin != list_end;++list_begin){
			m_parent_node->prop_holder->remove_property(gcnew String(list_begin.key()));			
		}
	}

	m_selected_value = value;
	(*m_properties_config)["default_value"] = (pcstr)unmanaged_string(m_selected_value).c_str();
	
	configs::lua_config_iterator list_begin	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].begin();
	configs::lua_config_iterator list_end	= (*m_defaults_config)[(pcstr)unmanaged_string(m_selected_value).c_str()].end();

	for(;list_begin != list_end;++list_begin){
		if((*list_begin).get_type() == configs::t_string){
			//(*m_list_config)[(pcstr)(list_begin.key())] = (pcstr)(*list_begin);
			m_parent_node->prop_holder->add_string(gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,gcnew String(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
				property_holder::property_read_write,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		}
		else if((*list_begin).get_type() == configs::t_float){
			//(*m_list_config)[(pcstr)(list_begin.key())] = (float)(*list_begin);
			m_parent_node->prop_holder->add_float(gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(float)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
				property_holder::property_read_write,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		}
		else if((*list_begin).get_type() == configs::t_boolean){
			//(*m_list_config)[(pcstr)(list_begin.key())] = (bool)(*list_begin);
			m_parent_node->prop_holder->add_bool(gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(bool)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
				property_holder::property_read_write,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		}
		else if((*list_begin).get_type() == configs::t_integer){
			//(*m_list_config)[(pcstr)(list_begin.key())] = (int)(*list_begin);
			m_parent_node->prop_holder->add_integer(gcnew String(list_begin.key()),m_prop_category ,m_prop_desc ,(int)(*list_begin), (*m_properties_config)[(pcstr)unmanaged_string(m_selected_value).c_str()],
				property_holder::property_read_write,
				property_holder::do_not_notify_parent_on_change,
				property_holder::no_password_char,
				property_holder::do_not_refresh_grid_on_change);
		}		
	}
	if (m_parent_node->is_on_graph)
		m_parent_node->parent_editor->multidocument_base->properties_panel->show_properties(m_parent_node->prop_holder->container());
}



} // namespace editor
} // namespace xray