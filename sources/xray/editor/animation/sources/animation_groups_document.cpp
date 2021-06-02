////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_groups_document.h"
#include "animation_groups_editor.h"
#include "animation_group_object.h"
#include "animation_groups_commands.h"
#include "animation_groups_intervals_panel.h"
#include "animation_editor.h"
#include "animation_node_clip_instance.h"
#include "animation_node_clip.h"
#include "animation_node_interval.h"
#include "animation_groups_animations_tree.h"
#include "animation_groups_controllers_tree.h"
#include <xray/animation/anim_track_common.h>

using xray::animation_editor::animation_groups_document;
using xray::animation_editor::animation_groups_editor;
using xray::animation_editor::animation_group_object;
using xray::editor_base::command_engine;
using xray::animation_editor::animation_node_interval;
using xray::animation_editor::animation_node_interval_instance;
using namespace Microsoft::Windows::Controls;
using namespace System::Windows::Media;

animation_groups_document::animation_groups_document(animation_groups_editor^ ed)
:super(ed->multidocument_base),
m_editor(ed),
m_first_save(true)
{
	m_model = "character/human/neutral/neutral_01/neutral_01";
	disable_events_handling = false;
	m_command_engine = gcnew command_engine(100);
	m_groups = gcnew groups_type();
	m_intervals_panel = gcnew animation_groups_intervals_panel(this);
	m_last_query_ids = gcnew query_ids_list();
	SuspendLayout();

	// m_save_file_dialog
	m_save_file_dialog = gcnew System::Windows::Forms::SaveFileDialog();
	m_save_file_dialog->AddExtension = true;
	m_save_file_dialog->DefaultExt = ".lua";
	m_save_file_dialog->Filter = "Animation controller files|*.lua";
	m_save_file_dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/sources/animations/controllers");
	m_save_file_dialog->RestoreDirectory = true;
	m_save_file_dialog->Title = "New controller";

	// m_model_label
	m_model_label = gcnew System::Windows::Forms::Label();
	m_model_label->AutoSize = true;
	m_model_label->Location = System::Drawing::Point(0, 0);
	m_model_label->Name = L"m_model_label";
	m_model_label->Size = System::Drawing::Size(35, 13);
	m_model_label->TabIndex = 0;
	m_model_label->Text = "Model: "+m_model;

	// m_model_panel
	m_model_panel = gcnew System::Windows::Forms::Panel();
	m_model_panel->Name = L"m_model_panel";
	m_model_panel->Size = System::Drawing::Size(35, 13);
	m_model_panel->TabIndex = 0;
	m_model_panel->Dock = System::Windows::Forms::DockStyle::Top;
	m_model_panel->Controls->Add(m_model_label);
	
	// m_hypergraph
	m_hypergraph = gcnew xray::editor::wpf_controls::hypergraph::hypergraph_host();
	m_hypergraph->Dock = System::Windows::Forms::DockStyle::Fill;
	m_hypergraph->Location = System::Drawing::Point(0, 0);
	m_hypergraph->Name = L"m_hypergraph";
	m_hypergraph->Size = System::Drawing::Size(562, 439);
	m_hypergraph->TabIndex = 0;
	m_hypergraph->Text = L"m_hypergraph";
	m_hypergraph->hypergraph->is_fix_min_field_size = true;
	m_hypergraph->hypergraph->remove_menu_item_enabled = true;
	m_hypergraph->hypergraph->add_context_menu_item("Add new group");
	m_hypergraph->hypergraph->menu_item_click += gcnew System::EventHandler<menu_event_args^>(this, &animation_groups_document::on_context_item_click);
	m_hypergraph->hypergraph->link_creating += gcnew System::EventHandler<link_event_args^>(this, &animation_groups_document::on_link_creating);
	m_hypergraph->hypergraph->deleting_selected += gcnew System::EventHandler<selection_event_args^>(this, &animation_groups_document::on_deleting_selection);
	m_hypergraph->hypergraph->can_link += gcnew System::EventHandler<can_link_event_args^>(this, &animation_groups_document::can_make_connection);
	m_hypergraph->hypergraph->node_moved += gcnew System::EventHandler<node_move_event_args^>(this, &animation_groups_document::on_node_moved);
	m_hypergraph->hypergraph->AllowDrop = true;
	m_hypergraph->hypergraph->DragOver += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_hypergraph_drag_over);
	m_hypergraph->hypergraph->Drop += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_hypergraph_drag_drop);

	// animation_groups_document
	AllowEndUserDocking = false;
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	BackColor = System::Drawing::SystemColors::ActiveCaption;
	ClientSize = System::Drawing::Size(562, 439);
	VisibleChanged += gcnew System::EventHandler(this, &animation_groups_document::on_document_activated);
	Controls->Add(m_hypergraph);
	Controls->Add(m_model_panel);
	ResumeLayout(false);
}

