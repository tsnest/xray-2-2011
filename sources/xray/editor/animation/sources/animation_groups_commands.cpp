////////////////////////////////////////////////////////////////////////////
//	Created		: 11.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_groups_commands.h"
#include "animation_groups_document.h"
#include "animation_group_object.h"
#include "animation_node_interval.h"
#include "animation_node_interval_instance.h"
#include <xray/strings_stream.h>
#include <xray/animation/anim_track_common.h>

using xray::animation_editor::animation_groups_add_group_command;
using xray::animation_editor::animation_groups_delete_selected_command;
using xray::animation_editor::animation_groups_add_link_command;
using xray::animation_editor::animation_groups_change_model_command;
using xray::animation_editor::animation_groups_nodes_move_command;
using xray::animation_editor::animation_groups_add_intervals_command;
using xray::animation_editor::animation_groups_remove_intervals_command;
using xray::animation_editor::animation_groups_change_property_command;
using xray::animation_editor::animation_groups_change_interval_property_command;
using xray::animation_editor::animation_groups_change_link_property_command;
using xray::animation_editor::animation_groups_change_node_value_property_command;
using xray::animation_editor::animation_groups_document;
using xray::animation_editor::animation_node_interval;
using namespace Microsoft::Windows::Controls;
using namespace System::Windows::Media;
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_add_group_command /////////////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_add_group_command::animation_groups_add_group_command(animation_groups_document^ d)
:m_document(d),
m_node_id(Guid::NewGuid())
{
}

animation_groups_add_group_command::~animation_groups_add_group_command()
{
}

bool animation_groups_add_group_command::commit()
{
	animation_group_object^ new_group = gcnew animation_group_object(m_document);
	new_group->id = m_node_id;
	properties_node^ n = m_document->hypergraph()->create_node(new_group->name, new_group, new_group->position );
	new_group->view = n;
	n->header_panel_height = animation::default_fps;
	groups_editor_node_above_header_content^ cont = gcnew groups_editor_node_above_header_content();
	cont->up_down->ValueChanged += gcnew RoutedPropertyChangedEventHandler<System::Object^>(m_document, &animation_groups_document::on_node_content_value_changed);
	n->above_header_content = cont;
	n->input_link_place_enabled = true;
	n->output_link_place_enabled = true;
	n->has_expanded = true;
	n->AllowDrop = true;
	n->PreviewDragOver += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
	n->PreviewDragEnter += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
	n->DragEnter += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
	n->DragOver += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
	n->Drop += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_drop);
	m_document->groups()->Add(new_group);
	return true;
}

