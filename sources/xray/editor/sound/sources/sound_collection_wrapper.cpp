////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_collection_wrapper.h"
#include "sound_collection_item.h"

using xray::sound_editor::sound_collection_wrapper;
using xray::sound_editor::collection_play_type;
using xray::sound_editor::sound_collection_item;

sound_collection_wrapper::sound_collection_wrapper(String^ name)
:sound_object_wrapper(name)
{
	play_type = collection_play_type::random;
	can_repeat_last = false;
	cycle_from = 0;
	min_delay = 0.0f;
	max_delay = 0.0f;
	m_items = gcnew items_list();
}

sound_collection_wrapper::~sound_collection_wrapper()
{
	for each(sound_collection_item^ itm in m_items)
		delete itm;

	delete m_items;
}

void sound_collection_wrapper::save(xray::configs::lua_config_value& root)
{
	unmanaged_string play_type_str(play_type.ToString());
	root["type"] = play_type_str.c_str();
	root["dont_repeat_sound_successively"] = can_repeat_last;
	root["cyclic_repeat_from_sound"] = cycle_from;
	root["min_delay"] = min_delay;
	root["max_delay"] = max_delay;
	for(int i=0; i<m_items->Count; ++i)
	{
		configs::lua_config_value val = root["sound_items"][i];
		m_items[i]->save(val);
	}
}

void sound_collection_wrapper::save()
{
	String^ file_path = gcnew String(sound_editor::sound_resources_path + "collections/" + m_name + ".sound_collection");
	unmanaged_string str(file_path);
	configs::lua_config_ptr const& config = configs::create_lua_config(str.c_str());
	configs::lua_config_value root = config->get_root()["collection"];
	save(root);
	config->save(configs::target_sources);
}

void sound_collection_wrapper::load(xray::configs::lua_config_value const& cfg)
{
	if(cfg.value_exists("collection"))
	{
		configs::lua_config_value root = cfg["collection"];
		String^ t = gcnew String((pcstr)root["type"]);
		play_type = safe_cast<collection_play_type>(System::Enum::Parse(collection_play_type::typeid, t));
		can_repeat_last = (bool)root["dont_repeat_sound_successively"];
		cycle_from = (u32)root["cyclic_repeat_from_sound"];
		if(root.value_exists("min_delay"))
			min_delay = (float)root["min_delay"];

		if(root.value_exists("max_delay"))
			max_delay = (float)root["max_delay"];

		if(root.value_exists("sound_items"))
		{
			configs::lua_config_iterator it = root["sound_items"].begin();
			for(;it!=root["sound_items"].end(); ++it)
			{
				sound_collection_item^ itm = gcnew sound_collection_item();
				itm->load(*it);
				m_items->Add(itm);
			}
		}
	}
}

void sound_collection_wrapper::load(Action<sound_object_wrapper^>^ options_callback, Action<sound_object_wrapper^>^ sound_callback)
{
	m_sound_loaded_callback = sound_callback;
	m_options_loaded_callback = options_callback;
	String^ path = gcnew String(sound_editor::sound_resources_path + "collections/" + m_name + ".sound_collection");
	unmanaged_string options_path(path);
	resources::request request[] = { 
		options_path.c_str(), resources::lua_config_class
	};
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_collection_wrapper::on_options_loaded), g_allocator);
	query_resources(request, 1, boost::bind(&query_result_delegate::callback, q, _1), g_allocator);
}

void sound_collection_wrapper::on_options_loaded(xray::resources::queries_result& data)
{
	if(data[0].is_successful())
	{
		configs::lua_config_ptr cfg = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
		load(cfg->get_root());
	}

	if(m_options_loaded_callback)
		m_options_loaded_callback(this);

	bool all_items_exist = true;
	for ( int i = 0; i < m_items->Count; ++i )
	{
		if ( !m_items[i]->sound_object->is_exist )
		{
			all_items_exist		= false;
			break;
		}
	}

	if ( !all_items_exist )
	{
		m_is_loaded = true;
		if(on_loaded)
			on_loaded(this, EventArgs::Empty);

		if(m_sound_loaded_callback)
			m_sound_loaded_callback(this);
		return;
	}

	math::float4x4 matrix = math::float4x4().identity();
	math::float4x4 const* matrix_pointers[] = {&matrix};
	unmanaged_string file_name(m_name);
	resources::request request = { 
		file_name.c_str(), resources::sound_collection_class
	};

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &sound_collection_wrapper::on_sound_loaded), g_allocator);
	resources::query_resource_params params(&request, NULL, 1, 
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator, NULL, matrix_pointers
	);

	query_resources(params);
}

void sound_collection_wrapper::on_sound_loaded(xray::resources::queries_result& data)
{
	ASSERT(data.is_successful());
	(*m_sound) = static_cast_resource_ptr<sound::sound_emitter_ptr>(data[0].get_unmanaged_resource());
	m_is_loaded = true;
	if(on_loaded)
		on_loaded(this, EventArgs::Empty);

	if(m_sound_loaded_callback)
		m_sound_loaded_callback(this);
}

void sound_collection_wrapper::apply_changes(bool load_resources)
{
	save();
	if (load_resources)
		load(nullptr, nullptr);
}

void sound_collection_wrapper::revert_changes()
{
	load(nullptr, nullptr);
}

void sound_collection_wrapper::add_item(sound_collection_item^ itm)
{
	if(!m_items->Contains(itm))
	{
		m_items->Add(itm);
		itm->parent = this;
	}
}

void sound_collection_wrapper::remove_item(sound_collection_item^ itm)
{
	if(m_items->Contains(itm))
	{
		m_items->Remove(itm);
		itm->parent = nullptr;
	}
}