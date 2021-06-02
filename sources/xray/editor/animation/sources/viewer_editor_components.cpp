////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "viewer_editor.h"
#include "animation_editor.h"
#include "viewer_document.h"
#include "animation_viewer_graph_node_style.h"
#include "animation_viewer_toolbar_panel.h"
#include "animation_viewer_properties_panel.h"
#include "animation_viewer_time_panel.h"
#include "animation_viewer_detailed_options_panel.h"
#include "animation_lexeme_editor.h"
#include "animation_viewer_graph_node_animation.h"

using xray::animation_editor::viewer_editor;
using xray::animation_editor::animation_viewer_graph_node_base;

typedef xray::editor::wpf_controls::property_container wpf_property_container;

void viewer_editor::init_controls()
{
	components = gcnew System::ComponentModel::Container();
	m_node_style_mgr = gcnew animation_viewer_graph_nodes_style_mgr();

	m_multidocument_base = gcnew xray::editor::controls::document_editor_base("animation_viewer_editor");
	m_multidocument_base->is_reuse_single_document = true;
	m_multidocument_base->main_dock_panel->DockBackColor = System::Drawing::SystemColors::AppWorkspace;
	m_multidocument_base->creating_new_document = gcnew xray::editor::controls::document_create_callback(this, &viewer_editor::on_document_creating);
	m_multidocument_base->content_reloading = gcnew xray::editor::controls::content_reload_callback(this, &viewer_editor::find_dock_content);
	m_multidocument_base->document_closed += gcnew System::EventHandler(this, &viewer_editor::on_document_closing);
	m_multidocument_base->document_created += gcnew System::EventHandler<document_event_args^>(this, &viewer_editor::on_new_document_created);
	m_multidocument_base->Dock = DockStyle::Fill;

	m_lexeme_editor = gcnew animation_lexeme_editor(this);
	m_toolbar_panel = gcnew animation_viewer_toolbar_panel(m_multidocument_base);
	m_properties_panel = gcnew animation_viewer_properties_panel(this);
	m_time_panel = gcnew animation_viewer_time_panel(this);
	m_detailed_options_panel = gcnew animation_viewer_detailed_options_panel(this);
	bool loaded = m_multidocument_base->load_panels(this);
	if(!loaded)
	{
		m_toolbar_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockRight);
		m_properties_panel->Show(m_toolbar_panel->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Bottom, 0.75f);
		m_time_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom);
	}

	m_open_file_dialog = gcnew System::Windows::Forms::OpenFileDialog();
	m_menu_strip = gcnew System::Windows::Forms::MenuStrip();
	fileToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	newToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	saveToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	saveasToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	openToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	recentToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	editToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	undoToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	redoToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	cutToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	copyToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	pasteToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	selectAllToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	delToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	viewToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	toolbarToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	propertiesToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	timeToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	timepanelToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	playfrombeginToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	followthumbToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	snaptoframesToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	follownpcToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	detailedweightsandscalesToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	lockToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	saveClipToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	modelToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	addmodelToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	removemodelToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	selectmodelToolStripMenuItem = gcnew System::Windows::Forms::ToolStripMenuItem();
	m_menu_strip->SuspendLayout();

	// m_open_file_dialog
	m_open_file_dialog->AddExtension = true;
	m_open_file_dialog->DefaultExt = ".avs";
	m_open_file_dialog->Filter = "Animation viewer scene files|*.avs";
	m_open_file_dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/sources/animations/viewer_scenes");
	m_open_file_dialog->Multiselect = false;
	m_open_file_dialog->RestoreDirectory = true;
	m_open_file_dialog->Title = "Open animation viewer scene";

	// menuStrip1
	m_menu_strip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
		fileToolStripMenuItem, 
		editToolStripMenuItem,
		viewToolStripMenuItem,
		modelToolStripMenuItem,
		timepanelToolStripMenuItem
	});
	m_menu_strip->Location = System::Drawing::Point(0, 0);
	m_menu_strip->Name = L"m_menu_strip";
	m_menu_strip->Size = System::Drawing::Size(292, 24);
	m_menu_strip->TabIndex = 1;
	m_menu_strip->Text = L"m_menu_strip";

	// fileToolStripMenuItem
	fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
		newToolStripMenuItem, saveToolStripMenuItem, saveasToolStripMenuItem, openToolStripMenuItem, recentToolStripMenuItem});
	fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
	fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
	fileToolStripMenuItem->Text = L"&File";
	fileToolStripMenuItem->DropDownOpening += gcnew System::EventHandler(this, &viewer_editor::file_on_drop_down_opening);

	// newToolStripMenuItem
	newToolStripMenuItem->Name = L"newToolStripMenuItem";
	newToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	newToolStripMenuItem->Text = L"&New scene";

	// saveToolStripMenuItem
	saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
	saveToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	saveToolStripMenuItem->Text = L"&Save scene";

	// saveasToolStripMenuItem
	saveasToolStripMenuItem->Name = L"saveasToolStripMenuItem";
	saveasToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	saveasToolStripMenuItem->Text = L"Save as";

	// openToolStripMenuItem
	openToolStripMenuItem->Name = L"openToolStripMenuItem";
	openToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	openToolStripMenuItem->Text = L"&Open scene";

	// recentToolStripMenuItem
	recentToolStripMenuItem->Name = L"recentToolStripMenuItem";
	recentToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	recentToolStripMenuItem->Text = L"&Recent scenes";
	recentToolStripMenuItem->DropDownOpening += gcnew System::EventHandler(this, &viewer_editor::recent_on_drop_down_opening);
	recent_on_drop_down_opening(nullptr, nullptr);

	// editToolStripMenuItem
	editToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {
		undoToolStripMenuItem, redoToolStripMenuItem, 
		cutToolStripMenuItem, copyToolStripMenuItem, 
		pasteToolStripMenuItem, selectAllToolStripMenuItem, 
		delToolStripMenuItem});
	editToolStripMenuItem->Name = L"editToolStripMenuItem";
	editToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	editToolStripMenuItem->Text = L"&Edit";

	// undoToolStripMenuItem
	undoToolStripMenuItem->Name = L"undoToolStripMenuItem";
	undoToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	undoToolStripMenuItem->Text = L"&Undo";

	// redoToolStripMenuItem
	redoToolStripMenuItem->Name = L"redoToolStripMenuItem";
	redoToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	redoToolStripMenuItem->Text = L"&Redo";

	// cutToolStripMenuItem
	cutToolStripMenuItem->Name = L"cutToolStripMenuItem";
	cutToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	cutToolStripMenuItem->Text = L"&Cut";

	// copyToolStripMenuItem
	copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
	copyToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	copyToolStripMenuItem->Text = L"&Copy";

	// pasteToolStripMenuItem
	pasteToolStripMenuItem->Name = L"pasteToolStripMenuItem";
	pasteToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	pasteToolStripMenuItem->Text = L"&Paste";

	// selectAllToolStripMenuItem
	selectAllToolStripMenuItem->Name = L"selectAllToolStripMenuItem";
	selectAllToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	selectAllToolStripMenuItem->Text = L"&Select All";

	// delToolStripMenuItem
	delToolStripMenuItem->Name = L"delToolStripMenuItem";
	delToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	delToolStripMenuItem->Text = L"&Delete";

	// viewToolStripMenuItem
	viewToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
		toolbarToolStripMenuItem, propertiesToolStripMenuItem, timeToolStripMenuItem});
	viewToolStripMenuItem->Name = L"viewToolStripMenuItem";
	viewToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	viewToolStripMenuItem->Text = L"&View";
	viewToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::on_view_menu_item_select);

	// toolbarToolStripMenuItem
	toolbarToolStripMenuItem->Checked = !(m_toolbar_panel->IsHidden);
	toolbarToolStripMenuItem->CheckOnClick = true;
	toolbarToolStripMenuItem->Name = L"toolbarToolStripMenuItem";
	toolbarToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	toolbarToolStripMenuItem->Text = L"&Toolbar";
	toolbarToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::show_toolbar_panel);

	// propertiesToolStripMenuItem
	propertiesToolStripMenuItem->Checked = !(m_properties_panel->IsHidden);
	propertiesToolStripMenuItem->CheckOnClick = true;
	propertiesToolStripMenuItem->Name = L"propertiesToolStripMenuItem";
	propertiesToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	propertiesToolStripMenuItem->Text = L"&Properties";
	propertiesToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::show_properties_panel);

	// timeToolStripMenuItem
	timeToolStripMenuItem->Checked = !(m_time_panel->IsHidden);
	timeToolStripMenuItem->CheckOnClick = true;
	timeToolStripMenuItem->Name = L"timeToolStripMenuItem";
	timeToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	timeToolStripMenuItem->Text = L"&Time layout";
	timeToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::show_time_panel);

	// timepanelToolStripMenuItem
	timepanelToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {
		playfrombeginToolStripMenuItem, followthumbToolStripMenuItem, snaptoframesToolStripMenuItem, 
		follownpcToolStripMenuItem, detailedweightsandscalesToolStripMenuItem, lockToolStripMenuItem, 
		saveClipToolStripMenuItem
	});
	timepanelToolStripMenuItem->Name = L"timepanelToolStripMenuItem";
	timepanelToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	timepanelToolStripMenuItem->Text = L"&Time panel options";
	timepanelToolStripMenuItem->DropDownOpening += gcnew System::EventHandler(this, &viewer_editor::update_options_items);

	// playfrombeginToolStripMenuItem
	playfrombeginToolStripMenuItem->Checked = play_from_beginning;
	playfrombeginToolStripMenuItem->CheckOnClick = true;
	playfrombeginToolStripMenuItem->Name = L"playfrombeginToolStripMenuItem";
	playfrombeginToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	playfrombeginToolStripMenuItem->Text = L"Play from begin";
	playfrombeginToolStripMenuItem->Click += gcnew System::EventHandler(m_time_panel, &animation_viewer_time_panel::on_play_from_beginning_check_changed);

	// followthumbToolStripMenuItem
	followthumbToolStripMenuItem->Checked = m_time_panel->get_following();
	followthumbToolStripMenuItem->CheckOnClick = true;
	followthumbToolStripMenuItem->Name = L"followthumbToolStripMenuItem";
	followthumbToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	followthumbToolStripMenuItem->Text = L"Follow animation time";
	followthumbToolStripMenuItem->Click += gcnew System::EventHandler(m_time_panel, &animation_viewer_time_panel::on_follow_animation_thumb_check_changed);

	// snaptoframesToolStripMenuItem
	snaptoframesToolStripMenuItem->Checked = m_time_panel->get_snapping();
	snaptoframesToolStripMenuItem->CheckOnClick = true;
	snaptoframesToolStripMenuItem->Name = L"snaptoframesToolStripMenuItem";
	snaptoframesToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	snaptoframesToolStripMenuItem->Text = L"Snap to frames";
	snaptoframesToolStripMenuItem->Click += gcnew System::EventHandler(m_time_panel, &animation_viewer_time_panel::on_snap_to_frames_check_changed);

	// follownpcToolStripMenuItem
	follownpcToolStripMenuItem->Checked = m_camera_follows_npc;
	follownpcToolStripMenuItem->CheckOnClick = true;
	follownpcToolStripMenuItem->Name = L"follownpcToolStripMenuItem";
	follownpcToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	follownpcToolStripMenuItem->Text = L"Camera follows NPC";
	follownpcToolStripMenuItem->Click += gcnew System::EventHandler(m_time_panel, &animation_viewer_time_panel::on_camera_follows_npc_check_changed);

	// detailedweightToolStripMenuItem
	detailedweightsandscalesToolStripMenuItem->Name = L"detailedweightsandscalesToolStripMenuItem";
	detailedweightsandscalesToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	detailedweightsandscalesToolStripMenuItem->Text = L"Detailed weight and time scale options...";
	detailedweightsandscalesToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::on_detailed_weights_and_scales_click);

	// lockToolStripMenuItem
	lockToolStripMenuItem->Checked = get_locked();
	lockToolStripMenuItem->Name = L"lockToolStripMenuItem";
	lockToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	lockToolStripMenuItem->Text = L"Lock/Unlock target";
	
	// saveClipToolStripMenuItem
	saveClipToolStripMenuItem->Checked = get_locked();
	saveClipToolStripMenuItem->Name = L"saveClipToolStripMenuItem";
	saveClipToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	saveClipToolStripMenuItem->Text = L"Save selected node as clip";

	// modelToolStripMenuItem
	modelToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
		addmodelToolStripMenuItem, removemodelToolStripMenuItem, selectmodelToolStripMenuItem});
	modelToolStripMenuItem->Name = L"modelToolStripMenuItem";
	modelToolStripMenuItem->Size = System::Drawing::Size(37, 20);
	modelToolStripMenuItem->Text = L"&Model";
	modelToolStripMenuItem->DropDownOpening += gcnew System::EventHandler(this, &viewer_editor::model_on_drop_down_opening);

	// addmodelToolStripMenuItem
	addmodelToolStripMenuItem->Name = L"addmodelToolStripMenuItem";
	addmodelToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	addmodelToolStripMenuItem->Text = L"&Add new";
	addmodelToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::on_add_model_click);

	// removemodelToolStripMenuItem
	removemodelToolStripMenuItem->Name = L"removemodelToolStripMenuItem";
	removemodelToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	removemodelToolStripMenuItem->Text = L"&Remove";
	removemodelToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::on_remove_model_click);

	// selectmodelToolStripMenuItem
	selectmodelToolStripMenuItem->Name = L"selectmodelToolStripMenuItem";
	selectmodelToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	selectmodelToolStripMenuItem->Text = L"&Select active";
	selectmodelToolStripMenuItem->Click += gcnew System::EventHandler(this, &viewer_editor::on_select_model_click);

	m_menu_strip->ResumeLayout(false);
	m_menu_strip->PerformLayout();			
	m_multidocument_base->add_menu_items(m_menu_strip);
	Controls->Add(m_multidocument_base);

	AllowEndUserDocking = false;
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
	HideOnClose = true;
	Name = L"viewer_editor";
	Padding = System::Windows::Forms::Padding(0);
	Size = System::Drawing::Size(300,300);
	Text = L"Viewer";
}

