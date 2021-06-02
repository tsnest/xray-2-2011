////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "viewer_document.h"
#include "viewer_editor.h"
#include "animation_editor.h"
#include "animation_editor_form.h"
#include "animation_model.h"
#include "animation_viewer_hypergraph.h"
#include "animation_node_connection_editor.h"
#include "animation_viewer_links_manager.h"
#include "animation_viewer_graph_node_style.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_viewer_graph_node_operator.h"
#include "animation_viewer_graph_node_root.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_weighted.h"
#include "animation_viewer_graph_link_timed.h"
#include "animation_viewer_time_panel.h"
#include "animation_node_clip_instance.h"
#include "animation_node_clip.h"
#include "animation_node_interval.h"
#include "animation_node_playing_instance.h"
#include "time_scale_instance.h"
#include "animation_node_interpolator.h"
#include "time_instance.h"
#include "models_panel.h"
#include "transform_control_helper.h"

#include <xray/strings_stream.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/animation/anim_track_common.h>

using xray::animation_editor::viewer_document;
using xray::animation_editor::viewer_editor;
using xray::animation_editor::animation_viewer_graph_node_base;

viewer_document::viewer_document(viewer_editor^ ed)
:super(ed->multidocument_base),
m_editor(ed),
m_first_save(true),
m_is_loaded(false)
{
	m_timer = NEW(xray::timing::timer)();
	m_active_model = nullptr;
	m_target = nullptr;
	m_models = gcnew models_list();
	m_lexemes = NEW(lexemes_collection_type)();
	m_animation_items = gcnew animation_items_list();
	m_assigned_ids = gcnew ids_by_anim_name_type();
	m_prev_target_ids = gcnew List<u32>();
	m_cur_target_ids = gcnew List<u32>();
	m_error_message = "";
	m_max_time = 10000.0f;
	m_current_time = 0.0f;
	m_time_scale = 1.0f;
	m_paused = true;
	m_models_changed = false;
	m_target_locked = false;

	SuspendLayout();

	// m_hypergraph
	m_hypergraph = (gcnew animation_viewer_hypergraph(this));
	m_hypergraph->AllowDrop = true;
	m_hypergraph->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
		| System::Windows::Forms::AnchorStyles::Left) 
		| System::Windows::Forms::AnchorStyles::Right));
	m_hypergraph->BackColor = System::Drawing::SystemColors::Window;
	m_hypergraph->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
	m_hypergraph->connection_editor = gcnew animation_node_connection_editor(this);
	m_hypergraph->current_view_mode = xray::editor::controls::hypergraph::view_mode::full;
	m_hypergraph->focused_link = nullptr;
	m_hypergraph->focused_node = nullptr;
	m_hypergraph->Location = System::Drawing::Point(2, 27);
	m_hypergraph->max_scale = 2.0F;
	m_hypergraph->min_scale = 0.5F;
	m_hypergraph->move_mode = xray::editor::controls::hypergraph::nodes_movability::movable;
	m_hypergraph->Name = L"m_hypergraph";
	m_hypergraph->node_size_brief = System::Drawing::Size(80, 40);
	m_hypergraph->nodes_style_manager = m_editor->node_style_manager;
	m_hypergraph->scale = 1.0F;					
	m_hypergraph->Size = System::Drawing::Size(288, 244);
	m_hypergraph->TabIndex = 0;
	m_hypergraph->visibility_mode = xray::editor::controls::hypergraph::nodes_visibility::all;
	m_hypergraph->Click += gcnew System::EventHandler(this, &viewer_document::on_hypergraph_click);
	m_hypergraph->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &viewer_document::on_hypergraph_drag_over);
	m_hypergraph->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &viewer_document::on_hypergraph_drag_drop);
	m_hypergraph->on_selection_changed += gcnew xray::editor::controls::hypergraph::selection_changed(this, &viewer_document::on_hypergraph_selection_changed);
	m_hypergraph->links_manager	= gcnew	animation_viewer_links_manager(m_hypergraph);
	m_hypergraph->clear();

	// m_save_file_dialog
	m_save_file_dialog = gcnew System::Windows::Forms::SaveFileDialog();
	m_save_file_dialog->AddExtension = true;
	m_save_file_dialog->DefaultExt = ".avs";
	m_save_file_dialog->Filter = "Animation viewer scene files|*.avs";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/sources/animations/viewer_scenes");
	m_save_file_dialog->RestoreDirectory = true;
	m_save_file_dialog->SupportMultiDottedExtensions = true;
	m_save_file_dialog->Title = "New document";

	AllowEndUserDocking = false;
	VisibleChanged += gcnew System::EventHandler(this, &viewer_document::on_document_activated);
	LostFocus += gcnew System::EventHandler(this, &viewer_document::on_focus_losted);
	Controls->Add(m_hypergraph);
	ResumeLayout(false);
}

