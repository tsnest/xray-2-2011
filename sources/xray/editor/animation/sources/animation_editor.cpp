////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_editor.h"
#include "channels_editor.h"
#include "viewer_editor.h"
#include "animation_setup_manager.h"
#include "animation_groups_editor.h"
#include "channels_files_view_panel.h"
#include "animation_editor_contexts_manager.h"
#include "animation_collections_editor.h"
#include "channels_layout_panel.h"
#include "animation_viewer_toolbar_panel.h"
#include "animation_viewer_properties_panel.h"
#include "animation_viewer_time_panel.h"
#include "animation_editor_form.h"
#include "transform_control_helper.h"
#include "actions.h"
#include "animation_node_clip.h"
#include "images/images16x16.h"

#pragma managed( push, off )
#	include <xray/render/facade/common_types.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/collision/api.h>
#	include <xray/timing_timer.h>
//#	include <xray/animation/i_animation_controller.h>
#	include <xray/animation/mixing_expression.h>
#	include <xray/animation/mixing_binary_tree_visitor.h>
#	include <xray/resources.h>
#	include <xray/resources_query_result.h>
#	include <xray/render/facade/scene_renderer.h>
//for test
#	include <xray/ui/world.h>
#	include <xray/ui/ui.h>
#	include <xray/text_tree.h>
#	include <xray/text_tree_item.h>
#	include <xray/text_tree_log_helper.h>
#	include <xray/ui/text_tree_draw_helper.h>
#	include <xray/profiler.h>
#	include <xray/animation/i_animation_controller.h>
#	include <xray/animation/anim_track_common.h>
#pragma managed( pop )

using namespace WeifenLuo::WinFormsUI::Docking;
using xray::animation_editor::animation_editor;
using xray::animation_editor::navigation_controller_query;
using xray::animation_editor::animation_editor_form;

static animation_editor::animation_editor ( )
{
	m_single_animations_path		= "animations/single";
	m_animation_collections_path	= "animations/collections";
	m_animation_clips_path			= "animations/clips";
	m_animation_controllers_path	= "animations/controllers";
	m_animation_viewer_scenes_path	= "animations/viewer_scenes";
	m_model_path					= "character/human/neutral/neutral_01/neutral_01";
}

animation_editor::animation_editor(
	System::String^ rp, 
	xray::render::world& render_world, 
	xray::ui::world& ui_wrld,							// for test
	xray::editor_base::tool_window_holder^ holder
):m_renderer(NULL),
m_ui_world(ui_wrld),									// for test
m_render_world(render_world),
m_holder(holder)
{
	xray::g_profiler = NEW(xray::profiler)();
	START_PROFILE("constructor");
	m_resources_path = rp;
	m_name = "animation_editor";
	m_in_navigation_mode = false;
	m_navigation_paused = false;
	learning_mode = false;
	m_learning_controller_name = "";
	learning_max_residual = 70.0f;
	learning_max_iterations_count = 25;
	m_timer = NEW(xray::timing::timer)();
	m_controller = NEW(xray::rtp::animation_controller_ptr)();
	m_query_callback = NEW(navigation_controller_query)(gcnew navigation_controller_delegate(this, &animation_editor::controller_set_target_callback));
	m_controller_config = NEW(xray::configs::lua_config_ptr)();
	m_scene = NEW(render::scene_ptr);
	m_scene_view = NEW(render::scene_view_ptr);
	m_output_window = NEW(render::render_output_window_ptr);
	m_clips = gcnew clips_collection_type();
	m_time_scale = 1.0f;
	m_previous_target_time = 0;
	m_expressions_counter = 0;
	m_last_target_pos_y = 0.0f;
	m_navigation_model_added = false;
	m_control_path = NEW(float2)(0.0f, 0.3f);
	m_control_view_dir = NEW(float2)(0.0f, 0.3f);
	m_control_jump = false;
	render_debug_info = false;
	m_pv = NULL;
	initialize();
	m_holder->register_tool_window(this);
// for test
	m_text_wnd = m_ui_world.create_window();
	m_text_wnd->set_position(float2(0, 0));
	m_text_wnd->set_size(float2(600,600));
	m_text_wnd->set_visible(true);
	STOP_PROFILE;
}

animation_editor::~animation_editor()
{
}

void animation_editor::close_internal()
{
	if(m_closed)
		return;

	m_closed = true;
	clear_resources();
	m_holder->unregister_tool_window(this);
	delete this;
}

void animation_editor::query_create_render_resources()
{
 	START_PROFILE("query_create_render_resources");
	render::editor_renderer_configuration render_configuration;
 	render_configuration.m_create_particle_world = false;
 	System::Int32 hwnd = m_view_window->view_handle();
 	resources::user_data_variant* temp_data[] = {NEW(resources::user_data_variant), 0, NEW(resources::user_data_variant)};
 	temp_data[0]->set(render_configuration);
 	temp_data[2]->set(*(HWND*)&hwnd);
 	resources::user_data_variant const* data[] = {temp_data[0], temp_data[1], temp_data[2]};
 	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_editor::on_render_resources_ready), g_allocator);
 	const_buffer temp_buffer("", 1);
 	resources::creation_request requests[] =
 	{
 		resources::creation_request("animation_editor_scene", temp_buffer, resources::scene_class),
 		resources::creation_request("animation_editor_scene_view", temp_buffer, resources::scene_view_class),
 		resources::creation_request("animation_editor_render_output_window", temp_buffer, resources::render_output_window_class)
 	};
  	resources::query_create_resources(
  		requests,
  		boost::bind(&query_result_delegate::callback, q, _1),
  		g_allocator,
  		data
  	);
 	DELETE(temp_data[0]);
 	DELETE(temp_data[2]);
	STOP_PROFILE;
}

void animation_editor::on_render_resources_ready(xray::resources::queries_result& data)
{
  	START_PROFILE("on_render_resources_ready");
	R_ASSERT(data.is_successful());
	*m_scene = static_cast_resource_ptr<render::scene_ptr>(data[0].get_unmanaged_resource());
 	*m_scene_view = static_cast_resource_ptr<render::scene_view_ptr>(data[1].get_unmanaged_resource());
 	*m_output_window = static_cast_resource_ptr<render::render_output_window_ptr>(data[2].get_unmanaged_resource());
 	m_renderer = &m_render_world.editor_renderer();
 	m_view_window->setup_scene(m_scene->c_ptr(), m_scene_view->c_ptr(), *m_renderer, true );
	m_viewer_editor->on_scene_created(*m_scene);
	m_setup_manager->on_scene_created(*m_scene);
	m_collections_editor->scene_created(*m_scene);

	m_transform_control_helper = gcnew ae_transform_control_helper(this);
	m_transform_control_helper->m_collision_tree = m_view_window->m_collision_tree;
	m_transform_control_helper->m_editor_renderer = m_renderer;

	// transform control actions
	m_input_engine->register_action(gcnew mouse_action_editor_control("manipulator drag axis mode", this, 0, false), "LButton");

	m_transform_control_translation = gcnew transform_control_translation(m_transform_control_helper);
	m_input_engine->register_action(gcnew action_select_active_control("select translate", this, m_transform_control_translation), "W(View)");

	m_transform_control_rotation = gcnew transform_control_rotation(m_transform_control_helper);
	m_input_engine->register_action(gcnew action_select_active_control("select rotate", this, m_transform_control_rotation), "E(View)");
	m_input_engine->register_action(gcnew action_select_active_control("select none", this, nullptr), "Escape(View)");

	m_view_window->register_actions(m_input_engine, m_gui_binder , true);
	STOP_PROFILE;
}

