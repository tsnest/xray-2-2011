//-------------------------------------------------------------------------------------------
//	Created		: 23.12.2009
//	Author		: Alexander Plichko
//	Copyright (C) GSC Game World - 2009
//-------------------------------------------------------------------------------------------

#include "pch.h"
#include "particle_editor.h"
#include "particle_graph_document.h"
#include "particle_groups_source.h"
#include "particle_graph_node_style.h"
#include "particle_graph_node.h"
#include "particle_graph_node_emitter.h"
#include "particle_node_properties.h"
#include "resource_editor.h"
#include "particle_source_nodes_container.h"
#include "particle_sources_panel.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows;
using namespace System::Windows::Forms;

namespace xray {
namespace editor {

void particle_editor::in_constructor							()
{
	m_node_style_mgr								= gcnew particle_graph_node_style_mgr();
	
	m_multidocument_base							= gcnew document_editor_base("particle_editor");
	m_multidocument_base->creating_new_document		= gcnew document_create_callback(this, &particle_editor::on_create_document);
	
	Controls->Add									(m_multidocument_base);
	m_time_layout_panel								= gcnew particle_time_based_layout_panel(this);


	dock_panels();

	m_multidocument_base->Dock						= DockStyle::Fill;

	Forms::ContextMenuStrip^	nodes_context_menu	= gcnew Forms::ContextMenuStrip();
	nodes_context_menu->Items->AddRange				(gcnew array<ToolStripMenuItem^>{
		gcnew ToolStripMenuItem("New Document", nullptr, gcnew EventHandler(this, &particle_editor::on_view_new_document)),
		gcnew ToolStripMenuItem("Remove",		nullptr, gcnew EventHandler(this, &particle_editor::on_view_remove_document)),
		gcnew ToolStripMenuItem("New Folder",	nullptr, gcnew EventHandler(this, &particle_editor::on_view_new_folder)),
	});

	Forms::ContextMenuStrip^	context_menu		= gcnew Forms::ContextMenuStrip();
	context_menu->Items->AddRange					(gcnew array<ToolStripMenuItem^>{
		gcnew ToolStripMenuItem("New Document", nullptr, gcnew EventHandler(this, &particle_editor::on_new_document)),
		gcnew ToolStripMenuItem("New Folder",	nullptr, gcnew EventHandler(this, &particle_editor::on_new_folder)),
	});

	//set properties
	m_multidocument_base->view_panel->tree_view->AfterLabelEdit			+= gcnew NodeLabelEditEventHandler(this, &particle_editor::on_view_label_edit);
	m_multidocument_base->view_panel->tree_view->ContextMenuStrip		= context_menu;
	m_multidocument_base->view_panel->tree_view->nodes_context_menu		= nodes_context_menu;
	m_multidocument_base->view_panel->tree_view->ShowRootLines			= true;
	m_multidocument_base->view_panel->tree_view->items_loaded			+= gcnew EventHandler(this, &particle_editor::tree_loaded);
	m_multidocument_base->view_panel->files_source						= gcnew particle_groups_source();
	m_multidocument_base->view_panel->refresh							();

	m_multidocument_base->toolbar_panel->SuspendLayout();
	
	m_node_style_mgr->library_loaded				+= gcnew EventHandler(this, &particle_editor::on_node_style_mgr_loaded);
	
	m_multidocument_base->toolbar_panel->Text		= L"Particle Nodes ToolBar";

	m_multidocument_base->toolbar_panel->AllowDrop						= true;
	this->AllowDrop														= true;
	
	m_multidocument_base->toolbar_panel->ResumeLayout(true);
	m_multidocument_base->toolbar_panel->PerformLayout();
	
	m_data_sources_panel = gcnew particle_data_sources_panel(this);
	m_data_sources_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom );

	m_time_layout_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom );	
}


particle_editor::~particle_editor							()	{
	
	if (components)
	{
		delete components;
	}
}

void			particle_editor::tree_loaded					(Object^ , EventArgs^ )
{
	TreeNodeCollection^ nodes = m_multidocument_base->view_panel->tree_view->root->Nodes;
	if(nodes->Count <= 0)
		return;
	
	nodes[0]->Expand();
	nodes[0]->ContextMenuStrip = m_multidocument_base->view_panel->tree_view->ContextMenuStrip;
}

controls::document_base^	particle_editor::on_create_document	()
{
	particle_graph_document^ doc =  gcnew particle_graph_document(m_multidocument_base, this);
	doc->create_root_node(this, EventArgs::Empty);
	return doc;
}

//dock panels to sound editor main form
void			particle_editor::dock_panels					()
{
	m_multidocument_base->load_panels(this);

 	//fill events
 	m_multidocument_base->view_panel->file_double_click		+= gcnew TreeNodeMouseClickEventHandler(this, &particle_editor::on_file_view_double_click);
	m_multidocument_base->properties_panel->property_grid_control->auto_update = true;
}