viewer_document::~viewer_document()
{
	set_camera_follower(false);
	if (components)
		delete components;

	m_target = nullptr;
	set_active_model(nullptr);
	m_animation_items->Clear();
	for each(KeyValuePair<String^, animation_model^>^ p in m_models)
	{
		if(m_editor->get_animation_editor()->form->active_content==m_editor)
			p->Value->remove_from_render();

		delete p->Value;
	}

	DELETE(m_timer);
	DELETE(m_lexemes);
	delete m_assigned_ids;
	delete m_prev_target_ids;
	delete m_cur_target_ids;
	delete m_models;
	delete m_animation_items;
	delete m_hypergraph->links_manager;
	delete m_hypergraph->connection_editor;
	delete m_hypergraph;
	delete m_save_file_dialog;
	delete m_error_message;
}

void viewer_document::save_as()
{
	bool tmp_1 = is_saved;
	bool tmp_2 = m_first_save;
	is_saved = false;
	m_first_save = true;

	save();

	is_saved = tmp_1;
	m_first_save = tmp_2;
}

void viewer_document::save()
{
	if( is_saved && !m_first_save )
		return;

	if( m_first_save )
	{
		m_save_file_dialog->DefaultExt = ".avs";
		m_save_file_dialog->Filter = "Animation viewer scene files|*.avs";
		m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/sources/animations/viewer_scenes");
		if(m_save_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
		{
			System::String^ name_with_ext = m_save_file_dialog->FileName->Remove(0, m_save_file_dialog->InitialDirectory->Length+1);
			Name = name_with_ext->Remove(name_with_ext->Length-4, 4);
			Text = Name;
		}
		else
			return;
	}

	System::String^ file_path = m_editor->source_path+"/"+Name+".avs";
	unmanaged_string path = unmanaged_string(file_path);
	xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config(path.c_str());
	xray::configs::lua_config_value scene_cfg = cfg->get_root()["scene"];
	m_hypergraph->save(scene_cfg);
	m_hypergraph->set_saved_flag();

	for each(KeyValuePair<String^, animation_model^>^ p in m_models)
	{
		unmanaged_string name = unmanaged_string(p->Key);
		unmanaged_string model_name = unmanaged_string(p->Value->model_name);
		scene_cfg["models"][name.c_str()]["name"] = model_name.c_str();

		float4x4 const& m = p->Value->transform;
		scene_cfg["models"][name.c_str()]["position"] = m.c.xyz();
		scene_cfg["models"][name.c_str()]["rotation"] = m.get_angles_xyz();
	}

	scene_cfg["options"]["max_time"] = max_time;
	scene_cfg["options"]["time_scale"] = time_scale;
	scene_cfg["options"]["time_layout_scale"] = m_editor->time_panel->get_time_layout_scale();

	cfg->save(configs::target_sources);
	m_editor->change_recent_list(Name, true);
	m_first_save = false;
	m_models_changed = false;
}

void viewer_document::load()
{
	m_first_save = false;
	fs_new::virtual_path_string path;
	path += unmanaged_string( animation_editor::animation_viewer_scenes_path + "/" ).c_str( );
	path += unmanaged_string(Name).c_str();
	path += ".avs";

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &viewer_document::on_resources_loaded), g_allocator);
	xray::resources::query_resource(
		path.c_str(),
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void viewer_document::on_focus_losted(System::Object^, System::EventArgs^)
{
	if(m_hypergraph->selected_nodes->Count>0 && !this->Disposing)
	{
		MouseEventArgs^ e = gcnew MouseEventArgs(System::Windows::Forms::MouseButtons::Left, 1, MousePosition.X, MousePosition.Y, 0);
		for each(xray::editor::controls::hypergraph::node^ n in m_hypergraph->selected_nodes)
			n->hypergraph_node_MouseUp(nullptr, e);
	}
}

void viewer_document::on_resources_loaded(xray::resources::queries_result& result)
{
	if(result[0].is_successful())
	{
		xray::strings::stream stream(g_allocator);
		xray::configs::lua_config_ptr cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[0].get_unmanaged_resource());
		cfg->save(stream);
		stream.append("%c", 0);
		m_hypergraph->add_group((pcstr)stream.get_buffer(), System::Drawing::Point(0,0));
		m_hypergraph->set_selected_node((xray::editor::controls::hypergraph::node^)nullptr);
		m_hypergraph->clear_command_stack();
		m_hypergraph->set_saved_flag();
		xray::configs::lua_config_value scene_cfg = cfg->get_root()["scene"];
		R_ASSERT(scene_cfg.value_exists("models"));
		xray::configs::lua_config_iterator it = scene_cfg["models"].begin();
		for(; it!=scene_cfg["models"].end(); ++it)
		{
			System::String^ name = gcnew System::String((pcstr)it.key());
			System::String^ model_name = gcnew System::String((pcstr)(*it)["name"]);
			float3 pos = (float3)(*it)["position"];
			float3 angles = (float3)(*it)["rotation"];
			float4x4 m = create_translation(pos) * create_rotation(angles);
			query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &viewer_document::on_new_model_loaded), g_allocator);
			animation_model^ loaded_model = gcnew animation_model( m_editor->scene(), m_editor->get_animation_editor()->get_renderer().scene(), m_editor->get_animation_editor()->get_renderer().debug(), name, model_name, m, rq);
			m_models->Add(name, loaded_model);
			set_active_model(name);
		}

		if(scene_cfg.value_exists("options"))
		{
			max_time = (float)scene_cfg["options"]["max_time"];
			time_scale = (float)scene_cfg["options"]["time_scale"];
			m_editor->time_panel->set_time_layout_scale((float)scene_cfg["options"]["time_layout_scale"]);
		}

		m_editor->time_panel->update();
		is_saved = true;
		m_is_loaded = true;
	}
}



