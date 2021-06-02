////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_view_panel.h"
#include "sound_editor.h"
#include "sound_object_wrapper.h"
#include "single_sound_wrapper.h"
#include "composite_sound_wrapper.h"
#include "sound_collection_wrapper.h"
#include "sound_document_editor_base.h"

using xray::sound_editor::tree_view_panel;
using xray::sound_editor::sound_editor;
using xray::sound_editor::sound_object_wrapper;
using xray::editor::controls::tree_node_type;
using System::Collections::ObjectModel::ReadOnlyCollection;

tree_view_panel::tree_view_panel ( sound_editor^ parent )
:super(parent->multidocument_base),
m_parent(parent)
{
	extension = ".wav";
	m_is_cutting_node = false;
	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	addToolStripMenuItem = gcnew ToolStripMenuItem();
	newFileToolStripMenuItem = gcnew ToolStripMenuItem();
	newFolderToolStripMenuItem = gcnew ToolStripMenuItem();
	applyChangesToolStripMenuItem = gcnew ToolStripMenuItem();
	revertChangesToolStripMenuItem = gcnew ToolStripMenuItem();
	addToCollectionToolStripMenuItem = gcnew ToolStripMenuItem();
	toolStripSeparator1 = gcnew ToolStripSeparator();
	cutToolStripMenuItem = gcnew ToolStripMenuItem();
	copyToolStripMenuItem = gcnew ToolStripMenuItem();
	pasteToolStripMenuItem = gcnew ToolStripMenuItem();
	removeItemToolStripMenuItem = gcnew ToolStripMenuItem();
	renameItemToolStripMenuItem = gcnew ToolStripMenuItem();
	tabControl1 = gcnew TabControl();
	tabPage1 = gcnew TabPage();
	tabPage2 = gcnew TabPage();
	tabPage3 = gcnew TabPage();
	tabPage4 = gcnew TabPage();

	m_context_menu->SuspendLayout();
	tabControl1->SuspendLayout();
	SuspendLayout();

	m_context_menu->Items->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(10) {
		addToolStripMenuItem, 
		applyChangesToolStripMenuItem, 
		revertChangesToolStripMenuItem,
		addToCollectionToolStripMenuItem,
		toolStripSeparator1,
		cutToolStripMenuItem,
		copyToolStripMenuItem,
		pasteToolStripMenuItem,	
		removeItemToolStripMenuItem, 
		renameItemToolStripMenuItem,
	});
	m_context_menu->Name = L"m_context_menu";
	m_context_menu->Size = System::Drawing::Size(153, 92);
	m_context_menu->Opened += gcnew EventHandler(this, &tree_view_panel::menu_opened);

	// addToolStripMenuItem
	addToolStripMenuItem->Name = L"addToolStripMenuItem";
	addToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	addToolStripMenuItem->Text = L"Add";
	addToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(2)
		{newFileToolStripMenuItem, newFolderToolStripMenuItem});

	// newFileToolStripMenuItem
	newFileToolStripMenuItem->Name = L"newFileToolStripMenuItem";
	newFileToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	newFileToolStripMenuItem->Text = L"New file";
	newFileToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::new_file_click);

	// newFolderToolStripMenuItem
	newFolderToolStripMenuItem->Name = L"newFolderToolStripMenuItem";
	newFolderToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	newFolderToolStripMenuItem->Text = L"New folder";
	newFolderToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::new_folder_click);

	// applyChangesToolStripMenuItem
	applyChangesToolStripMenuItem->Name = L"applyChangesToolStripMenuItem";
	applyChangesToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	applyChangesToolStripMenuItem->Text = L"Apply changes";
	applyChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::apply_changes_click);

	// revertChangesToolStripMenuItem
	revertChangesToolStripMenuItem->Name = L"revertChangesToolStripMenuItem";
	revertChangesToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	revertChangesToolStripMenuItem->Text = L"Revert changes";
	revertChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::revert_changes_click);

	// addToCollectionToolStripMenuItem
	addToCollectionToolStripMenuItem->Name = L"addToCollectionToolStripMenuItem";
	addToCollectionToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	addToCollectionToolStripMenuItem->Text = L"Add to collection";
	addToCollectionToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::add_to_collection_click);

	// toolStripSeparator1
	toolStripSeparator1->Name = L"toolStripSeparator1";
	toolStripSeparator1->Size = System::Drawing::Size(89, 6);

	// cutToolStripMenuItem
	cutToolStripMenuItem->Name = L"cutToolStripMenuItem";
	cutToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	cutToolStripMenuItem->Text = L"Cut";
	cutToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::cut_click);

	// copyToolStripMenuItem
	copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
	copyToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	copyToolStripMenuItem->Text = L"Copy";
	copyToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::copy_click);

	// pasteToolStripMenuItem
	pasteToolStripMenuItem->Name = L"pasteToolStripMenuItem";
	pasteToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	pasteToolStripMenuItem->Text = L"Paste";
	pasteToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::paste_click);

	// removeItemToolStripMenuItem
	removeItemToolStripMenuItem->Name = L"removeItemToolStripMenuItem";
	removeItemToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	removeItemToolStripMenuItem->Text = L"Remove item";
	removeItemToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::remove_click);

	// renameItemToolStripMenuItem
	renameItemToolStripMenuItem->Name = L"renameItemToolStripMenuItem";
	renameItemToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	renameItemToolStripMenuItem->Text = L"Rename item";
	renameItemToolStripMenuItem->Click += gcnew System::EventHandler(this, &tree_view_panel::rename_click);
	
	m_context_menu->ResumeLayout(false);

	// tabControl1
	tabControl1->Controls->Add(tabPage1);
	tabControl1->Controls->Add(tabPage2);
	tabControl1->Controls->Add(tabPage3);
	//tabControl1->Controls->Add(tabPage4);
	tabControl1->Location = System::Drawing::Point(19, 16);
	tabControl1->Name = L"tabControl1";
	tabControl1->Dock = DockStyle::Top;
	tabControl1->SelectedIndex = 0;
	tabControl1->Size = System::Drawing::Size(200, 20);
	tabControl1->TabIndex = 0;
	tabControl1->Selected += gcnew TabControlEventHandler(this, &tree_view_panel::on_tab_changed);

	// tabPage1
	tabPage1->Location = System::Drawing::Point(4, 22);
	tabPage1->Name = L"tabPage1";
	tabPage1->Padding = System::Windows::Forms::Padding(3);
	tabPage1->Size = System::Drawing::Size(192, 74);
	tabPage1->TabIndex = 0;
	tabPage1->Text = L"Single";
	tabPage1->UseVisualStyleBackColor = true;

	// tabPage2
	tabPage2->Location = System::Drawing::Point(4, 22);
	tabPage2->Name = L"tabPage2";
	tabPage2->Padding = System::Windows::Forms::Padding(3);
	tabPage2->Size = System::Drawing::Size(192, 74);
	tabPage2->TabIndex = 1;
	tabPage2->Text = L"Composite";
	tabPage2->UseVisualStyleBackColor = true;

	// tabPage3
	tabPage3->Location = System::Drawing::Point(4, 22);
	tabPage3->Name = L"tabPage3";
	tabPage3->Padding = System::Windows::Forms::Padding(3);
	tabPage3->Size = System::Drawing::Size(192, 74);
	tabPage3->TabIndex = 1;
	tabPage3->Text = L"Collections";
	tabPage3->UseVisualStyleBackColor = true;

	// tabPage4
	tabPage4->Location = System::Drawing::Point(4, 22);
	tabPage4->Name = L"tabPage4";
	tabPage4->Padding = System::Windows::Forms::Padding(3);
	tabPage4->Size = System::Drawing::Size(192, 74);
	tabPage4->TabIndex = 1;
	tabPage4->Text = L"All";
	tabPage4->UseVisualStyleBackColor = true;

	// tree_view_panel
	xray::editor_base::image_loader	loader;
	m_tree_view_image_list = loader.get_shared_images16();
	m_tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;
	m_raw_files_tree_view->ContextMenuStrip = m_context_menu;
	m_raw_files_tree_view->ImageList = m_tree_view_image_list;
	m_raw_files_tree_view->is_multiselect = true;
	m_raw_files_tree_view->source = gcnew tree_view_files_source(nullptr);
	m_raw_files_tree_view->refresh();
	m_raw_files_tree_view->selected_items_changed += gcnew EventHandler<editor::controls::tree_view_selection_event_args^>(this, &tree_view_panel::selection_changed);

	AllowDrop = true;
	HideOnClose = true;
	Controls->Add(tabControl1);
	tabControl1->ResumeLayout(false);
	Text = "Files tree view";
	ResumeLayout(false);
}

