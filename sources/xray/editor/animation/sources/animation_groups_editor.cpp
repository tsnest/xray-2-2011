////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_groups_editor.h"
#include "animation_groups_toolbar.h"
#include "animation_groups_controllers_tree.h"
#include "animation_groups_animations_tree.h"
#include "animation_groups_document.h"
#include "animation_editor.h"
#include "learning_properties_form.h"
#include "animation_group_object.h"
#include "animation_node_interval.h"
#include "animation_node_interval_instance.h"

using xray::animation_editor::animation_groups_editor;
using xray::animation_editor::animation_editor;

animation_groups_editor::animation_groups_editor(animation_editor^ ed)
:m_animation_editor(ed)
{
	InitializeComponent();
	m_multidocument_base = gcnew xray::editor::controls::document_editor_base("animation_groups_editor");
	m_multidocument_base->main_dock_panel->DockBackColor = System::Drawing::SystemColors::AppWorkspace;
	m_multidocument_base->creating_new_document = gcnew xray::editor::controls::document_create_callback(this, &animation_groups_editor::on_document_creating);
	m_multidocument_base->content_reloading = gcnew xray::editor::controls::content_reload_callback(this, &animation_groups_editor::find_dock_content);
	m_multidocument_base->Dock = DockStyle::Fill;

	m_toolbar = gcnew animation_groups_toolbar(m_multidocument_base);
	m_controllers_tree = gcnew animation_groups_controllers_tree(this);
	m_animations_tree = gcnew animation_groups_animations_tree(m_multidocument_base);
	bool loaded = m_multidocument_base->load_panels(this);
	if(!loaded)
	{
		m_controllers_tree->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft);
		m_animations_tree->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockRight);
		m_toolbar->Show(m_animations_tree->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Top, 0.25f);
	}

	m_menu = gcnew MenuStrip();
	m_menu_file = gcnew ToolStripMenuItem();
	m_menu_file_new = gcnew ToolStripMenuItem();
	m_menu_file_save = gcnew ToolStripMenuItem();
	m_menu_file_save_as = gcnew ToolStripMenuItem();
	m_menu_file_select_model = gcnew ToolStripMenuItem();
	m_menu_file_learn = gcnew ToolStripMenuItem();
	m_menu_file_show_animations = gcnew ToolStripMenuItem();
	m_menu_edit = gcnew ToolStripMenuItem();
	m_menu_edit_undo = gcnew ToolStripMenuItem();
	m_menu_edit_redo = gcnew ToolStripMenuItem();
	m_menu_edit_cut = gcnew ToolStripMenuItem();
	m_menu_edit_copy = gcnew ToolStripMenuItem();
	m_menu_edit_paste = gcnew ToolStripMenuItem();
	m_menu_edit_select_all = gcnew ToolStripMenuItem();
	m_menu_edit_del = gcnew ToolStripMenuItem();
	m_menu_edit_collapse_all = gcnew ToolStripMenuItem();
	m_menu_view = gcnew ToolStripMenuItem();
	m_menu_view_toolbar = gcnew ToolStripMenuItem();
	m_menu_view_controllers_tree = gcnew ToolStripMenuItem();
	m_menu_view_animations_tree = gcnew ToolStripMenuItem();
	m_menu->SuspendLayout();
	// m_menu
	m_menu->Items->AddRange(gcnew cli::array<ToolStripItem^ >(3) 
		{
			m_menu_file, 
			m_menu_edit,
			m_menu_view
		}
	);
	m_menu->Location = System::Drawing::Point(0, 0);
	m_menu->Name = L"m_menu";
	m_menu->Size = System::Drawing::Size(292, 24);
	m_menu->TabIndex = 1;
	m_menu->Text = L"m_menu";

	// m_menu_file
	m_menu_file->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^ >(6) 
		{
			m_menu_file_new,
			m_menu_file_save,
			m_menu_file_save_as,
			m_menu_file_select_model,
			m_menu_file_learn,
			m_menu_file_show_animations
		}
	);
	m_menu_file->Name = L"m_menu_file";
	m_menu_file->Size = System::Drawing::Size(37, 20);
	m_menu_file->Text = L"&File";

	// m_menu_file_new
	m_menu_file_new->Name = L"m_menu_file_new";
	m_menu_file_new->Size = System::Drawing::Size(197, 22);
	m_menu_file_new->Text = L"&New controller";

	// m_menu_file_save
	m_menu_file_save->Name = L"m_menu_file_save";
	m_menu_file_save->Size = System::Drawing::Size(197, 22);
	m_menu_file_save->Text = L"&Save";

	// m_menu_file_save_as
	m_menu_file_save_as->Name = L"m_menu_file_save_as";
	m_menu_file_save_as->Size = System::Drawing::Size(197, 22);
	m_menu_file_save_as->Text = L"Save &as";

	// m_menu_file_select_model
	m_menu_file_select_model->Name = L"m_menu_file_select_model";
	m_menu_file_select_model->Size = System::Drawing::Size(197, 22);
	m_menu_file_select_model->Text = L"Select &model";

	// m_menu_file_learn
	m_menu_file_learn->Name = L"m_menu_file_learn";
	m_menu_file_learn->Size = System::Drawing::Size(197, 22);
	m_menu_file_learn->Text = L"&Learn controller";
	
	// m_menu_file_show_animations
	m_menu_file_show_animations->Name = L"m_menu_file_show_animations";
	m_menu_file_show_animations->Size = System::Drawing::Size(197, 22);
	m_menu_file_show_animations->Text = L"Sho&w animations";

	// m_menu_edit
	m_menu_edit->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^ >(8) 
		{
			m_menu_edit_undo,
			m_menu_edit_redo,
			m_menu_edit_cut,
			m_menu_edit_copy,
			m_menu_edit_paste,
			m_menu_edit_select_all,
			m_menu_edit_del,
			m_menu_edit_collapse_all
		}
	);
	m_menu_edit->Name = L"m_menu_edit";
	m_menu_edit->Size = System::Drawing::Size(37, 20);
	m_menu_edit->Text = L"&Edit";

	// m_menu_edit_undo
	m_menu_edit_undo->Name = L"m_menu_edit_undo";
	m_menu_edit_undo->Size = System::Drawing::Size(197, 22);
	m_menu_edit_undo->Text = L"&Undo";

	// m_menu_edit_redo
	m_menu_edit_redo->Name = L"m_menu_edit_redo";
	m_menu_edit_redo->Size = System::Drawing::Size(197, 22);
	m_menu_edit_redo->Text = L"&Redo";

	// m_menu_edit_cut
	m_menu_edit_cut->Name = L"m_menu_edit_cut";
	m_menu_edit_cut->Size = System::Drawing::Size(197, 22);
	m_menu_edit_cut->Text = L"Cu&t";

	// m_menu_edit_copy
	m_menu_edit_copy->Name = L"m_menu_edit_copy";
	m_menu_edit_copy->Size = System::Drawing::Size(197, 22);
	m_menu_edit_copy->Text = L"&Copy";

	// m_menu_edit_paste
	m_menu_edit_paste->Name = L"m_menu_edit_paste";
	m_menu_edit_paste->Size = System::Drawing::Size(197, 22);
	m_menu_edit_paste->Text = L"&Paste";

	// m_menu_edit_select_all
	m_menu_edit_select_all->Name = L"m_menu_edit_select_all";
	m_menu_edit_select_all->Size = System::Drawing::Size(197, 22);
	m_menu_edit_select_all->Text = L"Select &All";

	// m_menu_edit_del
	m_menu_edit_del->Name = L"m_menu_edit_del";
	m_menu_edit_del->Size = System::Drawing::Size(197, 22);
	m_menu_edit_del->Text = L"&Delete";

	// m_menu_edit_collapse_all
	m_menu_edit_collapse_all->Name = L"m_menu_edit_collapse_all";
	m_menu_edit_collapse_all->Size = System::Drawing::Size(197, 22);
	m_menu_edit_collapse_all->Text = L"C&ollapse All";

	// m_menu_view
	m_menu_view->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^ >(3) 
		{
			m_menu_view_toolbar,
			m_menu_view_controllers_tree,
			m_menu_view_animations_tree
		}
	);
	m_menu_view->Name = L"m_menu_view";
	m_menu_view->Size = System::Drawing::Size(37, 20);
	m_menu_view->Text = L"&View";
	m_menu_view->DropDownOpening += gcnew System::EventHandler(this, &animation_groups_editor::view_on_drop_down_opening);

	// m_menu_view_toolbar
	m_menu_view_toolbar->Name = L"m_menu_view_toolbar";
	m_menu_view_toolbar->Size = System::Drawing::Size(197, 22);
	m_menu_view_toolbar->Text = L"&Toolbar";
	m_menu_view_toolbar->Click += gcnew System::EventHandler(this, &animation_groups_editor::show_toolbar);

	// m_menu_view_controllers_tree
	m_menu_view_controllers_tree->Name = L"m_menu_view_controllers_tree";
	m_menu_view_controllers_tree->Size = System::Drawing::Size(197, 22);
	m_menu_view_controllers_tree->Text = L"&Controllers tree";
	m_menu_view_controllers_tree->Click += gcnew System::EventHandler(this, &animation_groups_editor::show_controllers_tree);

	// m_menu_view_animations_tree
	m_menu_view_animations_tree->Name = L"m_menu_view_animations_tree";
	m_menu_view_animations_tree->Size = System::Drawing::Size(197, 22);
	m_menu_view_animations_tree->Text = L"&Animations tree";
	m_menu_view_animations_tree->Click += gcnew System::EventHandler(this, &animation_groups_editor::show_animations_tree);

	m_menu->ResumeLayout(false);
	m_menu->PerformLayout();			
	m_multidocument_base->add_menu_items(m_menu);
	Controls->Add(m_multidocument_base);
}