animation_groups_document::~animation_groups_document()
{
	m_last_query_ids->Clear();
	delete m_last_query_ids;
	delete m_intervals_panel;
	delete m_command_engine;
	delete m_groups;
	if (components)
		delete components;
}

void animation_groups_document::on_document_activated(System::Object^, System::EventArgs^)
{
	if(Visible)
		get_editor()->controllers_tree->track_active_item(this->Name);
}

void animation_groups_document::save_as()
{
	bool tmp = m_first_save;
	is_saved = false;
	m_first_save = true;
	save();
	m_first_save = tmp;
}

void animation_groups_document::save()
{
	if(is_saved)
		return;

	if(m_first_save)
	{
		if(m_save_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
		{
			System::String^ name_with_ext = m_save_file_dialog->FileName->Remove(0, m_save_file_dialog->InitialDirectory->Length+1);
			Name = name_with_ext->Remove(name_with_ext->Length-4, 4);
			Text = Name;
		}
		else
			return;
	}

	xray::fs_new::virtual_path_string controller_path;
	controller_path += unmanaged_string( animation_editor::animation_controllers_path + "/" ).c_str( );
	controller_path += unmanaged_string(Name).c_str();
	xray::fs_new::virtual_path_string scene_path = controller_path;
	controller_path += ".lua";
	scene_path += ".options";
	xray::configs::lua_config_ptr controller_cfg = xray::configs::create_lua_config(controller_path.c_str());
	xray::configs::lua_config_ptr scene_cfg = xray::configs::create_lua_config(scene_path.c_str());
	xray::configs::lua_config_value controller_root = controller_cfg->get_root()["animation_controller_set"];
	unmanaged_string model_name(model);
	controller_root["model"] = model_name.c_str();
	controller_root["fixed_bones"][0]["name"] = "LeftToe";
	controller_root["fixed_bones"][0]["vertex"] = float3(0.0f, 0.0f, 0.0f);
	controller_root["fixed_bones"][1]["name"] = "RightToe";
	controller_root["fixed_bones"][1]["vertex"] = float3(0.0f, 0.0f, 0.0f);
	controller_root = controller_cfg->get_root()["animation_controller_set"]["animation_groups"];
	xray::configs::lua_config_value scene_root = scene_cfg->get_root()["scene"]["nodes"];
	for(int i=0; i<m_groups->Count; ++i)
	{
		animation_group_object^ gr = m_groups[i];
		unmanaged_string group_path( animation_group_object::source_path + gr->path );
		controller_root[i] = group_path.c_str();
		xray::configs::lua_config_value val = scene_root[i];
		gr->save(val);

		xray::configs::lua_config_ptr group_cfg = xray::configs::create_lua_config(group_path.c_str());
		val = group_cfg->get_root()["animation_group"];
		gr->save_group_settings(val);
		group_cfg->save(configs::target_sources);
	}

	scene_root = scene_cfg->get_root()["scene"]["links"];
	u32 index = 0;
	for each(KeyValuePair<String^, link^> p in hypergraph()->links)
	{
		unmanaged_string output_node_id(p.Value->id->output_node_id);
		scene_root[index]["src_id"] = output_node_id.c_str();

		unmanaged_string input_node_id(p.Value->id->input_node_id);
		scene_root[index]["dst_id"] = input_node_id.c_str();

		scene_root[index]["value"] = (float)safe_cast<DoubleUpDown^>(p.Value->content)->Value;
		++index;
	}

	controller_cfg->save(configs::target_sources);
	scene_cfg->save(configs::target_sources);
	m_command_engine->set_saved_flag();
	m_first_save = false;
	is_saved = true;
	track_last_animation();
}

void animation_groups_document::load()
{
	m_first_save = false;
	xray::fs_new::virtual_path_string controller_path;
	controller_path += unmanaged_string( animation_editor::animation_controllers_path + "/" ).c_str( );
	controller_path += unmanaged_string(Name).c_str();
	xray::fs_new::virtual_path_string scene_path = controller_path;
	controller_path += ".lua";
	scene_path += ".options";
	xray::resources::request arr[2] = {
		{controller_path.c_str(), xray::resources::lua_config_class},
		{scene_path.c_str(), xray::resources::lua_config_class}
	};

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_groups_document::on_controller_loaded), g_allocator);
	xray::resources::query_resources(arr, 
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator,
		0, 0,
		assert_on_fail_false
	);

	track_last_animation();
}

