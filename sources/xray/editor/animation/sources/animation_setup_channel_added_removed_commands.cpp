////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_channel_added_removed_commands.h"
#include "animation_setup_document.h"
#include <xray/strings_stream.h>

using xray::animation_editor::animation_setup_channel_added_command;
using xray::animation_editor::animation_setup_channel_removed_command;
using xray::animation_editor::animation_setup_paste_channel_command;
using xray::animation_editor::animation_setup_document;
using namespace xray::configs;
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_channel_added_command ////////////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_channel_added_command::animation_setup_channel_added_command(animation_setup_document^ d, setup_panel_event_args^ e)
:m_document(d)
{
	m_channel_name = e->channel_name;
	m_first_partition_id = Guid::NewGuid();
}

animation_setup_channel_added_command::~animation_setup_channel_added_command()
{
}

bool animation_setup_channel_added_command::commit()
{
	animation_channel_type t;
	if(m_channel_name=="intervals")
		t = animation_channel_type::intervals;
	else if(m_channel_name=="foot_steps" || m_channel_name=="anim_intervals")
		t = animation_channel_type::partitions;
	else
		t = animation_channel_type::events;

	animation_channel^ ch = m_document->setup_panel->add_channel(m_channel_name, t);
	if(t==animation_channel_type::partitions)
		ch->objects[0]->id = m_first_partition_id;

	m_document->save_all_channels();
	return true;
}

void animation_setup_channel_added_command::rollback()
{
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	m_document->setup_panel->remove_channel(ch);
	m_document->save_all_channels();
}
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_channel_removed_command //////////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_channel_removed_command::animation_setup_channel_removed_command(animation_setup_document^ d, setup_panel_event_args^ e)
:m_document(d)
{
	m_channel_name = e->channel_name;
	lua_config_ptr cfg_ptr = create_lua_config();
	xray::strings::stream stream(g_allocator);
	lua_config_value val = cfg_ptr->get_root()["data"]["event_channels"][0];
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	m_document->save_channel(ch, val);
	val = cfg_ptr->get_root()["data"]["items_ids"][0];
	for(int i=0; i<ch->objects->Count; ++i)
	{
		animation_channel_item^ item = ch->objects[i];
		unmanaged_string id_str = item->id.ToString();
		val[i] = id_str.c_str();
	}
	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());
}

animation_setup_channel_removed_command::~animation_setup_channel_removed_command()
{
	FREE(m_cfg_string);
}

bool animation_setup_channel_removed_command::commit()
{
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	m_document->setup_panel->remove_channel(ch);
	m_document->save_all_channels();
	return true;
}

void animation_setup_channel_removed_command::rollback()
{
	lua_config_ptr config_ptr = create_lua_config_from_string(m_cfg_string);
	m_document->load_channel(*(config_ptr->get_root()["data"]["event_channels"].begin()));
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	for(u32 i=0; i<config_ptr->get_root()["data"]["items_ids"][0].size(); ++i)
	{
		String^ id_str = gcnew String((pcstr)config_ptr->get_root()["data"]["items_ids"][0][i]);
		animation_channel_item^ item = ch->objects[i];
		item->id = Guid(id_str);
	}

	m_document->save_all_channels();
}
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_paste_channel_command ////////////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_paste_channel_command::animation_setup_paste_channel_command(animation_setup_document^ d, pcstr cfg_string, u32 max_frames)
:m_document(d),
m_max_frames(max_frames),
m_channel_name("")
{
	m_cfg_string = strings::duplicate(g_allocator, cfg_string);
}

animation_setup_paste_channel_command::~animation_setup_paste_channel_command()
{
	FREE(m_cfg_string);
}

bool animation_setup_paste_channel_command::commit()
{
	lua_config_ptr config_ptr = create_lua_config_from_string(m_cfg_string);
	lua_config_value cfg = *(config_ptr->get_root()["data"]["event_channels"].begin());
	lua_config_iterator it_first = cfg["knots"].begin();
	u32 first_knot = (u32)(*it_first);
	u32 last_knot = first_knot;
	for(;it_first!=cfg["knots"].end(); ++it_first)
		last_knot = (u32)(*it_first);

	if(last_knot-first_knot != m_max_frames)
	{
		MessageBox::Show(m_document, "Can not insert channel because of different animation lengths!", "Animation setup manager", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
	}

	m_channel_name = gcnew String((pcstr)cfg["name"]);
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	if(ch!=nullptr)
	{
		MessageBox::Show(m_document, "Channel ["+m_channel_name+"] already exists!", "Animation setup manager", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return false;
	}

	m_document->load_channel(cfg);
	ch = m_document->find_channel(m_channel_name);
	for(u32 i=0; i<config_ptr->get_root()["data"]["items_ids"][0].size(); ++i)
	{
		String^ id_str = gcnew String((pcstr)config_ptr->get_root()["data"]["items_ids"][0][i]);
		animation_channel_item^ item = ch->objects[i];
		item->id = Guid(id_str);
	}

	m_document->save_all_channels();
	return true;
}

void animation_setup_paste_channel_command::rollback()
{
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	m_document->setup_panel->remove_channel(ch);
	m_document->save_all_channels();
}