animation_groups_editor::~animation_groups_editor()
{
	if (components)
		delete components;
}

void animation_groups_editor::tick()
{
}

void animation_groups_editor::clear_resources()
{
	delete m_animations_tree;
	delete m_controllers_tree;
	delete m_toolbar;
	delete m_multidocument_base;
}

bool animation_groups_editor::close_query()
{
	m_multidocument_base->save_panels(this);
	u32 unsaved_docs_counter = 0;
	System::String^ txt = "";
	for each(animation_groups_document^ doc in m_multidocument_base->opened_documents)
	{
		if(!(doc->is_saved))
		{
			txt += doc->Name+".lua\n";
			++unsaved_docs_counter;
		}
	}

	if(unsaved_docs_counter==0)
	{
		System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
		lst->AddRange(m_multidocument_base->opened_documents);
		for each(animation_groups_document^ doc in lst)
			doc->Close();

		return true;
	}

	::DialogResult res = MessageBox::Show(this, "Save changes to the following items?\n\n"+txt, 
		"Animation groups", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question);

	if(res== ::DialogResult::Cancel)
		return false;
	else if(res== ::DialogResult::Yes)
	{
		for each(animation_groups_document^ doc in m_multidocument_base->opened_documents)
		{
			if(!(doc->is_saved))
				doc->save();
		}
	}

	System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
	lst->AddRange(m_multidocument_base->opened_documents);
	for each(animation_groups_document^ doc in lst)
	{
		doc->is_saved = true;
		doc->Close();
	}

	m_multidocument_base->active_document = nullptr;
	return true;
}