void animation_groups_document::on_controller_loaded(xray::resources::queries_result& result)
{
	if(result[0].is_successful())
	{
		is_saved = true;
		m_command_engine->set_saved_flag();
		xray::configs::lua_config_ptr controller_cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[0].get_unmanaged_resource());
		xray::configs::lua_config_value root = controller_cfg->get_root()["animation_controller_set"];
		R_ASSERT(root.value_exists("model"));
		model = gcnew String((pcstr)root["model"]);
		root = root["animation_groups"];
		xray::configs::lua_config_iterator it_contr = root.begin();
		xray::configs::lua_config_iterator it_scene = root.begin();
		xray::configs::lua_config_ptr scene_cfg;
		if(result[1].is_successful())
		{
			scene_cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[1].get_unmanaged_resource());
			it_scene = scene_cfg->get_root()["scene"]["nodes"].begin();
		}

		xray::resources::request arr[255];
		u32 index = 0;
		for(; it_contr!=root.end(); ++it_contr)
		{
			pcstr group_path = (pcstr)*it_contr;
			arr[index].path = group_path;
			arr[index].id = xray::resources::lua_config_class;
			++index;

			System::String^ path = gcnew System::String(group_path);
			path = path->Remove(0, 40); // "resources/animations/controllers/groups/"
			animation_group_object^ new_group = gcnew animation_group_object(this);
			
			if(result[1].is_successful() && it_scene!=scene_cfg->get_root()["scene"]["nodes"].end())
			{
				new_group->load(*it_scene);
				++it_scene;
			}
			else
			{
				System::String^ name = path;
				disable_events_handling = true;
				new_group->name = name->Remove(name->Length-4); // ".lua"
				disable_events_handling = false;
			}

			properties_node^ n = m_hypergraph->hypergraph->create_node(new_group->name, new_group, new_group->position);
			new_group->view = n;
			n->header_panel_height = animation::default_fps;
			groups_editor_node_above_header_content^ cont = gcnew groups_editor_node_above_header_content();
			cont->up_down->ValueChanged += gcnew RoutedPropertyChangedEventHandler<System::Object^>(this, &animation_groups_document::on_node_content_value_changed);
			n->above_header_content = cont;
			n->input_link_place_enabled = true;
			n->output_link_place_enabled = true;
			n->has_expanded = true;
			n->AllowDrop = true;
			n->PreviewDragOver += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_group_drag_over);
			n->PreviewDragEnter += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_group_drag_over);
			n->DragEnter += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_group_drag_over);
			n->DragOver += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_group_drag_over);
			n->Drop += gcnew System::Windows::DragEventHandler(this, &animation_groups_document::on_group_drag_drop);
			m_groups->Add(new_group);
		}

		if(result[1].is_successful())
		{
			xray::configs::lua_config_ptr scene_cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[1].get_unmanaged_resource());
			xray::configs::lua_config_iterator it_links = scene_cfg->get_root()["scene"]["links"].begin();
			for(; it_links!=scene_cfg->get_root()["scene"]["links"].end(); ++it_links)
			{
				System::String^ src_id = gcnew System::String((pcstr)(*it_links)["src_id"]);
				System::String^ dst_id = gcnew System::String((pcstr)(*it_links)["dst_id"]);
				link^ l = hypergraph()->create_link(src_id, dst_id, "", "");
				DoubleUpDown^ tb = gcnew DoubleUpDown();
				tb->Background = gcnew SolidColorBrush(Colors::LightGray);
				tb->Height = 18;
				tb->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
				tb->Name = "double_up_down";
				tb->Value = ((*it_links).value_exists("value"))?(float)(*it_links)["value"]:0.0f;
				tb->Minimum = 0.0f;
				tb->Maximum = 10000.0f;
				tb->ValueChanged += gcnew RoutedPropertyChangedEventHandler<System::Object^>(this, &animation_groups_document::on_link_value_changed);
				l->content = tb;
			}
		}

		query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &animation_groups_document::on_groups_loaded), g_allocator);
		xray::resources::query_resources(arr, index,
			boost::bind(&query_result_delegate::callback, rq, _1),
			g_allocator
		);
	}

	track_last_animation();
}

