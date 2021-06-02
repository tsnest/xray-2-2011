////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_time_based_layout_panel.h"
#include "particle_scalable_control.h"
#include "particle_graph_node.h"
#include "particle_graph_node_collection.h"
#include "particle_graph_node_emitter.h"

namespace xray{
namespace editor{

void particle_time_based_layout_panel::in_constructor(){
	m_owner_node = nullptr;
}

void particle_time_based_layout_panel::show_node(particle_graph_node^ node){
	if (node == m_owner_node)
		return;

	clear_panel();

	if (!node->is_emitter_node())
		return;

	m_owner_node = node;
	
	
// 	particle_scalable_control^ ctrl = gcnew particle_scalable_control(node);
// 	ctrl->Location = Drawing::Point( 0 , this->Controls->Count * (ctrl->Height + 5) + 5);
// 	this->Controls->Add(ctrl);
	
	for each (particle_graph_node^ n in node->children){
		if (n->is_emitter_node() || n->is_action_node()){
			particle_scalable_control^ ctrl = gcnew particle_scalable_control(n);
			ctrl->Location = Drawing::Point( 0 , this->Controls->Count * (ctrl->Height + 5) + 5);
			this->Controls->Add(ctrl);	
		}
	}
	Refresh();
	if (m_owner_node->is_emitter_node()){
		particle_graph_node_emitter^ p_node = safe_cast<particle_graph_node_emitter^>(m_owner_node);
		this->HorizontalScroll->Value = p_node->time_layout_props->h_scroll_pos;
		this->HorizontalScroll->Value = p_node->time_layout_props->h_scroll_pos;
		this->VerticalScroll->Value = p_node->time_layout_props->v_scroll_pos;
		this->VerticalScroll->Value = p_node->time_layout_props->v_scroll_pos;
	}
}

void particle_time_based_layout_panel::clear_panel(){
	if (m_owner_node != nullptr && m_owner_node->is_emitter_node()){
		safe_cast<particle_graph_node_emitter^>(m_owner_node)->time_layout_props->h_scroll_pos = HorizontalScroll->Value;
		safe_cast<particle_graph_node_emitter^>(m_owner_node)->time_layout_props->v_scroll_pos = VerticalScroll->Value;
	}
	this->Controls->Clear();
	m_owner_node = nullptr;
}



}//namespace editor 
}//namespace xray