document_base^ animation_groups_editor::on_document_creating()
{
	return gcnew animation_groups_document(this);
}

IDockContent^ animation_groups_editor::find_dock_content(System::String^ persist_string)
{
	if(persist_string=="xray.animation_editor.animation_groups_toolbar")
		return m_toolbar;

	if(persist_string=="xray.animation_editor.animation_groups_controllers_tree")
		return m_controllers_tree;

	if(persist_string=="xray.animation_editor.animation_groups_animations_tree")
		return m_animations_tree;

	return (nullptr);
}

void animation_groups_editor::view_on_drop_down_opening(System::Object^, System::EventArgs^)
{
	m_menu_view_toolbar->Checked = !(m_toolbar->IsHidden);
	m_menu_view_controllers_tree->Checked = !(m_controllers_tree->IsHidden);
	m_menu_view_animations_tree->Checked = !(m_animations_tree->IsHidden);
}

void animation_groups_editor::show_toolbar(System::Object^, System::EventArgs^)
{
	if(m_menu_view_toolbar->Checked)
		m_toolbar->Hide();
	else
		m_toolbar->Show();
}

void animation_groups_editor::show_controllers_tree(System::Object^, System::EventArgs^)
{
	if(m_menu_view_controllers_tree->Checked)
		m_controllers_tree->Hide();
	else
		m_controllers_tree->Show();
}

