////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "composite_sound_wrapper.h"
#include "composite_sound_item.h"

using xray::sound_editor::composite_sound_wrapper;
using xray::sound_editor::sound_collection_item;

composite_sound_wrapper::composite_sound_wrapper(String^ name)
:sound_object_wrapper(name)
{
	m_items = gcnew List<composite_sound_item^>();
}

composite_sound_wrapper::~composite_sound_wrapper()
{
	for each(composite_sound_item^ itm in m_items)
		delete itm;

	delete m_items;
}

void composite_sound_wrapper::save(xray::configs::lua_config_value& root)
{
	root["after_playing_pause"] = after_playing_pause;
	root["before_playing_pause"] = before_playing_pause;
	for(int i=0; i<m_items->Count; ++i)
	{
		configs::lua_config_value val = root["sound_items"][i];
		m_items[i]->save(val);
	}
}

void composite_sound_wrapper::save()
{
	String^ file_path = gcnew String(sound_editor::sound_resources_path + "composite/" + m_name + ".composite_sound");
	unmanaged_string str(file_path);
	configs::lua_config_ptr const& config = configs::create_lua_config(str.c_str());
	configs::lua_config_value root = config->get_root()["composite_sound"];
	save(root);
	config->save(configs::target_sources);
}

void composite_sound_wrapper::load(xray::configs::lua_config_value const& cfg)
{
	if(cfg.value_exists("composite_sound"))
	{
		configs::lua_config_value root = cfg["composite_sound"];
		after_playing_pause = (u32)root["after_playing_pause"];
		before_playing_pause = (u32)root["before_playing_pause"];
		if(root.value_exists("sound_items"))
		{
			configs::lua_config_iterator it = root["sound_items"].begin();
			for(;it!=root["sound_items"].end(); ++it)
			{
				composite_sound_item^ itm = gcnew composite_sound_item();
				itm->load(*it);
				m_items->Add(itm);
			}
		}
	}
}

void composite_sound_wrapper::load(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback)
{
	m_sound_loaded_callback = sound_callback;
	m_options_loaded_callback = options_callback;
	String^ path = gcnew String(sound_editor::sound_resources_path + "composite/" + m_name + ".composite_sound");
	unmanaged_string options_path(path);
	resources::request request[] = { 
		options_path.c_str(), resources::lua_config_class
	};
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &composite_sound_wrapper::on_options_loaded), g_allocator);
	query_resources(request, 1, boost::bind(&query_result_delegate::callback, q, _1), g_allocator);
}

void composite_sound_wrapper::on_options_loaded(xray::resources::queries_result& data)
{
	if(data[0].is_successful())
	{
		configs::lua_config_ptr cfg = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
		load(cfg->get_root());
	}

	if(m_options_loaded_callback)
		m_options_loaded_callback(this);

	math::float4x4 matrix = math::float4x4().identity();
	math::float4x4 const* matrix_pointers[] = {&matrix};
	unmanaged_string file_name(m_name);
	resources::request request = { 
		file_name.c_str(), resources::composite_sound_class
	};

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &composite_sound_wrapper::on_sound_loaded), g_allocator);
	resources::query_resource_params params(&request, NULL, 1, 
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator, NULL, matrix_pointers
	);

	query_resources(params);
}

void composite_sound_wrapper::on_sound_loaded(xray::resources::queries_result& data)
{
	ASSERT(data.is_successful());
	(*m_sound) = static_cast_resource_ptr<sound::sound_emitter_ptr>(data[0].get_unmanaged_resource());
	m_is_loaded = true;
	if(on_loaded)
		on_loaded(this, EventArgs::Empty);

	if(m_sound_loaded_callback)
		m_sound_loaded_callback(this);
}

void composite_sound_wrapper::apply_changes(bool load_resources)
{
	save();
	if (load_resources)
		load(nullptr, nullptr);
}

void composite_sound_wrapper::revert_changes()
{
	load(nullptr, nullptr);
}

void composite_sound_wrapper::add_item(sound_collection_item^ itm)
{
	composite_sound_item^ item = dynamic_cast<composite_sound_item^>(itm);
	R_ASSERT(item!=nullptr);
	if(!m_items->Contains(item))
	{
		m_items->Add(item);
		item->parent = this;
	}
}

void composite_sound_wrapper::remove_item(sound_collection_item^ itm)
{
	composite_sound_item^ item = dynamic_cast<composite_sound_item^>(itm);
	R_ASSERT(item!=nullptr);
	if(m_items->Contains(item))
	{
		m_items->Remove(item);
		item->parent = nullptr;
	}
}

composite_sound_wrapper::items_list^ composite_sound_wrapper::items()
{
	items_list^ lst = gcnew items_list();
	for each(composite_sound_item^ item in m_items)
		lst->Add(item);

	return lst;
}