void viewer_document::undo()
{
	m_hypergraph->undo();
}

void viewer_document::redo()
{
	m_hypergraph->redo();
}

void viewer_document::select_all()
{
	m_hypergraph->set_selected_node(m_hypergraph->all_nodes());
	m_hypergraph->selected_links->Clear();
	m_hypergraph->selected_links->AddRange(m_hypergraph->links_manager->visible_links());
	m_hypergraph->Invalidate(false);
}

void viewer_document::copy(bool del)
{
	nodes^ nds = m_hypergraph->selected_nodes;
	links^ lnks = m_hypergraph->copy_links();
	links^ copied_lnks = gcnew links();
	copied_lnks->AddRange(lnks);
	for each(xray::editor::controls::hypergraph::link^ lnk in lnks)
	{
		if(!(nds->Contains(lnk->source_node) && nds->Contains(lnk->destination_node)))
			copied_lnks->Remove(lnk);
	}

	if(nds->Count==0)
		return;

	configs::lua_config_ptr cfg = configs::create_lua_config();
	xray::strings::stream stream(g_allocator);

	m_hypergraph->save(cfg->get_root()["scene"], nds, copied_lnks);

	cfg->save(stream);
	stream.append("%c", 0);
	xray::os::copy_to_clipboard((pcstr)stream.get_buffer());
	if(del)
	{
		set_target(nullptr);
		m_hypergraph->remove_group(nds, lnks);
	}
}

void viewer_document::paste()
{
	pstr string	= xray::os::paste_from_clipboard(*g_allocator);
	if(!string)
		return;

	m_hypergraph->add_group(string, m_hypergraph->PointToClient(MousePosition));
	FREE(string);
}

void viewer_document::del()
{
	links^ lnks = gcnew links();
	lnks->AddRange(m_hypergraph->copy_links());
	for each(xray::editor::controls::hypergraph::link^ lnk in m_hypergraph->selected_links)
		if(!lnks->Contains(lnk))
			lnks->Add(lnk);

	nodes^ nds = gcnew nodes();
	nds->AddRange(m_hypergraph->selected_nodes);
	m_hypergraph->set_selected_node((nodes^)nullptr);
	if(nds->Count!=0 || lnks->Count!=0)
	{
		set_target(nullptr);
		m_hypergraph->remove_group(nds, lnks);
	}

	m_hypergraph->selected_links->Clear();
}