viewer_editor::~viewer_editor()
{
	if(components)
		delete components;
}

bool viewer_editor::close_query()
{
	m_multidocument_base->save_panels(this);
	save_settings();

	u32 unsaved_docs_counter = 0;
	System::String^ txt = "";
	for each(viewer_document^ doc in m_multidocument_base->opened_documents)
	{
		if(!(doc->is_saved))
		{
			txt += doc->Name+".avs\n";
			++unsaved_docs_counter;
		}
	}

	if(unsaved_docs_counter==0)
	{
		System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
		lst->AddRange(m_multidocument_base->opened_documents);
		for each(viewer_document^ doc in lst)
			doc->Close();

		return true;
	}

	::DialogResult res = MessageBox::Show(this, "Save changes to the following items?\n\n"+txt, 
		"Viewer editor", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question);

	if(res== ::DialogResult::Cancel)
		return false;
	else if(res== ::DialogResult::Yes)
	{
		for each(viewer_document^ doc in m_multidocument_base->opened_documents)
		{
			if(!(doc->is_saved))
				doc->save();
		}
	}

	System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
	lst->AddRange(m_multidocument_base->opened_documents);
	for each(viewer_document^ doc in lst)
	{
		doc->is_saved = true;
		doc->Close();
	}

	m_multidocument_base->active_document = nullptr;
	return true;
}

