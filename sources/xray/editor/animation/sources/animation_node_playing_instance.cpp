////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_playing_instance.h"
#include "time_scale_instance.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_node_clip.h"
#include "time_instance.h"

using xray::animation_editor::animation_node_playing_instance;
using xray::animation_editor::mix_self;
using xray::animation_editor::animation_viewer_graph_node_base;
using xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute;

animation_node_playing_instance::animation_node_playing_instance(animation_viewer_graph_node_base^ parent)
{
	m_parent = parent;
	m_offset = gcnew time_instance(this);
	m_mixing_type = mix_self::mix_continue_self;
	m_time_scale_inst = gcnew time_scale_instance(this);
	m_container = gcnew wpf_property_container();
	m_container->owner = this;
	m_container->properties->add_from_obj(this);
}

animation_node_playing_instance::~animation_node_playing_instance()
{
	delete m_time_scale_inst;
	delete m_container;
}

void animation_node_playing_instance::save(xray::configs::lua_config_value& cfg)
{
	cfg["offset"] = m_offset->time_s;
	cfg["mixing_type"] = (u32)m_mixing_type;
	m_time_scale_inst->save(cfg);
}

void animation_node_playing_instance::load(xray::configs::lua_config_value const& cfg)
{
	m_offset->time_s = (float)cfg["offset"];
	m_mixing_type = (mix_self)(u32)cfg["mixing_type"];
	m_time_scale_inst->load(cfg);
}

xray::animation_editor::animation_node_playing_instance::wpf_property_container^ animation_node_playing_instance::time_offset::get()
{
	return m_offset->container();
}

mix_self animation_node_playing_instance::mixing_type::get()
{
	return m_mixing_type;
}

void animation_node_playing_instance::mixing_type::set(mix_self new_val)
{
	m_mixing_type = new_val;
}

animation_node_playing_instance::wpf_property_container^ animation_node_playing_instance::time_scale::get()
{
	return m_time_scale_inst->container();
}