////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_scene_document.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "transform_control_helper.h"
#include "sound_scene_contexts_manager.h"
#include "sound_object_wrapper.h"
#include "sound_object_instance.h"
#include "sound_scene_commands.h"
#include "toolbar_panel.h"

#include <xray/collision/api.h>
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/sound/world_user.h>

#pragma managed(push, off)
#include <xray/resources.h>
#include <xray/resources_query_result.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed(pop)

using xray::sound_editor::sound_scene_document;
using xray::sound_editor::sound_editor;
using xray::sound_editor::sound_object_instance;

sound_scene_document::sound_scene_document(sound_editor^ ed)
:super(ed->multidocument_base),
m_editor(ed),
m_render_world(ed->render_world),
m_first_save(true)
{
	m_sound_scene = NEW(sound::sound_scene_ptr)();
	m_command_engine = gcnew command_engine(100);
	m_objects = gcnew objects_list();
	m_selected_objects = gcnew objects_list();
	m_input_engine = gcnew input_engine(input_keys_holder::ref, gcnew sound_scene_contexts_manager(this));
  	m_gui_binder = gcnew gui_binder(m_input_engine);
	active_control = nullptr;

	m_view_window = gcnew scene_view_panel();
	m_scene = NEW(render::scene_ptr);
	m_scene_view = NEW(render::scene_view_ptr);
	m_output_window = NEW(render::render_output_window_ptr);
	initialize_components();
	query_create_render_resources();
	query_sound_scene();
}

sound_scene_document::~sound_scene_document()
{
	unregister_actions();
	for each(sound_object_instance^ inst in m_objects)
		delete inst;

	if(m_transform_control_helper->m_object!=nullptr)
		delete m_transform_control_helper->m_object;

	delete m_objects;
	delete m_selected_objects;
	m_view_window->clear_resources();
	m_view_window->clear_scene();
	collision::delete_space_partitioning_tree(m_transform_control_helper->m_collision_tree);
	m_renderer = NULL;
	m_editor->get_sound_world()->get_editor_world_user().remove_sound_scene(*m_sound_scene);
	(*m_sound_scene) = NULL;
	DELETE(m_sound_scene);
	DELETE(m_output_window);
	DELETE(m_scene_view);
	DELETE(m_scene);
	delete m_view_window;
	delete m_transform_control_translation;
	delete m_transform_control_rotation;
	delete m_transform_control_helper;
	delete m_save_file_dialog;
	delete m_input_engine;
	delete m_gui_binder;
	delete m_command_engine;
	if(components)
		delete components;
}

void sound_scene_document::query_create_render_resources()
{
 	render::editor_renderer_configuration render_configuration;
 	render_configuration.m_create_terrain = true;
 	System::Int32 hwnd = m_view_window->view_handle();
 	resources::user_data_variant* temp_data[] = {NEW(resources::user_data_variant), 0, NEW(resources::user_data_variant)};
 	temp_data[0]->set(render_configuration);
 	temp_data[2]->set(*(HWND*)&hwnd);
 	resources::user_data_variant const* data[] = {temp_data[0], temp_data[1], temp_data[2]};
 	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_scene_document::on_render_resources_ready), g_allocator);
 	const_buffer temp_buffer("", 1);
 	resources::creation_request requests[] =
 	{
 		resources::creation_request("sound_editor_scene", temp_buffer, resources::scene_class),
 		resources::creation_request("sound_editor_scene_view", temp_buffer, resources::scene_view_class),
 		resources::creation_request("sound_editor_render_output_window", temp_buffer, resources::render_output_window_class)
 	};
 	resources::query_create_resources(
 		requests,
 		boost::bind(&query_result_delegate::callback, q, _1),
 		g_allocator,
 		data
 	);
 	DELETE(temp_data[0]);
 	DELETE(temp_data[2]);
}

void sound_scene_document::on_render_resources_ready(xray::resources::queries_result& data)
{
 	R_ASSERT(data.is_successful());
	*m_scene = static_cast_resource_ptr<render::scene_ptr>(data[0].get_unmanaged_resource());
 	*m_scene_view = static_cast_resource_ptr<render::scene_view_ptr>(data[1].get_unmanaged_resource());
 	*m_output_window = static_cast_resource_ptr<render::render_output_window_ptr>(data[2].get_unmanaged_resource());
 	m_renderer = &m_render_world.editor_renderer();
 	m_view_window->setup_scene(m_scene->c_ptr(), m_scene_view->c_ptr(), *m_renderer, true);

	m_transform_control_helper = gcnew se_transform_control_helper(this);
	m_transform_control_helper->m_collision_tree = m_view_window->m_collision_tree;
	m_transform_control_helper->m_editor_renderer = m_renderer;

	m_transform_control_translation = gcnew transform_control_translation(m_transform_control_helper);
	m_transform_control_rotation = gcnew transform_control_rotation(m_transform_control_helper);

	register_actions();
}