void viewer_editor::on_document_closing(System::Object^, System::EventArgs^)
{
	show_properties(nullptr);
}

bool viewer_editor::predicate_save_scene_as()
{
	if(m_multidocument_base->active_document==nullptr)
		return false;

	return true;
}

bool viewer_editor::predicate_save_scene()
{
	if(m_multidocument_base->active_document==nullptr || m_multidocument_base->active_document->is_saved)
		return false;

	return true;
}

bool viewer_editor::predicate_has_target()
{
	if(m_multidocument_base->active_document==nullptr)
		return false;

	return safe_cast<viewer_document^>(m_multidocument_base->active_document)->has_target();
}

IDockContent^ viewer_editor::find_dock_content(System::String^ persist_string)
{
	if(persist_string=="xray.animation_editor.animation_viewer_toolbar_panel")
		return m_toolbar_panel;

	if(persist_string=="xray.animation_editor.animation_viewer_properties_panel")
		return m_properties_panel;

	if(persist_string=="xray.animation_editor.animation_viewer_time_panel")
		return m_time_panel;

	return (nullptr);
}

xray::editor::controls::document_base^ viewer_editor::on_document_creating()
{
	return gcnew viewer_document(this);
}

void viewer_editor::on_editor_key_down(Object^ , KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Tab && e->Control)
	{
		int cur_doc_count = m_multidocument_base->opened_documents->Count;
		if(cur_doc_count>1)
		{
			for(int i=0; i<cur_doc_count; i++)
				if(m_multidocument_base->opened_documents->default[i]==m_multidocument_base->active_document)
				{
					if(i==cur_doc_count-1)
						i = -1;

					m_multidocument_base->opened_documents->default[i+1]->Activate();
					m_multidocument_base->active_document = m_multidocument_base->opened_documents->default[i+1];
					return;
				}
		}
	}
	else if(e->KeyCode==Keys::F4 && e->Control)
	{
		if(m_multidocument_base->active_document!=nullptr)
		{
			m_multidocument_base->active_document->Close();
			return;
		}
	}
}