void animation_editor::initialize()
{
  	START_PROFILE("initialize");
	m_form = gcnew animation_editor_form(m_name);
	m_form->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &animation_editor::on_editor_closing);
	m_form->main_dock_panel->ActiveContentChanged += gcnew System::EventHandler(this, &animation_editor::on_form_active_document_changed);
	m_save_file_dialog = gcnew System::Windows::Forms::SaveFileDialog();
	m_save_file_dialog->AddExtension = true;
	m_save_file_dialog->DefaultExt = ".avs";
	m_save_file_dialog->Filter = "Animation viewer scene files|*.avs";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/sources/animations/viewer_scenes");
	m_save_file_dialog->RestoreDirectory = true;
	m_save_file_dialog->Title = "Save animation viewer scene";

	m_open_file_dialog = gcnew System::Windows::Forms::OpenFileDialog();
	m_open_file_dialog->AddExtension = true;
	m_open_file_dialog->DefaultExt = ".controller";
	m_open_file_dialog->Filter = "Controller set files|*.controller";
	m_open_file_dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/sources/rtp/controllers");
	m_open_file_dialog->Multiselect = false;
	m_open_file_dialog->RestoreDirectory = true;
	m_open_file_dialog->Title = "Open controller set";

	m_menu = gcnew MenuStrip();
	m_menu_view = gcnew ToolStripMenuItem();
	m_menu_view_channels = gcnew ToolStripMenuItem();
	m_menu_view_viewer = gcnew ToolStripMenuItem();
	m_menu_view_setup = gcnew ToolStripMenuItem();
	m_menu_view_groups = gcnew ToolStripMenuItem();
	m_menu_view_viewport = gcnew ToolStripMenuItem();
	m_menu_view_collections = gcnew ToolStripMenuItem();
	m_menu->SuspendLayout();
	// m_menu
	m_menu->Items->AddRange(gcnew cli::array<ToolStripItem^ >(1){m_menu_view});
	m_menu->Location = System::Drawing::Point(0, 0);
	m_menu->Name = L"m_menu";
	m_menu->Size = System::Drawing::Size(292, 24);
	m_menu->TabIndex = 1;
	m_menu->Text = L"m_menu";

	// m_menu_view
	m_menu_view->DropDownItems->AddRange(gcnew cli::array<ToolStripItem^ >(6)
		{
			m_menu_view_channels,
			m_menu_view_viewer,
			m_menu_view_setup,
			m_menu_view_groups,
			m_menu_view_viewport,
			m_menu_view_collections,
		}
	);
	m_menu_view->Name = L"m_menu_view";
	m_menu_view->Size = System::Drawing::Size(37, 20);
	m_menu_view->Text = L"&View";
	m_menu_view->DropDownOpening += gcnew System::EventHandler(this, &animation_editor::view_on_drop_down_opening);

	// m_menu_view_channels
	m_menu_view_channels->Name = L"m_menu_view_channels";
	m_menu_view_channels->Size = System::Drawing::Size(197, 22);
	m_menu_view_channels->Text = L"&Channels";
	m_menu_view_channels->Click += gcnew System::EventHandler(this, &animation_editor::show_channels);

	// m_menu_view_viewer
	m_menu_view_viewer->Name = L"m_menu_view_viewer";
	m_menu_view_viewer->Size = System::Drawing::Size(197, 22);
	m_menu_view_viewer->Text = L"&Viewer";
	m_menu_view_viewer->Click += gcnew System::EventHandler(this, &animation_editor::show_viewer);

	// m_menu_view_setup
	m_menu_view_setup->Name = L"m_menu_view_setup";
	m_menu_view_setup->Size = System::Drawing::Size(197, 22);
	m_menu_view_setup->Text = L"&Setup manager";
	m_menu_view_setup->Click += gcnew System::EventHandler(this, &animation_editor::show_setup);

	// m_menu_view_groups
	m_menu_view_groups->Name = L"m_menu_view_groups";
	m_menu_view_groups->Size = System::Drawing::Size(197, 22);
	m_menu_view_groups->Text = L"&Animation groups";
	m_menu_view_groups->Click += gcnew System::EventHandler(this, &animation_editor::show_groups);

	// m_menu_view_collection
	m_menu_view_collections->Name = L"m_menu_view_collections";
	m_menu_view_collections->Size = System::Drawing::Size(197, 22);
	m_menu_view_collections->Text = L"&Animation Collections";
	m_menu_view_collections->Click += gcnew System::EventHandler(this, &animation_editor::show_collections);

	// m_menu_view_viewport
	m_menu_view_viewport->Name = L"m_menu_view_viewport";
	m_menu_view_viewport->Size = System::Drawing::Size(197, 22);
	m_menu_view_viewport->Text = L"&Scene view";
	m_menu_view_viewport->Click += gcnew System::EventHandler(this, &animation_editor::show_viewport);

	m_menu->ResumeLayout(false);
	m_menu->PerformLayout();
	m_form->Controls->Add(m_menu);

	m_view_window = gcnew scene_view_panel();
	m_view_window->HideOnClose = true;
	xray::math::float3 eye_pos = float3(0.f, 1.5f, -1.5f);
	m_view_window->setup_view_matrix(eye_pos, float3(0.0f,  0.f, 0.0f));
	m_view_window->action_focus_to_origin();

	m_channels_editor = gcnew channels_editor(this);
	m_viewer_editor = gcnew viewer_editor(this);
	m_setup_manager = gcnew animation_setup_manager(this);
	m_groups_editor = gcnew animation_groups_editor(this);
	m_collections_editor = gcnew animation_collections_editor( this );

	bool is_loaded_normally = xray::editor::controls::serializer::deserialize_dock_panel_root(m_form, m_form->main_dock_panel, m_name,
		gcnew DeserializeDockContent(this, &animation_editor::find_dock_content),
		false );
	if(!is_loaded_normally)
	{
		m_view_window->Show(m_form->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft);
		m_channels_editor->Show(m_form->main_dock_panel);
		m_viewer_editor->Show(m_form->main_dock_panel);
		m_setup_manager->Show(m_form->main_dock_panel);
		m_groups_editor->Show(m_form->main_dock_panel);
	}

	m_viewport_old_dock_state = m_view_window->DockState;

	m_view_window->m_collision_tree = &(*(collision::new_space_partitioning_tree(g_allocator, 1.f, 1024)));
	active_control = nullptr;

	m_input_engine = gcnew input_engine(input_keys_holder::ref, gcnew animation_editor_contexts_manager(this));
  	m_gui_binder = gcnew gui_binder(m_input_engine);

	//
	// Viewer Editor
	//
	action_delegate^ a = nullptr;
	a = gcnew action_delegate("New scene", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::new_document));
	m_input_engine->register_action(a, "Control+N(viewer_editor)");

	a = gcnew action_delegate("Save scene", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::save_active));
	a->set_enabled(gcnew enabled_delegate_managed(m_viewer_editor, &viewer_editor::predicate_save_scene));
	m_input_engine->register_action(a, "Control+S(viewer_editor)");

	a = gcnew action_delegate("Save as", gcnew execute_delegate_managed(m_viewer_editor, &viewer_editor::save_document_as));
	a->set_enabled(gcnew enabled_delegate_managed(m_viewer_editor, &viewer_editor::predicate_save_scene_as));
	m_input_engine->register_action(a, "");

	a = gcnew action_delegate("Lock/Unlock target", gcnew execute_delegate_managed(m_viewer_editor, &viewer_editor::change_locked));
	a->set_enabled(gcnew enabled_delegate_managed(m_viewer_editor, &viewer_editor::predicate_has_target));
	m_input_engine->register_action(a, "Control+L(viewer_editor)");

	a = gcnew action_delegate("Save selected node as clip", gcnew execute_delegate_managed(m_viewer_editor, &viewer_editor::save_node_as_clip));
	a->set_enabled(gcnew enabled_delegate_managed(m_viewer_editor, &viewer_editor::anim_node_selected));
	m_input_engine->register_action(a, "");

	a = gcnew action_delegate("Open scene", gcnew execute_delegate_managed(m_viewer_editor, &viewer_editor::on_open_click));
	m_input_engine->register_action(a, "Control+O(viewer_editor_active_doc)");

	a = gcnew action_delegate("Undo", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::undo));
	m_input_engine->register_action(a, "Control+Z(viewer_editor_active_doc)");

	a = gcnew action_delegate("Redo", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::redo));
	m_input_engine->register_action(a, "Control+Y(viewer_editor_active_doc)");

	a = gcnew action_delegate("Cut", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::cut));
	m_input_engine->register_action(a, "Control+X(viewer_editor_active_doc)");

	a = gcnew action_delegate("Copy", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::copy));
	m_input_engine->register_action(a, "Control+C(viewer_editor_active_doc)");

	a = gcnew action_delegate("Paste", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::paste));
	m_input_engine->register_action(a, "Control+V(viewer_editor_active_doc)");

	a = gcnew action_delegate("Select all", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::select_all));
	m_input_engine->register_action(a, "Control+A(viewer_editor_active_doc)");

	a = gcnew action_delegate("Delete", gcnew execute_delegate_managed(m_viewer_editor->multidocument_base, &xray::editor::controls::document_editor_base::del));
	m_input_engine->register_action(a, "Delete(viewer_editor_active_doc)");

	//
	// Setup Manager
	//
	a = gcnew action_delegate("Save animation", gcnew execute_delegate_managed(m_setup_manager->multidocument_base, &xray::editor::controls::document_editor_base::save_active));
	a->set_enabled(gcnew enabled_delegate_managed(m_setup_manager, &animation_setup_manager::predicate_save_scene));
	m_input_engine->register_action(a, "Control+S(animation_setup_manager)");

	a = gcnew action_delegate("Undo ", gcnew execute_delegate_managed(m_setup_manager->multidocument_base, &xray::editor::controls::document_editor_base::undo));
	m_input_engine->register_action(a, "Control+Z(setup_manager_active_doc)");

	a = gcnew action_delegate("Redo ", gcnew execute_delegate_managed(m_setup_manager->multidocument_base, &xray::editor::controls::document_editor_base::redo));
	m_input_engine->register_action(a, "Control+Y(setup_manager_active_doc)");

	//
	// Groups Editor
	//
	a = gcnew action_delegate("New controller", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::new_document));
	m_input_engine->register_action(a, "Control+N(groups_editor)");

	a = gcnew action_delegate("Save controller", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::save_active));
	a->set_enabled(gcnew enabled_delegate_managed(m_groups_editor, &animation_groups_editor::predicate_save_scene));
	m_input_engine->register_action(a, "Control+S(groups_editor_active_doc)");

	a = gcnew action_delegate("Save controller as", gcnew execute_delegate_managed(m_groups_editor, &animation_groups_editor::save_document_as));
	a->set_enabled(gcnew enabled_delegate_managed(m_groups_editor, &animation_groups_editor::predicate_save_scene_as));
	m_input_engine->register_action(a, "");

	a = gcnew action_delegate("Select controller model", gcnew execute_delegate_managed(m_groups_editor, &animation_groups_editor::select_model));
	a->set_enabled(gcnew enabled_delegate_managed(m_groups_editor, &animation_groups_editor::predicate_save_scene_as));
	m_input_engine->register_action(a, "Control+M(groups_editor_active_doc)");

	a = gcnew action_delegate("Learn controller", gcnew execute_delegate_managed(m_groups_editor, &animation_groups_editor::learn_controller));
	a->set_enabled(gcnew enabled_delegate_managed(m_groups_editor, &animation_groups_editor::predicate_learn));
	m_input_engine->register_action(a, "Control+L(groups_editor_active_doc)");

	a = gcnew action_delegate("Show animations", gcnew execute_delegate_managed(m_groups_editor, &animation_groups_editor::show_animations));
	a->set_enabled(gcnew enabled_delegate_managed(m_groups_editor, &animation_groups_editor::predicate_save_scene_as));
	m_input_engine->register_action(a, "Control+H(groups_editor_active_doc)");

	a = gcnew action_delegate("Collapse//Expand all", gcnew execute_delegate_managed(m_groups_editor, &animation_groups_editor::collapse_all_groups));
	a->set_enabled(gcnew enabled_delegate_managed(m_groups_editor, &animation_groups_editor::predicate_save_scene_as));
	m_input_engine->register_action(a, "Alt+C(groups_editor_active_doc)");

	a = gcnew action_delegate(" Undo", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::undo));
	m_input_engine->register_action(a, "Control+Z(groups_editor_active_doc)");

	a = gcnew action_delegate(" Redo", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::redo));
	m_input_engine->register_action(a, "Control+Y(groups_editor_active_doc)");

	a = gcnew action_delegate(" Cut", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::cut));
	m_input_engine->register_action(a, "Control+X(groups_editor_active_doc)");

	a = gcnew action_delegate(" Copy", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::copy));
	m_input_engine->register_action(a, "Control+C(groups_editor_active_doc)");

	a = gcnew action_delegate(" Paste", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::paste));
	m_input_engine->register_action(a, "Control+V(groups_editor_active_doc)");

	a = gcnew action_delegate(" Select all", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::select_all));
	m_input_engine->register_action(a, "Control+A(groups_editor_active_doc)");

	a = gcnew action_delegate(" Delete", gcnew execute_delegate_managed(m_groups_editor->multidocument_base, &xray::editor::controls::document_editor_base::del));
	m_input_engine->register_action(a, "Delete(groups_editor_active_doc)");

	//
	// Collections editor
	//
	a = gcnew action_delegate("Collection Doc Save ", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::save_active));
	m_input_engine->register_action(a, "Control+S(collections_editor)");

	a = gcnew action_delegate("Collection Doc Undo", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::undo));
	m_input_engine->register_action(a, "Control+Z(collections_editor_active_doc)");

	a = gcnew action_delegate("Collection Doc Redo", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::redo));
	m_input_engine->register_action(a, "Control+Y(collections_editor_active_doc)");

	a = gcnew action_delegate("Collection Doc Cut", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::cut));
	m_input_engine->register_action(a, "Control+X(collections_editor_active_doc)");

	a = gcnew action_delegate("Collection Doc Copy", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::copy));
	m_input_engine->register_action(a, "Control+C(collections_editor_active_doc)");

	a = gcnew action_delegate("Collection Doc Paste", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::paste));
	m_input_engine->register_action(a, "Control+V(collections_editor_active_doc)");

	a = gcnew action_delegate("Collection Doc Select all", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::select_all));
	m_input_engine->register_action(a, "Control+A(collections_editor_active_doc)");

	a = gcnew action_delegate("Collection Doc Delete", gcnew execute_delegate_managed(m_collections_editor->multidocument_base, &xray::editor::controls::document_editor_base::del));
	m_input_engine->register_action(a, "Delete(collections_editor_active_doc)");


	// Maya style mouse actions
	m_input_engine->register_action(gcnew mouse_action_view_move_xy("view maya move xy", m_view_window), "Alt+MButton(View)");
	m_input_engine->register_action(gcnew mouse_action_view_move_z("view maya move z", m_view_window), "Alt+RButton(View)");
	m_input_engine->register_action(gcnew mouse_action_view_rotate_around("view maya rotate", m_view_window), "Alt+LButton(View)");

	// Old LE style mouse actions
	m_input_engine->register_action(gcnew mouse_action_view_rotate("view LE rotate", m_view_window), "Space+LButton+RButton(View)");
	m_input_engine->register_action(gcnew mouse_action_view_move_y_reverse("view LE move y",	m_view_window), "Space+RButton(View)");
	m_input_engine->register_action(gcnew mouse_action_view_move_xz_reverse("view LE move xz", m_view_window), "Space+LButton(View)");
	m_input_engine->register_action(gcnew action_delegate("Focus to origin", gcnew execute_delegate_managed(this, &animation_editor::action_focus_to_origin)), "F(View)");

	// navigation controller actions
	m_input_engine->register_action(gcnew action_navigation_mode("switch navigation mode", this), "");
	m_input_engine->register_action(gcnew action_navigation_pause("pause navigation mode", this), "P(navigation_mode)");
	m_input_engine->register_action(gcnew action_switch_saving_navigation_scene("save navigation scene", this), "Ctrl+S(navigation_mode)");

	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller path vector left", this, -0.025f, 0.0f, false), "Left(navigation_mode)");
	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller path vector right", this, 0.025f, 0.0f, false), "Right(navigation_mode)");
	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller path vector up", this, 0.0f, 0.025f, false), "Up(navigation_mode)");
	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller path vector down", this, 0.0f, -0.025f, false), "Down(navigation_mode)");

	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller dir vector left", this, -0.025f, 0.0f, true), "Ctrl+Left(navigation_mode)");
	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller dir vector right", this, 0.025f, 0.0f, true), "Ctrl+Right(navigation_mode)");
	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller dir vector up", this, 0.0f, 0.025f, true), "Ctrl+Up(navigation_mode)");
	m_input_engine->register_action(gcnew action_change_navigation_vector("move controller dir vector down", this, 0.0f, -0.025f, true), "Ctrl+Down(navigation_mode)");

	//m_input_engine->register_action(gcnew action_navigation_jump("controller jump", this), "Space(navigation_mode)");
	m_input_engine->register_action(gcnew action_delegate("increase controller timescale", gcnew execute_delegate_managed(this, &animation_editor::inc_time_scale)), "OemCloseBrackets(navigation_mode)");
	m_input_engine->register_action(gcnew action_delegate("decrease controller timescale", gcnew execute_delegate_managed(this, &animation_editor::dec_time_scale)), "OemOpenBrackets(navigation_mode)");
	m_input_engine->register_action(gcnew action_delegate("reset controller timescale", gcnew execute_delegate_managed(this, &animation_editor::reset_time_scale)), "Oem5(navigation_mode)");
	m_input_engine->register_action(gcnew action_render_debug_info("render controller debug info", this), "R(navigation_mode)");

	// GUI BINDING
	m_gui_binder->bind( m_collections_editor->m_menu_file_save, "Collection Doc Save " );
	m_gui_binder->bind( m_collections_editor->m_menu_edit_undo, "Collection Doc Undo" );
	m_gui_binder->bind( m_collections_editor->m_menu_edit_redo, "Collection Doc Redo" );

	m_gui_binder->bind( m_viewer_editor->newToolStripMenuItem, "New scene" );
	m_gui_binder->bind( m_viewer_editor->saveToolStripMenuItem, "Save scene" );
	m_gui_binder->bind( m_viewer_editor->saveasToolStripMenuItem, "Save as" );
	m_gui_binder->bind( m_viewer_editor->openToolStripMenuItem, "Open scene" );
	m_gui_binder->bind( m_viewer_editor->lockToolStripMenuItem, "Lock/Unlock target" );
	m_gui_binder->bind( m_viewer_editor->saveClipToolStripMenuItem, "Save selected node as clip" );

	m_gui_binder->bind( m_viewer_editor->undoToolStripMenuItem, "Undo" );
	m_gui_binder->bind( m_viewer_editor->redoToolStripMenuItem, "Redo" );
	m_gui_binder->bind( m_viewer_editor->cutToolStripMenuItem, "Cut" );
	m_gui_binder->bind( m_viewer_editor->copyToolStripMenuItem, "Copy" );
	m_gui_binder->bind( m_viewer_editor->pasteToolStripMenuItem, "Paste" );
	m_gui_binder->bind( m_viewer_editor->selectAllToolStripMenuItem, "Select all" );
	m_gui_binder->bind( m_viewer_editor->delToolStripMenuItem, "Delete" );

	m_gui_binder->bind( m_setup_manager->undoToolStripMenuItem, "Undo " );
	m_gui_binder->bind( m_setup_manager->redoToolStripMenuItem, "Redo " );

	m_gui_binder->bind( m_groups_editor->m_menu_file_new, "New controller" );
	m_gui_binder->bind( m_groups_editor->m_menu_file_save, "Save controller" );
	m_gui_binder->bind( m_groups_editor->m_menu_file_save_as, "Save controller as" );
	m_gui_binder->bind( m_groups_editor->m_menu_file_select_model, "Select controller model" );
	m_gui_binder->bind( m_groups_editor->m_menu_file_learn, "Learn controller" );
	m_gui_binder->bind( m_groups_editor->m_menu_file_show_animations, "Show animations" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_undo, " Undo" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_redo, " Redo" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_cut, " Cut" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_copy, " Copy" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_paste, " Paste" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_select_all, " Select all" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_del, " Delete" );
	m_gui_binder->bind( m_groups_editor->m_menu_edit_collapse_all, "Collapse//Expand all" );

	ImageList^ anim_editor_image_list16x16 = image_loader::load_images("images16x16", "animation_editor.resources", 16, safe_cast<int>(xray::animation_editor::images16x16_count), this->GetType()->Assembly);
	image_loader loader;
	m_gui_binder->set_image_list("base", loader.get_shared_images16());
	m_gui_binder->set_image_list("animation_editor", anim_editor_image_list16x16);
	m_gui_binder->register_image("base", "switch navigation mode", mode_editor);
	m_gui_binder->register_image("base", "pause navigation mode", mode_pause);
	m_gui_binder->register_image("animation_editor", "save navigation scene", save_controller_scene);
	m_gui_binder->register_image("animation_editor", "render controller debug info", debug_info_render);

	m_view_window->create_tool_strip("MainToolStrip", 0);
	m_view_window->add_action_button_item(m_gui_binder, "switch navigation mode", "MainToolStrip", 1);
	m_view_window->add_action_button_item(m_gui_binder, "pause navigation mode", "MainToolStrip", 1);
	m_view_window->add_action_button_item(m_gui_binder, "save navigation scene", "MainToolStrip", 1);
	m_view_window->add_action_button_item(m_gui_binder, "render controller debug info", "MainToolStrip", 1);

	query_create_render_resources();
	STOP_PROFILE;
}