void animation_groups_document::on_groups_loaded(xray::resources::queries_result& result)
{
	if(result.is_successful())
	{
		R_ASSERT_CMP(result.size(), ==, (u32)m_groups->Count);
		for(u32 i=0; i<result.size(); ++i)
		{
			xray::configs::lua_config_ptr cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[i].get_unmanaged_resource());
			m_groups[i]->load_group_settings(cfg->get_root()["animation_group"]);
		}
	}
}

void animation_groups_document::undo()
{
	m_command_engine->rollback(-1);
	is_saved = m_command_engine->saved_flag();
}

void animation_groups_document::redo()
{
	m_command_engine->rollback(1);
	is_saved = m_command_engine->saved_flag();
}

void animation_groups_document::select_all()
{
	hypergraph()->select_all_nodes();
	hypergraph()->select_all_links();
}

void animation_groups_document::copy(bool del)
{
	XRAY_UNREFERENCED_PARAMETER(del);
}

void animation_groups_document::paste()
{
}

void animation_groups_document::del()
{
	hypergraph()->cmd_delete_selected();
}

void animation_groups_document::on_context_item_click(System::Object^, menu_event_args^ e)
{
	if(e->item_name=="Add new group")
		is_saved &= !m_command_engine->run(gcnew animation_groups_add_group_command(this));
}

void animation_groups_document::on_link_creating(System::Object^, link_event_args^ e)
{
	is_saved &= !m_command_engine->run(gcnew animation_groups_add_link_command(this, e));
}

void animation_groups_document::on_deleting_selection(System::Object^, selection_event_args^ e)
{
	is_saved &= !m_command_engine->run(gcnew animation_groups_delete_selected_command(this, e));
}

animation_group_object^ animation_groups_document::get_group(System::Guid id)
{
	for each(animation_group_object^ gr in m_groups)
	{
		if(gr->id.Equals(id))
			return gr;
	}

	return nullptr;
}

void animation_groups_document::model::set(System::String^ new_val) 
{
	m_model = new_val; 
	m_model_label->Text = "Model: "+new_val;
}

void animation_groups_document::change_model(System::String^ new_val)
{
	is_saved &= !m_command_engine->run(gcnew animation_groups_change_model_command(this, new_val, m_model));
}

void animation_groups_document::can_make_connection(System::Object^, can_link_event_args^ e)
{
	link_point^ src = e->source;
	link_point^ dst = e->destination;
	if(src->node->id==dst->node->id)
	{
		e->can_link = false;
		return;
	}

	e->can_link = true;
}

