////////////////////////////////////////////////////////////////////////////
//	Created		: 10.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_manager.h"
#include "animation_editor.h"
#include "animations_list_panel.h"
#include "animation_setup_properties_panel.h"
#include "animation_setup_document.h"
#include "animation_model.h"
#include "animation_editor_form.h"

#pragma managed( push, off )
#	include <xray/animation/api.h>
#	include <xray/animation/mixing_expression.h>
#	include <xray/animation/mixing_animation_lexeme.h>
#	include <xray/render/facade/editor_renderer.h>
#pragma managed( pop )

using xray::animation_editor::animation_setup_manager;
using xray::animation_editor::animation_editor;
using xray::animation_editor::animation_editor_viewport_panel;
using xray::editor::controls::serializer;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI;
using namespace xray::animation;

animation_setup_manager::animation_setup_manager	( animation_editor^ editor ) :
	m_editor	( editor ),
	m_scene		( NEW( render::scene_ptr ) )
{
	AllowEndUserDocking = false;
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	DockAreas = static_cast<Docking::DockAreas>(Docking::DockAreas::Document);
	HideOnClose = true;
	Name = L"animation_setup_manager";
	Padding = System::Windows::Forms::Padding(0);
	Size = System::Drawing::Size(300,300);
	Text = L"Animations setup";

	m_multidocument_base = gcnew xray::editor::controls::document_editor_base("animation_setup_manager", false);
	m_multidocument_base->main_dock_panel->DockBackColor = System::Drawing::SystemColors::AppWorkspace; 
	m_multidocument_base->Dock = DockStyle::Fill;
	m_multidocument_base->creating_new_document = gcnew document_create_callback(this, &animation_setup_manager::on_document_creating);
	m_multidocument_base->content_reloading = gcnew content_reload_callback(this, &animation_setup_manager::find_dock_content);

	m_animations_list_panel = gcnew animations_list_panel(this);
	m_properties_panel = gcnew animation_setup_properties_panel(this);
	bool loaded = m_multidocument_base->load_panels(this);
	if(!loaded)
	{
		m_animations_list_panel->Show(m_multidocument_base->main_dock_panel, Docking::DockState::DockRight);
		m_properties_panel->Show(m_animations_list_panel->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Bottom, 0.75f);
	}

	m_menu_strip = gcnew MenuStrip();
	m_menu_strip->SuspendLayout();
	menuToolStripMenuItem = gcnew ToolStripMenuItem();
	loadmodelToolStripMenuItem = gcnew ToolStripMenuItem();
	saveallToolStripMenuItem = gcnew ToolStripMenuItem();
	saveanimationToolStripMenuItem = gcnew ToolStripMenuItem();
	editToolStripMenuItem = gcnew ToolStripMenuItem();
	undoToolStripMenuItem = gcnew ToolStripMenuItem();
	redoToolStripMenuItem = gcnew ToolStripMenuItem();
	viewToolStripMenuItem = gcnew ToolStripMenuItem();
	animlistToolStripMenuItem = gcnew ToolStripMenuItem();
	propertiesToolStripMenuItem = gcnew ToolStripMenuItem();

	// menuStrip1
	m_menu_strip->Items->AddRange(gcnew cli::array<ToolStripItem^>(3) {menuToolStripMenuItem, editToolStripMenuItem, viewToolStripMenuItem});
	m_menu_strip->Location = System::Drawing::Point(0, 0);
	m_menu_strip->Name = L"m_menu_strip";
	m_menu_strip->Size = System::Drawing::Size(292, 24);
	m_menu_strip->TabIndex = 1;
	m_menu_strip->Text = L"Menu";

	// menuToolStripMenuItem
	menuToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^>(3) {saveanimationToolStripMenuItem, saveallToolStripMenuItem, loadmodelToolStripMenuItem});
	menuToolStripMenuItem->Name = L"menuToolStripMenuItem";
	menuToolStripMenuItem->Size = System::Drawing::Size(37, 20);
	menuToolStripMenuItem->Text = L"&Menu";

	// saveanimationToolStripMenuItem
	saveanimationToolStripMenuItem->Name = L"saveanimationToolStripMenuItem";
	saveanimationToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	saveanimationToolStripMenuItem->Text = L"&Save";
	saveanimationToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_setup_manager::save_active_document);

	// saveallToolStripMenuItem
	saveallToolStripMenuItem->Name = L"saveallToolStripMenuItem";
	saveallToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	saveallToolStripMenuItem->Text = L"&Save all";
	saveallToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_setup_manager::save_all_documents);

	// loadmodelToolStripMenuItem
	loadmodelToolStripMenuItem->Name = L"loadmodelToolStripMenuItem";
	loadmodelToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	loadmodelToolStripMenuItem->Text = L"&Load model";
	loadmodelToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_setup_manager::on_load_model_click);

	// editToolStripMenuItem
	editToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^>(2) {undoToolStripMenuItem, redoToolStripMenuItem});
	editToolStripMenuItem->Name = L"editToolStripMenuItem";
	editToolStripMenuItem->Size = System::Drawing::Size(37, 20);
	editToolStripMenuItem->Text = L"&Edit";

	// undoToolStripMenuItem
	undoToolStripMenuItem->Name = L"undoToolStripMenuItem";
	undoToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	undoToolStripMenuItem->Text = L"&Undo";

	// redoToolStripMenuItem
	redoToolStripMenuItem->Name = L"redoToolStripMenuItem";
	redoToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	redoToolStripMenuItem->Text = L"&Redo";

	// viewToolStripMenuItem
	viewToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^>(2) {animlistToolStripMenuItem, propertiesToolStripMenuItem});
	viewToolStripMenuItem->Name = L"viewToolStripMenuItem";
	viewToolStripMenuItem->Size = System::Drawing::Size(37, 20);
	viewToolStripMenuItem->Text = L"&View";
	viewToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_setup_manager::on_view_menu_item_select);

	// animlistToolStripMenuItem
	animlistToolStripMenuItem->Checked = !(m_animations_list_panel->IsHidden);
	animlistToolStripMenuItem->CheckOnClick = true;
	animlistToolStripMenuItem->Name = L"animlistToolStripMenuItem";
	animlistToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	animlistToolStripMenuItem->Text = L"&Animations list";
	animlistToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_setup_manager::show_anim_list_panel);

	// propertiesToolStripMenuItem
	propertiesToolStripMenuItem->Checked = !(m_properties_panel->IsHidden);
	propertiesToolStripMenuItem->CheckOnClick = true;
	propertiesToolStripMenuItem->Name = L"propertiesToolStripMenuItem";
	propertiesToolStripMenuItem->Size = System::Drawing::Size(197, 22);
	propertiesToolStripMenuItem->Text = L"&Properties panel";
	propertiesToolStripMenuItem->Click += gcnew System::EventHandler(this, &animation_setup_manager::show_properties_panel);

	m_menu_strip->ResumeLayout(false);
	m_menu_strip->PerformLayout();			
	m_multidocument_base->add_menu_items(m_menu_strip);

	Controls->Add(m_multidocument_base);
}

