////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "channels_files_view_panel.h"
#include "channels_files_source.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using System::Windows::Forms::TreeNode;
using xray::animation_editor::channels_files_view_panel;

void channels_files_view_panel::in_constructor()
{
	xray::editor_base::image_loader	loader;
	tree_view_image_list = loader.get_shared_images16();
	tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->ImageList = tree_view_image_list;
	m_raw_files_tree_view->source = gcnew channels_files_source();
	m_raw_files_tree_view->refresh();
	extension = ".clip";
	HideOnClose = true;
}

void channels_files_view_panel::on_node_double_click(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	if( safe_cast<tree_node^>(e->Node)->m_node_type==tree_node_type::single_item)
	{
		System::String^ full_path = e->Node->FullPath->Remove(0,11);
		m_editor->load_document(full_path);
	}
}

void channels_files_view_panel::on_node_key_down(System::Object^, System::Windows::Forms::KeyEventArgs^ e)
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

void channels_files_view_panel::track_active_item(System::String^ path)
{
	m_raw_files_tree_view->track_active_node("animations/"+path);
}