void animation_groups_document::on_node_moved(System::Object^, node_move_event_args^ e)
{
	for each(node_move_args^ arg in e->args)
	{
		if(arg->new_position==arg->old_position)
			return;
	}

	is_saved &= !m_command_engine->run(gcnew animation_groups_nodes_move_command(this, e));
}

void animation_groups_document::on_hypergraph_drag_over(System::Object^, System::Windows::DragEventArgs^ e)
{
	e->Handled = true;
	if(e->Data->GetDataPresent(animation_groups_drag_drop_operation::typeid))
	{
		e->Effects = System::Windows::DragDropEffects::Copy;
		return;
	}

	e->Effects = System::Windows::DragDropEffects::None;
}

void animation_groups_document::on_hypergraph_drag_drop(System::Object^, System::Windows::DragEventArgs^ e)
{
	e->Handled = true;
	if(e->Data->GetDataPresent(animation_groups_drag_drop_operation::typeid))
	{
		animation_groups_drag_drop_operation^ o = safe_cast<animation_groups_drag_drop_operation^>(e->Data->GetData(animation_groups_drag_drop_operation::typeid));
		if(e->Effects==System::Windows::DragDropEffects::Copy)
		{
			switch(*o)
			{
			case animation_groups_drag_drop_operation::animation_group_node:
				{
					is_saved &= !m_command_engine->run(gcnew animation_groups_add_group_command(this));
					break;
				}
			default:
				NODEFAULT();
			}
		}
	}
	else
		e->Effects = System::Windows::DragDropEffects::None;
}

void animation_groups_document::on_group_drag_over(System::Object^, System::Windows::DragEventArgs^ e)
{
	e->Handled = true;
	if(e->Data->GetDataPresent("animation_tree_item"))
	{
		e->Effects = System::Windows::DragDropEffects::Move;
		return;
	}

	e->Effects = System::Windows::DragDropEffects::None;
}

void animation_groups_document::on_group_drag_drop(System::Object^ sender, System::Windows::DragEventArgs^ e)
{
	e->Handled = true;
	if(e->Data->GetDataPresent("animation_tree_item"))
	{
		node^ n = safe_cast<node^>(sender);
		List<String^>^ lst = safe_cast<List<String^>^>(e->Data->GetData("animation_tree_item"));
		animation_groups_drag_drop_operation^ o = safe_cast<animation_groups_drag_drop_operation^>(e->Data->GetData("item_type"));
		switch(*o)
		{
		case animation_groups_drag_drop_operation::animations_tree_files:
			{
				m_intervals_panel->animations_count(lst->Count);
				m_intervals_panel->show(n);
				for each(String^ list_path in lst)
				{
					String^ path = list_path->Remove(0, 11); // "animations/"
					animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_groups_document::on_animation_loaded);
					animation_node_clip^ clip = get_editor()->get_animation_editor()->clip_by_name(path);
					if(clip!=nullptr)
					{
						m_last_query_ids->Add(clip->id_queried);
						get_editor()->get_animation_editor()->request_animation_clip(path, cb);
					}
					else
						m_last_query_ids->Add(get_editor()->get_animation_editor()->request_animation_clip(path, cb));

				}

				break;
			}
		case animation_groups_drag_drop_operation::animations_tree_folder:
			{
				String^ path = lst[0];
				u32 nodes_counter = 0;
				calculate_animations_count(path, nodes_counter);
				m_intervals_panel->animations_count(nodes_counter);
				m_intervals_panel->show(n);
				iterate_animation_folder(path);
				break;
			}
		default:
			NODEFAULT();
		}
	}
	else
		e->Effects = System::Windows::DragDropEffects::None;
}

void animation_groups_document::calculate_animations_count(System::String^ folder_name, u32& nodes_counter)
{
	xray::editor::controls::tree_node^ n = get_editor()->animations_tree->get_node(folder_name);
	if(n->m_node_type==xray::editor::controls::tree_node_type::group_item)
	{
		for each(xray::editor::controls::tree_node^ child in n->nodes)
			calculate_animations_count(child->FullPath, nodes_counter);
	}
	else
		++nodes_counter;
}