void animation_editor::clear_resources()
{
	m_ui_world.destroy_window(m_text_wnd);					// for test
	//m_ui_world.destroy_text_tree_view(m_text_tree_view);	// for test

	m_groups_editor->clear_resources();
	m_setup_manager->clear_resources();
	m_viewer_editor->clear_resources();
	m_view_window->clear_resources();
	m_view_window->clear_scene();
	collision::delete_space_partitioning_tree(m_transform_control_helper->m_collision_tree);
	m_renderer = NULL;
	if(m_pv)
	{
		DELETE(m_pv);
		m_pv = NULL;
	}

	DELETE(m_controller);
	DELETE(m_output_window);
	DELETE(m_scene_view);
	DELETE(m_scene);
	DELETE(m_control_path);
	DELETE(m_control_view_dir);
	DELETE(m_timer);
	DELETE(m_query_callback);
	DELETE(m_controller_config);

	for each(System::Collections::Generic::KeyValuePair<System::String^, animation_node_clip^>^ clip in m_clips)
		delete clip->Value;

	delete m_clips;
	delete m_transform_control_translation;
	delete m_transform_control_rotation;
	delete m_menu;
	delete m_menu_view;
	delete m_menu_view_channels;
	delete m_menu_view_viewer;
	delete m_menu_view_setup;
	delete m_menu_view_groups;
	delete m_menu_view_viewport;
	delete m_save_file_dialog;
	delete m_open_file_dialog;
	delete m_learning_controller_name;
	delete m_view_window;
	delete m_channels_editor;
	delete m_collections_editor;
	delete m_viewer_editor;
	delete m_setup_manager;
	delete m_groups_editor;
	delete m_input_engine;
	delete m_gui_binder;
	delete m_transform_control_helper;
	delete m_name;
	delete m_form;
}