void tree_view_panel::track_active_item ( String^ path )
{
	m_raw_files_tree_view->track_active_node(path);
	safe_cast<tree_view_files_source^>(m_raw_files_tree_view->source)->do_not_refresh = true;
}

void tree_view_panel::track_active_items ( List<String^>^ paths )
{
	m_raw_files_tree_view->track_active_nodes( paths );
}

void tree_view_panel::on_tree_mouse_down ( Object^, MouseEventArgs^ e )
{
	m_drag_node = safe_cast<tree_node^>(m_raw_files_tree_view->GetNodeAt(e->X, e->Y));
	if(!m_drag_node || m_drag_node->m_node_type!=tree_node_type::single_item)
		m_parent->clear_properties();
}

void tree_view_panel::selection_changed ( Object^, xray::editor::controls::tree_view_selection_event_args^ )
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return m_parent->clear_properties();

	array<sound_object_wrapper^>^ lst = gcnew array<sound_object_wrapper^>(m_raw_files_tree_view->selected_nodes->Count);
	u32 i = 0;
	for each(tree_node^ node in m_raw_files_tree_view->selected_nodes)
	{
		sound_object_wrapper^ w = nullptr;
		if(node->m_node_type==tree_node_type::single_item)
		{
			if(get_mode()==tree_view_files_source_mode::show_single)
				w = m_parent->get_single_sound(node->FullPath);
			else
			{
				if(get_mode()==tree_view_files_source_mode::show_composite)
					w = m_parent->get_composite_sound(node->FullPath);
				else
					w = m_parent->get_sound_collection(node->FullPath);

				//continue;
			}
		}

		if(w!=nullptr && w->is_loaded)
			lst[i++] = w;
	}

	if(i>0)
		m_parent->show_properties(lst, false);
}

