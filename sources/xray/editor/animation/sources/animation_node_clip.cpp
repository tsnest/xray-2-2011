////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_node_clip.h"
#include "animation_node_interpolator.h"
#include <xray/animation/i_editor_animation.h>
#include "animation_node_interval.h"
#include "animation_node_clip_instance.h"
#include <xray/animation/anim_track_common.h>

using xray::animation_editor::animation_node_clip;
using xray::animation_editor::animation_node_clip_instance;
using namespace xray::animation::mixing;
using xray::editor::wpf_controls::property_descriptor;
using xray::animation_editor::animation_editor_form;

animation_node_clip::animation_node_clip(System::String^ name, 
										 xray::animation::skeleton_animation_ptr anim, 
										 xray::configs::lua_config_ptr cfg,
										 animation_editor_form^ f
										 )
{
	m_form = f;
	m_name = strings::duplicate(g_allocator, unmanaged_string(name).c_str());
	m_animation = xray::animation::create_editor_animation(*g_allocator, anim);
	m_config = NEW(xray::configs::lua_config_ptr)();
	(*m_config) = cfg;
	m_intervals = gcnew intervals_list();
	load_intervals();
}

animation_node_clip::~animation_node_clip()
{
	FREE(m_name);
	m_name = NULL;
	(*m_config) = NULL;
	DELETE(m_config);
	if(m_animation)
	{
		xray::animation::i_editor_animation* anim = m_animation;
		xray::animation::destroy_editor_animation(*g_allocator, anim);
	}

	m_animation = NULL;
}

void animation_node_clip::save()
{
	(*m_config)->save(configs::target_sources);
}

void animation_node_clip::load_intervals()
{
	m_intervals->Clear();
	bool anim_intervals_present = false;
	xray::configs::lua_config_iterator b = m_animation->config_value().begin();
	for(; b!=m_animation->config_value().end(); ++b)
	{
		if(xray::strings::compare((pcstr)(*b)["name"], "anim_intervals")==0)
		{
			anim_intervals_present = true;
			u32 id = 0;
			bool domains_exists = (*b).value_exists("domains");
			xray::configs::lua_config_iterator d_b = (*b)["knots"].begin();
			if(domains_exists)
				d_b = (*b)["domains"].begin();

			xray::configs::lua_config_iterator knots_b = (*b)["knots"].begin();
			float previous_knot = (float)(*knots_b) / animation::default_fps;
			float current_knot = previous_knot;
			++knots_b; // skip first zero
			for(; knots_b!=(*b)["knots"].end(); ++knots_b)
			{
				current_knot = (float)(*knots_b) / animation::default_fps;
				float partition_length = current_knot - previous_knot;
				u32 partition_type = (u32)xray::editor::wpf_controls::animation_setup::animation_channel_partition_type::undefined;
				if(domains_exists)
				{
					partition_type = (u32)(*d_b);
					++d_b;
				}

				m_intervals->Add(gcnew animation_node_interval(this, id, previous_knot, partition_length, partition_type));
				previous_knot = current_knot;
				++id;
			}

			break;
		}
	}

	if(!anim_intervals_present)
		m_intervals->Add(gcnew animation_node_interval(this, 0, 0.0f, m_animation->length(), u32(-1)));
}

animation_node_clip_instance^ animation_node_clip::make_instance()
{
	xray::configs::lua_config_ptr new_cfg = xray::configs::create_lua_config();
	new_cfg->add_super_table((*m_config)->get_root());
	return gcnew animation_node_clip_instance(this, new_cfg);
}

xray::animation::skeleton_animation_ptr animation_node_clip::animation() 
{
	return m_animation->animation();
}

xray::animation::i_editor_animation* animation_node_clip::editor_animation()
{
	return m_animation;
}

float animation_node_clip::length()
{
	return m_animation->length();
}