bool animation_editor::close_query()
{
	if(m_in_navigation_mode)
		switch_navigation_mode();

	xray::editor::controls::serializer::serialize_dock_panel_root(m_form, m_form->main_dock_panel, m_name);
	bool close = m_channels_editor->close_query();
	close &= m_viewer_editor->close_query();
	close &= m_setup_manager->close_query();// const!!!
	close &= m_groups_editor->close_query();
	close &= m_collections_editor->close_query();
	return close;
}

System::Windows::Forms::Form^ animation_editor::main_form() 
{ 
	return m_form; 
}

void animation_editor::tick()
{
  	START_PROFILE("tick");
	if(!m_renderer)
		return;

	m_input_engine->execute();

	m_gui_binder->update_items();


	WeifenLuo::WinFormsUI::Docking::IDockContent^ ac = m_form->active_content;
	if(m_viewer_editor && ac==m_viewer_editor)
		m_viewer_editor->tick();

	if(m_setup_manager && ac==m_setup_manager)
		m_setup_manager->tick();

	if(m_groups_editor && ac==m_groups_editor)
		m_groups_editor->tick();

	if(m_collections_editor && ac==m_collections_editor)
		m_collections_editor->tick();

	tick_navigation_mode();

	if(!m_view_window->Visible)
		return;

	show_statistics();		// for test
	m_view_window->tick();	// process camera effector(s)
	m_renderer->scene().set_view_matrix(*m_scene_view, math::invert4x3(m_view_window->get_inverted_view_matrix()));
	m_renderer->scene().set_projection_matrix(*m_scene_view, m_view_window->get_projection_matrix());
	m_view_window->render();
	if(active_control)
	{
		active_control->update();
		active_control->draw(m_scene_view->c_ptr(), m_output_window->c_ptr());
	}

	m_renderer->draw_scene(*m_scene, *m_scene_view, *m_output_window, render::viewport_type(float2(0.f, 0.f), float2(1.f, 1.f)));
	STOP_PROFILE;
}