void			particle_editor::on_node_style_mgr_loaded		(Object^ , EventArgs^ )
{
	m_nodes_panel											= gcnew particle_sources_panel(this);
	m_nodes_panel->Dock										= DockStyle::Fill;
	m_multidocument_base->toolbar_panel->Controls->Add		(m_nodes_panel);	
}

static void		set_default_name(TreeNode^ dst_node, String^ default_name)
{
	TreeNodeCollection^ nodes		= (dst_node->Parent != nullptr)?dst_node->Parent->Nodes:dst_node->TreeView->Nodes;
	Int32 index						= 1;

	for each(TreeNode^ node in nodes)
	{
		Int32 node_number;

		if(node->Text->StartsWith(default_name) &&
			Int32::TryParse(node->Text->Substring(default_name->Length), node_number) &&
			node_number >= index)

			index = node_number+1;
	}
	dst_node->Text = default_name+index.ToString();
}

static void		create_particle_file(String^ file_path, particle_editor^ p_editor)
{
	particle_graph_node^ root_node			= gcnew particle_graph_node_emitter(xray::editor_base::image_node_emitter, p_editor);
	root_node->Text							= "Root";
	root_node->node_config_field			= "Root";
	root_node->properties					= gcnew particle_node_properties( root_node, "emitter", "Root");
	root_node->load_node_properties( (*p_editor->node_sources_panel->particle_nodes_config)["Root"]["properties"] );
	
	
	root_node->movable						= true;
	

	configs::lua_config_ptr	const& config	= configs::create_lua_config(unmanaged_string(file_path).c_str());
	configs::lua_config_value config_item	= config->get_root()["root"];

	root_node->save(config_item);
	config->save();
	delete root_node;
}

