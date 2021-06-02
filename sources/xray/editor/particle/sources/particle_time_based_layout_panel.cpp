////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_time_based_layout_panel.h"
#include "particle_graph_node.h"
#include "particle_graph_node_collection.h"
#include "particle_graph_node_emitter.h"
#include "particle_time_parameters_holder.h"

using namespace System::Collections;
using xray::editor::wpf_controls::time_layout_item;

namespace xray{
namespace particle_editor{

void particle_time_based_layout_panel::in_constructor(){
	m_owner_node = nullptr;
	HideOnClose = true;
}
void particle_time_based_layout_panel::show_node(particle_graph_node^ node){

	if (node->is_action_node()){
		node = node->parent_node;
	}
	else if (node->is_root_node() || !node->is_emitter_node()){
		m_time_layout_control->m_time_layout->Visibility = System::Windows::Visibility::Hidden;
		return;
	}
	m_time_layout_control->m_time_layout->Visibility = System::Windows::Visibility::Visible;
	Generic::List<time_layout_item^>^ time_layout_items = gcnew Generic::List<time_layout_item^>();
	for each (String^ n_id in node->children){
		particle_graph_node^ n = node->children->nodes_dictionary[n_id];
		if (n->is_emitter_node() || n->is_action_node()){
			particle_time_parameters_holder^ time_holder = n->properties->time_parameteters_holder;
			time_layout_item^ item  = gcnew time_layout_item(m_time_layout_control->m_time_layout, 
					gcnew Action(n, &particle_graph_node::show_properties),n->node_text,
					gcnew Action<float>(time_holder,&particle_time_parameters_holder::set_start_time_impl),
					gcnew Func<float>(time_holder,&particle_time_parameters_holder::get_start_time_impl),
					gcnew Action<float>(time_holder,&particle_time_parameters_holder::set_length_time_impl),
					gcnew Func<float>(time_holder,&particle_time_parameters_holder::get_length_time_impl));
			time_layout_items->Add(item);	
		}
	}
	
	m_time_layout_control->set_new_time_layout_params(node->node_text,
		gcnew Action<float>(node->properties->time_parameteters_holder,&particle_time_parameters_holder::set_length_time_impl),
		gcnew Func<float>(node->properties->time_parameteters_holder,&particle_time_parameters_holder::get_length_time_impl),
		time_layout_items
		);

	m_time_layout_control->invalidate_child();
	
	Refresh();
	
}

void particle_time_based_layout_panel::clear_panel(){
	m_owner_node = nullptr;
	m_time_layout_control->m_time_layout->Visibility = System::Windows::Visibility::Hidden;
}



}//namespace particle_editor
}//namespace xray