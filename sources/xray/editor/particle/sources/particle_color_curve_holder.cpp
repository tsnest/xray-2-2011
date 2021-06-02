////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node.h"
#include "particle_graph_document.h"
#include "particle_data_source_nodes.h"
#include "particle_system_config_wrapper.h"
#include "particle_color_curve_holder.h"
#include "particle_data_sources_panel.h"

using namespace System::Windows::Forms;
using xray::editor::wpf_controls::color_curve;

namespace xray {
namespace particle_editor {
	
	extern xray::configs::lua_config_value create_changes_config_from_string_path(String^ path, xray::configs::lua_config_value cfg);
	
	particle_color_curve_holder::particle_color_curve_holder(xray::editor::wpf_controls::property_container^ container,configs::lua_config_value const& config, particle_graph_node^ node, String^ property_name,String^ property_description ,String^ property_category, u32 def_value, String^ property_path)
		:m_default_value(def_value)
	{
		m_parent_node			= node;
		m_property_name			= property_name;
		m_property_description	= property_description;
		m_property_category		= property_category;
		m_selected_source		= "Const";
		m_container				= container;
		m_config				= CRT_NEW(xray::configs::lua_config_value)(config);
		m_changes_config		= CRT_NEW(configs::lua_config_value)(configs::create_lua_config()->get_root());
		m_changes_config_value  = CRT_NEW(configs::lua_config_value)(create_changes_config_from_string_path(property_path, *m_changes_config));


	}
	void particle_color_curve_holder::synchronize_config(Object^ sender, EventArgs^ ){

		Int32 i = 0;
		configs::lua_config_ptr				config				= m_parent_node->parent_editor->particle_configs["particles/" + m_parent_node->parent_document->source_path]->c_ptr();
		configs::lua_config_value			data_source_config	= (*config)["data_sources"]["color_curves"][unmanaged_string(m_selected_source).c_str()];
		data_source_config.clear();
		data_source_config.create_table();
		for each(xray::editor::wpf_controls::color_curve_key^ p in ((color_curve^)sender)->keys){
			xray::configs::lua_config_value key = data_source_config[unmanaged_string("key" + i).c_str()];
			key["key"] = p->position;
			key["value"] = p->color.ToArgb();
			i++;
		}	
		(*m_changes_config_value)["keys"].assign_lua_value(data_source_config.copy());
		m_parent_node->parent_document->particle_system_property_changed(m_parent_node, *m_changes_config);


	}

	void	particle_color_curve_holder::set_data_source_impl(String^ name){

// 		if (m_selected_source == name)
// 			return;
// 		
// 		for each(Control^ c in m_parent_node->parent_editor->data_sources_panel->cc_page->Controls){
// 			if (c->Text == m_selected_source){
// 				particle_data_source_cc_node^ node = safe_cast<particle_data_source_cc_node^>(c);
// 				node->cc->edit_completed				-= gcnew EventHandler(this, &particle_color_curve_holder::synchronize_config);
// 			}
// 		}
// 
// 		m_selected_source = name;
// 		(*m_config)["selected_source"] = (pcstr)unmanaged_string(m_selected_source).c_str();
// 
// 		configs::lua_config_value selected_particle_config = m_parent_node->parent_editor->particle_configs["particles/" + m_parent_node->parent_document->source_path]->c_ptr()->get_root();
// 		((*m_config)["keys"]).clear();
// 		if (m_container->properties->contains(m_property_category + "/" + "ColorRamp"))
// 			m_container->properties->remove(m_property_category + "/" + "ColorRamp");
// 		for each(Control^ c in m_parent_node->parent_editor->data_sources_panel->cc_page->Controls){
// 			if (c->Text == name){
// 				particle_data_source_cc_node^ node = safe_cast<particle_data_source_cc_node^>(c);
// 
// 				m_container->properties->add_from_obj(node, "cc");
// 
// 				node->cc->edit_completed				+= gcnew EventHandler(this, &particle_color_curve_holder::synchronize_config);
// 				synchronize_config(node->cc, gcnew EventArgs());
// 				break;
// 			}
// 		}
// 		((*m_config)["keys"]).add_super_table(selected_particle_config, unmanaged_string("data_sources.color_curves." + m_selected_source).c_str());
	//	m_parent_node->parent_editor->properties_panel->show_properties(m_parent_node->properties->holder->c_ptr()->container());
	}


} // namespace particle_editor
} // namespace xray