void			particle_editor::on_view_label_edit				(Object^ , NodeLabelEditEventArgs^ e)
{
	Boolean is_file				= safe_cast<controls::tree_node^>(e->Node)->m_node_type == tree_node_type::single_item;
	if(e->Node->Text == " ")
	{
		if(e->Label == nullptr || e->Label->Trim() == "")
		{
			set_default_name		(e->Node, (is_file)?s_default_file_name:s_default_folder_name);
			e->CancelEdit			= true;
		}
		else
			e->Node->Text			= e->Label->Trim();
		
		e->Node->Name			= e->Node->Text;

		if(is_file)
		{
			String^ new_file_path	= (particle_editor::absolute_particle_resources_path + e->Node->FullPath);
			
			if(File::Exists(new_file_path + s_file_extension) || Directory::Exists(new_file_path))
			{
				System::Windows::Forms::MessageBox::Show("File or folder with name \""+e->Label+"\" already exists in the current scope.", "Particle Editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				set_default_name(e->Node, s_default_file_name);
				create_particle_file(Path::GetDirectoryName(new_file_path)+"/"+e->Node->Text+s_file_extension, this);
				e->CancelEdit	= true;
				e->Node->EnsureVisible();
			}
			else
				create_particle_file(new_file_path+s_file_extension, this);
		}
		else
		{
			String^ new_dir_path	= (particle_editor::absolute_particle_resources_path + e->Node->FullPath);
			
			if(Directory::Exists(new_dir_path) || File::Exists(new_dir_path+s_file_extension))
			{
				System::Windows::Forms::MessageBox::Show("File or folder with name \""+e->Label+"\" already exists in the current scope.", "Particle Editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				set_default_name(e->Node, s_default_folder_name);
				Directory::CreateDirectory(Path::GetDirectoryName(new_dir_path)+ "/" + e->Node->Text);
				e->CancelEdit	= true;
				e->Node->EnsureVisible();
			}
			else
				Directory::CreateDirectory(new_dir_path);
		}
	}
	else
	{
		if(e->Node->Text == e->Label || e->Label == nullptr)
			return;

		if(is_file)
		{
			String^ new_file_path	= (particle_editor::absolute_particle_resources_path + Path::GetDirectoryName(e->Node->FullPath) + "/" + e->Label->Trim() + s_file_extension);
				
			if(File::Exists(new_file_path) || Directory::Exists(Path::ChangeExtension(new_file_path, "")))
			{
				System::Windows::Forms::MessageBox::Show	("This name already in use!");
				e->CancelEdit		= true;
				return;
			}
			File::Move	(particle_editor::absolute_particle_resources_path + e->Node->FullPath + s_file_extension, new_file_path);
		}
		else
		{
			String^ new_dir_path	= (particle_editor::absolute_particle_resources_path + Path::GetDirectoryName(e->Node->FullPath) + "/" + e->Label->Trim());
				
			if(Directory::Exists(new_dir_path) || File::Exists(new_dir_path+s_file_extension))
			{
				System::Windows::Forms::MessageBox::Show	("This name already in use!");
				e->CancelEdit		= true;
				return;
			}
			else
				Directory::Move(particle_editor::absolute_particle_resources_path+e->Node->FullPath, new_dir_path);
		}
	}
	e->Node->EnsureVisible();
}

void 			particle_editor::on_new_document				(Object^ , EventArgs^ )
{
	TreeNode^ node = m_multidocument_base->view_panel->tree_view->add_item("particles/ ");
	m_multidocument_base->view_panel->tree_view->SelectedNode = node;
	node->BeginEdit();// Nodes->Add(new_node);
}

void 			particle_editor::on_new_folder					(Object^ , EventArgs^ )
{
	TreeNode^ node = m_multidocument_base->view_panel->tree_view->add_group("particles/ ");
	m_multidocument_base->view_panel->tree_view->SelectedNode = node;
	node->BeginEdit();
}

void 			particle_editor::on_view_new_document			(Object^ , EventArgs^ )
{
	String^ path;

	if(safe_cast<controls::tree_node^>(m_multidocument_base->view_panel->tree_view->SelectedNode)->m_node_type == tree_node_type::single_item)
		path = (m_multidocument_base->view_panel->tree_view->SelectedNode->Parent == nullptr)
			? " ": m_multidocument_base->view_panel->tree_view->SelectedNode->Parent->FullPath+"/ ";
	else
		path = m_multidocument_base->view_panel->tree_view->SelectedNode->FullPath+"/ ";

	controls::tree_node^ node	= m_multidocument_base->view_panel->tree_view->add_item(path);
	node->EnsureVisible	();
	m_multidocument_base->view_panel->tree_view->SelectedNode = node;
	node->BeginEdit();

}

void 			particle_editor::on_view_remove_document		(Object^ , EventArgs^ )
{
	Boolean is_file		= safe_cast<controls::tree_node^>(m_multidocument_base->view_panel->tree_view->SelectedNode)->m_node_type == tree_node_type::single_item;

	String^ message;
	if(is_file)
		message			= "Are you sure you want to remove this particle?";
	else
		message			= "Are you sure you want to remove this directory, with all particles?";

	if(Windows::Forms::MessageBox::Show(message, "Items Deleting", MessageBoxButtons::YesNo) == Windows::Forms::DialogResult::Yes)
	{
		if(is_file)
		{
			File::Delete		(particle_editor::absolute_particle_resources_path+m_multidocument_base->view_panel->tree_view->SelectedNode->FullPath+s_file_extension);
			m_multidocument_base->view_panel->tree_view->SelectedNode->Remove();
		}
		else
		{
			Directory::Delete	(particle_editor::absolute_particle_resources_path+m_multidocument_base->view_panel->tree_view->SelectedNode->FullPath, true);
			m_multidocument_base->view_panel->tree_view->SelectedNode->Remove();
		}
	}
}

void 			particle_editor::on_view_new_folder				(Object^ , EventArgs^ )
{
	String^ path;

	if(safe_cast<controls::tree_node^>(m_multidocument_base->view_panel->tree_view->SelectedNode)->m_node_type == tree_node_type::single_item)
		path = (m_multidocument_base->view_panel->tree_view->SelectedNode->Parent == nullptr)
			? " ": m_multidocument_base->view_panel->tree_view->SelectedNode->Parent->FullPath+"/ ";
	else
		path = m_multidocument_base->view_panel->tree_view->SelectedNode->FullPath+"/ ";

	controls::tree_node^ node	= m_multidocument_base->view_panel->tree_view->add_group(path);
	node->EnsureVisible	();
	m_multidocument_base->view_panel->tree_view->SelectedNode = node;
	node->BeginEdit();
}

void			particle_editor::particle_editor_Activated		(System::Object^  , System::EventArgs^  )
{
	m_multidocument_base->view_panel->tree_view->root->FirstNode->Expand();
}

//close form and save its settings
void			particle_editor::manual_close					()
{
	//Save panel settings
	
	m_multidocument_base->close_all_documents			();
	m_multidocument_base->save_panels(this);
	//serialize_dock_panel_root(this, m_main_dock_panel, "particle_editor");

	delete m_nodes_panel;
	delete m_node_style_mgr;
	this->Close();
}

void			particle_editor::particle_editor_FormClosing	(System::Object^ , System::Windows::Forms::FormClosingEventArgs^ e)
{
	if(e->CloseReason == System::Windows::Forms::CloseReason::UserClosing)
	{
		//manual hide this form
		this->Hide();
		//cancel closing the form
		e->Cancel = true;
	}
}

void			particle_editor::exitToolStripMenuItem_Click	(System::Object^  , System::EventArgs^  )
{
	this->Close();
}

void			particle_editor::on_file_view_double_click		(Object^ , TreeNodeMouseClickEventArgs^ e)
{
	if(safe_cast<controls::tree_node^>(e->Node)->m_node_type == tree_node_type::single_item)
		m_multidocument_base->load_document(e->Node->FullPath);
}

}//namespace editor
}//namespace xray