void tree_view_panel::on_node_double_click ( Object^, TreeNodeMouseClickEventArgs^ e )
{
	tree_view_files_source_mode m = get_mode();
	if(m!=tree_view_files_source_mode::show_single)
	{
		if(safe_cast<tree_node^>(e->Node)->m_node_type==tree_node_type::single_item)
		{
			System::String^ full_path = e->Node->FullPath;
			safe_cast<sound_document_editor_base^>(m_editor)->load_document(full_path, 
				(m==tree_view_files_source_mode::show_collections) ? 
					sound_document_type::collection_document : 
					sound_document_type::composite_document
			);
		}
	}
}

void tree_view_panel::on_node_name_edit(  Object^, NodeLabelEditEventArgs^ e )
{
	String^ new_name = e->Label;
	tree_node^ n = safe_cast<tree_node^>(e->Node);
	if(new_name==nullptr || new_name==n->Text)
	{
		e->CancelEdit = true;
		return;
	}

	TreeNodeCollection^ col;
	if(n->Parent)
		col = n->nodes;
	else
		col = m_raw_files_tree_view->nodes;

	if(col->ContainsKey(new_name))
	{
		MessageBox::Show(this, "Document or folder named [" + new_name + "] already exists!", 
						"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		e->CancelEdit = true;
		return;
	}

	if(new_name->Contains("."))
	{
		e->CancelEdit = true;
		new_name = new_name->Replace(".", "_");
		MessageBox::Show(this, "Name contains '.'!! Replacing with '_'", 
			"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Warning);
	}
	if(new_name->Contains("/") || new_name->Contains("\\"))
	{
		e->CancelEdit = true;
		new_name = new_name->Replace("\\", "_");
		new_name = new_name->Replace("/", "_");
		MessageBox::Show(this, "Name contains '/'!! Replacing with '_'", 
			"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Warning);
	}

	String^ old_node_path = n->FullPath;
	String^ new_node_path = "";
	int index = old_node_path->LastIndexOf("/");
	if(index>0)
		new_node_path += old_node_path->Remove(index + 1);

	new_node_path += new_name;
	if(m_editor->is_opened(old_node_path))
	{
		MessageBox::Show(this, "Document ["+old_node_path+"] is opened. Please, close it before modifying name.", 
							"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		e->CancelEdit = true;
		return;
	}

	n->Text = new_name;
	n->Name = new_name;
	do_fs_operation((n->m_node_type==tree_node_type::single_item) ? 
			fs_operation_type::file_rename : 
			fs_operation_type::folder_rename, 
		new_node_path, 
		old_node_path
	);
}

void tree_view_panel::on_node_drag_over ( Object^, DragEventArgs^ e )
{
	if(!e->Data->GetDataPresent("tree_view_item"))
	{
		e->Effect = DragDropEffects::None;
		return;
	}
	//if dragging with control - copy
	if((ModifierKeys & Keys::Control) == Keys::Control)
		e->Effect = DragDropEffects::Copy;
	else
		e->Effect = DragDropEffects::Move;
}

void tree_view_panel::on_node_drag_drop ( Object^, DragEventArgs^ e )
{
	if(e->Data->GetDataPresent("tree_view_item"))
	{
		System::Drawing::Point node_position = m_raw_files_tree_view->PointToClient(System::Drawing::Point(e->X, e->Y));
		tree_node^ dest_node = safe_cast<tree_node^>(m_raw_files_tree_view->GetNodeAt(node_position));
		List<String^>^ lst = safe_cast<List<String^>^>(e->Data->GetData("tree_view_item"));
		List<tree_node^>^ drag_nodes = gcnew List<tree_node^>();
		for each(String^ drag_node_name in lst)
		{
			tree_node^ drag_node = m_raw_files_tree_view->get_node(drag_node_name);
			R_ASSERT(drag_node!=nullptr);
			sound_object_type t = (sound_object_type)safe_cast<u32>(e->Data->GetData("mode"));;
			if(drag_node->m_node_type==tree_node_type::single_item && 
				!(m_parent->get_sound(drag_node_name, t)->is_loaded))
			{
				MessageBox::Show(this, "Sound ["+drag_node_name+"] is loading. Please, wait...", 
									"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				e->Effect = DragDropEffects::None;
				return;
			}

			TreeNodeCollection^ col;
			if(dest_node)
				col = dest_node->nodes;
			else
				col = m_raw_files_tree_view->nodes;

			if(dest_node==drag_node || col->Contains(drag_node))
			{
				e->Effect = DragDropEffects::None;
				return;
			}

			drag_nodes->Add(drag_node);
		}

		for each(tree_node^ drag_node in drag_nodes)
		{
			if(e->Effect==DragDropEffects::Move || e->Effect==DragDropEffects::Copy)
				copy_nodes_impl(drag_node, dest_node, e->Effect==DragDropEffects::Move);
		}
	}
	else
		e->Effect = DragDropEffects::None;
}

void tree_view_panel::on_tree_mouse_move ( Object^, MouseEventArgs^ e )
{
	u32 count = m_raw_files_tree_view->selected_nodes->Count;
	if(count>0 && m_drag_node!=nullptr)
	{
		if((e->Button & ::MouseButtons::Left) == ::MouseButtons::Left)
		{
			sound_editor_drag_drop_operation t;
			List<String^>^ lst = gcnew List<String^>();
			tree_node_type nt = m_raw_files_tree_view->selected_nodes[0]->m_node_type;
			lst->Add(m_raw_files_tree_view->selected_nodes[0]->FullPath);
			for(u32 i=1; i<count; ++i)
			{
				tree_node^ n = m_raw_files_tree_view->selected_nodes[i];
				if(n->m_node_type==nt)
					lst->Add(n->FullPath);
			}

			DataObject^ data = gcnew DataObject("tree_view_item", lst);
			if(nt==tree_node_type::single_item)
				t = sound_editor_drag_drop_operation::tree_view_file;
			else
				t = sound_editor_drag_drop_operation::tree_view_folder;

			data->SetData("item_type", t);
			sound_object_type m = (get_mode()==tree_view_files_source_mode::show_single) ? 
				sound_object_type::single : ((get_mode()==tree_view_files_source_mode::show_composite) ? 
					sound_object_type::composite : sound_object_type::collection);
			data->SetData("mode", (u32)m);
			if((ModifierKeys & Keys::Control) == Keys::Control)
				m_raw_files_tree_view->DoDragDrop(data, DragDropEffects::Copy);
			else
				m_raw_files_tree_view->DoDragDrop(data, DragDropEffects::Move);
		}
	}
}

void tree_view_panel::switch_mode ( u32 index )
{
	tree_view_files_source^ s = safe_cast<tree_view_files_source^>(m_raw_files_tree_view->source);
	switch(index)
	{
		case 0: 
			{
				s->mode = tree_view_files_source_mode::show_single; 
				extension = ".wav";
				break;
			}
		case 1: 
			{
				s->mode = tree_view_files_source_mode::show_composite; 
				extension = ".composite_sound";
				break;
			}
		case 2: 
			{
				s->mode = tree_view_files_source_mode::show_collections; 
				extension = ".sound_collection";
				break;
			}
		case 3: 
			{
				s->mode = tree_view_files_source_mode::show_all; 
				break;
			}
		default: NODEFAULT();
	}

	s->refresh();
}

void tree_view_panel::on_tab_changed(Object^, TabControlEventArgs^ e)
{
	switch_mode(e->TabPageIndex);
}

void tree_view_panel::menu_opened(Object^, EventArgs^)
{
	addToolStripMenuItem->Visible = true;
	newFileToolStripMenuItem->Visible = true;
	newFolderToolStripMenuItem->Visible = true;
	applyChangesToolStripMenuItem->Visible = true;
	revertChangesToolStripMenuItem->Visible = true;
	addToCollectionToolStripMenuItem->Visible = true;
	cutToolStripMenuItem->Visible = true;
	copyToolStripMenuItem->Visible = true;
	removeItemToolStripMenuItem->Visible = true;
	renameItemToolStripMenuItem->Visible = true;
	pasteToolStripMenuItem->Enabled = m_copied_node!=nullptr;

	if(m_raw_files_tree_view->selected_nodes->Count>0)
	{
		if(m_raw_files_tree_view->selected_nodes[0]->m_node_type==tree_node_type::single_item)
			addToolStripMenuItem->Visible = false;

		if(!m_parent->is_active_collection())
			addToCollectionToolStripMenuItem->Visible = false;
	}
	else
	{
		applyChangesToolStripMenuItem->Visible = false;
		revertChangesToolStripMenuItem->Visible = false;
		addToCollectionToolStripMenuItem->Visible = false;
		cutToolStripMenuItem->Visible = false;
		copyToolStripMenuItem->Visible = false;
		removeItemToolStripMenuItem->Visible = false;
		renameItemToolStripMenuItem->Visible = false;
	}

	switch(get_mode())
	{
		case tree_view_files_source_mode::show_single: 
			{
				newFileToolStripMenuItem->Visible = false;
				if(!m_parent->check_modified(m_raw_files_tree_view->selected_nodes))
				{
					applyChangesToolStripMenuItem->Visible = false;
					revertChangesToolStripMenuItem->Visible = false;
				}

				break;
			}
		case tree_view_files_source_mode::show_composite:
			{
				if(!m_parent->check_modified(m_raw_files_tree_view->selected_nodes))
				{
					applyChangesToolStripMenuItem->Visible = false;
					revertChangesToolStripMenuItem->Visible = false;
				}

				break;
			}
		case tree_view_files_source_mode::show_collections:
			{
				if(!m_parent->check_modified(m_raw_files_tree_view->selected_nodes))
				{
					applyChangesToolStripMenuItem->Visible = false;
					revertChangesToolStripMenuItem->Visible = false;
				}

				break;
			}
		case tree_view_files_source_mode::show_all:
			{
				applyChangesToolStripMenuItem->Visible = false;
				revertChangesToolStripMenuItem->Visible = false;
				break;
			}
		default: NODEFAULT();
	}
}

void tree_view_panel::new_file_click(Object^, EventArgs^)
{
	int nodes_count = m_raw_files_tree_view->selected_nodes->Count;
	if(nodes_count>0 && m_raw_files_tree_view->selected_nodes[0]->m_node_type==tree_node_type::single_item)
		return;
	
	String^ node_path = (nodes_count>0) ? m_raw_files_tree_view->selected_nodes[0]->FullPath+"/new_file" : "new_file";
	int i = 1;
	String^ absolute_path = get_absolute_path();
	String^ file_path = absolute_path + node_path + i + extension;
	while(System::IO::File::Exists(file_path))
	{
		i++;
		file_path = absolute_path + node_path + i + extension;
	}

	node_path += i;
	tree_node^ n = m_raw_files_tree_view->add_item(node_path, editor_base::node_sound_file);
	do_fs_operation(fs_operation_type::file_create, node_path, nullptr);
	n->BeginEdit();
}

void tree_view_panel::new_folder_click(Object^, EventArgs^)
{
	int nodes_count = m_raw_files_tree_view->selected_nodes->Count;
	if(nodes_count>0 && m_raw_files_tree_view->selected_nodes[0]->m_node_type==tree_node_type::single_item)
		return;
	
	String^ node_path = (nodes_count>0) ? m_raw_files_tree_view->selected_nodes[0]->FullPath+"/new_folder" : "new_folder";
	String^ absolute_path = get_absolute_path();
	String^ folder_path = absolute_path + node_path;
	int i = 1;
	while(System::IO::Directory::Exists(folder_path + i))
		i++;

	node_path += i;
	tree_node^ n = m_raw_files_tree_view->add_group(node_path, editor_base::folder_closed, editor_base::folder_open);
	do_fs_operation(fs_operation_type::folder_create, node_path, nullptr);
	n->BeginEdit();
}

void tree_view_panel::apply_changes_click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
		lst->Add(n->FullPath);

	m_parent->apply_changes(lst);
}

void tree_view_panel::revert_changes_click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
		lst->Add(n->FullPath);

	m_parent->revert_changes(lst);
}

void tree_view_panel::add_to_collection_click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
		lst->Add(n->FullPath);

	sound_object_type m = (get_mode()==tree_view_files_source_mode::show_single) ? 
		sound_object_type::single : ((get_mode()==tree_view_files_source_mode::show_composite) ? 
			sound_object_type::composite : sound_object_type::collection);
	m_parent->add_items_to_collection(lst, nullptr, m);
}

void tree_view_panel::cut_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	m_copied_node = m_raw_files_tree_view->selected_nodes[0];
	m_is_cutting_node = true;
}

void tree_view_panel::copy_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	m_copied_node = m_raw_files_tree_view->selected_nodes[0];
}