#define _recent_list_prefix	"Line"
#define _recent_key_name "recent files"
#define _settings_key_name "settings"

RegistryKey^ get_sub_key(RegistryKey^ root, System::String^ name)
{
	RegistryKey^ result = root->OpenSubKey(name, true);
	if(!result)
		result = root->CreateSubKey(name);

	return result;
}

void viewer_editor::get_recent_list(ArrayList^ recent_list)
{
	recent_list->Clear();

	RegistryKey^ product_key = base_registry_key::get();
	ASSERT(product_key);

	RegistryKey^ windows_key = get_sub_key(product_key, "windows");
	RegistryKey^ editor_key = get_sub_key(windows_key, "animation_viewer_editor");
	RegistryKey^ recent_key = get_sub_key(editor_key, _recent_key_name);

	array<String^>^	value_names	= recent_key->GetValueNames();

	for each(System::String^ name in value_names)
	{
		if(!name->StartsWith(_recent_list_prefix))
			continue; 

		String^ tmp_value = dynamic_cast<String^>(recent_key->GetValue(name));
		if(tmp_value != nullptr)
			recent_list->Add(tmp_value);
	}

	recent_key->Close();
	editor_key->Close();
	windows_key->Close();
	product_key->Close();
}

void viewer_editor::change_recent_list(System::String^ path, bool b_add)
{
	ArrayList recent_list;
	get_recent_list(%recent_list);

	if(recent_list.Contains(path))
		recent_list.Remove(path);

	if(b_add)
		recent_list.Insert(0, path);

	// save
	RegistryKey^ product_key = base_registry_key::get();
	ASSERT(product_key);

	RegistryKey^ windows_key = get_sub_key(product_key, "windows");

	RegistryKey^ editor_key = get_sub_key(windows_key, "animation_viewer_editor");
	editor_key->DeleteSubKey(_recent_key_name, false);

	RegistryKey^ recent_key = editor_key->CreateSubKey(_recent_key_name);
	ASSERT(recent_key);

	int i=0;
	for each(System::String^ p in recent_list)
	{
		ASSERT(p);
		recent_key->SetValue(_recent_list_prefix + (i++), p);
	}

	recent_key->Close();
	editor_key->Close();
	windows_key->Close();
	product_key->Close();
}