void viewer_document::add_connection(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination)
{
	if(source==destination)
		return;

	animation_viewer_graph_link^ lnk = create_connection(source, destination);
	if(lnk!=nullptr)
	{
		links^ lnks = gcnew links();
		lnks->Add(lnk);
		m_hypergraph->add_connection(lnks);
	}
}

xray::animation_editor::animation_viewer_graph_link^ viewer_document::create_connection(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination)
{
	animation_viewer_graph_node_base^ src = safe_cast<animation_viewer_graph_node_base^>(source);
	animation_viewer_graph_node_base^ dst = safe_cast<animation_viewer_graph_node_base^>(destination);
	R_ASSERT(src && dst);
	if(m_hypergraph->can_make_connection(src, dst))
	{
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::create_link(src, dst);
		m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
		animation_viewer_graph_node_operator^ dst_op = dynamic_cast<animation_viewer_graph_node_operator^>(dst);
		if(dst_op)
		{
			if(dst_op->childs->Count>1)
				safe_cast<animation_viewer_graph_link_weighted^>(lnk)->weight = 0.0f;
			else
				safe_cast<animation_viewer_graph_link_weighted^>(lnk)->weight = 1.0f;
		}

		return lnk;
	}
	return nullptr;
}

void viewer_document::on_hypergraph_drag_over(System::Object^ , System::Windows::Forms::DragEventArgs^ e)
{
	if(e->Data->GetDataPresent(viewer_drag_drop_operation::typeid))
	{
		e->Effect = DragDropEffects::Copy;
		return;
	}

	e->Effect = DragDropEffects::None;
}

void viewer_document::on_hypergraph_drag_drop(System::Object^ , System::Windows::Forms::DragEventArgs^ e)
{
	if(e->Data->GetDataPresent(viewer_drag_drop_operation::typeid))
	{
		viewer_drag_drop_operation^ o = safe_cast<viewer_drag_drop_operation^>(e->Data->GetData(viewer_drag_drop_operation::typeid));
		if(e->Effect==DragDropEffects::Copy)
		{
			switch(*o)
			{
			case viewer_drag_drop_operation::root_node:
				{
					on_drop_root_node(e);
					break;
				}
			case viewer_drag_drop_operation::animation_node:
				{
					on_drop_animation_node(e);
					break;
				}
			case viewer_drag_drop_operation::operator_node:
				{
					on_drop_operator_node(e);
					break;
				}
			}
		}
	}
	else
		e->Effect = DragDropEffects::None;
}

void viewer_document::on_drop_animation_node(System::Windows::Forms::DragEventArgs^ e)
{
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_ptr cfg = configs::create_lua_config();
	xray::configs::lua_config_value val = cfg->get_root()["scene"]["nodes"][0];
	val["id"] = 0;
	val["interpolator"]["type"] = 2;
	val["interpolator"]["epsilon"] = 0.005f;
	val["interpolator"]["time"] = 0.3f;
	val["mixing_type"] = 0;
	val["offset"] = 0.0f;
	val["pos_x"] = 0;
	val["pos_y"] = 0;
	val["time_scale"] = 1.0f;
	val["type"] = 1;
	cfg->save(stream);
	stream.append("%c", 0);
	m_hypergraph->add_group((pcstr)stream.get_buffer(), m_hypergraph->PointToClient(Point(e->X, e->Y)));
}

void viewer_document::on_drop_operator_node(System::Windows::Forms::DragEventArgs^ e)
{
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_ptr cfg = configs::create_lua_config();
	xray::configs::lua_config_value val = cfg->get_root()["scene"]["nodes"][0];
	val["id"] = 0;
	val["pos_x"] = 0;
	val["pos_y"] = 0;
	val["type"] = 2;
	cfg->save(stream);
	stream.append("%c", 0);
	m_hypergraph->add_group((pcstr)stream.get_buffer(), m_hypergraph->PointToClient(Point(e->X, e->Y)));
}