void tree_view_panel::paste_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	if(m_copied_node!=nullptr)
		copy_nodes_impl(m_copied_node, m_raw_files_tree_view->selected_nodes[0], m_is_cutting_node);
}

void tree_view_panel::remove_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	if(m_raw_files_tree_view->selected_nodes->Contains(m_copied_node))
		m_copied_node = nullptr;

	for each(tree_node^ n in m_raw_files_tree_view->selected_nodes)
	{
		String^ node_path = n->FullPath;
		if(m_editor->is_opened(node_path))
			continue;

		do_fs_operation((n->m_node_type==tree_node_type::single_item) ? 
				fs_operation_type::file_remove : 
				fs_operation_type::folder_remove, 
			nullptr, 
			node_path
		);
		m_raw_files_tree_view->remove_item(node_path);
	}
}

void tree_view_panel::rename_click(Object^, EventArgs^)
{
	if(m_raw_files_tree_view->selected_nodes->Count==0)
		return;

	m_raw_files_tree_view->selected_nodes[0]->BeginEdit();
}

xray::sound_editor::tree_view_files_source_mode	tree_view_panel::get_mode()
{
	tree_view_files_source^ s = safe_cast<tree_view_files_source^>(m_raw_files_tree_view->source);
	return s->mode;
}

void tree_view_panel::copy_nodes_impl(tree_node^ s, tree_node^ d, bool del_old)
{
	R_ASSERT(s!=nullptr);
	if(d!=nullptr && d->m_node_type == tree_node_type::single_item)
		d = d->Parent;

	String^ absolute_path = get_absolute_path();
	String^ source_node_path = s->FullPath;
	String^ dest_node_path = (d!=nullptr) ? (d->FullPath + "/" + s->Name) : s->Name;
	String^ dest_file_path = absolute_path + dest_node_path;
	if(System::IO::File::Exists(dest_file_path + extension) ||
		System::IO::Directory::Exists(dest_file_path))
	{
		MessageBox::Show(this, "File or folder ["+dest_node_path+"] already exists.", 
			"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}

	if(d!=nullptr && check_parent(d->FullPath, s->Name))
	{
		MessageBox::Show(this, "Can't copy ["+source_node_path+"] into ["+dest_node_path+"].", 
			"Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}

	if(del_old)
	{
		do_fs_operation((s->m_node_type==tree_node_type::single_item) ? 
				fs_operation_type::file_rename :
				fs_operation_type::folder_rename, 
			dest_node_path, 
			source_node_path
		);
		m_raw_files_tree_view->remove_item(source_node_path);
	}
	else
	{
		do_fs_operation((s->m_node_type==tree_node_type::single_item) ?
				fs_operation_type::file_copy :
				fs_operation_type::folder_copy, 
			dest_node_path, 
			source_node_path
		);
	}

	if(s->m_node_type==tree_node_type::single_item)
		m_raw_files_tree_view->add_item(dest_node_path, editor_base::node_sound_file);
	else
		add_folders_and_files(s->nodes, dest_node_path, editor_base::folder_closed, editor_base::folder_open, editor_base::node_sound_file);

	m_copied_node = nullptr;
	m_is_cutting_node = false;
	m_raw_files_tree_view->Sort();
	track_active_item(dest_node_path);
}

String^	tree_view_panel::get_absolute_path()
{
	String^ absolute_path = System::IO::Path::GetFullPath(sound_editor::absolute_sound_resources_path)->Replace("\\", "/");
	if(get_mode()==tree_view_files_source_mode::show_single)
		absolute_path += "single/";
	else if(get_mode()==tree_view_files_source_mode::show_composite)
		absolute_path += "composite/";
	else if(get_mode()==tree_view_files_source_mode::show_collections)
		absolute_path += "collections/";

	return absolute_path;
}

void tree_view_panel::remove_parent_sounds(tree_node^ s)
{
	R_ASSERT(s!=nullptr);
	if(s->m_node_type==tree_node_type::single_item)
	{
		sound_object_type m = (get_mode()==tree_view_files_source_mode::show_single) ? 
			sound_object_type::single : ((get_mode()==tree_view_files_source_mode::show_composite) ? 
				sound_object_type::composite : sound_object_type::collection);
		m_parent->remove_sound(s->FullPath, m);
	}
	else
	{
		for each(tree_node^ n in s->nodes)
			remove_parent_sounds(n);
	}
}

void tree_view_panel::do_fs_operation(tree_view_panel::fs_operation_type t, String^ new_node_path, String^ old_node_path)
{
	using namespace System::IO;
	tree_view_files_source_mode m = get_mode();
	String^ absolute_path = get_absolute_path();
	String^ new_item_path = absolute_path + new_node_path;
	String^ old_item_path = absolute_path + old_node_path;
	try
	{
		switch(t)
		{
		case fs_operation_type::file_create:
			{
				if(get_mode()==tree_view_files_source_mode::show_collections)
				{
					String^ res_path = sound_editor::sound_resources_path;
					unmanaged_string unm_res_path(res_path);
					xray::fs_new::virtual_path_string file_name = unm_res_path.c_str();
					file_name += "collections/";
					file_name += unmanaged_string(new_node_path).c_str();
					file_name += unmanaged_string(extension).c_str();
					xray::configs::lua_config_ptr cfg_ptr = xray::configs::create_lua_config(file_name.c_str());
					xray::configs::lua_config_value root = cfg_ptr->get_root()["collection"];
					root["cyclic_repeat_from_sound"] = 0;
					root["dont_repeat_sound_successively"] = false;
					root["max_delay"] = 0;
					root["min_delay"] = 0;
					root["type"] = "random";
					cfg_ptr->save(configs::target_sources);
				}
				else if(get_mode()==tree_view_files_source_mode::show_composite)
				{
					String^ res_path = sound_editor::sound_resources_path;
					unmanaged_string unm_res_path(res_path);
					xray::fs_new::virtual_path_string file_name = unm_res_path.c_str();
					file_name += "composite/";
					file_name += unmanaged_string(new_node_path).c_str();
					file_name += unmanaged_string(extension).c_str();
					xray::configs::lua_config_ptr cfg_ptr = xray::configs::create_lua_config(file_name.c_str());
					xray::configs::lua_config_value root = cfg_ptr->get_root()["composite_sound"];
					root["after_playing_pause"]		= 0;
					root["before_playing_pause"]	= 0;
					cfg_ptr->save(configs::target_sources);
				}
				else
					File::Create(new_item_path + extension);

				track_active_item(new_node_path);
				break;
			}
		case fs_operation_type::file_rename:
			{
				sound_object_type st = (m==tree_view_files_source_mode::show_single) ? 
					sound_object_type::single : ((m==tree_view_files_source_mode::show_composite) ? 
						sound_object_type::composite : sound_object_type::collection);
				m_parent->remove_sound(old_node_path, st);
				if(get_mode()==tree_view_files_source_mode::show_single)
				{
					File::Move(old_item_path + ".raw_options", new_item_path + ".raw_options");
					File::Move(old_item_path + ".single_sound_options", new_item_path + ".single_sound_options");
				}

				File::Move(old_item_path + extension, new_item_path + extension);
				break;
			}
		case fs_operation_type::file_remove:
			{
				sound_object_type m = (get_mode()==tree_view_files_source_mode::show_single) ? 
					sound_object_type::single : ((get_mode()==tree_view_files_source_mode::show_composite) ? 
						sound_object_type::composite : sound_object_type::collection);
				m_parent->remove_sound(old_node_path, m);
				if(get_mode()==tree_view_files_source_mode::show_single)
				{
					File::Delete(old_item_path + ".raw_options");
					File::Delete(old_item_path + ".single_sound_options");
				}

				File::Delete(old_item_path + extension);
				break;
			}
		case fs_operation_type::file_copy:
			{
				File::Copy(old_item_path + extension, new_item_path + extension);
				if(get_mode()==tree_view_files_source_mode::show_single)
				{
					File::Copy(old_item_path + ".raw_options", new_item_path + ".raw_options");
					File::Copy(old_item_path + ".single_sound_options", new_item_path + ".single_sound_options");
				}

				break;
			}
		case fs_operation_type::folder_create:
			{
				Directory::CreateDirectory(new_item_path);
				track_active_item(new_node_path);
				break;
			}
		case fs_operation_type::folder_rename:
			{
				remove_parent_sounds(m_raw_files_tree_view->get_node(new_node_path));
				Directory::Move(old_item_path, new_item_path);
				break;
			}
		case fs_operation_type::folder_remove:
			{
				remove_parent_sounds(m_raw_files_tree_view->get_node(old_node_path));
				Directory::Delete(old_item_path, true);
				break;
			}
		case fs_operation_type::folder_copy:
			{
				DirectoryInfo^ di = gcnew DirectoryInfo(old_node_path);
				iterate_folders(di, new_item_path);
				delete di;
				break;
			}
		default:
			NODEFAULT();
		}
	}
	catch(IOException^ e)
	{
		R_ASSERT(false, "%s", e->ToString());
	}
}

ReadOnlyCollection<tree_node^>^ tree_view_panel::get_selected_nodes()
{
	return m_raw_files_tree_view->selected_nodes;
}


bool tree_view_panel::is_selected_node(String^ path)
{
	tree_node^ n = m_raw_files_tree_view->get_node(path);
	if(n==nullptr)
		return false;

	return m_raw_files_tree_view->selected_nodes->Contains(n);
}