////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_item_added_removed_commands.h"
#include "animation_setup_document.h"
#include <xray/strings_stream.h>

using xray::animation_editor::animation_setup_item_added_command;
using xray::animation_editor::animation_setup_item_removed_command;
using xray::animation_editor::animation_setup_document;
using namespace xray::configs;
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_item_added_command ///////////////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_item_added_command::animation_setup_item_added_command(animation_setup_document^ d, setup_panel_event_args^ e)
:m_document(d)
{
	m_channel_name = e->channel_name;
	m_position = (Single)e->new_values[0];
	m_partition_to_split_id = Guid(e->item_names[0]);
	m_item_id = Guid::NewGuid();
}

animation_setup_item_added_command::~animation_setup_item_added_command()
{
}

bool animation_setup_item_added_command::commit()
{
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	if(ch==nullptr)
		return false;

	animation_channel_item^ item;
	int item_insert_index = 0;
	if(ch->type==animation_channel_type::intervals)
	{
		Single scale = m_document->setup_panel->time_layout_scale;
		Single start_time = m_position/scale + 1;
		Single length = m_document->setup_panel->snap_to_frames ? 1000/m_document->setup_panel->fps : 10;
		Single left_x = 0;
		Single right_x = ch->length/scale;
		for(int index=0; index<ch->objects->Count; index++)
		{
			animation_channel_interval^ i = (animation_channel_interval^)ch->objects[index];
			if((i->start_time+i->length)/scale < start_time)
			{
				left_x = (i->start_time+i->length)/scale;
				item_insert_index = index + 1;
				if(item_insert_index < ch->objects->Count-1)
					right_x = ((animation_channel_interval^)ch->objects[index+1])->start_time/scale;
				else
					right_x = ch->length/scale;
			}
		}

		if(right_x-left_x-length<-0.0001)
			return false;

		if(start_time+length > right_x)
			start_time = right_x - length;

		item = gcnew animation_channel_interval(ch, start_time, length, 0);
	}
	else if(ch->type==animation_channel_type::partitions)
	{
		animation_channel_partition^ partition_to_split = (animation_channel_partition^)m_document->find_item(ch, m_partition_to_split_id);
		Single scale = m_document->setup_panel->time_layout_scale;
		if(partition_to_split==nullptr)
			return false;

		Single l1 = m_position / scale;
		if(m_document->setup_panel->snap_to_frames)
			l1 = (Single)Math::Round(l1 * m_document->setup_panel->fps / 1000.0f) * 1000.0f / m_document->setup_panel->fps;

		Single l2 = partition_to_split->length / scale - l1;
		partition_to_split->change_property("length", l1);
		item = gcnew animation_channel_partition(ch, l2, partition_to_split->type);
		item_insert_index = ch->objects->IndexOf(partition_to_split) + 1;
	}
	else
	{
		Single time = m_position/m_document->setup_panel->time_layout_scale;
		item = gcnew animation_channel_event(ch, time, 0);
		item_insert_index = ch->objects->Count;
	}

	item->id = m_item_id;
	m_document->setup_panel->add_item(m_channel_name, item, item_insert_index);
	m_document->save_channel(m_channel_name);
	return true;
}

void animation_setup_item_added_command::rollback()
{
	m_document->setup_panel->remove_item(m_channel_name, m_item_id);
	m_document->save_channel(m_channel_name);
}
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_item_removed_command /////////////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_item_removed_command::animation_setup_item_removed_command(animation_setup_document^ d, setup_panel_event_args^ e)
:m_document(d)
{
	m_channel_name = e->channel_name;
	m_item_id = Guid(e->item_names[0]);
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	R_ASSERT(ch!=nullptr);
	animation_channel_item^ item = m_document->find_item(ch, m_item_id);
	R_ASSERT(item!=nullptr);
	Single scale = m_document->setup_panel->time_layout_scale;
	animation_channel_interval^ i = dynamic_cast<animation_channel_interval^>(item);
	animation_channel_partition^ p = dynamic_cast<animation_channel_partition^>(item);
	animation_channel_event^ ev = dynamic_cast<animation_channel_event^>(item);
	if(i)
	{
		m_position = i->start_time/scale;
		m_length = i->length/scale;
	}
	else if(p)
	{
		m_partition_insert_id = ch->objects->IndexOf(p);
		int index = m_partition_insert_id - 1;
		if(index<0)
			index = 1;

		m_partition_to_split_id = ch->objects[index]->id;
		m_position = p->length / scale;
		m_partition_type = p->type;
	}
	else if(ev)
		m_position = ev->time/scale;
}

animation_setup_item_removed_command::~animation_setup_item_removed_command()
{
}

bool animation_setup_item_removed_command::commit()
{
	m_document->setup_panel->remove_item(m_channel_name, m_item_id);
	m_document->save_channel(m_channel_name);
	return true;
}

void animation_setup_item_removed_command::rollback()
{
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	if(ch==nullptr)
		return;

	animation_channel_item^ item;
	int item_insert_index = 0;
	if(ch->type==animation_channel_type::intervals)
	{
		Single scale = m_document->setup_panel->time_layout_scale;
		Single start_time = m_position;
		Single length = m_length;
		Single left_x = 0;
		Single right_x = ch->length/scale;
		for(int index=0; index<ch->objects->Count; index++)
		{
			animation_channel_interval^ i = (animation_channel_interval^)ch->objects[index];
			if((i->start_time+i->length)/scale < start_time+1)
			{
				left_x = (i->start_time+i->length)/scale;
				item_insert_index = index + 1;
				if(index < ch->objects->Count-1)
					right_x = ((animation_channel_interval^)ch->objects[index+1])->start_time/scale;
				else
					right_x = ch->length/scale;
			}
		}

		if(right_x-left_x-length<-0.0001)
			return;

		if(start_time+length > right_x)
			start_time = right_x - length;

		item = gcnew animation_channel_interval(ch, start_time, length, 0);
	}
	else if(ch->type==animation_channel_type::partitions)
	{
		animation_channel_partition^ partition_to_split = (animation_channel_partition^)m_document->find_item(ch, m_partition_to_split_id);
		Single scale = m_document->setup_panel->time_layout_scale;
		if(partition_to_split==nullptr)
			return;

		Single l1 = m_position;
		if(m_document->setup_panel->snap_to_frames)
			l1 = (Single)Math::Round(l1 * m_document->setup_panel->fps / 1000.0f) * 1000.0f / m_document->setup_panel->fps;

		Single l2 = partition_to_split->length / scale - l1;
		partition_to_split->change_property("length", l2);
		item = gcnew animation_channel_partition(ch, l1, m_partition_type);
		item_insert_index = m_partition_insert_id;
	}
	else
	{
		Single time = m_position/m_document->setup_panel->time_layout_scale;
		item = gcnew animation_channel_event(ch, time, 0);
		item_insert_index = ch->objects->Count;
	}

	item->id = m_item_id;
	m_document->setup_panel->add_item(m_channel_name, item, item_insert_index);
	m_document->save_channel(m_channel_name);
}