void viewer_document::on_drop_root_node(System::Windows::Forms::DragEventArgs^ e)
{
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_ptr cfg = configs::create_lua_config();
	xray::configs::lua_config_value val = cfg->get_root()["scene"]["nodes"][0];
	val["caption"] = "TEMP";
	val["id"] = 0;
	val["pos_x"] = 0;
	val["pos_y"] = 0;
	val["type"] = 0;
	cfg->save(stream);
	stream.append("%c", 0);
	m_hypergraph->add_group((pcstr)stream.get_buffer(), m_hypergraph->PointToClient(Point(e->X, e->Y)));
}

void viewer_document::on_property_changed(wpf_property_container^ pc, System::String^ l, Object^ new_val, Object^ old_val)
{
	m_hypergraph->on_property_changed(pc, l, new_val, old_val);
}

void viewer_document::on_hypergraph_selection_changed()
{
	if(!m_is_loaded && !m_first_save)
		return;

	if(m_hypergraph->selected_nodes->Count!=0)
	{
		animation_viewer_graph_node_base^ n = safe_cast<animation_viewer_graph_node_base^>(m_hypergraph->selected_nodes[0]);
		animation_viewer_graph_node_animation^ anim = dynamic_cast<animation_viewer_graph_node_animation^>(n);
		if(anim)
			m_editor->show_properties(anim->container());
		else
			m_editor->show_properties(n);

		if(!m_target_locked)
			set_target(n);

		return;
	}

	if(!m_target_locked)
		set_target(nullptr);

	m_editor->show_properties(nullptr);
}

void viewer_document::on_hypergraph_click(System::Object^ , System::EventArgs^ e)
{
	if((ModifierKeys & Keys::Shift) == Keys::Shift ||		
		(ModifierKeys & Keys::Alt) == Keys::Alt)
		return;

	if(safe_cast<System::Windows::Forms::MouseEventArgs^>(e)->Button==System::Windows::Forms::MouseButtons::Left)
	{
		links^ l = m_hypergraph->get_links_under_cursor();
		if(l->Count!=0)
		{
			animation_viewer_graph_link^ lnk = dynamic_cast<animation_viewer_graph_link^>(l[0]);
			m_editor->show_properties(lnk->container());
		}
		else if(m_hypergraph->selected_nodes->Count==0)
			m_editor->show_properties(nullptr);
	}
}

void viewer_document::on_document_activated(System::Object^, System::EventArgs^)
{
	if(Visible)
	{
		if(m_editor->get_animation_editor()->form->active_content==m_editor)
			add_models_to_render();

		set_camera_follower(m_editor->camera_follows_npc);
		reset_target();
		on_hypergraph_selection_changed();
		m_editor->time_panel->update();
	}
	else
	{
		set_camera_follower(false);
		if(m_editor->get_animation_editor()->form->active_content==m_editor)
			remove_models_from_render();
	}
}

void viewer_document::set_animation_node_new_intervals_list(animation_viewer_graph_node_base^ n, List<xray::animation_editor::animation_node_interval^>^ lst, u32 cycle_index)
{
	m_hypergraph->set_animation_node_new_intervals_list(n, lst, cycle_index);
}

