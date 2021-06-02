////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "add_delete_group_nodes.h"
#include "particle_hypergraph.h"
#include "particle_graph_node.h"
#include "particle_graph_node_action.h"
#include "particle_graph_node_emitter.h"
#include "particle_graph_node_event.h"
#include "particle_graph_node_property.h"
#include "particle_graph_node_collection.h"

using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::link;


//-------------------------------------------------------------------------------------------
//- class command_add_group -----------------------------------------------------------------
//-------------------------------------------------------------------------------------------
namespace xray {
namespace editor {
namespace particle_graph_commands{


//-------------------------------------------------------------------------------------------
//- static functions	    -----------------------------------------------------------------
//-------------------------------------------------------------------------------------------

static void get_children(particle_graph_node^ node, System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ >^ nods, System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ >^ p_nods){

	if(node->children->Count == 0){
		nods->Add(node);
		if (p_nods != nullptr)
			p_nods->Add(node->parent_node);
		return;
	}
	for each(particle_graph_node^ nod in node->children){
		get_children(nod, nods, p_nods);
	}
	nods->Add(node);
	if (p_nods != nullptr)
		p_nods->Add(node->parent_node);	
}

static Boolean is_parent(particle_graph_node^ self, particle_graph_node^ parent)
{
	if (self == parent)
		return false;

	while(self != parent && self != nullptr)
	{
		self = self->parent_node;
	}
	return self == parent;
}

static int compare_node_indexes(node^ n1 , node^ n2){
	particle_graph_node^ p_n1	= safe_cast<particle_graph_node^>(n1);
	particle_graph_node^ p_n2	= safe_cast<particle_graph_node^>(n2);
	int p_n1_id					= p_n1->parent_node->children->IndexOf(p_n1);
	int p_n2_id					= p_n2->parent_node->children->IndexOf(p_n2);

	if (p_n1_id <= p_n2_id)
		return 1;
	else
		return -1;
}

add_group::add_group(particle_hypergraph^ h, configs::lua_config_ptr config, particle_graph_node^ selected_node)
:m_hypergraph(h)
{
	m_all_nodes				= gcnew nodes();
	m_all_nodes_parents		= gcnew nodes();

	
	configs::lua_config_value::iterator begin	= (*config)["root"].begin();
	configs::lua_config_value::iterator end		= (*config)["root"].end();
	
	nodes^ added_nodes = gcnew nodes();
	
	m_selected_node = selected_node;
	if (!m_selected_node->enabled()){
		MessageBox::Show("You can not paste into '"+m_selected_node->properties->type+"' node which is disabled!", 
			"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
		return;
	}

	
	String^ not_added_nodes = "";

	while(begin != end)
	{
		String^ node_type = gcnew String((*begin)["type"]);
		String^ node_name = gcnew String((*begin)["name"]);

		particle_editor^ p_editor = m_selected_node->parent_editor;

		particle_graph_node^ new_node	= nullptr;
		if (node_type == "action")
			new_node = gcnew particle_graph_node_action(xray::editor_base::image_node_emitter, p_editor);
		if (node_type == "emitter")
			new_node = gcnew particle_graph_node_emitter(xray::editor_base::image_node_emitter, p_editor);
		if (node_type == "event")
			new_node = gcnew particle_graph_node_event(xray::editor_base::image_node_emitter, p_editor);
		if (node_type == "property")
			new_node = gcnew particle_graph_node_property(xray::editor_base::image_node_emitter, p_editor);
 		new_node->load											(*begin);		
		if ((m_selected_node->can_accept_node(new_node)) && !new_node->is_root_node()){	
			
			new_node->style											= m_selected_node->style;
			m_selected_node->children->Add(new_node);
			added_nodes->Add(new_node);
		}
		else
		{
			delete(new_node);
			not_added_nodes = not_added_nodes + node_name + ", ";
			
		}
		++begin;
	}
	if (not_added_nodes != "")
		MessageBox::Show("You can not paste '"+not_added_nodes->Substring(0,not_added_nodes->Length-2)+"'  into '"+m_selected_node->properties->type+"' node!", 
		"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
	
	for each(node^ added_node in added_nodes){
		get_children(safe_cast<particle_graph_node^>(added_node), m_all_nodes, m_all_nodes_parents);
	}	
	m_first_commit = true;
}

bool add_group::commit()
{
	if (m_all_nodes->Count == 0)
		return false;

	if (m_first_commit){
		m_first_commit = false;
		return true;
	}
	
	int i = 0;
	for each(node^ n in m_all_nodes){
		particle_graph_node^ p_node = safe_cast<particle_graph_node^>(n);
		m_hypergraph->append_node(p_node);	
			safe_cast<particle_graph_node^>(m_all_nodes_parents[i])->add_children(p_node);
		i++;
	}
	
	return true;
}

void add_group::rollback()
{
	int i = m_all_nodes->Count-1;
	while (i>=0) {
		particle_graph_node^ p_node = safe_cast<particle_graph_node^>(m_all_nodes[i]);	
		safe_cast<particle_graph_node^>(m_all_nodes_parents[i])->remove_children(p_node);
		p_node->parent_hypergraph->Controls->Remove(m_all_nodes[i]);
		p_node->parent_hypergraph->all_nodes()->Remove(m_all_nodes[i]);
		i--;
	}

	m_hypergraph->Invalidate(false);
}
//-------------------------------------------------------------------------------------------
//- class command_remove_group --------------------------------------------------------------
//-------------------------------------------------------------------------------------------



remove_group::remove_group(particle_hypergraph^ h, nodes^ nds)
:m_hypergraph(h)
{
	nds->Sort(gcnew Comparison<node^>(compare_node_indexes));

	m_nodes						= gcnew nodes();
	m_parent_nodes				= gcnew nodes();
	m_selected_nodes_indexes	= gcnew indexes();
	m_selected_nodes			= gcnew nodes();

	for each(node^ nod in nds){
		bool has_parent = false;
		for each(node^ chk_node in nds){
			if (is_parent(safe_cast<particle_graph_node^>(nod), safe_cast<particle_graph_node^>(chk_node)))
				has_parent = true;
		}
		if (has_parent)
			continue;

		particle_graph_node^ p_node = safe_cast<particle_graph_node^>(nod);
		m_selected_nodes_indexes->Add(p_node->parent_node->children->IndexOf(p_node));
		m_selected_nodes->Add(nod);

		get_children(p_node, m_nodes, m_parent_nodes);
	}
	m_nodes->Reverse();
	m_parent_nodes->Reverse();
}



bool remove_group::commit()
{
	for each(node^ n in m_nodes){
		particle_graph_node^ p_node = safe_cast<particle_graph_node^>(n);	
		p_node->parent_node->remove_children(p_node);
		p_node->parent_hypergraph->Controls->Remove(n);
		p_node->parent_hypergraph->all_nodes()->Remove(n);
	}

	m_hypergraph->Invalidate(false);
	return true;
}

void remove_group::rollback()
{
	int i = 0;
	for each(node^ n in m_nodes){
		particle_graph_node^ p_node = safe_cast<particle_graph_node^>(n);
		m_hypergraph->append_node(p_node);	
		if (m_selected_nodes->Contains(p_node))
			safe_cast<particle_graph_node^>(m_parent_nodes[i])->children->Insert(m_selected_nodes_indexes[m_selected_nodes->IndexOf(p_node)],p_node);			
		else
			safe_cast<particle_graph_node^>(m_parent_nodes[i])->children->Insert(0,p_node);
		i++;
	}

	m_hypergraph->Invalidate(false);
}
		};// namespace particle_graph_commands
	} // namespace editor
} // namespace xray