void animation_editor::Show( System::String^, System::String^ )
{
	m_form->Show();
	if(m_view_window->DockState==WeifenLuo::WinFormsUI::Docking::DockState::Hidden && m_view_window->IsFloat)
		m_view_window->Show();
}

void animation_editor::action_focus_to_origin()
{
	if(!m_in_navigation_mode)
		m_view_window->action_focus_to_origin();
	else
	{
		if(m_navigation_model_followed)
		{
			m_view_window->set_camera_effector(NULL);
			m_navigation_model_followed = false;
		}
		else
		{
			if(!m_navigation_model_added)
				return;

			m_view_window->set_camera_effector((*m_controller)->get_camera_follower());
			float4x4 m = (*m_controller)->get_camera_follower()->m_follow_object->get_object_matrix_for_camera();
			float3 position, direction;
			m_view_window->get_camera_props(position, direction);
			m_view_window->setup_view_matrix(position, m.c.xyz());
			m_view_window->set_view_point_distance((position-m.c.xyz()).length());
			m_navigation_model_followed = true;
		}
	}
}

IDockContent^ animation_editor::find_dock_content(System::String^ persist_string)
{
	if(persist_string=="xray.animation_editor.channels_editor")
		return m_channels_editor;
	else if(persist_string=="xray.animation_editor.viewer_editor")
		return m_viewer_editor;
	else if(persist_string=="xray.animation_editor.animation_setup_manager")
		return m_setup_manager;
	else if(persist_string=="xray.animation_editor.animation_groups_editor")
		return m_groups_editor;
	else if(persist_string=="xray.editor_base.scene_view_panel" )
		return m_view_window;
	else if(persist_string=="xray.animation_editor.animation_collections_editor" )
		return m_collections_editor;

	return (nullptr);
}