void viewer_document::tick()
{
	if(m_error_message!="")
	{
		MessageBox::Show(this, m_error_message, "Viewer editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		m_error_message = "";
	}

	if(m_models->Count>0)
	{
		if(!paused)
		{
			float elapsed_ms_delta = m_timer->get_elapsed_msec() * m_time_scale;
			m_timer->start();
			current_time += elapsed_ms_delta;
		}

		for each(KeyValuePair<String^, animation_model^>^ p in m_models)
			p->Value->render();
	}
}

void viewer_document::home()
{
	current_time = 0.0f;
}

void viewer_document::move_backward()
{
	paused = true;
	current_time -= 1000.0f/animation::default_fps*m_time_scale;
}

void viewer_document::play()
{
	paused = !paused;
}

void viewer_document::stop()
{
	paused = true;
	current_time = 0.0f;
}

void viewer_document::move_forward()
{
	paused = true;
	current_time += 1000.0f/animation::default_fps*m_time_scale;
}

void viewer_document::end()
{
	current_time = max_time;
}

void viewer_document::paused::set(bool new_val)
{
	m_paused = new_val;
	m_editor->time_panel->update_time();
	if(!m_paused)
		m_timer->start();
}

void viewer_document::time_scale::set(float new_val)
{
	m_time_scale = new_val;
}

void viewer_document::max_time::set(float new_val)
{
	m_max_time = new_val;
	current_time = current_time;
}

void viewer_document::current_time::set(float new_val)
{
	if(m_editor->play_from_beginning)
	{
		if(new_val>max_time)
			new_val = 0.0f;
		else if(new_val<0)
			new_val = max_time;
	}
	else
	{
		if(new_val>max_time)
			new_val = max_time;
		else if(new_val<0)
			new_val = 0.0f;
	}

	m_current_time = new_val;
	R_ASSERT(m_current_time>=0.0f && m_current_time<=max_time);
	if(m_target)
	{
		R_ASSERT(m_active_model!=nullptr);
		u32 cur_tm = (u32)System::Math::Round(m_current_time);
		vectora<editor_animation_state> cur_anim_states(g_allocator);
		set_target_impl();
		m_active_model->tick(cur_tm);
		m_active_model->get_current_anim_states(cur_anim_states, cur_tm);
		update_animation_items(cur_anim_states);
	}

	m_editor->time_panel->update_time();
}


void viewer_document::add_models_to_render()
{
	if(m_models->Count>0)
	{
		for each(KeyValuePair<String^, animation_model^>^ p in m_models)
			p->Value->add_to_render();
	}
}

void viewer_document::remove_models_from_render()
{
	if(m_models->Count>0)
	{
		for each(KeyValuePair<String^, animation_model^>^ p in m_models)
			p->Value->remove_from_render();
	}
}

void viewer_document::set_camera_follower(bool val)
{
	if(m_editor->get_animation_editor()->viewport==nullptr || m_active_model==nullptr)
		return;

	if(val)
	{
		float3 model_pos = m_active_model->get_object_matrix().c.xyz();
		m_editor->get_animation_editor()->viewport->set_camera_effector(m_active_model->get_camera_follower());
		m_editor->get_animation_editor()->viewport->action_focus_to_point(model_pos);
		m_active_model->camera_follow_me = true;
	}
	else
	{
		m_editor->get_animation_editor()->viewport->set_camera_effector(NULL);
		m_active_model->camera_follow_me = false;
	}
}

void viewer_document::add_model(System::String^ model_path, System::String^ model_name)
{
	if(m_models->ContainsKey(model_name))
	{
		System::Windows::Forms::MessageBox::Show(this, "Model with name ["+model_name+"] already exsists!", 
			"Viewer editor", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
		return;
	}
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &viewer_document::on_new_model_loaded), g_allocator);
	animation_model^ new_model = gcnew animation_model( m_editor->scene(), m_editor->get_animation_editor()->get_renderer().scene(), m_editor->get_animation_editor()->get_renderer().debug(), model_name, model_path, float4x4().identity(), rq);
	m_models->Add(model_name, new_model);
	if(m_models->Count==1)
		set_active_model(model_name);

	m_models_changed = true;
}

void viewer_document::add_model(System::String^ model_path)
{
	System::String^ model_name = "model_";
	UInt32 i = 0;
	while(m_models->ContainsKey(model_name+i.ToString()))
		++i;

	model_name += i.ToString();
	List<String^>^ lst = gcnew List<String^>();
	for each(KeyValuePair<String^, animation_model^>^ pr in m_models)
		lst->Add(pr->Key);

	models_panel^ p = gcnew models_panel(model_name, lst, models_panel_mode::add);
	if(p->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		model_name = p->get_name();
		add_model(model_path, model_name);
	}
}

void viewer_document::on_new_model_loaded(xray::resources::queries_result& result)
{
	if(!result.is_successful())
	{
		LOGI_INFO("viewer_editor", "Model loading failed!");
		m_error_message = "Model loading failed!";
		return;
	}

	if(m_editor->get_animation_editor()->form->active_content==m_editor)
	{
		remove_models_from_render();
		add_models_to_render();
	}

	set_camera_follower(m_editor->camera_follows_npc);
}