void sound_scene_document::query_sound_scene()
{
	xray::const_buffer temp_buffer("", 1);
	unmanaged_string scene_name(this->Name+".editor_sound_scene");
 	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_scene_document::on_sound_scene_created), g_allocator);
	query_create_resource(scene_name.c_str(), temp_buffer, resources::sound_scene_class, boost::bind(&query_result_delegate::callback, q, _1), g_allocator);
}

xray::sound::sound_scene_ptr& sound_scene_document::get_sound_scene()
{
	R_ASSERT(m_sound_scene->c_ptr()); 
	return *m_sound_scene;
}

void sound_scene_document::on_sound_scene_created(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());
	(*m_sound_scene) = static_cast_resource_ptr<xray::sound::sound_scene_ptr>(data[0].get_unmanaged_resource());
	if(m_sound_scene->c_ptr() && m_editor->multidocument_base->active_document==this)
	{
		m_editor->get_sound_world()->get_editor_world_user().set_active_sound_scene(get_sound_scene(), 1000, 0);
		m_editor->get_sound_world()->get_editor_world_user().set_listener_properties_interlocked(
			get_sound_scene(),
			m_view_window->get_inverted_view_matrix().c.xyz(),
			m_view_window->get_inverted_view_matrix().k.xyz(),
			m_view_window->get_inverted_view_matrix().j.xyz() );
	}
}

void sound_scene_document::save()
{
	if(is_saved)
		return;

	if(m_first_save)
	{
		if(m_save_file_dialog->ShowDialog(this)==System::Windows::Forms::DialogResult::OK)
		{
			System::String^ name_with_ext = m_save_file_dialog->FileName->Remove(0, m_save_file_dialog->InitialDirectory->Length+1);
			Name = name_with_ext->Remove(name_with_ext->Length-19);
			Text = Name;
		}
		else
			return;
	}

	System::String^ file_path = sound_editor::sound_resources_path+"scenes/"+Name+".editor_sound_scene";
	unmanaged_string path = unmanaged_string(file_path);
	xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config(path.c_str());
	u32 i = 0;
	xray::configs::lua_config_value scene_cfg = cfg->get_root()["scene"];
	for each(sound_object_instance^ inst in m_objects)
		inst->save(scene_cfg[i++]);

	cfg->save(configs::target_sources);
	m_command_engine->set_saved_flag();
	is_saved = true;
	m_first_save = false;
}

void sound_scene_document::load()
{
	fs_new::virtual_path_string path;
	path += unmanaged_string(sound_editor::sound_resources_path).c_str();
	path += "scenes/";
	path += unmanaged_string(Name).c_str();
	path += ".editor_sound_scene";

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_scene_document::on_resources_loaded), g_allocator);
	xray::resources::query_resource(
		path.c_str(),
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void sound_scene_document::on_resources_loaded(xray::resources::queries_result& result)
{
	if(result[0].is_successful())
	{
		m_command_engine->clear_stack();
		m_command_engine->set_saved_flag();
		is_saved = true;
		m_first_save = false;
		xray::configs::lua_config_ptr cfg = static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[0].get_unmanaged_resource());
		if(!(cfg->get_root().value_exists("scene")))
			return;

		xray::configs::lua_config_value scene_cfg = cfg->get_root()["scene"];
		xray::configs::lua_config_iterator it = scene_cfg.begin();
		for(; it!=scene_cfg.end(); ++it)
		{
			sound_object_type st = (sound_object_type)(u32)(*it)["resource_type"];
			String^ sound_name = gcnew String((pcstr)(*it)["filename"]);
			sound_object_wrapper^ w = get_editor()->get_sound(sound_name, st);
			sound_object_instance^ inst = w->create_instance();
			inst->set_parent(this);
			inst->load(*it);
			m_objects->Add(inst);
		}
	}
}