animation_setup_manager::~animation_setup_manager()
{
	DELETE			( m_scene );
}

animation_setup_manager::!animation_setup_manager()
{
	this->~animation_setup_manager	( );
}

void animation_setup_manager::tick()
{
	if(m_model)
	{
		m_model->render();
		if(m_multidocument_base->active_document)
			safe_cast<animation_setup_document^>(m_multidocument_base->active_document)->tick();
	}
}

void animation_setup_manager::clear_resources()
{
	if(m_editor->form->active_content==this)
		m_model->remove_from_render();

	delete m_model;
	delete m_animations_list_panel;
	delete m_properties_panel;
	delete m_multidocument_base;
	delete m_menu_strip;
	delete menuToolStripMenuItem;
	delete loadmodelToolStripMenuItem;
}

void animation_setup_manager::save_active_document(System::Object^, System::EventArgs^)
{
	m_multidocument_base->save_active();
}

void animation_setup_manager::save_all_documents(System::Object^, System::EventArgs^)
{
	m_multidocument_base->save_all();
}

void animation_setup_manager::on_scene_created( xray::render::scene_ptr const& scene )
{
	*m_scene					= scene;
	query_result_delegate* rq	= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_setup_manager::on_resources_loaded), g_allocator);
	m_model						= gcnew animation_model( *m_scene, m_editor->get_renderer().scene(), m_editor->get_renderer().debug(), "model_0", animation_editor::models_path, float4x4().identity(), rq);
}

