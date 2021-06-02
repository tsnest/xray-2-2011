////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_files_view_panel.h"
#include "dialog_files_source.h"
#include "dialog_editor.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using System::Windows::Forms::TreeNode;
using xray::dialog_editor::dialog_files_view_panel;
using xray::dialog_editor::dialog_editor_impl;

dialog_files_view_panel::dialog_files_view_panel(dialog_editor_impl^ de)
:xray::editor::controls::file_view_panel_base(de->multidocument_base),
m_dialog_editor(de)
{
	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	m_remove_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	m_remove_item->Name = L"m_remove_item";
	m_remove_item->Size = System::Drawing::Size(37, 20);
	m_remove_item->Text = L"&Remove item";
	m_remove_item->Click += gcnew System::EventHandler(this, &dialog_files_view_panel::remove_item_click);
	m_context_menu->Items->Add(m_remove_item);

	xray::editor_base::image_loader	loader;
	tree_view_image_list = loader.get_shared_images16();
	tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	m_raw_files_tree_view->ImageList = tree_view_image_list;
	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->source = gcnew dialog_files_source();
	m_raw_files_tree_view->refresh();
	extension = ".dlg";
	m_copied_node = nullptr;
	HideOnClose = true;
	m_is_creating_node = false;
}

void dialog_files_view_panel::on_node_double_click(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	if(!m_dialog_editor->loc_file_loaded || !m_dialog_editor->ref_file_loaded)
		return;

	if( safe_cast<tree_node^>(e->Node)->m_node_type==tree_node_type::single_item)
	{
		System::String^ full_path = e->Node->FullPath->Remove(0,8);
		m_dialog_editor->creating_new_document = true;
		m_editor->load_document(full_path);
		m_dialog_editor->creating_new_document = false;
	}
}

void dialog_files_view_panel::on_node_key_down(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Return)
	{
		TreeNode^ n = m_raw_files_tree_view->SelectedNode;
		if(n->Nodes->Count==0)
		{
			System::String^ full_path = n->FullPath->Remove(0,8);
			m_dialog_editor->creating_new_document = true;
			m_editor->load_document(full_path);
			m_dialog_editor->creating_new_document = false;
		}
	}
	else if(e->KeyCode==Keys::Delete)
	{
		remove_node(sender, gcnew xray::editor::tree_view_event_args());
	}
	else if(e->KeyCode==Keys::F2)
	{
		if(!m_raw_files_tree_view->SelectedNode->IsEditing)
			m_raw_files_tree_view->SelectedNode->BeginEdit();
	}
	else if(e->KeyCode==Keys::Insert)
	{
		if(e->Control)
			add_new_folder(sender, gcnew xray::editor::tree_view_event_args());
		else
			add_new_file(sender, gcnew xray::editor::tree_view_event_args());
	}
}

void dialog_files_view_panel::track_active_item(System::String^ path)
{
	m_raw_files_tree_view->track_active_node("dialogs/"+path);
}

void dialog_files_view_panel::add_new_folder(System::Object^ , xray::editor::tree_view_event_args^ )
{
	if(safe_cast<tree_node^>(m_raw_files_tree_view->SelectedNode)->m_node_type!=tree_node_type::single_item)
	{
		m_is_creating_node = true;
		System::String^ node_path = m_raw_files_tree_view->SelectedNode->FullPath+"/new_folder";
		node_path = node_path->Remove(0,8);
		System::String^ folder_path = (source_path+node_path);
		
		int i = 1;
		while(System::IO::Directory::Exists(folder_path+i))
			i++;

		TreeNode^ n = m_raw_files_tree_view->add_group("dialogs/"+node_path+i, editor_base::folder_closed, editor_base::folder_open);
		m_raw_files_tree_view->SelectedNode->Expand();
		m_raw_files_tree_view->SelectedNode = n;
		n->BeginEdit();
	}
}

void dialog_files_view_panel::add_new_file(System::Object^ , xray::editor::tree_view_event_args^ )
{
	if(safe_cast<tree_node^>(m_raw_files_tree_view->SelectedNode)->m_node_type!=tree_node_type::single_item)
	{
		m_is_creating_node = true;
		System::String^ node_path = m_raw_files_tree_view->SelectedNode->FullPath+"/new_file";
		node_path = node_path->Remove(0,8);
		int i = 1;
		System::String^ file_path = (source_path+node_path+i+extension);

		while(System::IO::File::Exists(file_path))
		{
			i++;
			file_path = (source_path+node_path+i+extension);
		}

		m_raw_files_tree_view->SelectedNode->Expand();
		TreeNode^ n = m_raw_files_tree_view->add_item("dialogs/"+node_path+i, editor_base::node_resource);
		m_raw_files_tree_view->SelectedNode = n;
		n->BeginEdit();
	}
}