void viewer_document::remove_model()
{
	List<String^>^ names_lst = gcnew List<String^>();
	for each(KeyValuePair<String^, animation_model^>^ pr in m_models)
		names_lst->Add(pr->Key);

	models_panel^ p = gcnew models_panel("", names_lst, models_panel_mode::remove);
	if(p->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		List<String^>% selected_lst = List<String^>();
		p->get_selected_list(selected_lst);
		for each(String^ name in selected_lst)
		{
			bool active_model_removed = false;
			for each(KeyValuePair<String^, animation_model^>^ p in m_models)
			{
				if(p->Value->name==name)
				{
					active_model_removed = (p->Value==m_active_model);
					if(p->Value->camera_follow_me)
						m_editor->camera_follows_npc = false;

					p->Value->remove_from_render();
					delete p->Value;
					m_models->Remove(name);
					m_models_changed = true;
					break;
				}
			}

			if(active_model_removed)
			{
				set_active_model(nullptr);
				if(m_models->Count>0)
				{
					for each(KeyValuePair<String^, animation_model^>^ pr in m_models)
					{
						set_active_model(pr->Key);
						set_camera_follower(m_editor->camera_follows_npc);
						break;
					}
				}
			}
		}
	}
}

bool viewer_document::has_loaded_models()
{
	if(m_models->Count>0)
		return true;

	return false;
}

void viewer_document::select_active_model()
{
	List<String^>^ lst = gcnew List<String^>();
	for each(KeyValuePair<String^, animation_model^>^ pr in m_models)
		lst->Add(pr->Key);

	if(lst->Count==0)
		return;

	models_panel^ p = gcnew models_panel("", lst, models_panel_mode::select);
	if(p->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		List<String^>% selected_lst = List<String^>();
		p->get_selected_list(selected_lst);
		R_ASSERT(selected_lst.Count>0);
		set_active_model(selected_lst[0]);
	}
}










void viewer_document::target_locked::set(bool new_val) 
{
	if(new_val)
	{
		if(m_target!=nullptr)
			m_target_locked = true;
		else
			MessageBox::Show(this, "There is no target to lock!", "Viewer editor", MessageBoxButtons::OK, MessageBoxIcon::Warning);
	}
	else
	{
		m_target_locked = false;
		m_hypergraph->set_selected_node((animation_viewer_graph_node_base^)nullptr);
	}

	m_editor->time_panel->update();
};

void viewer_document::update_panel_values() 
{
	max_time = m_max_time;
	time_scale = m_time_scale;
	m_editor->time_panel->set_time_layout_scale(1.0f);
	m_editor->time_panel->update();
	reset_target();
}

void viewer_document::save_node_as_clip()
{
	m_save_file_dialog->DefaultExt = ".anim_clip";
	m_save_file_dialog->Filter = "Animation viewer clip files|*.anim_clip";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath( animation_editor::animation_clips_absolute_path );
	if(m_save_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
	{
		System::String^ file_path = animation_editor::animation_clips_path + "/" + m_save_file_dialog->FileName->Remove(0, m_save_file_dialog->InitialDirectory->Length+1)->Replace("\\", "/");
		unmanaged_string path = unmanaged_string(file_path);
		xray::configs::lua_config_ptr cfg_ptr = xray::configs::create_lua_config(path.c_str());
		xray::configs::lua_config_value cfg = cfg_ptr->get_root()["clip"];

		typedef List<animation_node_interval^> intervals_list;
		intervals_list^ lst = dynamic_cast<animation_viewer_graph_node_animation^>(m_hypergraph->selected_nodes[0])->intervals;
		for(int index=0; index<lst->Count; ++index)
		{
			animation_node_interval^ interval = lst[index];
			cfg["intervals"][index]["animation"] = interval->parent->name();
			if(interval->id!=u32(-1))
				cfg["intervals"][index]["interval_id"] = interval->id;
			else
			{
				cfg["intervals"][index]["offset"] = lst[index]->offset;
				cfg["intervals"][index]["length"] = lst[index]->length;
			}
		}

		cfg_ptr->save(configs::target_sources);
	}
}

bool viewer_document::anim_node_selected()
{
	if(m_hypergraph->selected_nodes->Count>0)
	{
		if(dynamic_cast<animation_viewer_graph_node_animation^>(m_hypergraph->selected_nodes[0]))
			return true;
	}

	return false;
}

void viewer_document::set_saved_flag	()
{
	models_changed		= false;
	m_hypergraph->get_command_engine( )->clear_stack	( );
	m_hypergraph->get_command_engine( )->set_saved_flag	( );
}