bool animation_setup_manager::close_query()
{
	m_multidocument_base->save_panels(this);
	u32 unsaved_docs_counter = 0;
	System::String^ txt = "";
	for each(animation_setup_document^ doc in m_multidocument_base->opened_documents)
	{
		if(!(doc->is_saved))
		{
			txt += doc->Name+".clip_data\n";
			++unsaved_docs_counter;
		}
	}

	if(unsaved_docs_counter==0)
	{
		System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
		lst->AddRange(m_multidocument_base->opened_documents);
		for each(animation_setup_document^ doc in lst)
			doc->Close();

		return true;
	}

	::DialogResult res = MessageBox::Show(this, "Save changes to the following items?\n\n"+txt, 
		"Animation setup manager", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question);

	if(res== ::DialogResult::Cancel)
		return false;
	else if(res== ::DialogResult::Yes)
	{
		for each(animation_setup_document^ doc in m_multidocument_base->opened_documents)
		{
			if(!(doc->is_saved))
				doc->save();
		}
	}

	System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
	lst->AddRange(m_multidocument_base->opened_documents);
	for each(animation_setup_document^ doc in lst)
	{
		doc->is_saved = true;
		doc->Close();
	}

	m_multidocument_base->active_document = nullptr;
	return true;
}

xray::editor::controls::document_base^ animation_setup_manager::on_document_creating()
{
	return gcnew animation_setup_document(this);
}

IDockContent^ animation_setup_manager::find_dock_content(System::String^ persist_string)
{
	if(persist_string=="xray.animation_editor.animations_list_panel")
		return m_animations_list_panel;

	if(persist_string=="xray.animation_editor.animation_setup_properties_panel")
		return m_properties_panel;

	return nullptr;
}

void animation_setup_manager::on_load_model_click(System::Object^, System::EventArgs^)
{
	System::String^ selected = nullptr;
	bool result = editor_base::resource_chooser::choose("solid_visual", m_model->name, nullptr, selected, true);
	if(result && m_model->name!=selected)
	{
		m_model->remove_from_render();
		delete m_model;
		query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_setup_manager::on_resources_loaded), g_allocator);
		m_model = gcnew animation_model( *m_scene, m_editor->get_renderer().scene(), m_editor->get_renderer().debug(), "model_0", selected, float4x4().identity(), rq);
	}
}

void animation_setup_manager::on_resources_loaded(xray::resources::queries_result& result)
{
	if(!result.is_successful())
	{
		LOGI_INFO("animation_setup_manager", "MODEL LOADING FAILED!!!");
		return;
	}

	if(m_editor->form->active_content==this)
		m_model->add_to_render();
}

void animation_setup_manager::set_target(xray::animation::mixing::animation_lexeme& l, u32 const current_time_in_ms)
{
	if(m_model)
	{
		m_model->reset();
		m_model->subscribe_footsteps(l);
		m_model->set_target(mixing::expression(l), 0);
		m_model->tick(current_time_in_ms);
	}
}

bool animation_setup_manager::predicate_save_scene()
{
	if(m_multidocument_base->active_document==nullptr || m_multidocument_base->active_document->is_saved)
		return false;

	return true;
}

void animation_setup_manager::add_model_to_render()
{
	if( m_model != nullptr && m_editor->form->active_content == this )
		m_model->add_to_render		( );
}

void animation_setup_manager::remove_model_from_render()
{
	if( m_model != nullptr && m_editor->form->active_content == this )
		m_model->remove_from_render	( );
}

bool animation_setup_manager::model_loaded() 
{
	return (m_model!=nullptr && m_model->model()!=NULL);
}

void animation_setup_manager::document_saved(System::String^ doc_name)
{
	m_editor->refresh_animation_node_clip(doc_name);
}

void animation_setup_manager::on_view_menu_item_select(System::Object^, System::EventArgs^)
{
	animlistToolStripMenuItem->Checked = !(m_animations_list_panel->IsHidden);
	propertiesToolStripMenuItem->Checked = !(m_properties_panel->IsHidden);
}

void animation_setup_manager::show_anim_list_panel(System::Object^ , System::EventArgs^)
{
	if(!(animlistToolStripMenuItem->Checked))
		m_animations_list_panel->Hide();
	else
		m_animations_list_panel->Show(m_multidocument_base->main_dock_panel);
}

void animation_setup_manager::show_properties_panel(System::Object^ , System::EventArgs^)
{
	if(!(propertiesToolStripMenuItem->Checked))
		m_properties_panel->Hide();
	else
		m_properties_panel->Show(m_multidocument_base->main_dock_panel);
}

void animation_setup_manager::show_properties(Object^ obj)
{
	m_properties_panel->show_properties(obj);
}

void animation_setup_manager::on_property_changed(System::Object^, value_changed_event_args^ e)
{
	safe_cast<animation_setup_document^>(m_multidocument_base->active_document)->on_property_changed(e->changed_property->name, e->new_value, e->old_value);
}
