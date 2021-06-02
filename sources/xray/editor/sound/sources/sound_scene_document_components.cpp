////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_scene_document.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "transform_control_helper.h"
#include "sound_scene_contexts_manager.h"
#include "sound_scene_actions.h"
#include "toolbar_panel.h"
#include <xray/collision/api.h>

using xray::sound_editor::sound_scene_document;

void sound_scene_document::initialize_components()
{
	// m_save_file_dialog
	m_save_file_dialog = gcnew SaveFileDialog();
	m_save_file_dialog->AddExtension = true;
	m_save_file_dialog->DefaultExt = ".editor_sound_scene";
	m_save_file_dialog->Filter = "Sound scene files|*.editor_sound_scene";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath(sound_editor::absolute_sound_resources_path+"/scenes");
	m_save_file_dialog->RestoreDirectory = true;
	m_save_file_dialog->SupportMultiDottedExtensions = true;
	m_save_file_dialog->Title = "Save document";

	DockPanelSkin^ dockPanelSkin1 = gcnew DockPanelSkin();
	AutoHideStripSkin^ autoHideStripSkin1 = gcnew AutoHideStripSkin();
	DockPanelGradient^ dockPanelGradient1 = gcnew DockPanelGradient();
	TabGradient^ tabGradient1 = gcnew TabGradient();
	DockPaneStripSkin^ dockPaneStripSkin1 = gcnew DockPaneStripSkin();
	DockPaneStripGradient^ dockPaneStripGradient1 = gcnew DockPaneStripGradient();
	TabGradient^ tabGradient2 = gcnew TabGradient();
	DockPanelGradient^ dockPanelGradient2 = gcnew DockPanelGradient();
	TabGradient^ tabGradient3 = gcnew TabGradient();
	DockPaneStripToolWindowGradient^ dockPaneStripToolWindowGradient1 = gcnew DockPaneStripToolWindowGradient();
	TabGradient^ tabGradient4 = gcnew TabGradient();
	TabGradient^ tabGradient5 = gcnew TabGradient();
	DockPanelGradient^ dockPanelGradient3 = gcnew DockPanelGradient();
	TabGradient^ tabGradient6 = gcnew TabGradient();
	TabGradient^ tabGradient7 = gcnew TabGradient();
	m_main_dock_panel = gcnew WeifenLuo::WinFormsUI::Docking::DockPanel();

	SuspendLayout();

	// m_view_window
	m_view_window->AllowDrop = true;
	m_view_window->HideOnClose = true;
	m_view_window->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
	m_view_window->setup_view_matrix(float3(0.f, 5.f, -5.f), float3(0.0f, 0.0f, 0.0f));
	m_view_window->action_focus_to_origin();
	m_view_window->m_collision_tree = &(*(collision::new_space_partitioning_tree(g_allocator, 1.f, 1024)));
	m_view_window->subscribe_on_drag_drop(gcnew DragEventHandler(this, &sound_scene_document::view_window_drag_drop));
	m_view_window->subscribe_on_drag_enter(gcnew DragEventHandler(this, &sound_scene_document::view_window_drag_enter));

	// m_main_dock_panel
	m_main_dock_panel->ActiveAutoHideContent = nullptr;
	m_main_dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;
	m_main_dock_panel->DockBackColor = System::Drawing::SystemColors::Control;
	m_main_dock_panel->DockLeftPortion = 0.5;
	m_main_dock_panel->DocumentStyle = DocumentStyle::DockingWindow;
	m_main_dock_panel->Location = System::Drawing::Point(0, 24);
	m_main_dock_panel->Name = L"m_main_dock_panel";
	m_main_dock_panel->Size = System::Drawing::Size(292, 249);
	dockPanelGradient1->EndColor = System::Drawing::SystemColors::ControlLight;
	dockPanelGradient1->StartColor = System::Drawing::SystemColors::ControlLight;
	autoHideStripSkin1->DockStripGradient = dockPanelGradient1;
	tabGradient1->EndColor = System::Drawing::SystemColors::Control;
	tabGradient1->StartColor = System::Drawing::SystemColors::Control;
	tabGradient1->TextColor = System::Drawing::SystemColors::ControlDarkDark;
	autoHideStripSkin1->TabGradient = tabGradient1;
	dockPanelSkin1->AutoHideStripSkin = autoHideStripSkin1;
	tabGradient2->EndColor = System::Drawing::SystemColors::ControlLightLight;
	tabGradient2->StartColor = System::Drawing::SystemColors::ControlLightLight;
	tabGradient2->TextColor = System::Drawing::SystemColors::ControlText;
	dockPaneStripGradient1->ActiveTabGradient = tabGradient2;
	dockPanelGradient2->EndColor = System::Drawing::SystemColors::Control;
	dockPanelGradient2->StartColor = System::Drawing::SystemColors::Control;
	dockPaneStripGradient1->DockStripGradient = dockPanelGradient2;
	tabGradient3->EndColor = System::Drawing::SystemColors::ControlLight;
	tabGradient3->StartColor = System::Drawing::SystemColors::ControlLight;
	tabGradient3->TextColor = System::Drawing::SystemColors::ControlText;
	dockPaneStripGradient1->InactiveTabGradient = tabGradient3;
	dockPaneStripSkin1->DocumentGradient = dockPaneStripGradient1;
	tabGradient4->EndColor = System::Drawing::SystemColors::ActiveCaption;
	tabGradient4->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
	tabGradient4->StartColor = System::Drawing::SystemColors::GradientActiveCaption;
	tabGradient4->TextColor = System::Drawing::SystemColors::ActiveCaptionText;
	dockPaneStripToolWindowGradient1->ActiveCaptionGradient = tabGradient4;
	tabGradient5->EndColor = System::Drawing::SystemColors::Control;
	tabGradient5->StartColor = System::Drawing::SystemColors::Control;
	tabGradient5->TextColor = System::Drawing::SystemColors::ControlText;
	dockPaneStripToolWindowGradient1->ActiveTabGradient = tabGradient5;
	dockPanelGradient3->EndColor = System::Drawing::SystemColors::ControlLight;
	dockPanelGradient3->StartColor = System::Drawing::SystemColors::ControlLight;
	dockPaneStripToolWindowGradient1->DockStripGradient = dockPanelGradient3;
	tabGradient6->EndColor = System::Drawing::SystemColors::GradientInactiveCaption;
	tabGradient6->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
	tabGradient6->StartColor = System::Drawing::SystemColors::GradientInactiveCaption;
	tabGradient6->TextColor = System::Drawing::SystemColors::ControlText;
	dockPaneStripToolWindowGradient1->InactiveCaptionGradient = tabGradient6;
	tabGradient7->EndColor = System::Drawing::Color::Transparent;
	tabGradient7->StartColor = System::Drawing::Color::Transparent;
	tabGradient7->TextColor = System::Drawing::SystemColors::ControlDarkDark;
	dockPaneStripToolWindowGradient1->InactiveTabGradient = tabGradient7;
	dockPaneStripSkin1->ToolWindowGradient = dockPaneStripToolWindowGradient1;
	dockPanelSkin1->DockPaneStripSkin = dockPaneStripSkin1;
	m_main_dock_panel->Skin = dockPanelSkin1;
	m_main_dock_panel->TabIndex = 0;

	Controls->Add(m_main_dock_panel);
	AllowEndUserDocking = false;
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	ClientSize = System::Drawing::Size(562, 439);
	VisibleChanged += gcnew System::EventHandler(this, &sound_scene_document::on_document_activated);
	m_view_window->Show(m_main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document);
	ResumeLayout(false);
}

