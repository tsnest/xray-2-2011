////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_property_changed_commands.h"
#include "animation_setup_document.h"
#include "animation_node_interpolator.h"

using xray::animation_editor::animation_setup_item_property_changed_command;
using xray::animation_editor::animation_setup_property_changed_command;
using xray::animation_editor::animation_setup_document;
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_item_property_changed_command ////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_item_property_changed_command::animation_setup_item_property_changed_command(animation_setup_document^ d, setup_panel_event_args^ e)
:m_document(d)
{
	m_item_ids = gcnew List<Guid>();
	m_property_names = gcnew List<String^>();
	m_new_values = gcnew List<Object^>();
	m_old_values = gcnew List<Object^>();

	m_channel_name = e->channel_name;
	m_property_names->AddRange(e->property_names);
	m_new_values->AddRange(e->new_values);
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	for(int i=0; i<e->item_names->Count; ++i)
	{
		String^ ids_name = e->item_names[i];
		Guid id(ids_name);
		m_item_ids->Add(id);
		animation_channel_item^ item = m_document->find_item(ch, id);
		m_old_values->Add(item->get_property(m_property_names[i]));
	}
}

animation_setup_item_property_changed_command::~animation_setup_item_property_changed_command()
{
}

bool animation_setup_item_property_changed_command::commit()
{
	u32 changed_count = 0;
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	R_ASSERT(ch!=nullptr);
	R_ASSERT(m_old_values->Count==m_new_values->Count);
	for(int i=0; i<m_old_values->Count; ++i)
	{
		if(m_old_values[i]->Equals(m_new_values[i]))
			continue;

		animation_channel_item^ item = m_document->find_item(ch, m_item_ids[i]);
		R_ASSERT(item!=nullptr);
		item->change_property(m_property_names[i], m_new_values[i]);
		++changed_count;
	}

	if(changed_count>0)
	{
		m_document->save_channel(m_channel_name);
		return true;
	}

	return false;
}

void animation_setup_item_property_changed_command::rollback()
{
	animation_channel^ ch = m_document->find_channel(m_channel_name);
	R_ASSERT(ch!=nullptr);
	for(int i=0; i<m_old_values->Count; ++i)
	{
		if(m_old_values[i]->Equals(m_new_values[i]))
			continue;

		animation_channel_item^ item = m_document->find_item(ch, m_item_ids[i]);
		R_ASSERT(item!=nullptr);
		item->change_property(m_property_names[i], m_old_values[i]);
	}

	m_document->save_channel(m_channel_name);
}
////////////////////////////////////////////////////////////////////////////
/// ref class animation_setup_property_changed_command /////////////////////
////////////////////////////////////////////////////////////////////////////
animation_setup_property_changed_command::animation_setup_property_changed_command(animation_setup_document^ d, 
																	System::String^ property_name, 
																	System::Object^ new_v, 
																	System::Object^ old_v)
:m_document(d),
m_changed_property_name("Node properties/"+property_name),
m_new_value(new_v),
m_old_value(old_v)
{
}

bool animation_setup_property_changed_command::commit()
{
	if(m_new_value->Equals(m_old_value))
		return false;

	m_document->disable_events_handling = true;
	if(m_changed_property_name=="Node properties/synchronization_group")
		m_document->container->properties[m_changed_property_name]->value = m_new_value;
	else // m_changed_property_name=="type"||"time_s"||"time_f"||"epsilon"
	{
		if(m_changed_property_name=="Node properties/type")
			m_document->interpolator()->inner_value->value = m_new_value;
		else if(m_changed_property_name=="Node properties/in seconds" || m_changed_property_name=="Node properties/in frames")
		{
			wpf_property_container^ pc = dynamic_cast<animation_node_interpolator^>(m_document->interpolator()->owner)->time;
			pc->properties[m_changed_property_name]->value = m_new_value;
		}
		else
			m_document->interpolator()->properties[m_changed_property_name]->value = m_new_value;
	}

	m_document->update_properties();
	m_document->disable_events_handling = false;
	return true;
}

void animation_setup_property_changed_command::rollback()
{
	m_document->disable_events_handling = true;
	if(m_changed_property_name=="Node properties/synchronization_group")
		m_document->container->properties[m_changed_property_name]->value = m_old_value;
	else // m_changed_property_name=="type"||"time_s"||"time_f"||"epsilon"
	{
		if(m_changed_property_name=="Node properties/type")
			m_document->interpolator()->inner_value->value = m_old_value;
		else if(m_changed_property_name=="Node properties/in seconds" || m_changed_property_name=="Node properties/in frames")
		{
			wpf_property_container^ pc = dynamic_cast<animation_node_interpolator^>(m_document->interpolator()->owner)->time;
			pc->properties[m_changed_property_name]->value = m_old_value;
		}
		else
			m_document->interpolator()->properties[m_changed_property_name]->value = m_old_value;
	}

	m_document->update_properties();
	m_document->disable_events_handling = false;
}
