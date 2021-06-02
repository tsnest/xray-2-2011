////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "time_instance.h"

using xray::animation_editor::time_instance;

time_instance::time_instance(System::Object^ parent)
{
	m_parent = parent;
	m_time = 0.3f;
	m_container = gcnew wpf_property_container();
	m_container->owner = this;
	m_container->properties->add_from_obj(this);
}

time_instance::~time_instance()
{
	delete m_container;
}

float time_instance::time_s::get()
{
	return m_time;
}

void time_instance::time_s::set(float new_val)
{
	m_time = new_val;
	property_changed("time");
}

float time_instance::time_f::get()
{
	return m_time*30;
}

void time_instance::time_f::set(float new_val)
{
	m_time = new_val/30;
	property_changed("time");
}