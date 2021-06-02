//-------------------------------------------------------------------------------------------
//	Created		: 29.06.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
//-------------------------------------------------------------------------------------------
#include "pch.h"
#include "sound_document_editor_base.h"
#include "sound_collection_document.h"
#include "composite_sound_document.h"
#include "sound_scene_document.h"
#include "sound_editor.h"

using xray::sound_editor::sound_document_editor_base;
using xray::sound_editor::sound_editor;
using xray::sound_editor::sound_collection_document;
using xray::sound_editor::composite_sound_document;
using xray::sound_editor::sound_scene_document;
using xray::sound_editor::sound_document_type;

sound_document_editor_base::sound_document_editor_base(sound_editor^ ed)
:super("sound_editor", true),
m_editor(ed)
{
	m_creating_mode = sound_document_type::scene_document;
	initialize_menu();
}

void sound_document_editor_base::initialize_menu()
{
	fileToolStripMenuItem->DropDownItems->Remove(newToolStripMenuItem);
	fileToolStripMenuItem->DropDownItems->Remove(exitToolStripMenuItem);

	m_menu_file_new = gcnew ToolStripMenuItem();
	m_menu_file_new_scene = gcnew ToolStripMenuItem();
	m_menu_file_new_composite = gcnew ToolStripMenuItem();
	m_menu_file_new_collection = gcnew ToolStripMenuItem();
	m_menu_file_open = gcnew ToolStripMenuItem();
	m_open_file_dialog = gcnew OpenFileDialog();

	// m_menu_file_new
	m_menu_file_new->Name = L"m_menu_file_new";
	m_menu_file_new->Size = System::Drawing::Size(197, 22);
	m_menu_file_new->Text = L"&New";
	m_menu_file_new->DropDownItems->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(3) {
		m_menu_file_new_scene, 
		m_menu_file_new_composite, 
		m_menu_file_new_collection,
	});

	// m_menu_file_new_scene
	m_menu_file_new_scene->Name = L"m_menu_file_new_scene";
	m_menu_file_new_scene->Size = System::Drawing::Size(197, 22);
	m_menu_file_new_scene->Text = L"&Sound scene";
	m_menu_file_new_scene->Click += gcnew System::EventHandler(this, &sound_document_editor_base::new_scene_document);

	// m_menu_file_new_composite
	m_menu_file_new_composite->Name = L"m_menu_file_new_composite";
	m_menu_file_new_composite->Size = System::Drawing::Size(197, 22);
	m_menu_file_new_composite->Text = L"&Composite sound";
	m_menu_file_new_composite->Click += gcnew System::EventHandler(this, &sound_document_editor_base::new_composite_document);

	// m_menu_file_new_collection
	m_menu_file_new_collection->Name = L"m_menu_file_new_collection";
	m_menu_file_new_collection->Size = System::Drawing::Size(197, 22);
	m_menu_file_new_collection->Text = L"S&ound collection";
	m_menu_file_new_collection->Click += gcnew System::EventHandler(this, &sound_document_editor_base::new_collection_document);

	// m_menu_file_open
	m_menu_file_open->Name = L"m_menu_file_open";
	m_menu_file_open->Size = System::Drawing::Size(197, 22);
	m_menu_file_open->Text = L"&Open sound scene";

	fileToolStripMenuItem->DropDownItems->Insert(0, m_menu_file_new);
	fileToolStripMenuItem->DropDownItems->Add(m_menu_file_open);

	// m_open_file_dialog
	m_open_file_dialog->AddExtension = true;
	m_open_file_dialog->DefaultExt = ".editor_sound_scene";
	m_open_file_dialog->Filter = "Sound scene files|*.editor_sound_scene";
	m_open_file_dialog->InitialDirectory = System::IO::Path::GetFullPath(sound_editor::absolute_sound_resources_path+"/scenes");
	m_open_file_dialog->Multiselect = false;
	m_open_file_dialog->RestoreDirectory = true;
	m_open_file_dialog->Title = "Open sound scene";
}

void sound_document_editor_base::new_collection_document(Object^, EventArgs^)
{
	m_creating_mode = sound_document_type::collection_document;
	create_document();
}

void sound_document_editor_base::new_composite_document(Object^, EventArgs^)
{
	m_creating_mode = sound_document_type::composite_document;
	create_document();
}

void sound_document_editor_base::new_scene_document(Object^, EventArgs^)
{
	m_creating_mode = sound_document_type::scene_document;
	create_document();
}

void sound_document_editor_base::open_scene()
{
	if(m_open_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
	{
		System::String^ path = m_open_file_dialog->FileName->Remove(0, m_open_file_dialog->InitialDirectory->Length+1);
		path = path->Remove(path->Length-19);
		load_document(path, sound_document_type::scene_document);
	}
}

document_base^ sound_document_editor_base::create_document()
{
	document_base^ doc;
	if(m_creating_mode==sound_document_type::scene_document)
		doc = gcnew sound_scene_document(m_editor);
	else if(m_creating_mode==sound_document_type::composite_document)
		doc = gcnew composite_sound_document(m_editor);
	else
		doc = gcnew sound_collection_document(m_editor);

	doc->Text = "Untitled Document";
	doc->Name = "Untitled Document";
	doc->is_saved = false;
	doc->Show(m_main_dock_panel);
	m_opened_documents->Add(doc);
	m_active_document = doc;
	return doc;
}

void sound_document_editor_base::load_document(System::String^ full_path, sound_document_type mode)
{
	m_creating_mode = mode;
	load_document(full_path);
}

void sound_document_editor_base::load_document(System::String^ full_path)
{
	if(m_loaded)
		return;

	m_loaded = true;
	for each(document_base^ d in m_opened_documents)
	{
		if(d->Name==full_path)
		{
			d->Activate();
			m_loaded = false;
			return;
		}
	}

	document_base^ doc = create_document();
	doc->Text = doc->get_document_name(full_path);
	doc->Name = doc->Text;
	doc->load();	
	doc->Show(m_main_dock_panel);
	m_loaded = false;
}
