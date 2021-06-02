////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "time_scale_instance.h"
#include "animation_node_interpolator.h"
#include "animation_node_playing_instance.h"
#include "animation_viewer_graph_node_animation.h"

using xray::animation_editor::time_scale_instance;
using xray::editor::wpf_controls::property_descriptor;
using xray::animation_editor::animation_node_playing_instance;

time_scale_instance::time_scale_instance(animation_node_playing_instance^ parent)
{
	m_parent = parent;
	m_value = 1.0f;
	m_interpolator = gcnew animation_node_interpolator(this);
	m_container = gcnew wpf_property_container();
	m_container->owner = this;
	m_container->properties->add_from_obj(this);

	property_descriptor^ desc = interpolator->properties["Node properties/type"];
	interpolator->properties->remove(desc);
	interpolator->inner_value = desc;
}

time_scale_instance::~time_scale_instance()
{
	delete m_interpolator;
	delete m_container;
}

void time_scale_instance::save(xray::configs::lua_config_value& cfg)
{
	cfg["time_scale"] = m_value;
	m_interpolator->save(cfg);
}

void time_scale_instance::load(xray::configs::lua_config_value const& cfg)
{
	m_value = (float)cfg["time_scale"];
	m_interpolator->load(cfg);
}

float time_scale_instance::value::get()
{
	u32 parent_length = safe_cast<animation_viewer_graph_node_animation^>(m_parent->parent())->lexeme_length_in_frames();
	if(parent_length==u32(-1))
		return m_value;
	else
		return parent_length / m_value;
}

void time_scale_instance::value::set(float new_val)
{
	u32 parent_length = safe_cast<animation_viewer_graph_node_animation^>(m_parent->parent())->lexeme_length_in_frames();
	if(parent_length==u32(-1))
		m_value = new_val;
	else
		m_value = parent_length / new_val;
}

time_scale_instance::wpf_property_container^ time_scale_instance::interpolator::get()
{
	return m_interpolator->container();
}