void animation_groups_add_group_command::rollback()
{
	animation_group_object^ gr = m_document->get_group(m_node_id);
	if(gr!=nullptr)
	{
		m_document->groups()->Remove(gr);
		m_document->hypergraph()->remove_node(m_node_id.ToString());
		delete gr;
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_add_link_command //////////////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_add_link_command::animation_groups_add_link_command(animation_groups_document^ d, link_event_args^ e)
:m_document(d),
m_src_node_id(e->link_id->output_node_id),
m_dst_node_id(e->link_id->input_node_id)
{
}

animation_groups_add_link_command::~animation_groups_add_link_command()
{
}

bool animation_groups_add_link_command::commit()
{
	link^ l = m_document->hypergraph()->create_link(m_src_node_id, m_dst_node_id, "output", "input");
	DoubleUpDown^ tb = gcnew DoubleUpDown();
	tb->Background = gcnew SolidColorBrush(Colors::LightGray);
	tb->Height = 18;
	tb->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	tb->Name = "double_up_down";
	tb->Value = 0.0f;
	tb->Minimum = 0.0f;
	tb->Maximum = 10000.0f;
	tb->ValueChanged += gcnew RoutedPropertyChangedEventHandler<System::Object^>(m_document, &animation_groups_document::on_link_value_changed);
	l->content = tb;
	return true;
}

void animation_groups_add_link_command::rollback()
{
	m_document->hypergraph()->remove_link(m_src_node_id, m_dst_node_id, "", "");
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_delete_selected_command ///////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_delete_selected_command::animation_groups_delete_selected_command(animation_groups_document^ d, selection_event_args^ e)
:m_document(d)
{
	m_nodes_ids = gcnew nodes_ids_collection();
	m_links_ids = gcnew links_ids_collection();

	m_nodes_ids->AddRange(e->selected_node_ids);
	for each(link_id^ l in e->selected_link_ids)
		m_links_ids->Add(KeyValuePair<String^, String^>(l->output_node_id, l->input_node_id));

	xray::configs::lua_config_ptr controller_cfg = xray::configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_value controller_root = controller_cfg->get_root()["animation_controller_set"]["animation_groups"];
	xray::configs::lua_config_value scene_root = controller_cfg->get_root()["scene"]["nodes"];
	xray::configs::lua_config_value groups_root = controller_cfg->get_root()["groups"];
	for(int i=0; i<m_nodes_ids->Count; ++i)
	{
		animation_group_object^ gr = m_document->get_group(Guid(m_nodes_ids[i]));
		unmanaged_string group_path(gr->path);
		controller_root[i] = group_path.c_str();
		xray::configs::lua_config_value val = scene_root[i];
		gr->save(val);
		val = groups_root[i];
		gr->save_group_settings(val);
	}

	scene_root = controller_cfg->get_root()["scene"]["links"];
	u32 index = 0;
	for each(KeyValuePair<String^, String^> p in m_links_ids)
	{
		unmanaged_string output_node_id(p.Key);
		scene_root[index]["src_id"] = output_node_id.c_str();

		unmanaged_string input_node_id(p.Value);
		scene_root[index]["dst_id"] = input_node_id.c_str();

		link^ l = m_document->hypergraph()->get_link(gcnew link_id(p.Key, "", p.Value, ""));
		R_ASSERT(l);
		scene_root[index]["value"] = (float)safe_cast<DoubleUpDown^>(l->content)->Value;
		++index;
	}

	controller_cfg->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

animation_groups_delete_selected_command::~animation_groups_delete_selected_command()
{
	FREE(m_cfg_string);
	m_nodes_ids->Clear();
	m_links_ids->Clear();
	delete m_nodes_ids;
	delete m_links_ids;
}

bool animation_groups_delete_selected_command::commit()
{
	for each(KeyValuePair<String^, String^> p in m_links_ids)
		m_document->hypergraph()->remove_link(p.Key, p.Value, "", "");

	for each(String^ node_id in m_nodes_ids)
	{
		animation_group_object^ gr = m_document->get_group(Guid(node_id));
		if(gr!=nullptr)
		{
			m_document->groups()->Remove(gr);
			m_document->hypergraph()->remove_node(node_id);
			delete gr;
		}
	}

	return true;
}

void animation_groups_delete_selected_command::rollback()
{
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_cfg_string);

	if(!config_ptr || config_ptr->empty()) 
		return;

	xray::configs::lua_config_iterator it_contr = config_ptr->get_root()["animation_controller_set"]["animation_groups"].begin();
	xray::configs::lua_config_iterator it_scene = config_ptr->get_root()["scene"]["nodes"].begin();
	xray::configs::lua_config_iterator it_groups = config_ptr->get_root()["groups"].begin();
	for(; it_contr!=config_ptr->get_root()["animation_controller_set"]["animation_groups"].end(); ++it_contr, ++it_scene, ++it_groups)
	{
		String^ path = gcnew String((pcstr)*it_contr);
		path = path->Remove(0, 40); // "resources/animations/controllers/groups/"
		animation_group_object^ new_group = gcnew animation_group_object(m_document);
		new_group->load(*it_scene);
		new_group->load_group_settings(*it_groups);
		properties_node^ n = m_document->hypergraph()->create_node( new_group->name, new_group, new_group->position );
		new_group->view = n;
		n->header_panel_height = animation::default_fps;
		groups_editor_node_above_header_content^ cont = gcnew groups_editor_node_above_header_content();
		cont->up_down->ValueChanged += gcnew RoutedPropertyChangedEventHandler<System::Object^>(m_document, &animation_groups_document::on_node_content_value_changed);
		n->above_header_content = cont;
		n->input_link_place_enabled = true;
		n->output_link_place_enabled = true;
		n->has_expanded = true;
		n->AllowDrop = true;
		n->PreviewDragOver += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
		n->PreviewDragEnter += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
		n->DragEnter += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
		n->DragOver += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_over);
		n->Drop += gcnew System::Windows::DragEventHandler(m_document, &animation_groups_document::on_group_drag_drop);
		m_document->groups()->Add(new_group);
	}

	xray::configs::lua_config_iterator it_links = config_ptr->get_root()["scene"]["links"].begin();
	for(; it_links!=config_ptr->get_root()["scene"]["links"].end(); ++it_links)
	{
		String^ src_id = gcnew String((pcstr)(*it_links)["src_id"]);
		String^ dst_id = gcnew String((pcstr)(*it_links)["dst_id"]);
		link^ l = m_document->hypergraph()->create_link(src_id, dst_id, "", "");
		DoubleUpDown^ tb = gcnew DoubleUpDown();
		tb->Background = gcnew SolidColorBrush(Colors::LightGray);
		tb->Height = 18;
		tb->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
		tb->Name = "double_up_down";
		tb->Value = ((*it_links).value_exists("value"))?(float)(*it_links)["value"]:0.0f;
		tb->Minimum = 0.0f;
		tb->Maximum = 10000.0f;
		tb->ValueChanged += gcnew RoutedPropertyChangedEventHandler<System::Object^>(m_document, &animation_groups_document::on_link_value_changed);
		l->content = tb;
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_change_model_command //////////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_change_model_command::animation_groups_change_model_command(animation_groups_document^ d, String^ new_val, String^ old_val)
:m_document(d),
m_new_val(new_val), 
m_old_val(old_val)
{
}

animation_groups_change_model_command::~animation_groups_change_model_command()
{
	delete m_old_val;
	delete m_new_val;
}

bool animation_groups_change_model_command::commit()
{
	m_document->model = m_new_val;
	return true;
}

void animation_groups_change_model_command::rollback()
{
	m_document->model = m_old_val;
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_node_move_command /////////////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_nodes_move_command::animation_groups_nodes_move_command(animation_groups_document^ d, node_move_event_args^ e)
:m_document(d),
m_args(e->args)
{
}

animation_groups_nodes_move_command::~animation_groups_nodes_move_command()
{
}

bool animation_groups_nodes_move_command::commit()
{
	for each(node_move_args^ arg in m_args)
	{
		if(arg->new_position==arg->old_position)
			return false;

		animation_group_object^ gr = m_document->get_group(Guid(arg->node_id));
		if(gr==nullptr)
			return false;

		gr->position = arg->new_position;
	}

	return true;
}

void animation_groups_nodes_move_command::rollback()
{
	for each(node_move_args^ arg in m_args)
	{
		animation_group_object^ gr = m_document->get_group(Guid(arg->node_id));
		if(gr==nullptr)
			return;

		gr->position = arg->old_position;
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_add_intervals_command /////////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_add_intervals_command::animation_groups_add_intervals_command(animation_groups_document^ d, node^ n, List<animation_node_interval^>^ lst)
:m_document(d),
m_node_id(n->id)
{	
	m_intervals = gcnew intervals_list();
	for each(animation_node_interval^ interval in lst)
		m_intervals->Add(gcnew animation_node_interval_instance(interval, 1.0f, 0.0f));
}

animation_groups_add_intervals_command::~animation_groups_add_intervals_command()
{
	delete m_intervals;
}

bool animation_groups_add_intervals_command::commit()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	R_ASSERT(gr!=nullptr);
	m_document->disable_events_handling = true;
	for each(animation_node_interval_instance^ interval in m_intervals)
	{
		interval->property_changed += gcnew Action<animation_node_interval_instance^, String^, Object^, Object^>(gr, &animation_group_object::interval_property_changed);
		gr->intervals->Add(interval);
	}

	m_document->disable_events_handling = false;
	gr->refresh_view();
	return true;
}

void animation_groups_add_intervals_command::rollback()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	m_document->disable_events_handling = true;
	for each(animation_node_interval_instance^ interval in m_intervals)
	{
		interval->property_changed -= gcnew Action<animation_node_interval_instance^, String^, Object^, Object^>(gr, &animation_group_object::interval_property_changed);
		gr->intervals->Remove(interval);
	}

	m_document->disable_events_handling = false;
	gr->refresh_view();
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_remove_intervals_command //////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_remove_intervals_command::animation_groups_remove_intervals_command(animation_groups_document^ d, node^ n, System::Collections::IList^ lst)
:m_document(d),
m_node_id(n->id),
m_first_run(true)
{	
	m_intervals = gcnew intervals_list();
	for each(animation_node_interval_instance^ interval in lst)
		m_intervals->Add(interval);
}

animation_groups_remove_intervals_command::~animation_groups_remove_intervals_command()
{
	delete m_intervals;
}

bool animation_groups_remove_intervals_command::commit()
{
	if(m_first_run)
		return true;

	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	m_document->disable_events_handling = true;
	for each(animation_node_interval_instance^ interval in m_intervals)
	{
		interval->property_changed -= gcnew Action<animation_node_interval_instance^, String^, Object^, Object^>(gr, &animation_group_object::interval_property_changed);
		gr->intervals->Remove(interval);
	}

	gr->refresh_view();
	m_document->disable_events_handling = false;
	return true;
}

void animation_groups_remove_intervals_command::rollback()
{
	m_first_run = false;
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	R_ASSERT(gr!=nullptr);
	for each(animation_node_interval_instance^ interval in m_intervals)
	{
		interval->property_changed += gcnew Action<animation_node_interval_instance^, String^, Object^, Object^>(gr, &animation_group_object::interval_property_changed);
		gr->intervals->Add(interval);
	}

	gr->refresh_view();
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_change_property_command ///////////////
////////////////////////////////////////////////////////////////////////////
animation_groups_change_property_command::animation_groups_change_property_command(animation_groups_document^ d, String^ node_id, String^ property_name, Object^ new_val, Object^ old_val)
:m_document(d),
m_node_id(node_id),
m_property_name(property_name),
m_new_val(new_val),
m_old_val(old_val)
{	
}

animation_groups_change_property_command::~animation_groups_change_property_command()
{
}

bool animation_groups_change_property_command::commit()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	m_document->disable_events_handling = true;
	gr->GetType()->GetProperty(m_property_name)->SetValue(gr, m_new_val, nullptr);
	m_document->disable_events_handling = false;
	gr->refresh_view();
	return true;
}

void animation_groups_change_property_command::rollback()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	m_document->disable_events_handling = true;
	gr->GetType()->GetProperty(m_property_name)->SetValue(gr, m_old_val, nullptr);
	m_document->disable_events_handling = false;
	gr->refresh_view();
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_change_interval_property_command //////
////////////////////////////////////////////////////////////////////////////
animation_groups_change_interval_property_command::animation_groups_change_interval_property_command(animation_groups_document^ d, String^ node_id, u32 index, String^ property_name, Object^ new_val, Object^ old_val)
:m_document(d),
m_node_id(node_id),
m_interval_index(index),
m_property_name(property_name),
m_new_val(new_val),
m_old_val(old_val)
{	
}

animation_groups_change_interval_property_command::~animation_groups_change_interval_property_command()
{
}

bool animation_groups_change_interval_property_command::commit()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	m_document->disable_events_handling = true;
	animation_node_interval_instance^ interval = gr->intervals[m_interval_index];
	interval->GetType()->GetProperty(m_property_name)->SetValue(interval, m_new_val, nullptr);
	m_document->disable_events_handling = false;
	gr->refresh_view();
	return true;
}

void animation_groups_change_interval_property_command::rollback()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	m_document->disable_events_handling = true;
	animation_node_interval_instance^ interval = gr->intervals[m_interval_index];
	interval->GetType()->GetProperty(m_property_name)->SetValue(interval, m_old_val, nullptr);
	m_document->disable_events_handling = false;
	gr->refresh_view();
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_change_link_property_command //////////
////////////////////////////////////////////////////////////////////////////
animation_groups_change_link_property_command::animation_groups_change_link_property_command(animation_groups_document^ d, link^ lnk, Double new_val, Double old_val)
:m_document(d),
m_new_val(new_val),
m_old_val(old_val)
{	
	m_src_node_id = lnk->id->output_node_id;
	m_dst_node_id = lnk->id->input_node_id;
}

animation_groups_change_link_property_command::~animation_groups_change_link_property_command()
{
}

bool animation_groups_change_link_property_command::commit()
{
	animation_group_object^ src_gr = m_document->get_group(Guid(m_src_node_id));
	animation_group_object^ dst_gr = m_document->get_group(Guid(m_dst_node_id));
	link^ l = m_document->hypergraph()->get_link(gcnew link_id(m_src_node_id, "output", m_dst_node_id, "input"));
	R_ASSERT(src_gr && dst_gr && l);
	m_document->disable_events_handling = true;
	safe_cast<DoubleUpDown^>(l->content)->Value = m_new_val;
	m_document->disable_events_handling = false;
	return true;
}

void animation_groups_change_link_property_command::rollback()
{
	animation_group_object^ src_gr = m_document->get_group(Guid(m_src_node_id));
	animation_group_object^ dst_gr = m_document->get_group(Guid(m_dst_node_id));
	link^ l = m_document->hypergraph()->get_link(gcnew link_id(m_src_node_id, "output", m_dst_node_id, "input"));
	R_ASSERT(src_gr && dst_gr && l);
	m_document->disable_events_handling = true;
	safe_cast<DoubleUpDown^>(l->content)->Value = m_old_val;
	m_document->disable_events_handling = false;
}
////////////////////////////////////////////////////////////////////////////
// public ref class animation_groups_change_node_value_property_command ////
////////////////////////////////////////////////////////////////////////////
animation_groups_change_node_value_property_command::animation_groups_change_node_value_property_command(animation_groups_document^ d, node^ n, Double new_val, Double old_val)
:m_document(d),
m_node_id(n->id),
m_new_val(new_val),
m_old_val(old_val)
{	
}

animation_groups_change_node_value_property_command::~animation_groups_change_node_value_property_command()
{
}

bool animation_groups_change_node_value_property_command::commit()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	R_ASSERT(gr);
	m_document->disable_events_handling = true;
	groups_editor_node_above_header_content^ cont = safe_cast<groups_editor_node_above_header_content^>(gr->view->above_header_content);
	cont->up_down->Value = m_new_val;
	m_document->disable_events_handling = false;
	return true;
}

void animation_groups_change_node_value_property_command::rollback()
{
	animation_group_object^ gr = m_document->get_group(Guid(m_node_id));
	R_ASSERT(gr);
	m_document->disable_events_handling = true;
	groups_editor_node_above_header_content^ cont = safe_cast<groups_editor_node_above_header_content^>(gr->view->above_header_content);
	cont->up_down->Value = m_old_val;
	m_document->disable_events_handling = false;
}