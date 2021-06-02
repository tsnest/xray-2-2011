////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_collection_commands.h"
#include "sound_collection_document.h"
#include "sound_object_wrapper.h"
#include "single_sound_wrapper.h"
#include "sound_collection_wrapper.h"
#include "sound_collection_item.h"
#include <xray/strings_stream.h>

using xray::sound_editor::sound_collection_add_items_command;
using xray::sound_editor::sound_collection_remove_items_command;
using xray::sound_editor::sound_collection_change_objects_property_command;
using xray::sound_editor::sound_collection_create_collection_command;
using xray::sound_editor::sound_collection_document;
using xray::sound_editor::sound_object_type;
using xray::editor::controls::tree_node;
////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_add_items_command /////////////////////
////////////////////////////////////////////////////////////////////////////
sound_collection_add_items_command::sound_collection_add_items_command(sound_collection_document^ d, String^ root_path, List<String^>^ names_list, sound_object_type t)
:m_document(d),
m_root_path(root_path)
{
	xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_value root = cfg->get_root()["items"];
	for(int i=0; i<names_list->Count; ++i)
	{
		unmanaged_string name(names_list[i]);
		root[i]["filename"] = name.c_str();
		if(t==sound_object_type::single)
			root[i]["resource_type"] = resources::single_sound_class;
		else if(t==sound_object_type::composite)
			root[i]["resource_type"] = resources::composite_sound_class;
		else
			root[i]["resource_type"] = resources::sound_collection_class;
	}

	cfg->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

sound_collection_add_items_command::~sound_collection_add_items_command()
{
	FREE(m_cfg_string);
}

bool sound_collection_add_items_command::commit()
{
	xray::editor::controls::tree_node^ dest = m_document->get_tree_view()->get_node(m_root_path);
	R_ASSERT(dest!=nullptr);
	xray::configs::lua_config_ptr config_ptr = xray::configs::create_lua_config_from_string(m_cfg_string);
	if(!config_ptr || config_ptr->empty()) 
		return false;

	xray::configs::lua_config_iterator it = config_ptr->get_root()["items"].begin();
	for(; it!=config_ptr->get_root()["items"].end(); ++it)
	{
		sound_collection_item^ item = gcnew sound_collection_item();
		item->load(*it);
		m_document->add_item_to_collection(dest, item);
	}

	return true;
}

void sound_collection_add_items_command::rollback()
{
	xray::editor::controls::tree_node^ dest = m_document->get_tree_view()->get_node(m_root_path);
	R_ASSERT(dest!=nullptr);
	sound_object_wrapper^ w = nullptr;
	if(dest->Tag==nullptr)
		w = m_document->collection();
	else
		w = safe_cast<sound_collection_item^>(dest->Tag)->sound_object;

	R_ASSERT(w->items()!=nullptr);
	xray::configs::lua_config_ptr config_ptr = xray::configs::create_lua_config_from_string(m_cfg_string);
	if(!config_ptr || config_ptr->empty()) 
		return;

	xray::configs::lua_config_iterator it = config_ptr->get_root()["items"].begin();
	for(; it!=config_ptr->get_root()["items"].end(); ++it)
	{
		String^ name = gcnew String((*it)["filename"]);
		for each(sound_collection_item^ item in w->items())
		{
			if(item->ToString()==name)
			{
				m_document->remove_item_from_collection(item);
				break;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_remove_items_command //////////////////
////////////////////////////////////////////////////////////////////////////
sound_collection_remove_items_command::sound_collection_remove_items_command(sound_collection_document^ d, List<tree_node^>^ items_list)
:m_document(d)
{
	m_saved_nodes = gcnew List<tree_node^>();
	xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_value root = cfg->get_root()["items"];
	for(int i=0; i<items_list->Count; ++i)
	{
		xray::configs::lua_config_value val = root[i];
		save_childs(items_list[i], val);
	}

	delete m_saved_nodes;
	cfg->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

sound_collection_remove_items_command::~sound_collection_remove_items_command()
{	
	FREE(m_cfg_string);
}

void sound_collection_remove_items_command::save_childs(tree_node^ n, xray::configs::lua_config_value& root)
{
	if(m_saved_nodes->Contains(n))
		return;

	if(n->Tag==nullptr)
		return;

	m_saved_nodes->Add(n);
	sound_collection_item^ item = safe_cast<sound_collection_item^>(n->Tag);
	unmanaged_string name(item->ToString());
	root["filename"] = name.c_str();
	unmanaged_string full_path(n->Parent->FullPath);
	root["full_path"] = full_path.c_str();
	if(item->sound_type==sound_object_type::single)
		root["resource_type"] = resources::single_sound_class;
	else if(item->sound_type==sound_object_type::composite)
		root["resource_type"] = resources::composite_sound_class;
	else
		root["resource_type"] = resources::sound_collection_class;

	for(int i=0; i<n->nodes->Count; ++i)
	{
		xray::configs::lua_config_value val = root["items"][i];
		save_childs(safe_cast<tree_node^>(n->nodes[i]), val);
	}
}

void sound_collection_remove_items_command::remove_childs(xray::configs::lua_config_value& root)
{
	if(root.value_exists("items"))
	{
		xray::configs::lua_config_iterator it = root["items"].begin();
		for(; it!=root["items"].end(); ++it)
		{
			xray::configs::lua_config_value val = *it;
			remove_childs(val);
		}
	}

	String^ filename = gcnew String((pcstr)root["filename"]);
	String^ full_path = gcnew String((pcstr)root["full_path"]);
	xray::editor::controls::tree_node^ parent = m_document->get_tree_view()->get_node(full_path);
	sound_object_wrapper^ w = nullptr;
	if(parent->Tag!=nullptr)
		w = safe_cast<sound_collection_item^>(parent->Tag)->sound_object;
	else
		w = m_document->collection();

	sound_collection_item^ item = nullptr;
	for each(sound_collection_item^ itm in w->items())
	{
		if(itm->ToString()==filename)
		{
			item = itm;
			break;
		}
	}
	
	m_document->remove_item_from_collection(item);
}

bool sound_collection_remove_items_command::commit()
{
	xray::configs::lua_config_ptr config_ptr = xray::configs::create_lua_config_from_string(m_cfg_string);
	if(!config_ptr || config_ptr->empty()) 
		return false;

	xray::configs::lua_config_iterator it = config_ptr->get_root()["items"].begin();
	for(; it!=config_ptr->get_root()["items"].end(); ++it)
	{
		xray::configs::lua_config_value val = *it;
		remove_childs(val);
	}

	return true;
}

void sound_collection_remove_items_command::add_childs(xray::configs::lua_config_value& root)
{
	sound_collection_item^ item = gcnew sound_collection_item();
	item->load(root);
	String^ full_path = gcnew String((pcstr)root["full_path"]);
	xray::editor::controls::tree_node^ dest = m_document->get_tree_view()->get_node(full_path);
	R_ASSERT(dest!=nullptr);
	m_document->add_item_to_collection(dest, item);
}

void sound_collection_remove_items_command::rollback()
{
	xray::configs::lua_config_ptr config_ptr = xray::configs::create_lua_config_from_string(m_cfg_string);
	if(!config_ptr || config_ptr->empty()) 
		return;

	xray::configs::lua_config_iterator it = config_ptr->get_root()["items"].begin();
	for(; it!=config_ptr->get_root()["items"].end(); ++it)
	{
		xray::configs::lua_config_value val = *it;
		add_childs(val);
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_change_objects_property_command ///////
////////////////////////////////////////////////////////////////////////////
sound_collection_change_objects_property_command::sound_collection_change_objects_property_command(sound_collection_document^ d, List<tree_node^>^ obj_list, String^ prop_name, Object^ old_val, Object^ new_val)
:m_document(d),
m_property_name(prop_name),
m_old_val(old_val),
m_new_val(new_val)
{
	m_nodes_paths = gcnew paths_list();
	for each(tree_node^ n in obj_list)
		m_nodes_paths->Add(n->FullPath);
}

sound_collection_change_objects_property_command::~sound_collection_change_objects_property_command()
{
	delete m_nodes_paths;
}

bool sound_collection_change_objects_property_command::commit()
{
	for(int i=0; i<m_nodes_paths->Count; ++i)
	{
		tree_node^ n = m_document->get_tree_view()->get_node(m_nodes_paths[i]);
		sound_object_wrapper^ w = nullptr;
		if(n->Tag==nullptr)
			w = m_document->collection();
		else
			w = safe_cast<sound_collection_item^>(n->Tag)->sound_object;

		R_ASSERT(w!=nullptr);
		System::Reflection::PropertyInfo^ inf = w->GetType()->GetProperty(m_property_name);
		if(inf!=nullptr)
			inf->SetValue(w, m_new_val, nullptr);
	}

	return true;
}

void sound_collection_change_objects_property_command::rollback()
{
	for(int i=0; i<m_nodes_paths->Count; ++i)
	{
		tree_node^ n = m_document->get_tree_view()->get_node(m_nodes_paths[i]);
		sound_object_wrapper^ w = nullptr;
		if(n->Tag==nullptr)
			w = m_document->collection();
		else
			w = safe_cast<sound_collection_item^>(n->Tag)->sound_object;

		R_ASSERT(w!=nullptr);
		System::Reflection::PropertyInfo^ inf = w->GetType()->GetProperty(m_property_name);
		if(inf!=nullptr)
			inf->SetValue(w, m_old_val, nullptr);
	}
}
////////////////////////////////////////////////////////////////////////////
// public ref class sound_collection_create_collection_command /////////////
////////////////////////////////////////////////////////////////////////////
sound_collection_create_collection_command::sound_collection_create_collection_command(sound_collection_document^ d, String^ root_path)
:m_document(d),
m_root_path(root_path)
{
	m_id = System::Guid::NewGuid().ToString();
	xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_value root = cfg->get_root()["collection"];
	root["cyclic_repeat_from_sound"] = 0;
	root["dont_repeat_sound_successively"] = false;
	root["max_delay"] = 0;
	root["min_delay"] = 0;
	root["type"] = "random";
	cfg->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

sound_collection_create_collection_command::~sound_collection_create_collection_command()
{
	FREE(m_cfg_string);
}

bool sound_collection_create_collection_command::commit()
{
	xray::configs::lua_config_ptr config_ptr = xray::configs::create_lua_config_from_string(m_cfg_string);
	if(!config_ptr || config_ptr->empty()) 
		return false;

	sound_collection_wrapper^ w = gcnew sound_collection_wrapper(m_id);
	w->load(config_ptr->get_root());
	xray::editor::controls::tree_node^ dest = m_document->get_tree_view()->get_node(m_root_path);
	R_ASSERT(dest!=nullptr);
	sound_collection_item^ itm = gcnew sound_collection_item();
	itm->sound_object = w;
	itm->sound_type = sound_object_type::collection;
	itm->is_link = false;
	m_document->add_item_to_collection(dest, itm);
	return true;
}

void sound_collection_create_collection_command::rollback()
{
	xray::editor::controls::tree_node^ dest = m_document->get_tree_view()->get_node(m_root_path);
	R_ASSERT(dest!=nullptr);
	sound_object_wrapper^ w = m_document->collection();
	if(dest->Tag!=nullptr)
		w = safe_cast<sound_collection_item^>(dest->Tag)->sound_object;

	R_ASSERT(!dynamic_cast<single_sound_wrapper^>(w));
	for each(sound_collection_item^ itm in w->items())
	{
		if(itm->ToString()==m_id)
		{
			m_document->remove_item_from_collection(itm);
			return;
		}
	}
}