void animation_groups_editor::show_animations_tree(System::Object^, System::EventArgs^)
{
	if(m_menu_view_animations_tree->Checked)
		m_animations_tree->Hide();
	else
		m_animations_tree->Show();
}

bool animation_groups_editor::predicate_save_scene_as()
{
	if(m_multidocument_base->active_document==nullptr)
		return false;

	return true;
}

bool animation_groups_editor::predicate_save_scene()
{
	if(m_multidocument_base->active_document==nullptr || m_multidocument_base->active_document->is_saved)
		return false;

	return true;
}

bool animation_groups_editor::predicate_learn()
{
	if(m_multidocument_base->active_document==nullptr || !m_multidocument_base->active_document->is_saved)
		return false;

	return true;
}

void animation_groups_editor::save_document_as()
{
	R_ASSERT(m_multidocument_base->active_document!=nullptr);
	safe_cast<animation_groups_document^>(m_multidocument_base->active_document)->save_as();
}

void animation_groups_editor::select_model()
{
	if(m_multidocument_base->active_document==nullptr)
		return;

	animation_groups_document^ d = safe_cast<animation_groups_document^>(m_multidocument_base->active_document);
	System::String^ selected = nullptr;
	if(xray::editor_base::resource_chooser::choose("solid_visual", d->model, nullptr, selected, true))
		d->change_model(selected);
}

void animation_groups_editor::learn_controller()
{
	animation_groups_document^ d = safe_cast<animation_groups_document^>(m_multidocument_base->active_document);
	if(d->can_learn_controller())
	{
		learning_properties_form^ f = gcnew learning_properties_form(m_animation_editor, d->Name);
		f->ShowDialog(this);
	}
}

void animation_groups_editor::show_animations()
{
	if(m_multidocument_base->active_document==nullptr)
		return;

	animation_groups_document^ d = safe_cast<animation_groups_document^>(m_multidocument_base->active_document);
	if(d->hypergraph()->selected_nodes->Count==0)
		return;

	List<String^>^ lst = gcnew List<String^>();
	for each(node^ n in d->hypergraph()->selected_nodes)
	{
		animation_group_object^ gr = d->get_group(System::Guid(n->id));
		R_ASSERT(gr);
		for each(animation_node_interval_instance^ inst in gr->intervals)
		{
			String^ str = inst->ToString()+" => ["+inst->interval->type.ToString()+"]";
			if(!lst->Contains(str))
				lst->Add(str);
		}
	}

	String^ txt = "";
	for each(String^ str in lst)
		txt += str+"\n";

	MessageBox::Show(this, txt, "Animation groups", MessageBoxButtons::OK, MessageBoxIcon::Information);
}

void animation_groups_editor::collapse_all_groups()
{
	if(m_multidocument_base->active_document==nullptr)
		return;

	animation_groups_document^ d = safe_cast<animation_groups_document^>(m_multidocument_base->active_document);
	u32 collapsed_count = 0;
	u32 expanded_count = 0;
	for each(KeyValuePair<System::String^, node^> p in d->hypergraph()->nodes)
		(((properties_node^)p.Value)->is_expanded)?++expanded_count:++collapsed_count;

	for each(KeyValuePair<System::String^, node^> p in d->hypergraph()->nodes)
		((properties_node^)p.Value)->is_expanded = (expanded_count>collapsed_count) ? (collapsed_count!=0) : (expanded_count==0);
}
