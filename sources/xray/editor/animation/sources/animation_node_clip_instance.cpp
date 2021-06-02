////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_clip_instance.h"
#include "animation_node_clip.h"
#include "animation_node_interpolator.h"
#include <xray/animation/i_editor_animation.h>
#include "animation_node_interval.h"
#include "animation_editor_form.h"
#include <xray/animation/cubic_spline_skeleton_animation.h>

using xray::animation_editor::animation_node_clip_instance;
using xray::animation_editor::animation_node_clip;
using namespace xray::animation::mixing;
using xray::editor::wpf_controls::property_descriptor;
using namespace System::Windows::Forms;

animation_node_clip_instance::animation_node_clip_instance(animation_node_clip^ base, xray::configs::lua_config_ptr cfg) :
	m_animation_clip	( NEW(animation::skeleton_animation_ptr) )
{
	m_base = base;
	m_time_changed = false;
	m_epsilon_changed = false;
	m_type_changed = false;
	m_sg_changed = false;
	m_config = NEW(xray::configs::lua_config_ptr)();
	(*m_config) = cfg;
	xray::configs::lua_config_value val = (*m_config)->get_root()["viewer_options"];
	m_synchronization_group = (u32)val["synchronizing_group"];

	m_interpolator = gcnew animation_node_interpolator(this);
	m_interpolator->load(val);
	m_interpolator->property_changed += gcnew Action<String^>(this, &animation_node_clip_instance::reset_animation_clip);

	*m_animation_clip = m_base->animation();
	float const animation_length	= this->animation_length();
	m_old_clip_length = val.value_exists("old_clip_length")?(float)val["old_clip_length"]:animation_length;
	val["old_clip_length"] = animation_length;

	m_container = gcnew wpf_property_container();
	m_container->owner = this;
	m_container->properties->add_from_obj(this);
	property_descriptor^ desc = interpolator->properties["Node properties/type"];
	interpolator->properties->remove(desc);
	interpolator->inner_value = desc;
}

float animation_node_clip_instance::animation_length	()
{
	return animation::cubic_spline_skeleton_animation_pinned( *m_animation_clip )->length_in_frames() / animation::default_fps;
}

animation_node_clip_instance::~animation_node_clip_instance()
{
	DELETE(m_animation_clip);
	m_animation_clip = NULL;
	
	(*m_config) = NULL;
	DELETE(m_config);

	delete m_interpolator;
	delete m_container;
}

void animation_node_clip_instance::save()
{
	if(m_sg_changed || m_time_changed || m_epsilon_changed || m_type_changed)
	{
		System::String^ n = gcnew System::String(m_base->name());
		System::String^ m = "Parameters for animation["+n+"] has been changed:\n";
		if(m_sg_changed)
			m += "[synchronization group]\n";
		if(m_time_changed)
			m += "[time]\n";
		if(m_epsilon_changed)
			m += "[epsilon]\n";
		if(m_type_changed)
			m += "[interpolator type]\n";

		m += "Do you want to apply them?";
		DialogResult res = MessageBox::Show(
			m_base->form, m,
			"Animation editor",
			MessageBoxButtons::OKCancel,
			MessageBoxIcon::Question
		);
		if(res==DialogResult::OK)
		{
			m_base->save();
			m_sg_changed = false;
			m_time_changed  = false;
			m_epsilon_changed = false;
			m_type_changed = false;
		}
	}
}

pcstr animation_node_clip_instance::name()
{
	return m_base->name();
}

u32 animation_node_clip_instance::fps::get() 
{
	return 30;
};

System::String^ animation_node_clip_instance::length::get() 
{
	Single seconds = 1.0f;
	if(m_animation_clip) 
		seconds = (Single)System::Math::Round(animation_length(), 3);

	UInt32 frames = 1;
	if(m_animation_clip) 
		frames = (UInt32)System::Math::Round(animation_length() * fps );

	return frames.ToString()+" frames   "+seconds.ToString()+" seconds";
};

bool animation_node_clip_instance::additive::get()
{
	return m_base->editor_animation()->animation_type() == animation::animation_type_additive;
}

s32 animation_node_clip_instance::synchronization_group::get()
{
	return (s32)m_synchronization_group;
}

void animation_node_clip_instance::synchronization_group::set(s32 new_val)
{
	m_synchronization_group = (u32)new_val;
	xray::configs::lua_config_value val = (*m_config)->get_root()["viewer_options"];
	val["synchronizing_group"] = m_synchronization_group;
	m_sg_changed = true;
}

animation_node_clip_instance::wpf_property_container^ animation_node_clip_instance::interpolator::get()
{
	return m_interpolator->container();
}

animation_node_clip_instance::intervals_list^ animation_node_clip_instance::intervals::get()
{
	return m_base->intervals;
}

long animation_node_clip_instance::id_queried::get()
{
	return m_base->id_queried;
}
void animation_node_clip_instance::id_queried::set(long new_val)
{
	m_base->id_queried = new_val;
}

void animation_node_clip_instance::reset_animation_clip(String^ param)
{
	*m_animation_clip = m_base->animation();
	xray::configs::lua_config_value val = (*m_config)->get_root()["viewer_options"];
	m_interpolator->save(val);
	if(param=="time")
		m_time_changed = true;
	else if(param=="epsilon")
		m_epsilon_changed = true;
	else if(param=="type")
		m_type_changed = true;
}

xray::animation::skeleton_animation_ptr animation_node_clip_instance::animation() 
{
	return m_base->animation();
}

xray::animation::i_editor_animation* animation_node_clip_instance::editor_animation()
{
	return m_base->editor_animation();
}

xray::animation_editor::animation_node_interval^ animation_node_clip_instance::get_interval_by_offset_and_length(float offset, float length)
{
	for each(animation_node_interval^ interval in intervals)
	{
		if(xray::math::is_similar(interval->offset, offset) && xray::math::is_similar(interval->length, length))
			return interval;
	}

	return (gcnew animation_node_interval(m_base, u32(-1), offset, length, u32(-1)));
}