void animation_editor::on_editor_closing(Object^, FormClosingEventArgs^ e)
{
	e->Cancel = true;
	if(close_query())
		close_internal();
}

animation_editor_form^ animation_editor::form::get()
{
  	PROFILE_FUNCTION("form::get");
	return m_form;
}

void animation_editor::on_form_active_document_changed(System::Object^, System::EventArgs^)
{
	IDockContent^ new_content = m_form->main_dock_panel->ActiveContent;
	if(new_content==nullptr || new_content==m_view_window)
		new_content = m_form->main_dock_panel->ActiveDocument;

	if(m_form->active_content!=nullptr)
	{
		if( m_form->active_content == m_setup_manager )
			m_setup_manager->remove_model_from_render( );
		else if( m_form->active_content == m_viewer_editor )
			m_viewer_editor->remove_models_from_render( );
		else if( m_form->active_content == m_collections_editor )
			m_collections_editor->remove_models_from_render( );
	}

	m_form->active_content = new_content;
	if( m_form->active_content != nullptr )
	{
		if( m_form->active_content == m_setup_manager )
			m_setup_manager->add_model_to_render( );
		else if( m_form->active_content == m_viewer_editor )
			m_viewer_editor->add_models_to_render( );
		else if( m_form->active_content == m_collections_editor )
			m_collections_editor->add_models_to_render( );
	}
}

void animation_editor::switch_navigation_mode()
{
	if(m_in_navigation_mode)
	{
		if(!m_controller->c_ptr() || !(*m_controller)->is_loaded())
			return;

		m_view_window->set_camera_effector(NULL);
		m_channels_editor->Show(m_form->main_dock_panel);
		m_viewer_editor->Show(m_form->main_dock_panel);
		m_setup_manager->Show(m_form->main_dock_panel);
		m_groups_editor->Show(m_form->main_dock_panel);
		m_in_navigation_mode = false;
		if(m_navigation_model_added)
		{
			m_renderer->scene().remove_model(*m_scene, (*m_controller)->model()->m_render_model);
			(*m_controller)->get_controller()->clear_mixer();
		}

		m_navigation_model_added = false;
		(*m_controller) = NULL;
		m_view_window->Show(m_form->main_dock_panel, m_viewport_old_dock_state);
	}
	else
	{
		if(m_open_file_dialog->ShowDialog()==DialogResult::OK)
		{
			System::String^ file_name = m_open_file_dialog->SafeFileName;
			file_name = "resources/rtp/controllers/"+file_name->Remove(file_name->Length - 11);
			unmanaged_string path(file_name);
			m_channels_editor->Hide();
			m_viewer_editor->Hide();
			m_setup_manager->Hide();
			m_groups_editor->Hide();
			m_viewport_old_dock_state = m_view_window->DockState;
			m_view_window->Show(m_form->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document);
			if(m_form->active_content==m_setup_manager)
				m_setup_manager->remove_model_from_render();
			else if(m_form->active_content==m_viewer_editor)
				m_viewer_editor->remove_models_from_render();

			m_pv = NEW(xray::resources::user_data_variant)();
			xray::rtp::controller_resource_user_data data;
			m_pv->set<xray::rtp::controller_resource_user_data>(data);

			query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_editor::on_controller_loaded), g_allocator);
			xray::resources::query_resource(
				path.c_str(),
				xray::resources::animation_controller_class,
				boost::bind(&query_result_delegate::callback, rq, _1),
				g_allocator,
				m_pv
			);

			m_in_navigation_mode = true;
		}
	}
}

void animation_editor::on_controller_loaded(xray::resources::queries_result& result)
{
	if(result.is_successful())
	{
		(*m_controller) = static_cast_resource_ptr<xray::rtp::animation_controller_ptr>(result[0].get_unmanaged_resource());
		if(m_pv)
		{
			xray::rtp::controller_resource_user_data data;
			bool res = m_pv->try_get<xray::rtp::controller_resource_user_data>(data);
			if(res && data.name)
				FREE(data.name);

			DELETE(m_pv);
			m_pv = NULL;
		}
	}
}

void animation_editor::pause_navigation()
{
	m_navigation_paused = !m_navigation_paused;
	if(!m_navigation_paused)
		m_timer->start();
}

