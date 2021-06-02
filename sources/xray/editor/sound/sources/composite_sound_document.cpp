////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "composite_sound_document.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "composite_sound_wrapper.h"
#include "single_sound_wrapper.h"
#include "composite_sound_item.h"
#include "sound_collection_item.h"
#include "composite_sound_commands.h"
#include "tree_view_panel.h"

using xray::sound_editor::composite_sound_document;
using xray::sound_editor::sound_editor;
using xray::sound_editor::composite_sound_item;
using xray::sound_editor::sound_collection_item;
using xray::sound_editor::sound_object_wrapper;
using xray::sound_editor::sound_object_type;

composite_sound_document::composite_sound_document(sound_editor^ ed)
:super(ed->multidocument_base),
m_editor(ed)
{
	m_name = "new sound";
	m_first_save = true;
	m_command_engine = gcnew command_engine(100);
	m_sound = nullptr;
	SuspendLayout();
	// m_save_file_dialog
	m_save_file_dialog = gcnew SaveFileDialog();
	m_save_file_dialog->AddExtension = true;
	m_save_file_dialog->DefaultExt = ".sound_collection";
	m_save_file_dialog->Filter = "Composite sound files|*.composite_sound";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath(sound_editor::absolute_sound_resources_path+"/composite");
	m_save_file_dialog->RestoreDirectory = true;
	m_save_file_dialog->SupportMultiDottedExtensions = true;
	m_save_file_dialog->Title = "Save composite sound";

	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	removeToolStripMenuItem = gcnew ToolStripMenuItem();
	addCollectionToolStripMenuItem = gcnew ToolStripMenuItem();

	// m_context_menu
	m_context_menu->SuspendLayout();
	m_context_menu->Items->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(2) {
		addCollectionToolStripMenuItem,
		removeToolStripMenuItem,
	});
	m_context_menu->Name = L"m_context_menu";
	m_context_menu->Size = System::Drawing::Size(153, 92);
	m_context_menu->Opened += gcnew EventHandler(this, &composite_sound_document::menu_opened);

	// removeToolStripMenuItem
	removeToolStripMenuItem->Name = L"removeItemToolStripMenuItem";
	removeToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	removeToolStripMenuItem->Text = L"Remove item";
	removeToolStripMenuItem->Click += gcnew System::EventHandler(this, &composite_sound_document::remove_click);

	// addCollectionToolStripMenuItem
	addCollectionToolStripMenuItem->Name = L"addCollectionToolStripMenuItem";
	addCollectionToolStripMenuItem->Size = System::Drawing::Size(152, 22);
	addCollectionToolStripMenuItem->Text = L"Add new composite sound";
	addCollectionToolStripMenuItem->Click += gcnew System::EventHandler(this, &composite_sound_document::add_collection_click);

	m_context_menu->ResumeLayout(false);

	// m_tree_view_image_list
	xray::editor_base::image_loader	loader;
	m_tree_view_image_list = loader.get_shared_images16();
	m_tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;

	// m_tree_view
	m_tree_view = gcnew tree_view();
	m_tree_view->AllowDrop = true;
	m_tree_view->ContextMenuStrip = m_context_menu;
	m_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
	m_tree_view->ImageList = m_tree_view_image_list;
	m_tree_view->is_multiselect = true;
	m_tree_view->Name = L"m_tree_view";
	m_tree_view->TabIndex = 0;
	m_tree_view->DragOver += gcnew DragEventHandler(this, &composite_sound_document::on_drag_over);
	m_tree_view->DragDrop += gcnew DragEventHandler(this, &composite_sound_document::on_drag_drop);
	m_tree_view->selected_items_changed += gcnew EventHandler<xray::editor::controls::tree_view_selection_event_args^>(this, &composite_sound_document::on_selection_changed);

	AllowEndUserDocking = false;
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	ClientSize = System::Drawing::Size(562, 439);
	Controls->Add(m_tree_view);
	VisibleChanged += gcnew System::EventHandler(this, &composite_sound_document::on_document_activated);
	ResumeLayout(false);
}

composite_sound_document::~composite_sound_document()
{
	delete m_command_engine;
	delete m_save_file_dialog;
	delete m_tree_view_image_list;
	delete m_tree_view;
}

