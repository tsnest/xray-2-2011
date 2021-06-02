////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_collection_item.h"
#include "sound_object_wrapper.h"
#include "single_sound_wrapper.h"
#include "composite_sound_wrapper.h"
#include "sound_collection_wrapper.h"

using xray::sound_editor::sound_collection_item;
using xray::sound_editor::sound_object_type;
using xray::sound_editor::sound_object_wrapper;

sound_collection_item::sound_collection_item()
{
	sound_type = sound_object_type::single;
	m_sound_object = nullptr;
	parent = nullptr;
	is_link = true;
}

sound_collection_item::~sound_collection_item()
{
	delete m_sound_object;
}

void sound_collection_item::save(xray::configs::lua_config_value& cfg)
{
	R_ASSERT(m_sound_object!=nullptr);
	unmanaged_string str(m_sound_object->name);
	cfg["filename"] = str.c_str();
	if(sound_type==sound_object_type::single)
		cfg["resource_type"] = resources::single_sound_class;
	else
	{
		if(!is_link)
		{
			xray::configs::lua_config_value val = cfg["collection"];
			m_sound_object->save(val);
		}

		if(sound_type==sound_object_type::composite)
			cfg["resource_type"] = resources::composite_sound_class;
		else
			cfg["resource_type"] = resources::sound_collection_class;
	}

	if(is_link)
		m_sound_object->save();
}

void sound_collection_item::load(xray::configs::lua_config_value const& cfg)
{
	String^ n = gcnew String((pcstr)cfg["filename"]);
	resources::class_id_enum class_id = (resources::class_id_enum)(u32)cfg["resource_type"];
	if(class_id==resources::single_sound_class)
	{
		m_sound_object = gcnew single_sound_wrapper(n);
		sound_type = sound_object_type::single;
	}
	else if(class_id==resources::composite_sound_class)
	{
		m_sound_object = gcnew composite_sound_wrapper(n);
		sound_type = sound_object_type::composite;
	}
	else
	{
		m_sound_object = gcnew sound_collection_wrapper(n);
		sound_type = sound_object_type::collection;
	}

	if(cfg.value_exists("collection"))
	{
		m_sound_object->load(cfg);
		is_link = false;
	}
	else
	{
		Action<sound_object_wrapper^>^ cb = gcnew Action<sound_object_wrapper^>(this, &sound_collection_item::on_sound_loaded);
		m_sound_object->load(cb, cb);
	}
}

void sound_collection_item::on_sound_loaded(sound_object_wrapper^ obj)
{
	XRAY_UNREFERENCED_PARAMETER(obj);
}

String^ sound_collection_item::ToString()
{
	if(m_sound_object!=nullptr)
		return m_sound_object->name;

	return "";
}