////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_hypergraph.h"
#include "particle_graph_document.h"
#include "particle_graph_node.h"
#include "particle_graph_node_collection.h"
#include "add_delete_node.h"
#include "move_node.h"
#include "add_delete_group_nodes.h"
#include "change_node_index.h"
#include "disable_group_nodes.h"


using xray::editor::particle_hypergraph;
using xray::editor::particle_graph_node;
using xray::editor::particle_graph_document;
using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::connection_point;
using System::Windows::Forms::PropertyGrid;



particle_hypergraph::particle_hypergraph(particle_graph_document^ owner)
{
	m_owner						= owner;
	m_command_engine			= gcnew xray::editor_base::command_engine(100);
	m_inner_to_outer_distance	= 10000;
	scalable					= false;
	show_scrollbars				= false;
	m_outer_field_rect			= Drawing::Rectangle(-20000, -20000, 30000, 30000);
}
particle_hypergraph::~particle_hypergraph()
{
	delete m_command_engine;
	if (m_nodes->Count > 0){
		for each(node^ n in m_nodes)
		{
			delete n;
		}
	}
}

void	particle_hypergraph::append_node(node^ nod)
{
	if (all_nodes()->Count == 0)
		m_root_node = safe_cast<particle_graph_node^>(nod);

	all_nodes()->Add(nod);
	nod->on_added(this);
	links_manager->on_node_added(nod);
	Controls->Add(nod);
//	m_command_engine->run(NEW(command_add_nodes)(this, node));
}

void	particle_hypergraph::append_node(node^ nod, node^ parent_nod)
{
	append_node(nod);
	m_command_engine->run( gcnew particle_graph_commands::add_node(safe_cast<particle_graph_node^>(nod), safe_cast<particle_graph_node^>(parent_nod)));
}

void	particle_hypergraph::append_node(nodes^ nds)
{
	XRAY_UNREFERENCED_PARAMETERS(nds);
//	m_command_engine->run(NEW(command_add_nodes)(this, nds));
}

void	particle_hypergraph::remove_node(node^ node)
{
	XRAY_UNREFERENCED_PARAMETERS(node);
//	m_command_engine->run(NEW(command_remove_nodes)(this, node));
}

void	particle_hypergraph::remove_node(nodes^ nds)
{
	XRAY_UNREFERENCED_PARAMETERS(nds);
}

void	particle_hypergraph::move_node(node^ nod, node^ new_parent)
{	
		m_command_engine->run(gcnew particle_graph_commands::move_node(safe_cast<particle_graph_node^>(nod), safe_cast<particle_graph_node^>(new_parent)));
}


void	particle_hypergraph::undo()
{
	m_command_engine->rollback(-1);
	Invalidate(true);
}

void	particle_hypergraph::redo()
{
	m_command_engine->rollback(1);
	Invalidate(true);
}

void	particle_hypergraph::destroy_connection(link^ lnk)
{
	XRAY_UNREFERENCED_PARAMETERS(lnk);
//	m_command_engine->run(NEW(command_remove_links)(this, lnk));
}

void	particle_hypergraph::destroy_connection(links^ lnks)
{
	XRAY_UNREFERENCED_PARAMETERS(lnks);
//	m_command_engine->run(NEW(command_remove_links)(this, lnks));
}

void	particle_hypergraph::make_connection(connection_point^ pt_src, connection_point^ pt_dst)
{
	link^ lnk = gcnew link(pt_src, pt_dst);
	XRAY_UNREFERENCED_PARAMETERS(lnk);
//	m_command_engine->run(NEW(command_add_links)(this, lnk));
}

void	particle_hypergraph::add_connection(links^ lnks)
{
	XRAY_UNREFERENCED_PARAMETERS(lnks);
//	m_command_engine->run(NEW(command_add_links)(this, lnks));
}