void sound_scene_document::tick()
{
	if(!m_renderer)
		return;

	m_input_engine->execute();
	m_gui_binder->update_items();

	m_view_window->tick();	// process camera effector(s)
	if(m_sound_scene->c_ptr())
	{
		m_editor->get_sound_world()->get_editor_world_user().set_listener_properties_interlocked(
			get_sound_scene(),
			m_view_window->get_inverted_view_matrix().c.xyz(),
			m_view_window->get_inverted_view_matrix().k.xyz(),
			m_view_window->get_inverted_view_matrix().j.xyz() );
	}

	m_renderer->scene().set_view_matrix(*m_scene_view, math::invert4x3(m_view_window->get_inverted_view_matrix()));
	m_renderer->scene().set_projection_matrix(*m_scene_view, m_view_window->get_projection_matrix());
	m_view_window->render();
	for each(sound_object_instance^ inst in m_objects)
		inst->render(m_render_world.editor_renderer().debug(), *m_scene);

	if(active_control)
	{
		active_control->update();
		active_control->draw(m_scene_view->c_ptr(), m_output_window->c_ptr());
	}

	m_renderer->draw_scene(*m_scene, *m_scene_view, *m_output_window, render::viewport_type(float2(0.f, 0.f), float2(1.f, 1.f)));
}

void sound_scene_document::undo()
{
	m_command_engine->rollback(-1);
	is_saved = m_command_engine->saved_flag();
}

void sound_scene_document::redo()
{
	m_command_engine->rollback(1);
	is_saved = m_command_engine->saved_flag();
}

void sound_scene_document::copy(bool del)
{
	XRAY_UNREFERENCED_PARAMETER(del);
}

void sound_scene_document::paste()
{
}

void sound_scene_document::select_all()
{
}

void sound_scene_document::del()
{
	is_saved &= !m_command_engine->run(gcnew sound_scene_remove_objects_command(this, m_selected_objects));
}

void sound_scene_document::on_document_activated(Object^, EventArgs^)
{
	if(Visible)
	{
		objects_list^ lst = gcnew objects_list(m_selected_objects);
		select_objects(lst);
		if(m_sound_scene->c_ptr())
		{
			m_editor->get_sound_world()->get_editor_world_user().set_active_sound_scene(get_sound_scene(), 1000, 0);
			m_editor->get_sound_world()->get_editor_world_user().set_listener_properties_interlocked(
				get_sound_scene(),
				m_view_window->get_inverted_view_matrix().c.xyz(),
				m_view_window->get_inverted_view_matrix().k.xyz(),
				m_view_window->get_inverted_view_matrix().j.xyz());
		}

		if(m_selected_objects->Count>0)
			m_editor->toolbar->enable_buttons(true);
	}
}

void sound_scene_document::on_property_changed(String^ prop_name, Object^ old_val, Object^ new_val)
{
	is_saved &= !m_command_engine->run(gcnew sound_scene_change_objects_property_command(this, m_selected_objects, prop_name, old_val, new_val));
}

void sound_scene_document::view_window_drag_enter(Object^, DragEventArgs^ e)
{
	if(!e->Data->GetDataPresent("tree_view_item"))
	{
		e->Effect = DragDropEffects::None;
		return;
	}
	//if dragging with control - copy
	if((ModifierKeys & Keys::Control) == Keys::Control)
		e->Effect = DragDropEffects::Copy;
	else
		e->Effect = DragDropEffects::Move;
}

void sound_scene_document::view_window_drag_drop(Object^, DragEventArgs^ e)
{
	if(e->Data->GetDataPresent("tree_view_item"))
	{
		u32 t = safe_cast<u32>(e->Data->GetData("mode"));
		List<String^>^ lst = safe_cast<List<String^>^>(e->Data->GetData("tree_view_item"));
		float3 origin;
		float3 direction;
		float3 picked_position;
		m_view_window->get_mouse_ray(origin, direction);
		collision::object const* picked_collision = NULL;
		bool bpick = m_view_window->ray_query(
			xray::editor_base::collision_object_type_dynamic, 
			origin, 
			direction, 
			&picked_collision, 
			&picked_position
		);

		bpick |= xray::editor_base::transform_control_base::plane_ray_intersect(
			float3(0, 0, 0),
			float3(0, 1, 0),
			origin,						
			direction,
			picked_position
		);

		if(bpick)
		{
			add_objects(t, picked_position, lst);
			m_view_window->Activate();
		}
	}
}

void sound_scene_document::add_objects(u32 t, float3 position, List<String^>^ obj_list)
{
	is_saved &= !m_command_engine->run(gcnew sound_scene_add_objects_command(this, (sound_object_type)t, position, obj_list));
}

void sound_scene_document::add_object(sound_object_instance^ inst)
{
	if(!m_objects->Contains(inst))
		m_objects->Add(inst);

	m_selected_objects->Clear();
	select_object(inst);
}

