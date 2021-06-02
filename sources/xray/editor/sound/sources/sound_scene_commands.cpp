////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_scene_commands.h"
#include "sound_scene_document.h"
#include "sound_object_wrapper.h"
#include "single_sound_wrapper.h"
#include "composite_sound_wrapper.h"
#include "sound_object_instance.h"

using xray::sound_editor::sound_scene_add_objects_command;
using xray::sound_editor::sound_scene_remove_objects_command;
using xray::sound_editor::sound_scene_change_object_transform_command;
using xray::sound_editor::sound_scene_change_objects_property_command;
using xray::sound_editor::sound_scene_document;
using xray::sound_editor::sound_object_instance;
using xray::sound_editor::sound_object_type;
////////////////////////////////////////////////////////////////////////////
// public ref class sound_scene_add_objects_command ////////////////////////
////////////////////////////////////////////////////////////////////////////
sound_scene_add_objects_command::sound_scene_add_objects_command(sound_scene_document^ d, sound_object_type t, float3 position, List<String^>^ obj_list)
:m_document(d),
m_objects_type(t)
{
	m_ids_list = gcnew ids_list();
	m_instances = gcnew instances_list();
	m_positions = gcnew positions_list();
	for each(String^ obj_name in obj_list)
	{
		m_ids_list->Add(Guid::NewGuid());
		m_instances->Add(obj_name);
		m_positions->Add(Float3(position));
	}
}

sound_scene_add_objects_command::~sound_scene_add_objects_command()
{
	delete m_instances;
	delete m_positions;
}

bool sound_scene_add_objects_command::commit()
{
	for(int i=0; i<m_instances->Count; ++i)
	{
		float4x4 transform = float4x4().identity();
		transform.c.xyz() = float3(m_positions[i]);
		sound_object_wrapper^ w = m_document->get_editor()->get_sound(m_instances[i], m_objects_type);
		sound_object_instance^ inst = w->create_instance();
		inst->set_parent(m_document);
		inst->set_transform(transform);
		inst->id = m_ids_list[i];
		m_document->add_object(inst);
	}

	return true;
}

void sound_scene_add_objects_command::rollback()
{
	for each(Guid inst_id in m_ids_list)
		m_document->remove_object(inst_id);
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_scene_remove_objects_command /////////////////////
////////////////////////////////////////////////////////////////////////////
sound_scene_remove_objects_command::sound_scene_remove_objects_command(sound_scene_document^ d, List<sound_object_instance^>^ obj_list)
:m_document(d)
{
	m_ids_list = gcnew ids_list();
	m_instances = gcnew instances_list();
	m_positions = gcnew positions_list();
	m_directions = gcnew positions_list();
	for each(sound_object_instance^ inst in obj_list)
	{
		m_ids_list->Add(inst->id);
		sound_object_wrapper^ w = inst->get_sound_object();
		String^ name = w->name;
		sound_object_type t = sound_object_type::collection;
		if(dynamic_cast<composite_sound_wrapper^>(w))
			t = sound_object_type::composite;
		else if(dynamic_cast<single_sound_wrapper^>(w))
			t = sound_object_type::single;
		m_instances->Add(KeyValuePair<String^, sound_object_type>(name, t));
		m_positions->Add(inst->position);
		m_directions->Add(inst->rotation);
	}
}

sound_scene_remove_objects_command::~sound_scene_remove_objects_command()
{
	delete m_ids_list;
	delete m_instances;
	delete m_positions;
	delete m_directions;
}

bool sound_scene_remove_objects_command::commit()
{
	for each(Guid inst_id in m_ids_list)
		m_document->remove_object(inst_id);

	return true;
}

void sound_scene_remove_objects_command::rollback()
{
	for(int i=0; i<m_instances->Count; ++i)
	{
		float4x4 transform = float4x4().identity();
		transform = math::create_scale(transform.get_scale()) * math::create_rotation(m_directions[i]) * math::create_translation(m_positions[i]);
		sound_object_wrapper^ w = m_document->get_editor()->get_sound(m_instances[i].Key, m_instances[i].Value);
		sound_object_instance^ inst = w->create_instance();
		inst->set_parent(m_document);
		inst->set_transform(transform);
		inst->id = m_ids_list[i];
		m_document->add_object(inst);
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_scene_change_object_transform_command ////////////
////////////////////////////////////////////////////////////////////////////
sound_scene_change_object_transform_command::sound_scene_change_object_transform_command(sound_scene_document^ d, sound_object_instance^ inst, float4x4 old_val, float4x4 new_val)
:m_document(d),
m_instance_id(inst->id)
{
	m_old_val = NEW(float4x4)(old_val);
	m_new_val = NEW(float4x4)(new_val);
}

sound_scene_change_object_transform_command::~sound_scene_change_object_transform_command()
{
	DELETE(m_old_val);
	DELETE(m_new_val);
}

bool sound_scene_change_object_transform_command::commit()
{
	sound_object_instance^ inst = m_document->get_object_by_id(m_instance_id);
	R_ASSERT(inst!=nullptr);
	inst->set_transform(*m_new_val);
	if(inst->is_selected())
		m_document->get_editor()->show_properties(inst->get_property_container(), true);

	return true;
}

void sound_scene_change_object_transform_command::rollback()
{
	sound_object_instance^ inst = m_document->get_object_by_id(m_instance_id);
	R_ASSERT(inst!=nullptr);
	inst->set_transform(*m_old_val);
	if(inst->is_selected())
		m_document->get_editor()->show_properties(inst->get_property_container(), true);
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_scene_change_objects_property_command ////////////
////////////////////////////////////////////////////////////////////////////
sound_scene_change_objects_property_command::sound_scene_change_objects_property_command(sound_scene_document^ d, List<sound_object_instance^>^ obj_list, String^ prop_name, Object^ old_val, Object^ new_val)
:m_document(d),
m_property_name(prop_name),
m_old_val(old_val),
m_new_val(new_val)
{
	m_instance_ids = gcnew ids_list();
	for each(sound_object_instance^ inst in obj_list)
		m_instance_ids->Add(inst->id);
}

sound_scene_change_objects_property_command::~sound_scene_change_objects_property_command()
{
	delete m_instance_ids;
}

bool sound_scene_change_objects_property_command::commit()
{
	for(int i=0; i<m_instance_ids->Count; ++i)
	{
		sound_object_instance^ inst = m_document->get_object_by_id(m_instance_ids[i]);
		R_ASSERT(inst!=nullptr);
		inst->GetType()->GetProperty(m_property_name)->SetValue(inst, m_new_val, nullptr);
		if(inst->is_selected())
			m_document->get_editor()->show_properties(inst->get_property_container(), true);
	}

	return true;
}

void sound_scene_change_objects_property_command::rollback()
{
	for(int i=0; i<m_instance_ids->Count; ++i)
	{
		sound_object_instance^ inst = m_document->get_object_by_id(m_instance_ids[i]);
		R_ASSERT(inst!=nullptr);
		inst->GetType()->GetProperty(m_property_name)->SetValue(inst, m_old_val, nullptr);
		if(inst->is_selected())
			m_document->get_editor()->show_properties(inst->get_property_container(), true);
	}
}