void viewer_editor::load_settings()
{
	RegistryKey^ product_key = base_registry_key::get();
	ASSERT(product_key);

	RegistryKey^ windows_key = get_sub_key(product_key, "windows");
	RegistryKey^ editor_key = get_sub_key(windows_key, "animation_viewer_editor");
	RegistryKey^ settings_key = get_sub_key(editor_key, _settings_key_name);

	System::Object^ obj = settings_key->GetValue("play_from_beginning", true);
	play_from_beginning = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("snap_to_frames", false);
	snap_to_frames = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("follow_animation_thumb", true);
	follow_animation_thumb = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("camera_follows_npc", false);
	camera_follows_npc = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("use_detailed_weights_and_scales", false);
	m_use_detailed_weights_and_scales = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("show_detailed_weights", false);
	show_detailed_weights = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("show_detailed_scales", false);
	show_detailed_scales = System::Convert::ToBoolean(obj);

	obj = settings_key->GetValue("detalization_delta", 33);
	detalization_delta = System::Convert::ToUInt32(obj);

	settings_key->Close();
	editor_key->Close();
	windows_key->Close();
	product_key->Close();
}

void viewer_editor::save_settings()
{
	// save
	RegistryKey^ product_key = base_registry_key::get();
	ASSERT(product_key);

	RegistryKey^ windows_key = get_sub_key(product_key, "windows");
	RegistryKey^ editor_key = get_sub_key(windows_key, "animation_viewer_editor");
	editor_key->DeleteSubKey(_settings_key_name, false);

	RegistryKey^ settings_key = editor_key->CreateSubKey(_settings_key_name);
	ASSERT(settings_key);

	settings_key->SetValue("play_from_beginning", play_from_beginning);
	settings_key->SetValue("snap_to_frames", snap_to_frames);
	settings_key->SetValue("follow_animation_thumb", follow_animation_thumb);
	settings_key->SetValue("camera_follows_npc", camera_follows_npc);
	settings_key->SetValue("use_detailed_weights_and_scales", m_use_detailed_weights_and_scales);
	settings_key->SetValue("show_detailed_weights", show_detailed_weights);
	settings_key->SetValue("show_detailed_scales", show_detailed_scales);
	settings_key->SetValue("detalization_delta", detalization_delta);

	settings_key->Close();
	editor_key->Close();
	windows_key->Close();
	product_key->Close();
}