void animation_groups_document::iterate_animation_folder(System::String^ folder_name)
{
	xray::editor::controls::tree_node^ n = get_editor()->animations_tree->get_node(folder_name);
	if(n->m_node_type==xray::editor::controls::tree_node_type::group_item)
	{
		for each(xray::editor::controls::tree_node^ child in n->nodes)
			iterate_animation_folder(child->FullPath);
	}
	else
	{
		System::String^ path = n->FullPath->Remove(0, 11); // "animations/"
		animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_groups_document::on_animation_loaded);
		animation_node_clip^ clip = get_editor()->get_animation_editor()->clip_by_name(path);
		if(clip!=nullptr)
		{
			m_last_query_ids->Add(clip->id_queried);
			get_editor()->get_animation_editor()->request_animation_clip(path, cb);
		}
		else
			m_last_query_ids->Add(get_editor()->get_animation_editor()->request_animation_clip(path, cb));
	}
}

void animation_groups_document::on_animation_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	if(!m_last_query_ids->Contains(new_clip->id_queried))
		return;

	if(new_clip==nullptr)
		return;

	m_intervals_panel->animation_loaded(new_clip);
	track_last_animation();
}

void animation_groups_document::cancel_intervals_adding()
{
	m_last_query_ids->Clear();
	track_last_animation();
}

void animation_groups_document::add_group_intervals(node^ n, List<animation_node_interval^>^ lst)
{
	m_last_query_ids->Clear();
	is_saved &= !m_command_engine->run(gcnew animation_groups_add_intervals_command(this, n, lst));
	track_last_animation();
}

void animation_groups_document::track_last_animation()
{
	if(!String::IsNullOrEmpty(get_editor()->animations_tree->last_selected_node_name))
	{
		System::String^ path = get_editor()->animations_tree->last_selected_node_name->Remove(0, 11);
		get_editor()->animations_tree->track_active_item(path);
	}
}

void animation_groups_document::remove_group_intervals(node^ n, System::Collections::IList^ lst)
{
	is_saved &= !m_command_engine->run(gcnew animation_groups_remove_intervals_command(this, n, lst));
}

void animation_groups_document::change_group_property(System::String^ node_id, System::String^ property_name, System::Object^ new_val, System::Object^ old_val)
{
	if(new_val->Equals(old_val))
		return;

	is_saved &= !m_command_engine->run(gcnew animation_groups_change_property_command(this, node_id, property_name, new_val, old_val));
}

void animation_groups_document::change_interval_property(System::String^ node_id, u32 index, System::String^ property_name, System::Object^ new_val, System::Object^ old_val)
{
	if(new_val->Equals(old_val))
		return;

	is_saved &= !m_command_engine->run(gcnew animation_groups_change_interval_property_command(this, node_id, index, property_name, new_val, old_val));
}

void animation_groups_document::on_link_value_changed(System::Object^ sender, System::Windows::RoutedPropertyChangedEventArgs<System::Object^>^ e)
{
	if(!disable_events_handling)
	{
		link^ lnk = link::get_parent_link((System::Windows::DependencyObject^)sender);
		is_saved &= !m_command_engine->run(gcnew animation_groups_change_link_property_command(this, lnk, (Double)e->NewValue, (Double)e->OldValue));
	}
}

void animation_groups_document::on_node_content_value_changed(System::Object^ sender, System::Windows::RoutedPropertyChangedEventArgs<System::Object^>^ e)
{
	if(!disable_events_handling)
	{
		node^ n = node::get_parent_node((System::Windows::DependencyObject^)sender);
		is_saved &= !m_command_engine->run(gcnew animation_groups_change_node_value_property_command(this, n, (Double)e->NewValue, (Double)e->OldValue));
	}
}

bool animation_groups_document::can_learn_controller()
{
	for each(animation_group_object^ gr in m_groups)
	{
		if(gr->intervals->Count<=1)
		{
			System::Windows::Forms::MessageBox::Show(this, "Can not learn controller because group ["+gr->name+"] has no intervals selected!", 
				"Animation groups", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return false;
		}
	}

	return true;
}