void animation_editor::tick_navigation_mode()
{
	if(!m_in_navigation_mode || ! m_controller->c_ptr() || !(*m_controller)->is_loaded())
		return;

	if(learning_mode)
	{
		bool res = (*m_controller)->learn_step(learning_max_residual, learning_max_iterations_count);
		if(!res)
		{
			String^ path = "resources/rtp/controllers/"+m_learning_controller_name+".controller";
			unmanaged_string save_path(path);
			xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config(save_path.c_str());
			(*m_controller)->save(cfg->get_root(), false);
			cfg->save(configs::target_sources);
			(*m_controller) = NULL;
			learning_mode = false;
			m_in_navigation_mode = false;
			MessageBox::Show(m_form, "Controller ["+m_learning_controller_name+"] learning finished succesfully",
				"Animation groups", MessageBoxButtons::OK, MessageBoxIcon::Information);
			m_learning_controller_name = "";
		}

		return;
	}

	if(!m_navigation_model_added && (*m_controller)->model().c_ptr())
	{
		m_expressions_counter = 0;
		m_last_target_pos_y = 0.0f;
		m_renderer->scene().add_model(*m_scene, (*m_controller)->model()->m_render_model, float4x4().identity());
		(*m_controller)->get_controller()->set_mixer_callback(boost::bind(&navigation_controller_query::callback, m_query_callback, _1, _2));
		move_controller();
		m_navigation_model_added = true;
		if(!m_navigation_paused)
			m_timer->start();

		if(m_navigation_model_followed)
		{
			m_view_window->set_camera_effector((*m_controller)->get_camera_follower());
			float4x4 m = (*m_controller)->get_camera_follower()->m_follow_object->get_object_matrix_for_camera();
			float3 position, direction;
			m_view_window->get_camera_props(position, direction);
			m_view_window->setup_view_matrix(position, m.c.xyz());
			m_view_window->set_view_point_distance((position-m.c.xyz()).length());
		}
	}

	if(!m_navigation_paused && m_navigation_model_added)
	{
		const float normal_frame_rate = animation::default_fps;
		float elapsed_sec_delta = (float)(m_timer->get_elapsed_sec());
		elapsed_sec_delta = elapsed_sec_delta * normal_frame_rate * m_time_scale;
		m_timer->start();
		(*m_controller)->dbg_update_walk(elapsed_sec_delta);
		xray::render::skeleton_model_ptr model = (*m_controller)->model();
		(*m_controller)->render(*m_scene, m_renderer->scene(), m_renderer->debug(), model, render_debug_info);
	}

	float4x4 m;
	(*m_controller)->get_controller()->get_bone_world_matrix(m, 0);
	const float length_factor = 0.5f;
	const float3 root_pos = m.c.xyz();
	const float3 control_path_end = root_pos + float3(m_control_path->x, 0, m_control_path->y) * length_factor * 3.f;
	m_renderer->draw_3D_screen_line(*m_scene, root_pos, control_path_end, math::color(155, 0, 0), 2.0f, 0xFFFFFFFF);
	const float3 root_dir_end = m.c.xyz() - float3(m.k.x, 0, m.k.z) * length_factor * 0.8f;
	m_renderer->draw_3D_screen_line(*m_scene, root_pos, root_dir_end, math::color(0, 155, 0), 2.0f, 0xFFFFFFFF);
	const float3 view_dir_end = root_pos + float3(m_control_view_dir->x, 0, m_control_view_dir->y) * length_factor * 3.f;;
	m_renderer->draw_3D_screen_line(*m_scene, root_pos, view_dir_end, math::color(0, 0, 155), 2.0f, 0xFFFFFFFF);
}

void animation_editor::move_controller()
{
	if(*m_controller)
		(*m_controller)->dbg_move_control(*m_control_path, *m_control_view_dir, m_control_jump);
}

void animation_editor::controller_set_target_callback(xray::animation::mixing::expression const& expr, u32 current_time_in_ms)
{
	if(m_controller_config->c_ptr())
	{
		xray::configs::lua_config_value root = (*m_controller_config)->get_root()["scene"]["nodes"][m_expressions_counter];
		System::String^ caption = "TARGET_"+m_expressions_counter.ToString();
		unmanaged_string capt(caption);
		root["caption"] = capt.c_str();
		root["type"] = 0;
		root["id"] = m_expressions_counter;
		if(m_previous_target_time==0)
			m_previous_target_time = current_time_in_ms;

		root["playing_time"] = (float)(current_time_in_ms - m_previous_target_time) / 1000.0f;
		float2 position(0.0f, m_last_target_pos_y);
		root["position"] = position;
		position.x += 250.0f;
		m_previous_target_time = current_time_in_ms;
		root = root["childs"][0];
		++m_expressions_counter;
		xray::animation::mixing::binary_tree_visitor* wr = xray::animation::create_binary_tree_writer(*g_allocator, root, position);
		expr.node().accept(*wr);
		m_last_target_pos_y = position.y + 100.0f;
		xray::animation::destroy_binary_tree_writer(*g_allocator, wr);
	}
}

void animation_editor::start_saving_controller_scene()
{
	if(!m_navigation_model_added)
		return;

	if(m_save_file_dialog->ShowDialog(m_form)==System::Windows::Forms::DialogResult::OK)
	{
		System::String^ path = m_save_file_dialog->FileName->Remove(0, m_save_file_dialog->InitialDirectory->Length+1);
		path = path->Remove(path->Length-4);

		unmanaged_string config_path( animation_editor::animation_viewer_scenes_path + "/" + path + ".avs" );
		*m_controller_config = xray::configs::create_lua_config(config_path.c_str());
		xray::configs::lua_config_value root = (*m_controller_config)->get_root()["scene"];

		float4x4 const& m = float4x4().identity();
		root["models"]["model_0"]["name"] = (*m_controller)->get_controller()->get_model_path();
		root["models"]["model_0"]["position"] = m.c.xyz();
		root["models"]["model_0"]["rotation"] = m.get_angles_xyz();
	}
}

void animation_editor::stop_saving_controller_scene()
{
	(*m_controller_config)->save(configs::target_sources);
	(*m_controller_config) = NULL;
}

bool animation_editor::is_saving_controller_scene()
{
  	PROFILE_FUNCTION("is_saving_controller_scene");
	if(m_controller_config->c_ptr())
		return true;

	return false;
}

void animation_editor::inc_time_scale()
{
	const float scale_factor = 1.1f;
	m_time_scale *= scale_factor;
	m_time_scale = xray::math::clamp_r(m_time_scale, 0.f, 100.f);
}

void animation_editor::dec_time_scale()
{
	const float scale_factor = 1.1f;
	m_time_scale /= scale_factor;
	m_time_scale = xray::math::clamp_r(m_time_scale, 0.f, 100.f);
}

void animation_editor::reset_time_scale()
{
	m_time_scale = 1.0f;
}

xray::animation_editor::animation_node_clip^ animation_editor::clip_by_name(System::String^ clip_name)
{
  	PROFILE_FUNCTION("clip_by_name");
	if(m_clips->ContainsKey(clip_name))
		return m_clips[clip_name];

	return nullptr;
}

void animation_editor::refresh_animation_node_clip(System::String^ name)
{
  	START_PROFILE("refresh_animation_node_clip");
	animation_node_clip^ clip = clip_by_name(name);
	if(clip!=nullptr)
		clip->load_intervals();
	STOP_PROFILE;
}

long animation_editor::request_animation_clip(System::String^ name, xray::animation_editor::animation_clip_request_callback^ callback)
{
  	PROFILE_FUNCTION("request_animation_clip");
	animation_node_clip^ clip = clip_by_name(name);
	if(clip!=nullptr)
	{
		callback(clip->make_instance());
		return clip->id_queried;
	}

	xray::fs_new::virtual_path_string animation_path;
	animation_path += unmanaged_string( animation_editor::single_animations_path + "/" ).c_str( );
	animation_path += unmanaged_string(name).c_str();
	xray::fs_new::virtual_path_string options_path = animation_path;
	options_path += ".options";

	xray::resources::request arr[2] = {
		{animation_path.c_str(), xray::resources::animation_class},
		{options_path.c_str(), xray::resources::lua_config_class}
	};

	animation_clip_query* rq = NEW(animation_clip_query)(gcnew request_delegate(this, &animation_editor::on_anim_clip_loaded), callback);
	return xray::resources::query_resources(arr,
		array_size(arr),
		boost::bind(&animation_clip_query::callback, rq, _1),
		g_allocator,
		0,
		0,
		assert_on_fail_false);
}