void sound_scene_document::remove_object(Guid inst_id)
{
	objects_list^ lst = gcnew objects_list(m_objects);
	for each(sound_object_instance^ inst in lst)
	{
		if(inst->id==inst_id)
		{
			m_objects->Remove(inst);
			if(m_selected_objects->Contains(inst))
				m_selected_objects->Remove(inst);

			delete inst;
		}
	}

	m_editor->clear_properties();
	clear_helper();
}

void sound_scene_document::clear_helper()
{
	if(m_transform_control_helper && m_transform_control_helper->m_object!=nullptr)
	{
		delete m_transform_control_helper->m_object;
		m_transform_control_helper->m_object = nullptr;
		m_transform_control_helper->m_changed = true;
	}
}

void sound_scene_document::clear_selection()
{
	m_selected_objects->Clear();
	m_editor->clear_properties();
	m_editor->toolbar->enable_buttons(false);
}

bool sound_scene_document::is_selected(sound_object_instance^ inst)
{
	return m_selected_objects->Contains(inst);
}

void sound_scene_document::select_object(sound_object_instance^ inst)
{
	if(inst==nullptr)
	{
		clear_helper();
		m_editor->clear_properties();
		return;
	}

	objects_list^ lst = gcnew objects_list();
	lst->Add(inst);
	select_objects(lst);
}

void sound_scene_document::select_objects(objects_list^ lst)
{
	clear_helper();
	m_selected_objects->Clear();
	m_selected_objects->AddRange(lst);
	if(m_selected_objects->Count==0)
		return m_editor->clear_properties();

	m_editor->toolbar->enable_buttons(true);
	typedef array<xray::editor::wpf_controls::property_container^> containers_list;
	containers_list^ containers = gcnew containers_list(m_selected_objects->Count);
	List<String^>^ wrapper_names = gcnew List<String^>();
	u32 i = 0;
	for each(sound_object_instance^ inst in m_selected_objects)
	{
		containers[i++] = inst->get_property_container();
		if(!wrapper_names->Contains(inst->get_sound_object()->name))
			wrapper_names->Add(inst->get_sound_object()->name);
	}

	m_editor->show_properties(containers, true);
	m_editor->track_selected(wrapper_names);
	m_transform_control_helper->m_object = gcnew se_transform_value_object(m_selected_objects[0]->get_transform(), gcnew Action<bool>(this, &sound_scene_document::active_object_matrix_modified));
}

void sound_scene_document::active_object_matrix_modified(bool apply)
{
	if(m_selected_objects->Count>0)
	{
		float4x4 m = m_transform_control_helper->m_object->get_ancor_transform();
		if(apply)
		{
			se_transform_value_object^ obj = safe_cast<se_transform_value_object^>(m_transform_control_helper->m_object);
			change_object_transform(m_selected_objects[0], obj->get_start_transform(), m);
		}
		else
			m_selected_objects[0]->set_transform(m);
	}
}

void sound_scene_document::change_object_transform(sound_object_instance^ inst, float4x4 old_transform, float4x4 new_transform)
{
	is_saved &= !m_command_engine->run(gcnew sound_scene_change_object_transform_command(this, inst, old_transform, new_transform));
}

sound_object_instance^ sound_scene_document::get_object_by_id(Guid obj_id)
{
	for each(sound_object_instance^ inst in m_objects)
	{
		if(inst->id.Equals(obj_id))
			return inst;
	}

	return nullptr;
}

void sound_scene_document::play_selected()
{
	for each(sound_object_instance^ inst in m_selected_objects)
		inst->play();
}

void sound_scene_document::pause_selected()
{
	for each(sound_object_instance^ inst in m_selected_objects)
		inst->pause();
}

void sound_scene_document::stop_selected()
{
	for each(sound_object_instance^ inst in m_selected_objects)
		inst->stop();
}

void sound_scene_document::play_all()
{
	for each(sound_object_instance^ inst in m_objects)
		inst->play();
}

void sound_scene_document::stop_all()
{
	for each(sound_object_instance^ inst in m_objects)
		inst->stop_now();
}

bool sound_scene_document::all_playing()
{
	for each(sound_object_instance^ inst in m_objects)
	{
		if(!inst->is_playing())
			return false;
	}

	return true;
}

bool sound_scene_document::all_stoped()
{
	for each(sound_object_instance^ inst in m_objects)
	{
		if(inst->is_playing())
			return false;
	}

	return true;
}