void viewer_editor::model_on_drop_down_opening(System::Object^, System::EventArgs^)
{
	addmodelToolStripMenuItem->Enabled = m_multidocument_base->active_document!=nullptr;
	removemodelToolStripMenuItem->Enabled = m_multidocument_base->active_document!=nullptr;
	if(m_multidocument_base->active_document!=nullptr)
	{
		viewer_document^ d = safe_cast<viewer_document^>(m_multidocument_base->active_document);
		bool res = d->has_loaded_models();
		selectmodelToolStripMenuItem->Enabled = res;
		removemodelToolStripMenuItem->Enabled = res;
	}
	else
	{
		selectmodelToolStripMenuItem->Enabled = false;
		removemodelToolStripMenuItem->Enabled = false;
	}
}

void viewer_editor::file_on_drop_down_opening(System::Object^, System::EventArgs^)
{
	saveToolStripMenuItem->Enabled = m_multidocument_base->active_document!=nullptr && !m_multidocument_base->active_document->is_saved;
}

void viewer_editor::recent_on_drop_down_opening(System::Object^, System::EventArgs^)
{
	recentToolStripMenuItem->DropDownItems->Clear();
	System::Collections::ArrayList recent_list;
	get_recent_list(%recent_list);
	System::Collections::Generic::List<System::String^>^ unexisting_list = gcnew System::Collections::Generic::List<System::String^>();
	for each(System::String^ filename in recent_list)
	{
		System::String^ full_path = System::IO::Path::GetFullPath("../../resources/sources/animations/viewer_scenes/"+filename+".avs");
		if(System::IO::File::Exists(full_path))
		{
			System::Windows::Forms::ToolStripItem^ item = recentToolStripMenuItem->DropDownItems->Add(filename);
			item->Click += gcnew System::EventHandler(this, &viewer_editor::recent_item_click);
		}
		else
			unexisting_list->Add(filename);
	}

	for each(System::String^ filename in unexisting_list)
		change_recent_list(filename, false);
}

void viewer_editor::recent_item_click(System::Object^ sender, System::EventArgs^)
{
	System::Windows::Forms::ToolStripItem^ item = safe_cast<System::Windows::Forms::ToolStripItem^>(sender);
	m_multidocument_base->load_document(item->Text);
}