void	particle_hypergraph::on_key_down(Object^ , System::Windows::Forms::KeyEventArgs^ e)
{	
	
	
	if (selected_nodes->Count != 0 && e->Control){

		particle_graph_node^ selected_node = safe_cast<particle_graph_node^>(selected_nodes[0]);
		if (selected_node->parent_node != nullptr){		
			if(e->KeyCode == Keys::D){
			comm_engine->run(gcnew particle_graph_commands::disable_group_nodes(this, selected_nodes));

			}
			if(e->KeyCode == Keys::Up){
				comm_engine->run(gcnew particle_graph_commands::change_node_index(selected_node->parent_node,selected_node, selected_node->parent_node->children->IndexOf(selected_node)-1));
			}
			else if (e->KeyCode == Keys::Down){
				comm_engine->run(gcnew particle_graph_commands::change_node_index(selected_node->parent_node,selected_node, selected_node->parent_node->children->IndexOf(selected_node)+1));				
			}
		}
	}		


	if(e->KeyCode == Keys::Delete)
	{
		links^ tmp_links = gcnew links();
		tmp_links->AddRange(copy_links());
		tmp_links->AddRange(m_selected_links);
		remove_group(selected_nodes);
		m_selected_links->Clear();
		set_selected_node((node^)nullptr);
	}
	else if (e->KeyCode == Keys::F)
		focus_on_node(root_node);
	else
		super::on_key_down(nullptr, e);
}

void	particle_hypergraph::on_key_up(Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	super::on_key_up(nullptr, e);
}

particle_hypergraph::links^ particle_hypergraph::copy_links()
{
	links^ tmp_lnks = gcnew links();
	for each(link^ lnk in links_manager->visible_links())
	{
		for each(node^ n in selected_nodes)
		{
			if(lnk->is_relative(n))
				tmp_lnks->Add(lnk);
		}
	}
	return tmp_lnks;
}

void	particle_hypergraph::save()
{
	m_command_engine->set_saved_flag();
	Invalidate(false);
}

bool	particle_hypergraph::is_saved()
{
	return m_command_engine->saved_flag();
}

void	particle_hypergraph::clear_command_stack()
{
//	m_command_engine->clear_stack();
	Invalidate(false);
}

void	particle_hypergraph::add_group(nodes^ nds, links^ lnks)
{
	XRAY_UNREFERENCED_PARAMETERS(nds, lnks);
//	m_command_engine->run(NEW(command_add_group)(this, nds, lnks));
}

void	particle_hypergraph::remove_group(nodes^ nds)
{
	XRAY_UNREFERENCED_PARAMETERS(nds);
	m_command_engine->run(gcnew particle_graph_commands::remove_group(this, nds));
//	m_command_engine->run(NEW(remove_group)(this, nds));
}

void	particle_hypergraph::on_node_property_changed(System::Object^ sender, System::Windows::Forms::PropertyValueChangedEventArgs^ e)
{
	PropertyGrid^ g = safe_cast<PropertyGrid^>(sender);
	particle_graph_node^ n = safe_cast<particle_graph_node^>(g->SelectedObject);
	XRAY_UNREFERENCED_PARAMETERS(e, n);
//	m_command_engine->run(NEW(command_change_node_properties)(n, e->ChangedItem->Label, e->ChangedItem->Value, e->OldValue));
}


void	particle_hypergraph::change_parent_node(node^ nod, node^ new_parent_node){
	safe_cast<particle_graph_node^>(nod)->parent_node->remove_children(safe_cast<particle_graph_node^>(nod));
	safe_cast<particle_graph_node^>(new_parent_node)->add_children(safe_cast<particle_graph_node^>(nod));	
	Invalidate(false);
}

static void show_nodes(particle_graph_node^ nod, particle_graph_node^ dragged_node, bool highlighted, bool need_check){

	for each(node^ n in nod->children){
		show_nodes(safe_cast<particle_graph_node^>(n),dragged_node, highlighted, need_check);
	}
	if (!need_check || nod->can_accept_node(dragged_node)){
		nod->m_highlighted = highlighted;
	}
}

void	particle_hypergraph::show_available_nodes				(particle_graph_node^ node){
	show_nodes(m_root_node, node, true, true);
	Invalidate(false);
}
void	particle_hypergraph::hide_available_nodes				(particle_graph_node^ node){
	show_nodes(m_root_node, node, false, false);
	Invalidate(false);
}
System::Void particle_hypergraph::hypergraph_Paint(System::Object^ , System::Windows::Forms::PaintEventArgs^ e)
{
	if(!is_saved())
		m_owner->is_saved = false;
	else
		m_owner->is_saved  = true;
	super::hypergraph_Paint(nullptr, e);
}

float particle_hypergraph::get_total_length(){
	return (float)root_node->prop_holder->get_value("TotalLength");
}