////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "add_delete_group_nodes.h"
#include "particle_hypergraph.h"
#include "particle_graph_node.h"
#include "particle_graph_document.h"
#include "particle_graph_node_action.h"
#include "particle_graph_node_emitter.h"
#include "particle_graph_node_event.h"
#include "particle_graph_node_property.h"
#include "particle_graph_node_collection.h"

using namespace xray::editor::controls;

using namespace System::Collections::Generic;


////////////////////////////////////////////////////////////////////////////
//- class command_add_group -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
namespace xray {
namespace particle_editor {
namespace particle_graph_commands{


////////////////////////////////////////////////////////////////////////////
//- static functions	    -----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////

static void get_children(particle_graph_node^ node, List<hypergraph::node^>^ nods, List<hypergraph::node^>^ p_nods){

	if(node->children->Count == 0){
		nods->Add(node);
		if (p_nods != nullptr)
			p_nods->Add(node->parent_node);
		return;
	}
	for each(String^ nod_id in node->children){
		particle_graph_node^ nod = node->children->nodes_dictionary[nod_id];
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

static int compare_node_indexes(hypergraph::node^ n1 , hypergraph::node^ n2){
	particle_graph_node^ p_n1	= safe_cast<particle_graph_node^>(n1);
	particle_graph_node^ p_n2	= safe_cast<particle_graph_node^>(n2);
	int p_n1_id					= p_n1->parent_node->children->IndexOf(p_n1->id);
	int p_n2_id					= p_n2->parent_node->children->IndexOf(p_n2->id);

	if (p_n1_id <= p_n2_id)
		return 1;
	else
		return -1;
}

add_group::add_group(particle_hypergraph^ h, configs::lua_config_ptr config, String^ selected_node_id)
:m_hypergraph(h)
{
	
	m_hypergraph		= h;
	m_config_ptr		= NEW(configs::lua_config_ptr)(config);
	m_selected_node_id	= selected_node_id;
	m_added_nodes		= gcnew System::Collections::Generic::List<String^>();



}

bool add_group::commit()
{
	configs::lua_config_value::iterator begin	= (**m_config_ptr)["root"].begin();
	configs::lua_config_value::iterator end		= (**m_config_ptr)["root"].end();

	particle_graph_node^ selected_node = m_hypergraph->get_node_by_id(m_selected_node_id);
	if (!selected_node->enabled){
		MessageBox::Show("You can not paste into '"+selected_node->properties->type+"' node which is disabled!", 
			"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
		return false;
	}


	String^ not_added_nodes = "";

	while(begin != end)
	{
		configs::lua_config_value node_config = *begin;

		particle_editor^ p_editor = selected_node->parent_editor;

		particle_graph_node^ new_node	= p_editor->deserialize_particle_node_from_config( node_config);
		new_node->style		= selected_node->style;
		
		if ((selected_node->can_accept_node(new_node)) && !new_node->is_root_node()){				
			String^ new_node_id = Guid::NewGuid().ToString();
			
			
			configs::lua_config_value root_config = (*begin).copy();

			new_node->node_config->assign_lua_value(root_config);
			
			new_node->id = new_node_id;
			m_hypergraph->append_node(new_node);

			new_node->clone_from_config				(root_config);
			selected_node->children->Add(new_node);

			new_node->parent_lod_name		= selected_node->parent_document->lod_name;

			
			selected_node->on_new_node_added(new_node, selected_node->children->Count);

			if (!m_added_nodes->Contains(new_node->id))
				m_added_nodes->Add(new_node->id);

			new_node->process_node_selection();	
		}
		else
		{
			not_added_nodes = not_added_nodes + new_node->node_text + ", ";
			m_hypergraph->remove_node(new_node);
			delete(new_node);
		}
		++begin;    
	}
	if (not_added_nodes != "")
		MessageBox::Show("You can not paste '"+not_added_nodes->Substring(0,not_added_nodes->Length-2)+"'  into '"+selected_node->properties->type+"' node!", 
		"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);

	

	return true;
}

void add_group::rollback()
{
	int i = m_added_nodes->Count - 1;
	while(i>=0)
	{
		particle_graph_node^ selected_node = m_hypergraph->get_node_by_id(m_selected_node_id);
		selected_node->on_node_removed(m_hypergraph->get_node_by_id(m_added_nodes[i]));
		m_hypergraph->delete_node(m_added_nodes[i]);
		
		i--;
	}
	m_added_nodes->Clear();
}
////////////////////////////////////////////////////////////////////////////
//- class command_remove_group --------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////



remove_group::remove_group(particle_hypergraph^ h, List<xray::editor::controls::hypergraph::node^ >^ nds)
:m_hypergraph(h)
{

	configs::lua_config_ptr cfg = configs::create_lua_config( );
	m_config = NEW(configs::lua_config_value)((*cfg)["root"]);

	nds->Sort(gcnew Comparison<hypergraph::node^>(compare_node_indexes));
	
	m_nodes_to_delete	= gcnew List<String^>();
	m_parent_ids		= gcnew List<String^>();
	m_node_indexes		= gcnew List<Int32>();

	for each(hypergraph::node^ nod in nds){
		bool has_parent = false;
		for each(hypergraph::node^ chk_node in nds){
			if (is_parent(safe_cast<particle_graph_node^>(nod), safe_cast<particle_graph_node^>(chk_node)))
				has_parent = true;
		}
		if (has_parent)
			continue;

		particle_graph_node^ p_node = safe_cast<particle_graph_node^>(nod);
		m_nodes_to_delete->Add(p_node->id);
		m_parent_ids->Add(p_node->parent_node->id);
		m_node_indexes->Add(p_node->parent_node->children->IndexOf(p_node->id));

	}
	m_nodes_to_delete->Reverse();
	m_parent_ids->Reverse();
	m_node_indexes->Reverse();


	Int32 index = 0;

	for each(String^ node_id in m_nodes_to_delete)
		m_hypergraph->get_node_by_id(node_id)->save((*m_config)[index++]);
	
}



bool remove_group::commit()
{
	
	int i = m_nodes_to_delete->Count - 1;
	while(i>=0)
	{
		particle_graph_node^ node_to_delete = m_hypergraph->get_node_by_id(m_nodes_to_delete[i]);
		node_to_delete->parent_node->on_node_removed(node_to_delete);
		m_hypergraph->delete_node(m_nodes_to_delete[i]);
		i--;
	}
	
	return true;
}

void remove_group::rollback()
{
	
	int index = 0;

	configs::lua_config_value::iterator begin	= (*m_config).begin();
	configs::lua_config_value::iterator end		= (*m_config).end();
	
	while(begin != end)
	{
		
		configs::lua_config_value node_config = *begin;

		//node_config.save_as("d:/node_config");

		particle_graph_node^ parent_node	= m_hypergraph->get_node_by_id(m_parent_ids[index]);
		Int32 node_index					= m_node_indexes[index];
		

		//String^ node_type = gcnew String((node_config)["type"]);
	
		particle_editor^ p_editor = parent_node->parent_editor;

		particle_graph_node^ new_node	= p_editor->deserialize_particle_node_from_config( node_config );
		new_node->style		= parent_node->style;
		m_hypergraph->append_node(new_node);
	
		new_node->load											(node_config/*.copy()*/);

		parent_node->children->Insert(node_index, new_node);

		parent_node->on_new_node_added(new_node, node_index + 1);
		
		index++;

		++begin;    
	}	
}
		};// namespace particle_graph_commands
	} // namespace particle_editor
} // namespace xray