////////////////////////////////////////////////////////////////////////////
//	Created		: 11.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animations_list_panel.h"
#include "animations_list_files_source.h"
#include "animation_setup_manager.h"

using xray::animation_editor::animations_list_panel;
using xray::animation_editor::animation_setup_manager;

animations_list_panel::animations_list_panel(animation_setup_manager^ m)
:file_view_panel_base(m->multidocument_base),
m_manager(m)
{
	xray::editor_base::image_loader	loader;
	tree_view_image_list = loader.get_shared_images16();
	tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->ImageList = tree_view_image_list;
	m_raw_files_tree_view->source = gcnew animations_list_files_source();
	m_raw_files_tree_view->refresh();
	extension = ".clip";
	HideOnClose = true;
}

void animations_list_panel::on_node_double_click(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	if(safe_cast<tree_node^>(e->Node)->m_node_type==tree_node_type::single_item && m_manager->model_loaded())
	{
		System::String^ full_path = e->Node->FullPath->Remove(0,11);
		m_editor->load_document(full_path);
	}
}

void animations_list_panel::on_node_key_down(System::Object^, System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Return)
	{
		TreeNode^ n = m_raw_files_tree_view->SelectedNode;
		if(n->Nodes->Count==0)
		{
			System::String^ full_path = n->FullPath->Remove(0,11);
			m_editor->load_document(full_path);
		}
	}
}

void animations_list_panel::track_active_item(System::String^ path)
{
	m_raw_files_tree_view->track_active_node("animations/"+path);
}
