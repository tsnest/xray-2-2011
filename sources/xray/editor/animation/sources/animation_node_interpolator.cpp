////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_interpolator.h"
#include "time_instance.h"
#include <xray/animation/api.h>
#include <xray/animation/fermi_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/instant_interpolator.h>

using xray::animation_editor::animation_node_interpolator;
using xray::animation_editor::interpolator_type;
using namespace xray::animation;
using xray::editor::wpf_controls::float_curve_key;
using xray::editor::wpf_controls::float_curve_key_type;
using System::Windows::Point;

animation_node_interpolator::animation_node_interpolator(System::Object^ parent)
{
	m_parent = parent;
	m_time = gcnew time_instance(this);
	m_curve = gcnew float_curve();
	m_time->property_changed += gcnew Action<String^>(this, &animation_node_interpolator::reset_interpolator);
	m_epsilon = 0.005f;
	m_interpolator = NEW(fermi_interpolator)(m_time->time_s, m_epsilon);
	m_container = gcnew wpf_property_container();
	m_container->owner = this;
	m_container->properties->add_from_obj(this);
	m_container->properties["Node properties/time"]->is_browsable = true;
	m_container->properties["Node properties/epsilon"]->is_browsable = true;
}

animation_node_interpolator::~animation_node_interpolator()
{
	DELETE(m_interpolator);
	m_interpolator = NULL;
	delete m_curve;
	delete m_container;
}

void animation_node_interpolator::save(xray::configs::lua_config_value& cfg)
{
	cfg["interpolator"]["type"] = (u32)type;
	cfg["interpolator"]["time"] = m_time->time_s;
	cfg["interpolator"]["epsilon"] = m_epsilon;
}

void animation_node_interpolator::load(xray::configs::lua_config_value const& cfg)
{
	xray::configs::lua_config_value val = cfg;
	if(val["interpolator"].get_type()==xray::configs::t_table_named)
	{
		m_time->time_s = (float)val["interpolator"]["time"];
		m_epsilon = (float)val["interpolator"]["epsilon"];
		type = (interpolator_type)(u32)val["interpolator"]["type"];
	}
	else
	{
		m_time->time_s = (float)val["time"];
		m_epsilon = (float)val["epsilon"];
		type = (interpolator_type)(u32)val["interpolator"];
	}
}

interpolator_type animation_node_interpolator::type::get()
{
	if(dynamic_cast<instant_interpolator*>(m_interpolator))
		return interpolator_type(0);
	else if(dynamic_cast<linear_interpolator*>(m_interpolator))
		return interpolator_type(1);
	else
		return interpolator_type(2);
}

void animation_node_interpolator::type::set(interpolator_type new_val)
{
	set_interpolator_impl(new_val);
	property_changed("type");
}

void animation_node_interpolator::set_interpolator_impl(interpolator_type new_val)
{
	DELETE(m_interpolator);
	m_interpolator = NULL;
	if(new_val==interpolator_type::instant)
	{
		m_interpolator = NEW(instant_interpolator)();
		m_container->properties["Node properties/time"]->is_browsable = false;
		m_container->properties["Node properties/epsilon"]->is_browsable = false;
		m_curve->keys->Clear();
		m_curve->keys->Add(gcnew float_curve_key(Point(0.0f, 0.0f)));
		m_curve->keys->Add(gcnew float_curve_key(Point(0.0f, 1.0f)));
	}
	else if(new_val==interpolator_type::linear)
	{
		m_interpolator = NEW(linear_interpolator)(m_time->time_s);
		m_container->properties["Node properties/time"]->is_browsable = true;
		m_container->properties["Node properties/epsilon"]->is_browsable = false;
		m_curve->keys->Clear();
		m_curve->keys->Add(gcnew float_curve_key(Point(0.0f, 0.0f)));
		m_curve->keys->Add(gcnew float_curve_key(Point(m_time->time_f, 1.0f)));
	}
	else if(new_val==interpolator_type::fermi)
	{
		m_interpolator = NEW(fermi_interpolator)(m_time->time_s, m_epsilon);
		m_container->properties["Node properties/time"]->is_browsable = true;
		m_container->properties["Node properties/epsilon"]->is_browsable = true;
		m_curve->keys->Clear();
		m_curve->keys->Add(gcnew float_curve_key(Point(0.0f, 0.0f), float_curve_key_type::locked_auto));
		const u8 steps = 7;
		for(u8 i=1; i<steps; ++i)
		{
			float x = m_time->time_s * i / steps;
			float y = m_interpolator->interpolated_value(x);
			m_curve->keys->Add(gcnew float_curve_key(Point(x, y), float_curve_key_type::locked_auto));
		}

		m_curve->keys->Add(gcnew float_curve_key(Point(m_time->time_s, 1.0f), float_curve_key_type::locked_auto));
	}
	else
		NODEFAULT(0);
}

xray::animation_editor::animation_node_interpolator::wpf_property_container^ animation_node_interpolator::time::get()
{
	return m_time->container();
}

float animation_node_interpolator::epsilon::get()
{
	return m_epsilon;
}

void animation_node_interpolator::epsilon::set(float new_val)
{
	m_epsilon = new_val;
	set_interpolator_impl(type);
	property_changed("epsilon");
}

void animation_node_interpolator::reset_interpolator(String^ param)
{
	set_interpolator_impl(type);
	property_changed(param);
}