void composite_sound_document::save()
{
	if(is_saved)
		return;
	
	if(m_first_save)
	{
		if(m_save_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
		{
			System::String^ name_with_ext = m_save_file_dialog->FileName->Remove(0, m_save_file_dialog->InitialDirectory->Length+1);
			Name = name_with_ext->Remove(name_with_ext->Length-16)->Replace("\\", "/");
			Text = Name;
			m_sound = gcnew composite_sound_wrapper(this->Name);
			m_sound->save();
			load();
		}

		return;
	}

	m_sound->save();
	m_command_engine->set_saved_flag();
	is_saved = true;
	m_first_save = false;
}

void composite_sound_document::load()
{
	m_sound = safe_cast<composite_sound_wrapper^>(m_editor->get_sound(this->Name, sound_object_type::composite));
	if(m_sound!=nullptr)
	{
		if(m_sound->is_loaded)
			return on_resources_loaded(nullptr, nullptr);

		m_sound->on_loaded += gcnew EventHandler(this, &composite_sound_document::on_resources_loaded);
	}
}

void composite_sound_document::on_resources_loaded(Object^, EventArgs^)
{
	if(m_sound!=nullptr && m_sound->is_loaded)
	{
		m_command_engine->clear_stack();
		m_command_engine->set_saved_flag();
		is_saved = true;
		m_first_save = false;
		m_tree_view->nodes->Clear();
		m_name = this->Name;
		int ind = m_name->LastIndexOf("/");
		if(ind>0)
			m_name = m_name->Remove(0, ind+1);

		tree_node^ n = m_tree_view->add_group(m_name, editor_base::folder_closed, editor_base::folder_open);
		fill_tree_view(n, m_sound->items());
		n->Expand();
	}
}

void composite_sound_document::fill_tree_view(tree_node^ root, List<sound_collection_item^>^ itms)
{
	for each(sound_collection_item^ itm in itms)
	{
		String^ name = itm->sound_object->name;
		int ind = name->LastIndexOf("/");
		if(ind>0)
			name = name->Remove(0, ind+1);
		
		String^ path = root->FullPath + "/" + name;
		tree_node^ n = nullptr;
		if(itm->sound_type==sound_object_type::single)
			n = m_tree_view->add_item(path, editor_base::node_sound_file);
		else
		{
			n = m_tree_view->add_group(path, editor_base::folder_closed, editor_base::folder_open);
			fill_tree_view(n, itm->sound_object->items());
		}

		n->Tag = itm;
	}
}

void composite_sound_document::undo()
{
	m_command_engine->rollback(-1);
	is_saved = m_command_engine->saved_flag();
}

void composite_sound_document::redo()
{
	m_command_engine->rollback(1);
	is_saved = m_command_engine->saved_flag();
}

void composite_sound_document::copy(bool del)
{
	XRAY_UNREFERENCED_PARAMETER(del);
}

void composite_sound_document::paste()
{
}

void composite_sound_document::select_all()
{
	m_tree_view->select_all();
}

void composite_sound_document::del()
{
	remove_click(nullptr, nullptr);
}

void composite_sound_document::on_document_activated(Object^, EventArgs^)
{
	if(Visible)
	{
		m_editor->clear_properties();
		on_selection_changed(nullptr, nullptr);
	}
}

void composite_sound_document::on_property_changed(Object^, value_changed_event_args^ e)
{
	List<tree_node^>^ lst = gcnew List<tree_node^>(m_tree_view->selected_nodes);
	is_saved &= !m_command_engine->run(gcnew composite_sound_change_objects_property_command(this, lst, e->changed_property->name, e->old_value, e->new_value));
}

void composite_sound_document::on_drag_over(Object^, DragEventArgs^ e)
{
	if(!e->Data->GetDataPresent("tree_view_item"))
	{
		e->Effect = DragDropEffects::None;
		return;
	}
	else
		e->Effect = DragDropEffects::Move;
}

void composite_sound_document::on_drag_drop(Object^, DragEventArgs^ e)
{
	if(m_sound==nullptr)
	{
		e->Effect = DragDropEffects::None;
		MessageBox::Show(this, "Save document first, please!", "Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}

	if(e->Data->GetDataPresent("tree_view_item"))
	{
		tree_node_type tnt = safe_cast<tree_node_type>(e->Data->GetData("item_type"));
		if(tnt!=tree_node_type::single_item)
		{
			e->Effect = DragDropEffects::None;
			return;
		}

		System::Drawing::Point node_position = m_tree_view->PointToClient(System::Drawing::Point(e->X, e->Y));
		tree_node^ dest_node = safe_cast<tree_node^>(m_tree_view->GetNodeAt(node_position));
		List<String^>^ lst = safe_cast<List<String^>^>(e->Data->GetData("tree_view_item"));
		sound_object_type t = (sound_object_type)safe_cast<u32>(e->Data->GetData("mode"));
		for each(String^ drag_node_name in lst)
		{
			tree_node^ drag_node = m_editor->files_view->get_node(drag_node_name);
			R_ASSERT(drag_node!=nullptr);
			if(drag_node->m_node_type==tree_node_type::single_item && 
				!(m_editor->get_sound(drag_node_name, t)->is_loaded))
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
				col = m_tree_view->nodes;

			if(dest_node==drag_node || col->Contains(drag_node))
			{
				e->Effect = DragDropEffects::None;
				return;
			}
		}

		m_editor->add_items_to_collection(lst, dest_node, t);
	}
	else
		e->Effect = DragDropEffects::None;
}

void composite_sound_document::on_selection_changed(Object^, xray::editor::controls::tree_view_selection_event_args^)
{
	if(m_tree_view->selected_nodes->Count==0)
		return m_editor->clear_properties();

	List<sound_collection_item^>^ l = gcnew List<sound_collection_item^>();
	for each(tree_node^ node in m_tree_view->selected_nodes)
	{
		sound_collection_item^ itm = safe_cast<sound_collection_item^>(node->Tag);
		if(itm==nullptr)
		{
			//R_ASSERT(node->FullPath==m_name);
			//l->Add(m_sound);
			continue;
		}

		if(itm->sound_object->is_loaded/* && !dynamic_cast<single_sound_wrapper^>(itm->sound_object)*/)
			l->Add(itm/*->sound_object*/);
	}

	array<sound_collection_item^>^ lst = gcnew array<sound_collection_item^>(l->Count);
	u32 i = 0;
	for each(sound_collection_item^ w in l)
		lst[i++] = w;

	if(i>0)
		m_editor->show_properties(lst, false);
	else
		m_editor->clear_properties();
}

void composite_sound_document::add_items_to_collection(List<String^>^ names_list, tree_node^ dest_node, sound_object_type t)
{
	tree_node^ root = dest_node;
	if(root==nullptr)
		root = safe_cast<tree_node^>(m_tree_view->nodes[0]);

	is_saved &= !m_command_engine->run(gcnew composite_sound_add_items_command(this, root->FullPath, names_list, t));
}

void composite_sound_document::add_item_to_collection(tree_node^ dest, composite_sound_item^ item)
{
	sound_object_wrapper^ w = nullptr;
	if(dest->Tag==nullptr)
		w = m_sound;
	else
	{
		composite_sound_item^ itm = safe_cast<composite_sound_item^>(dest->Tag);
		w = itm->sound_object;
	}

	w->add_item(item);
	List<sound_collection_item^>^ lst = gcnew List<sound_collection_item^>();
	lst->Add(item);
	fill_tree_view(dest, lst);
}

tree_node^ composite_sound_document::get_node_by_tag(TreeNodeCollection^ root, composite_sound_item^ tag)
{
	tree_node^ res = nullptr;
	for each(tree_node^ n in root)
	{
		if(n->Tag==tag)
			return n;

		if(n->nodes->Count>0)
			res = get_node_by_tag(n->nodes, tag);

		if(res!=nullptr)
			return res;
	}

	return nullptr;
}

void composite_sound_document::remove_item_from_collection(composite_sound_item^ item)
{
	tree_node^ n = get_node_by_tag(m_tree_view->nodes, item);
	R_ASSERT(n!=nullptr);
	tree_node^ p = safe_cast<tree_node^>(n->Parent);
	R_ASSERT(p!=nullptr);
	m_tree_view->remove_item(n->FullPath);
	sound_object_wrapper^ w = m_sound;
	if(p->Tag!=nullptr)
		w = safe_cast<composite_sound_item^>(p->Tag)->sound_object;

	w->remove_item(item);
	delete item;
}

void composite_sound_document::menu_opened(Object^, EventArgs^)
{
	addCollectionToolStripMenuItem->Visible = true;
	removeToolStripMenuItem->Visible = true;
	if(m_tree_view->selected_nodes->Count==0)
		removeToolStripMenuItem->Visible = false;
	else
	{
		if(m_tree_view->selected_nodes[0]->m_node_type==tree_node_type::single_item)
			addCollectionToolStripMenuItem->Visible = false;
	}
}

void composite_sound_document::remove_click(Object^, EventArgs^)
{
	if(m_tree_view->selected_nodes->Count==0)
		return;

	List<tree_node^>^ items_list = gcnew List<tree_node^>(m_tree_view->selected_nodes);
	is_saved &= !m_command_engine->run(gcnew composite_sound_remove_items_command(this, items_list));
}

void composite_sound_document::add_collection_click(Object^, EventArgs^)
{
	if(m_sound==nullptr)
	{
		MessageBox::Show(this, "Save document first, please!", "Sound editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return;
	}

	tree_node^ n = safe_cast<tree_node^>(m_tree_view->nodes[0]);
	if(m_tree_view->selected_nodes->Count!=0)
		n = m_tree_view->selected_nodes[0];

	//is_saved &= !m_command_engine->run(gcnew sound_collection_create_collection_command(this, n->FullPath));
}