void viewer_editor::on_open_click()
{
	if(m_open_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
	{
		System::String^ path = m_open_file_dialog->FileName->Remove(0, m_open_file_dialog->InitialDirectory->Length+1);
		path = path->Remove(path->Length-4);
		m_multidocument_base->load_document(path);
		change_recent_list(path, true);
	}
}

void viewer_editor::show_properties(Object^ obj)
{
	m_properties_panel->show_properties(obj);
}

void viewer_editor::on_node_property_changed(System::Object^ sender, value_changed_event_args^ e)
{
	xray::editor::wpf_controls::property_grid::property_grid_control^ g = safe_cast<xray::editor::wpf_controls::property_grid::property_grid_control^>(sender);
	wpf_property_container^ pc = dynamic_cast<wpf_property_container^>(g->selected_object);
	System::String^ property_name = e->changed_property->name;
	if(property_name=="in seconds" || property_name=="in frames")
	{
		//safe_cast<xray::editor::wpf_controls::property_editors::item::item_editor_base^>(e->changed_property->property_parent->owner_editor)->reset_sub_properties();
		if(e->changed_property->property_parent)
			property_name = e->changed_property->property_parent->name+"/"+property_name;

		if(e->changed_property->property_parent->property_parent && e->changed_property->property_parent->property_parent->property_parent)
			property_name = e->changed_property->property_parent->property_parent->property_parent->name+"/"+property_name;
	}
	else if(property_name=="type" || property_name=="epsilon")
	{
		//safe_cast<xray::editor::wpf_controls::property_editors::item::item_editor_base^>(e->changed_property->property_parent->owner_editor)->reset_sub_properties();
		if(e->changed_property->property_parent->property_parent)
			property_name = e->changed_property->property_parent->property_parent->name+"/"+property_name;
	}

	safe_cast<viewer_document^>(m_multidocument_base->active_document)->on_property_changed(pc, property_name, e->new_value, e->old_value);
}

void viewer_editor::on_view_menu_item_select(System::Object^ , System::EventArgs^ )
{
	toolbarToolStripMenuItem->Checked = !(m_toolbar_panel->IsHidden);
	propertiesToolStripMenuItem->Checked = !(m_properties_panel->IsHidden);
	timeToolStripMenuItem->Checked = !(m_time_panel->IsHidden);
}

void viewer_editor::show_toolbar_panel(System::Object^ , System::EventArgs^)
{
	if(!(toolbarToolStripMenuItem->Checked))
		m_toolbar_panel->Hide();
	else
		m_toolbar_panel->Show(m_multidocument_base->main_dock_panel);
}

void viewer_editor::show_properties_panel(System::Object^ , System::EventArgs^)
{
	if(!(propertiesToolStripMenuItem->Checked))
		m_properties_panel->Hide();
	else
		m_properties_panel->Show(m_multidocument_base->main_dock_panel);
}

void viewer_editor::show_time_panel(System::Object^ , System::EventArgs^)
{
	if(!(timeToolStripMenuItem->Checked))
		m_time_panel->Hide();
	else
		m_time_panel->Show(m_multidocument_base->main_dock_panel);
}

void viewer_editor::save_document_as()
{
	R_ASSERT(m_multidocument_base->active_document!=nullptr);
	safe_cast<viewer_document^>(m_multidocument_base->active_document)->save_as();
}

void viewer_editor::update_options_items(System::Object^ , System::EventArgs^)
{
	playfrombeginToolStripMenuItem->Checked = play_from_beginning;
	followthumbToolStripMenuItem->Checked = m_time_panel->get_following();
	snaptoframesToolStripMenuItem->Checked = m_time_panel->get_snapping();
	follownpcToolStripMenuItem->Checked = m_camera_follows_npc;
}

void viewer_editor::on_detailed_weights_and_scales_click(System::Object^, System::EventArgs^)
{
	m_detailed_options_panel->show( );
}

void viewer_editor::run_lexeme_editor(xray::animation_editor::animation_viewer_graph_node_base^ n)
{
	m_lexeme_editor->show(m_multidocument_base->main_dock_panel, n);
}

void viewer_editor::set_animation_node_new_intervals_list(animation_viewer_graph_node_base^ n, List<xray::animation_editor::animation_node_interval^>^ lst, u32 cycle_index)
{
	if( m_multidocument_base->active_document != nullptr )
		safe_cast<viewer_document^>( m_multidocument_base->active_document )->set_animation_node_new_intervals_list(n, lst, cycle_index);
}

void viewer_editor::on_new_document_created(System::Object^, document_event_args^ e)
{
	viewer_document^ doc = safe_cast<viewer_document^>(e->document);
	doc->is_saved = true;
	doc->update_panel_values();
	doc->add_model("character/human/neutral/neutral_01/neutral_01", "model_0");
	doc->set_saved_flag( );
}

bool viewer_editor::get_locked()
{
	if(m_multidocument_base->active_document!=nullptr)
		return safe_cast<viewer_document^>(m_multidocument_base->active_document)->target_locked;

	return false;
}

void viewer_editor::change_locked()
{
	if(m_multidocument_base->active_document!=nullptr)
	{
		viewer_document^ d = safe_cast<viewer_document^>(m_multidocument_base->active_document);
		d->target_locked = !d->target_locked;
		lockToolStripMenuItem->Checked = d->target_locked;
	}
}

void viewer_editor::save_node_as_clip()
{
	if(m_multidocument_base->active_document!=nullptr)
	{
		viewer_document^ d = safe_cast<viewer_document^>(m_multidocument_base->active_document);
		d->save_node_as_clip();
	}
}

bool viewer_editor::anim_node_selected()
{
	if(m_multidocument_base->active_document!=nullptr)
	{
		viewer_document^ d = safe_cast<viewer_document^>(m_multidocument_base->active_document);
		return d->anim_node_selected();
	}

	return false;
}