void dialog_files_view_panel::on_node_name_edit(System::Object^ , System::Windows::Forms::NodeLabelEditEventArgs^ e)
{
	if(m_is_creating_node)
	{
		m_is_creating_node = false;
		if(!(e->Node->Parent->Nodes->ContainsKey(e->Label)))
		{
			if(e->Label!=nullptr)
			{
				e->Node->Text = e->Label;
				e->Node->Name = e->Label;
			}
		}
		else
		{
			MessageBox::Show(this, "Document or folder named '"+e->Label+"' already exists!", 
							"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}

		if(e->Node->Text->Contains("."))
		{
			e->Node->Text = e->Node->Text->Replace(".", "_");
			MessageBox::Show(this, "Name contains '.'!! Replacing with '_'", 
				"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
		if(e->Node->Text->Contains("/") || e->Node->Text->Contains("\\"))
		{
			e->Node->Text = e->Node->Text->Replace("\\", "_");
			e->Node->Text = e->Node->Text->Replace("/", "_");
			MessageBox::Show(this, "Name contains '/'!! Replacing with '_'", 
				"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		}

		System::String^ new_path = (source_path+e->Node->FullPath->Remove(0,8));
		if(safe_cast<tree_node^>(e->Node)->m_node_type!=tree_node_type::single_item)
			System::IO::Directory::CreateDirectory(new_path);
		else
		{
			System::IO::FileStream^ fs = System::IO::File::Create(new_path+extension);
			if(fs)
				delete (System::IDisposable^)fs;
		}
	}
	else
	{
		if(e->Node->FullPath=="dialogs" || e->Label==nullptr)
		{
			e->CancelEdit = true;
			return;
		}
		
		System::String^ node_path = e->Node->FullPath->Remove(0,8);
		System::String^ old_path = (source_path+node_path);

		if(m_editor->is_opened(node_path))
		{
			MessageBox::Show(this, "Document '"+node_path+"' is opened. Please, close it before modifying name.", 
								"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
			e->CancelEdit = true;
			return;
		}

		if(e->Label!=e->Node->Text)
		{
			if(e->Node->Parent->Nodes->ContainsKey(e->Label))
			{
				MessageBox::Show(this, "Document or folder named '"+e->Label+"' already exists!", 
									"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				e->CancelEdit = true;
				return;
			}

			if(e->Label->Contains(".") || e->Label->Contains("/") || e->Label->Contains("\\")/* || e->Label->IndexOf('\\')!=-1*/)
			{
				MessageBox::Show(this, "Name contains unexceptable symbols!", 
					"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				e->CancelEdit = true;
				return;
			}

			e->Node->Text = e->Label;
			e->Node->Name = e->Label;

			System::String^ new_path = (source_path+e->Node->FullPath->Remove(0,8));
			if(safe_cast<tree_node^>(e->Node)->m_node_type!=tree_node_type::single_item)
				System::IO::Directory::Move(old_path, new_path);
			else
				System::IO::File::Move(old_path+extension, new_path+extension);
		}
	}
}

void dialog_files_view_panel::remove_node(System::Object^ , xray::editor::tree_view_event_args^ )
{
	m_copied_node = nullptr;
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	if(m_raw_files_tree_view->selected_nodes[0]->FullPath=="dialogs")
		return;

	System::String^ node_path = m_raw_files_tree_view->selected_nodes[0]->FullPath->Remove(0,8);
	System::String^ path = (source_path+node_path);

	if(m_editor->is_opened(node_path))
		return;

	if( safe_cast<tree_node^>(m_raw_files_tree_view->selected_nodes[0])->m_node_type==tree_node_type::single_item)
	{
		m_raw_files_tree_view->remove_item(m_raw_files_tree_view->selected_nodes[0]->FullPath);
		if(System::IO::File::Exists(path+extension))
			System::IO::File::Delete(path+extension);
		if(System::IO::File::Exists(path+".lt"))
			System::IO::File::Delete(path+".lt");
	}
	else
	{
		m_raw_files_tree_view->remove_group(m_raw_files_tree_view->selected_nodes[0]->FullPath);
		if(System::IO::Directory::Exists(path))
			System::IO::Directory::Delete(path, true);
	}
}

void dialog_files_view_panel::on_tree_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	m_drag_node = safe_cast<tree_node^>(m_raw_files_tree_view->GetNodeAt(e->X, e->Y));
	if(m_drag_node!=nullptr && m_drag_node->FullPath=="dialogs")
	{
		m_drag_node = nullptr;
		return;
	}
}

void dialog_files_view_panel::on_tree_mouse_move(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(m_drag_node!=nullptr)
	{
		if((e->Button & ::MouseButtons::Left) == ::MouseButtons::Left)
			this->m_raw_files_tree_view->DoDragDrop(m_drag_node->FullPath, DragDropEffects::Move|DragDropEffects::Copy);
	}
}

void dialog_files_view_panel::on_node_drag_over(System::Object^ , System::Windows::Forms::DragEventArgs^ e)
{
	if(!e->Data->GetDataPresent(System::String::typeid))
	{
		e->Effect = DragDropEffects::None;
		return;
	}
	//if dragging with control - copy
	if((e->KeyState & 8) == 8)
		e->Effect = DragDropEffects::Copy;
	else
		e->Effect = DragDropEffects::Move;
}

void dialog_files_view_panel::on_node_drag_drop(System::Object^ , System::Windows::Forms::DragEventArgs^ e)
{
	if(e->Data->GetDataPresent(System::String::typeid))
	{
		tree_node^ dest_node = safe_cast<tree_node^>(m_raw_files_tree_view->GetNodeAt(m_raw_files_tree_view->PointToClient(Point(e->X, e->Y))));
		if(dest_node==m_drag_node || dest_node->nodes->Contains(m_drag_node))
		{
			e->Effect = DragDropEffects::None;
			return;
		}

		if(e->Effect==DragDropEffects::Move || e->Effect==DragDropEffects::Copy)
			copy_node_impl(m_drag_node, dest_node, e->Effect==DragDropEffects::Move);
	}
	else
		e->Effect = DragDropEffects::None;
}

void dialog_files_view_panel::copy(bool del)
{
	if(m_copied_node!=nullptr)
		m_copied_node->ForeColor = System::Drawing::Color::BlueViolet;

	m_copied_node = safe_cast<xray::editor::controls::tree_node^>(m_raw_files_tree_view->SelectedNode);
	if(m_copied_node->Name=="dialogs")
	{
		MessageBox::Show(this, "Can't cut or copy 'dialogs' node.", 
			"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		m_copied_node = nullptr;
		return;
	}

	if(del)
		m_copied_node->ForeColor = System::Drawing::Color::LightGray;
}

void dialog_files_view_panel::paste()
{
	if(m_copied_node!=nullptr)
	{
		bool res = copy_node_impl(m_copied_node, safe_cast<xray::editor::controls::tree_node^>(m_raw_files_tree_view->SelectedNode), 
									m_copied_node->ForeColor==System::Drawing::Color::LightGray);
		if(m_copied_node->ForeColor==System::Drawing::Color::LightGray && res==true)
			m_copied_node = nullptr;
	}
}

bool dialog_files_view_panel::copy_node_impl(xray::editor::controls::tree_node^ s, xray::editor::controls::tree_node^ d, bool del_old)
{
	if(d->m_node_type == tree_node_type::single_item)
		d = d->Parent;

	System::String^ source_node_path = s->FullPath->Remove(0,8);
	System::String^ dest_node_path = d->FullPath;
	System::String^ source_file_path = source_path+source_node_path;
	System::String^ dest_file_path;
	if(dest_node_path!="dialogs")
	{
		dest_node_path = dest_node_path->Remove(0,8);
		dest_file_path = (source_path+dest_node_path+"/"+s->Name);
	}
	else
	{
		dest_node_path = "";
		dest_file_path = (source_path+s->Name);
	}
	if(s->m_node_type == tree_node_type::single_item)
	{
		if(System::IO::File::Exists(dest_file_path+extension))
		{
			MessageBox::Show(this, "File '"+dest_node_path+"/"+s->Name+extension+"' already exists.", 
				"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return false;
		}
	}
	else
	{
		if(System::IO::Directory::Exists(dest_file_path))
		{
			MessageBox::Show(this, "Folder '"+dest_node_path+"/"+s->Name+"' already exists.", 
				"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return false;
		}
	}

	if(check_parent(dest_node_path, s->Name))
	{
		MessageBox::Show(this, "Can't copy '"+source_node_path+"' into '"+dest_node_path+"'.", 
			"Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
	}

	if(s->m_node_type == tree_node_type::single_item)
	{
		m_raw_files_tree_view->add_item("dialogs/"+dest_node_path+"/"+s->Name);
		d->Expand();
		if(del_old)
		{
			System::IO::File::Move(source_file_path+extension, dest_file_path+extension);
			m_raw_files_tree_view->remove_item("dialogs/"+source_node_path);
		}
		else
			System::IO::File::Copy(source_file_path+extension, dest_file_path+extension);
	}
	else
	{
		add_folders_and_files(s->nodes, "dialogs/"+dest_node_path+"/"+s->Name, editor_base::folder_closed, editor_base::folder_open, editor_base::node_resource);
		if(del_old)
		{
			System::IO::Directory::Move(source_file_path, dest_file_path);
			m_raw_files_tree_view->remove_group("dialogs/"+source_node_path);
		}
		else
		{
			System::IO::DirectoryInfo^ di = gcnew System::IO::DirectoryInfo(source_file_path);
			iterate_folders(di, dest_file_path);
			delete di;
		}
	}

	m_raw_files_tree_view->Sort();
	return true;
}

void dialog_files_view_panel::remove_item_click(System::Object^, System::EventArgs^)
{
	remove_node(nullptr, nullptr);
}