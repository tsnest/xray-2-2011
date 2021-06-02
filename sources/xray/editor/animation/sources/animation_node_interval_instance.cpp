////////////////////////////////////////////////////////////////////////////
//	Created		: 25.05.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_interval_instance.h"
#include "animation_node_interval.h"
#include "animation_node_clip.h"

using xray::animation_editor::animation_node_interval_instance;
using xray::animation_editor::animation_node_interval;

animation_node_interval_instance::animation_node_interval_instance(animation_node_interval^ interval, float max_weight, float min_weight)
:m_interval(interval),
m_max_weight(max_weight),
m_min_weight(min_weight)
{
}

animation_node_interval_instance::~animation_node_interval_instance()
{
}

void animation_node_interval_instance::max_weight::set(float new_val)
{
	property_changed(this, "max_weight", new_val, m_max_weight);
	m_max_weight = new_val;
}

void animation_node_interval_instance::min_weight::set(float new_val)
{
	property_changed(this, "min_weight", new_val, m_min_weight);
	m_min_weight = new_val;
}

System::String^ animation_node_interval_instance::ToString()
{
	return gcnew System::String(m_interval->parent->name());
}