void animation_editor::on_anim_clip_loaded(xray::resources::queries_result& result, xray::animation_editor::animation_clip_request_callback^ cb)
{
  	PROFILE_FUNCTION("on_anim_clip_loaded");
	System::String^ requested_path = gcnew System::String(result[0].get_requested_path());
	requested_path = requested_path->Remove(0, animation_editor::single_animations_path->Length + 1 );
	if( result[0].is_successful( ) )
	{
		animation_node_clip^ new_clip = clip_by_name(requested_path);
		if(new_clip!=nullptr)
		{
			cb(new_clip->make_instance());
			return;
		}

		xray::configs::lua_config_ptr cfg;
		if(result[1].is_successful())
			cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[1].get_unmanaged_resource());
		else
		{
			System::String^ file_path = animation_editor::single_animations_path + "/" + requested_path + ".options";
			cfg = xray::configs::create_lua_config(unmanaged_string(file_path).c_str());
		}

		xray::configs::lua_config_value val = cfg->get_root();
		if(!val.value_exists("viewer_options"))
		{
			val["viewer_options"]["interpolator"]["type"] = 2;
			val["viewer_options"]["interpolator"]["time"] = 0.3f;
			val["viewer_options"]["interpolator"]["epsilon"] = 0.005f;
			val["viewer_options"]["synchronizing_group"] = -1;
			cfg->save(configs::target_sources);
		}

		xray::animation::skeleton_animation_ptr anim = static_cast_resource_ptr<xray::animation::skeleton_animation_ptr>(result[0].get_managed_resource());
		new_clip = gcnew animation_node_clip(requested_path, anim, cfg, m_form);
		new_clip->id_queried = result.uid();
		m_clips->Add(requested_path, new_clip);
		cb(new_clip->make_instance());
	}
	else
	{
		MessageBox::Show(m_form, "Animation ["+requested_path+"] loading failed!!!",
			"Animation editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		LOGI_INFO("animation_editor", "ANIMATION LOADING FAILED!!!");
		cb(nullptr);
	}
}

void animation_editor::start_learning_controller(System::String^ controller_name)
{
	m_learning_controller_name = controller_name;
	unmanaged_string name(controller_name);
	unmanaged_string path( animation_editor::animation_collections_path + "/" + controller_name );
	m_pv = NEW(xray::resources::user_data_variant)();
	xray::rtp::controller_resource_user_data data;
	data.empty_set = true;
	data.name = xray::strings::duplicate(g_allocator, name.c_str());
	m_pv->set<xray::rtp::controller_resource_user_data>(data);

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_editor::on_controller_loaded), g_allocator);
	xray::resources::query_resource(
		path.c_str(),
		xray::resources::animation_controller_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator,
		m_pv
	);

	learning_mode = true;
	m_in_navigation_mode = true;
}

void animation_editor::view_on_drop_down_opening(System::Object^, System::EventArgs^)
{
	m_menu_view_channels->Checked = !(m_channels_editor->IsHidden);
	m_menu_view_viewer->Checked = !(m_viewer_editor->IsHidden);
	m_menu_view_setup->Checked = !(m_setup_manager->IsHidden);
	m_menu_view_groups->Checked = !(m_groups_editor->IsHidden);
	m_menu_view_viewport->Checked = !(m_view_window->IsHidden);
	m_menu_view_collections->Checked = !(m_collections_editor->IsHidden);
}

void animation_editor::show_channels(System::Object^, System::EventArgs^)
{
	if(m_menu_view_channels->Checked)
		m_channels_editor->Hide();
	else
		m_channels_editor->Show(m_form->main_dock_panel);
}

void animation_editor::show_viewer(System::Object^, System::EventArgs^)
{
	if(m_menu_view_viewer->Checked)
		m_viewer_editor->Hide();
	else
		m_viewer_editor->Show(m_form->main_dock_panel);
}

void animation_editor::show_setup(System::Object^, System::EventArgs^)
{
	if(m_menu_view_setup->Checked)
		m_setup_manager->Hide();
	else
		m_setup_manager->Show(m_form->main_dock_panel);
}

void animation_editor::show_groups(System::Object^, System::EventArgs^)
{
	if(m_menu_view_groups->Checked)
		m_groups_editor->Hide();
	else
		m_groups_editor->Show(m_form->main_dock_panel);
}

void animation_editor::show_collections(System::Object^, System::EventArgs^)
{
	if( m_menu_view_collections->Checked )
		m_collections_editor->Hide( );
	else
		m_collections_editor->Show( m_form->main_dock_panel );
}

void animation_editor::show_viewport(System::Object^, System::EventArgs^)
{
	if(m_menu_view_viewport->Checked)
		m_view_window->Hide();
	else
		m_view_window->Show();
}

// for test
#include "animation_node_interval.h"
void animation_editor::show_statistics()
{
	using xray::strings::text_tree;
	using xray::strings::text_tree_item;
	using xray::strings::text_tree_log_helper;
	
	pvoid buffer = ALLOCA(64*xray::Kb);
	text_tree m_text_tree_view(buffer, 64*xray::Kb, " ----------- Loaded animations --------------- "); 
	xray::get_profiler().show_stats(&m_text_tree_view.root(), true);
	//text_tree m_text_tree_view(buffer, 64*xray::Kb, " ----------- Loaded animations --------------- "); 
	//for each(KeyValuePair<String^, animation_node_clip^> p in m_clips)
	//{
	//	unmanaged_string clip_name(p.Key);
	//	text_tree_item* clip_node = m_text_tree_view.root().new_child(clip_name.c_str());
	//	text_tree_item* descr = clip_node->new_child("type");
	//	descr->add_column("id");
	//	descr->add_column("offset");
	//	descr->add_column("length");
	//	for each(animation_node_interval^ interval in p.Value->intervals)
	//	{
	//		unmanaged_string interval_name(interval->ToString());
	//		text_tree_item* interval_node = clip_node->new_child(interval_name.c_str());
	//		interval_node->add_column(interval->id);
	//		interval_node->add_column(interval->offset);
	//		interval_node->add_column(interval->length);
	//	}

	//	m_text_tree_view.break_page();
	//}

	m_text_wnd->remove_all_childs();

	using ui::text_tree_draw_helper;
	using ui::text_tree_draw_helper_params;
	text_tree_draw_helper_params p;
	p.color1 = 0xffff0000;
	p.color2 = 0xff00ff00;
	p.is_multipaged = true;
	p.fnt = xray::ui::fnt_arial;
	p.row_height = 15.0f;
	p.space_between_pages = 10.0f;
	p.start_pos = float2(0.0f, 0.0f);
	text_tree_draw_helper h(m_ui_world, m_text_wnd, p, *g_allocator);
	h.output(&m_text_tree_view.root());

	//text_tree_log_helper h2;
	//h2.output(&m_text_tree_view.root());
	m_text_wnd->tick();
	m_text_wnd->draw(m_ui_world.get_renderer(), *m_scene_view);
}
