////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_groups_animations_tree.h"
#include "animations_list_files_source.h"
#include "animation_groups_editor.h"

using xray::animation_editor::animation_groups_animations_tree;

animation_groups_animations_tree::animation_groups_animations_tree(xray::editor::controls::document_editor_base^ d)
:super(d)
{
	xray::editor_base::image_loader	loader;
	tree_view_image_list = loader.get_shared_images16();
	tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	m_raw_files_tree_view->is_multiselect = true;
	m_raw_files_tree_view->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &animation_groups_animations_tree::on_tree_node_mouse_click);
	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->ImageList = tree_view_image_list;
	m_raw_files_tree_view->source = gcnew animations_list_files_source();
	m_raw_files_tree_view->refresh();
	extension = ".clip";
	HideOnClose = true;
	last_selected_node_name = "";
}

void animation_groups_animations_tree::track_active_item(System::String^ path)
{
	m_raw_files_tree_view->track_active_node("animations/"+path);
	last_selected_node_name = "animations/"+path;
}

void animation_groups_animations_tree::on_tree_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	m_drag_node = safe_cast<tree_node^>(m_raw_files_tree_view->GetNodeAt(e->X, e->Y));
}

void animation_groups_animations_tree::on_tree_mouse_move(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	u32 count = m_raw_files_tree_view->selected_nodes->Count;
	if(count>0 && m_drag_node!=nullptr)
	{
		if((e->Button & ::MouseButtons::Left) == ::MouseButtons::Left)
		{
			animation_groups_drag_drop_operation t;
			List<String^>^ lst = gcnew List<String^>();
			tree_node_type nt = m_raw_files_tree_view->selected_nodes[0]->m_node_type;
			lst->Add(m_raw_files_tree_view->selected_nodes[0]->FullPath);
			for(u32 i=1; i<count; ++i)
			{
				tree_node^ n = m_raw_files_tree_view->selected_nodes[i];
				if(n->m_node_type==nt)
					lst->Add(n->FullPath);
			}

			DataObject^ data = gcnew DataObject("animation_tree_item", lst);
			if(nt==tree_node_type::single_item)
				t = animation_groups_drag_drop_operation::animations_tree_files;
			else
			{
				t = animation_groups_drag_drop_operation::animations_tree_folder;
				if(count>1)
					return;
			}

			data->SetData("item_type", t);
			m_raw_files_tree_view->DoDragDrop(data, DragDropEffects::Move);
		}
	}
}

tree_node^ animation_groups_animations_tree::get_node(System::String^ path)
{
	return m_raw_files_tree_view->get_node(path);
}


void animation_groups_animations_tree::on_tree_node_mouse_click(System::Object^, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	last_selected_node_name = e->Node->FullPath;
}