void sound_scene_document::register_actions()
{
	image_loader loader;
	m_gui_binder->set_image_list("base", loader.get_shared_images16());

	action_delegate^ a = nullptr;
	a = gcnew action_delegate("Save active", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::save_active));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_save_scene));
	m_input_engine->register_action(a, "Control+S(Active_document)");

	a = gcnew action_delegate("Save all", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::save_all));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_save_all));
	m_input_engine->register_action(a, "");

	a = gcnew action_delegate("Open scene", gcnew execute_delegate_managed(m_editor->multidocument_base, &sound_document_editor_base::open_scene));
	m_input_engine->register_action(a, "Control+O(sound_editor)");

	a = gcnew action_delegate("Undo", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::undo));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+Z(Active_document)");

	a = gcnew action_delegate("Redo", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::redo));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+Y(Active_document)");

	a = gcnew action_delegate("Cut", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::cut));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+X(Active_document)");

	a = gcnew action_delegate("Copy", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::copy));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+C(Active_document)");

	a = gcnew action_delegate("Paste", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::paste));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+V(Active_document)");

	a = gcnew action_delegate("Select all", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::select_all));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+A(Active_document)");

	a = gcnew action_delegate("Delete", gcnew execute_delegate_managed(m_editor->multidocument_base, &document_editor_base::del));
	a->set_enabled(gcnew enabled_delegate_managed(m_editor, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Delete(Active_document)");

	// transform actions
	m_input_engine->register_action(gcnew mouse_action_editor_control("manipulator drag axis mode", this, 0, false), "LButton("+Name+")");
	m_input_engine->register_action(gcnew action_select_active_control("select translate", this, m_transform_control_translation), "W("+Name+")");
	m_input_engine->register_action(gcnew action_select_active_control("select rotate", this, m_transform_control_rotation), "E("+Name+")");
	m_input_engine->register_action(gcnew action_select_active_control("select none", this, nullptr), "Q("+Name+")");

	// Object select mouse action
	m_input_engine->register_action(gcnew mouse_action_touch_object("touch", m_view_window), "LButton("+Name+")");
	m_input_engine->register_action(gcnew mouse_action_select_object("select", this, enum_selection_method::enum_selection_method_set), "LButton("+Name+")");

	// Maya style mouse actions
	m_input_engine->register_action(gcnew mouse_action_view_move_xy("view maya move xy", m_view_window), "Alt+MButton("+Name+")");
	m_input_engine->register_action(gcnew mouse_action_view_move_z("view maya move z", m_view_window), "Alt+RButton("+Name+")");
	m_input_engine->register_action(gcnew mouse_action_view_rotate_around("view maya rotate", m_view_window), "Alt+LButton("+Name+")");

	// Old LE style mouse actions
	m_input_engine->register_action(gcnew mouse_action_view_rotate("view LE rotate", m_view_window), "Space+LButton+RButton("+Name+")");
	m_input_engine->register_action(gcnew mouse_action_view_move_y_reverse("view LE move y", m_view_window), "Space+RButton("+Name+")");
	m_input_engine->register_action(gcnew mouse_action_view_move_xz_reverse("view LE move xz", m_view_window), "Space+LButton("+Name+")");
	m_input_engine->register_action(gcnew action_delegate("Focus to origin", gcnew execute_delegate_managed(m_view_window, &scene_view_panel::action_focus_to_origin)), "F("+Name+")");
	m_view_window->register_actions(m_input_engine, m_gui_binder, true);

	// sound controls
	m_input_engine->register_action(gcnew start_all_sound_action("start all sounds", this), "");
	m_input_engine->register_action(gcnew stop_all_sound_action("stop all sounds", this), "");

	m_gui_binder->bind(m_editor->multidocument_base->saveToolStripMenuItem, "Save active");
	m_gui_binder->bind(m_editor->multidocument_base->saveAllToolStripMenuItem, "Save all");
	//m_gui_binder->bind(m_editor->multidocument_base->m_menu_file_open, "Open scene");
	m_gui_binder->bind(m_editor->multidocument_base->undoToolStripMenuItem, "Undo");
	m_gui_binder->bind(m_editor->multidocument_base->redoToolStripMenuItem, "Redo");
	m_gui_binder->bind(m_editor->multidocument_base->cutToolStripMenuItem, "Cut");
	m_gui_binder->bind(m_editor->multidocument_base->copyToolStripMenuItem, "Copy");
	m_gui_binder->bind(m_editor->multidocument_base->pasteToolStripMenuItem, "Paste");
	m_gui_binder->bind(m_editor->multidocument_base->selectAllToolStripMenuItem, "Select all");
	m_gui_binder->bind(m_editor->multidocument_base->delToolStripMenuItem, "Delete");

	m_gui_binder->register_image("base", "select none", tool_select);
	m_gui_binder->register_image("base", "select translate", tool_translate);
	m_gui_binder->register_image("base", "select rotate", tool_rotate);
	m_gui_binder->register_image("base", "start all sounds", mode_pause);
	m_gui_binder->register_image("base", "stop all sounds", mode_pause);

	m_view_window->create_tool_strip("sound_editor_toopstrip", 0);
	m_view_window->add_action_button_item(m_gui_binder, "start all sounds", "sound_editor_toopstrip", 1);
	m_view_window->add_action_button_item(m_gui_binder, "stop all sounds", "sound_editor_toopstrip", 1);

}

void sound_scene_document::unregister_actions()
{
	m_input_engine->unregister_action("Save active");
	m_input_engine->unregister_action("Save all");
	m_input_engine->unregister_action("Open scene");
	m_input_engine->unregister_action("Undo");
	m_input_engine->unregister_action("Redo");
	m_input_engine->unregister_action("Cut");
	m_input_engine->unregister_action("Copy");
	m_input_engine->unregister_action("Paste");
	m_input_engine->unregister_action("Select all");
	m_input_engine->unregister_action("Delete");
	m_input_engine->unregister_action("manipulator drag axis mode");
	m_input_engine->unregister_action("select translate");
	m_input_engine->unregister_action("select rotate");
	m_input_engine->unregister_action("select none");
	m_input_engine->unregister_action("touch");
	m_input_engine->unregister_action("view maya move xy");
	m_input_engine->unregister_action("view maya move z");
	m_input_engine->unregister_action("view maya rotate");
	m_input_engine->unregister_action("view LE rotate");
	m_input_engine->unregister_action("view LE move y");
	m_input_engine->unregister_action("view LE move xz");
	m_input_engine->unregister_action("Focus to origin");
	m_input_engine->unregister_action("start all sounds");
	m_input_engine->unregister_action("stop all sounds");

	m_gui_binder->unbind(m_editor->multidocument_base->saveToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->saveAllToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->undoToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->redoToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->cutToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->copyToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->pasteToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->selectAllToolStripMenuItem);
	m_gui_binder->unbind(m_editor->multidocument_base->delToolStripMenuItem);
}

void sound_scene_document::on_form_closing(Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
{
	m_editor->toolbar->enable_buttons(false);
	super::on_form_closing(sender, e);
}
