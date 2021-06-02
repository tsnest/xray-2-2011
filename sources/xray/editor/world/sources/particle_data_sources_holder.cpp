////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_data_sources_holder.h"
#include "particle_graph_document.h"
#include "particle_editor.h"

namespace xray {
namespace editor {

void particle_data_sources_holder::in_constructor(){
	m_float_curves			= gcnew float_curve_data_sources();
	m_color_curves			= gcnew color_curve_data_sources();
	
	m_data_sources_config	= NEW(xray::configs::lua_config_value)((*xray::configs::create_lua_config())["data_sources"]);
	
	(*m_data_sources_config)["float_curves"].create_table();
	(*m_data_sources_config)["color_curves"].create_table();
}

void particle_data_sources_holder::add_sources_to_panel(particle_data_sources_panel^ panel){
	panel->fc_page->Controls->Clear();
	panel->cc_page->Controls->Clear();
	for each(Control^ c in m_float_curves)
	{
		panel->add_new_control(c, panel->fc_page);
	}
	for each(Control^ c in m_color_curves)
	{
		panel->add_new_control(c, panel->cc_page);
	}
}

particle_data_sources_holder::~particle_data_sources_holder()
{
	DELETE(m_data_sources_config);
	for each(particle_data_source_cc_node^ n in m_color_curves)
	{
		delete n;
	}
	for each(particle_data_source_fc_node^ n in m_float_curves)
	{
		delete n;
	}
}

void particle_data_sources_holder::add_data_source(particle_data_source_cc_node^ n){
	m_color_curves->Add(n);
}

void particle_data_sources_holder::add_data_source(particle_data_source_fc_node^ n){
	m_float_curves->Add(n);
}

void particle_data_sources_holder::save(configs::lua_config_value config){
	config.assign_lua_value(*m_data_sources_config);
}

void particle_data_sources_holder::create_fc_data_source(String^ name, configs::lua_config_value config){

	particle_data_source_fc_node^ node = gcnew particle_data_source_fc_node(m_parent_document->parent_editor);
	add_data_source(node);

	node->Text = gcnew String(name);

	float_curve^ fc = gcnew float_curve(config[(pcstr)unmanaged_string(name).c_str()]);
	node->fc = fc;
	node->is_on_graph					= false;

	fc->key_added += gcnew EventHandler<float_curve_event_args^>(node, &particle_data_source_fc_node::curve_changed);
	fc->key_altered += gcnew EventHandler<float_curve_event_args^>(node, &particle_data_source_fc_node::curve_changed);

	m_parent_document->parent_editor->data_sources_panel->add_new_control(node, m_parent_document->parent_editor->data_sources_panel->fc_page);
}
void particle_data_sources_holder::create_cc_data_source(String^ name, configs::lua_config_value config){

	particle_data_source_cc_node^ node = gcnew particle_data_source_cc_node(m_parent_document->parent_editor);
	add_data_source(node);

	node->Text = gcnew String(name);

	color_curve^ cc = gcnew color_curve(config[(pcstr)unmanaged_string(name).c_str()]);
	node->cc = cc;
	node->is_on_graph					= false;

	cc->on_curve_changed += gcnew color_curve_changed(node, &particle_data_source_cc_node::curve_changed);

	m_parent_document->parent_editor->data_sources_panel->add_new_control(node, m_parent_document->parent_editor->data_sources_panel->cc_page);
}

void particle_data_sources_holder::load(configs::lua_config_value config){

	m_data_sources_config->assign_lua_value(config);

	configs::lua_config_value float_curves_config	= (*m_data_sources_config)["float_curves"];
	configs::lua_config_value::iterator begin		= float_curves_config.begin();
	configs::lua_config_value::iterator end			= float_curves_config.end();

	for(;begin != end;++begin){
		create_fc_data_source(gcnew String(begin.key()), float_curves_config);
	}

	configs::lua_config_value color_curves_config	= (*m_data_sources_config)["color_curves"];
	begin		= color_curves_config.begin();
	end			= color_curves_config.end();

	for(;begin != end;++begin){
		create_cc_data_source(gcnew String(begin.key()), color_curves_config);
	}

}

float_curve^ particle_data_sources_holder::get_float_curve_by_name		(String^ name){
	for each(particle_data_source_fc_node^ n in m_float_curves)
	{
		if (n->Text == name)
			return n->fc;
	}
	return nullptr;
}

color_curve^ particle_data_sources_holder::get_color_curve_by_name		(String^ name){
	for each(particle_data_source_cc_node^ n in m_color_curves)
	{
		if (n->Text == name)
			return n->cc;
	}
	return nullptr;
}





} // namespace editor
} // namespace xray