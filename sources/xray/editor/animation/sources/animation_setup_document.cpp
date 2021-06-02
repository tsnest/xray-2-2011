////////////////////////////////////////////////////////////////////////////
//	Created		: 12.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_setup_document.h"
#include "animation_setup_manager.h"
#include "animation_node_interpolator.h"
#include "animation_setup_channel_added_removed_commands.h"
#include "animation_node_clip_instance.h"
#include "animation_editor.h"
#include <xray/animation/i_editor_animation.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include <xray/timing_timer.h>
#include <xray/strings_stream.h>

using xray::animation_editor::animation_setup_document;
using xray::animation_editor::animation_setup_manager;
using namespace xray::animation;
using namespace System;
using namespace System::Collections;
using xray::editor_base::command_engine;
using xray::editor::wpf_controls::property_descriptor;

animation_setup_document::animation_setup_document(animation_setup_manager^ m)
:super(m->multidocument_base),
m_setup_manager(m),
m_clip(nullptr),
m_current_time_in_ms(0)
{
	m_timer = NEW(xray::timing::timer)();
	m_command_engine = gcnew command_engine(100);
	disable_events_handling = false;
	m_spin_is_updating = false;
	m_paused = true;
	m_save_first_as_last = true;
	play_from_beginning = true;
	follow_animation_thumb = true;
	snap_to_frames = true;
	m_max_time = 0.0f;
	m_time_scale = 1.0f;
	m_panel_fps = 30;
	m_ms_in_frame = 1000.0f/m_panel_fps*m_time_scale;
	init_components();
}

animation_setup_document::~animation_setup_document()
{
	DELETE(m_timer);
	delete m_clip;
	delete m_command_engine;
	if(components)
		delete components;
}

void animation_setup_document::save()
{
	if(is_saved)
		return;

	save_all_channels();
	m_clip->save();

	String^ file_path = animation_editor::single_animations_path + "/" + Name + ".clip_data";
	unmanaged_string path = unmanaged_string(file_path);
	m_clip->editor_animation()->save_config_value(path.c_str());
	m_setup_manager->document_saved(Name);
	m_command_engine->set_saved_flag();
	is_saved = true;
	Invalidate(false);
}

void animation_setup_document::save_all_channels()
{
	xray::configs::lua_config_value val = m_clip->editor_animation()->config_value();
	val.clear();
	for each(animation_channel^ ch in m_host->panel->channels)
	{
		xray::configs::lua_config_value v = val[m_host->panel->channels->IndexOf(ch)];
		save_channel(ch, v);
	}
}

void animation_setup_document::save_channel(System::String^ channel_name)
{
	animation_channel^ ch = find_channel(channel_name);
	if(ch!=nullptr)
	{
		xray::configs::lua_config_value val = m_clip->editor_animation()->config_value()[m_host->panel->channels->IndexOf(ch)];
		save_channel(ch, val);
	}
}

void animation_setup_document::save_channel(animation_channel^ ch, xray::configs::lua_config_value& cfg)
{
	if(ch->objects->Count==0)
		return;

	unmanaged_string name_str(ch->name);
	cfg["name"] = name_str.c_str();
	cfg["type"] = (u8)ch->type;
	switch(ch->type)
	{
	case animation_channel_type::intervals:
		{
			u32 knots_index = 0;
			for each(animation_channel_interval^ i in ch->objects)
			{
				float start_time = (float)System::Math::Round(i->start_time / m_host->panel->time_layout_scale / 1000.0f * m_panel_fps);
				float interval_length = (float)System::Math::Round(i->length / m_host->panel->time_layout_scale / 1000.0f * m_panel_fps);
				cfg["knots"][knots_index] = start_time;
				cfg["domains"][knots_index] = i->type;
				++knots_index;
				cfg["knots"][knots_index] = start_time+interval_length;
				if(xray::math::floor(knots_index/2 + 1.0f)!=ch->objects->Count)
					cfg["domains"][knots_index] = u8(-1);

				++knots_index;
			}
			break;
		}
	case animation_channel_type::partitions:
		{
			for each(animation_channel_partition^ p in ch->objects)
			{
				if(p->type==animation_channel_partition_type::undefined)
				{
					MessageBox::Show(this, "Channel with name ["+ch->name+"] has undefined interval type!", "Animation setup manager", MessageBoxButtons::OK, MessageBoxIcon::Error);
					return;
				}
			}

			u32 config_index = 0;
			float summary_length = 0;
			float const koeff = 1 / m_host->panel->time_layout_scale / 1000.0f * m_panel_fps;
			animation_channel_partition^ first_part = safe_cast<animation_channel_partition^>(ch->objects[0]);
			animation_channel_partition^ last_part = safe_cast<animation_channel_partition^>(ch->objects[ch->objects->Count-1]);
			bool save_first_as_last = (ch->objects->Count>2 && first_part->type==last_part->type);
			if(save_first_as_last && ch->name=="anim_intervals")
				save_first_as_last = m_save_first_as_last;

			u32 partitions_count = ch->objects->Count;
			if(save_first_as_last)
			{
				summary_length = (float)System::Math::Round(first_part->length * koeff);
				--partitions_count;
			}

			cfg["knots"][config_index] = summary_length;
			++config_index;

			u32 i = (save_first_as_last) ? 1 : 0;
			for(; i<partitions_count; ++i, ++config_index)
			{
				animation_channel_partition^ p = safe_cast<animation_channel_partition^>(ch->objects[i]);
				summary_length += (float)System::Math::Round(p->length * koeff);
				cfg["knots"][config_index] = summary_length;
				cfg["domains"][config_index-1] = (u32)p->type;
			}

			if(save_first_as_last)
			{
				summary_length += (float)System::Math::Round(last_part->length * koeff);
				summary_length += (float)System::Math::Round(first_part->length * koeff);
				cfg["knots"][config_index] = summary_length;
				cfg["domains"][config_index-1] = (u32)last_part->type;
			}

			break;
		}
	case animation_channel_type::events:
		{
			u32 index = 0;
			for each(animation_channel_event^ e in ch->objects)
			{
				float time = (float)System::Math::Round(e->time / m_host->panel->time_layout_scale / 1000.0f * m_panel_fps);
				cfg["knots"][index] = time;
				++index;
			}
			break;
		}
	default:
		NODEFAULT();
	}
}

void animation_setup_document::load()
{
	animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_setup_document::on_resources_loaded);
	m_setup_manager->get_editor()->request_animation_clip(Name, cb);
}

void animation_setup_document::on_resources_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	if(new_clip==nullptr)
		return;

	m_clip = new_clip;
	m_max_time = m_clip->animation_length() * 1000.0f;
	m_max_time_s_spin->Value = System::Decimal(m_max_time/1000.0f);
	m_max_time_f_spin->Value = System::Decimal(m_max_time/1000.0f)*m_fps_spin->Value;
	m_cur_time_s_spin->Maximum = m_max_time_s_spin->Value;
	m_cur_time_f_spin->Maximum = m_max_time_f_spin->Value;
	m_host->panel->animation_length = m_max_time;

	m_setup_manager->show_properties(m_clip->container());

	set_buttons_enable(true);
	set_target();
	if(m_clip->editor_animation())
	{
		xray::configs::lua_config_iterator b = m_clip->editor_animation()->config_value().begin();
		for(; b!=m_clip->editor_animation()->config_value().end(); ++b)
			load_channel(*b);
	}

	m_command_engine->set_saved_flag();
	Invalidate(false);
}

void animation_setup_document::load_channel(xray::configs::lua_config_value const& cfg)
{
	String^ channel_name = gcnew String((pcstr)cfg["name"]);
	animation_channel_type channel_type = (animation_channel_type)((u32)cfg["type"]);
	animation_channel^ ch = gcnew animation_channel(channel_name, m_max_time, channel_type);
	float old_max_time = m_clip->old_clip_length * 1000.0f;
	switch(channel_type)
	{
	case animation_channel_type::intervals:
		{
			typedef ObjectModel::ObservableCollection<animation_channel_item^> intervals_collection;
			intervals_collection^ intervals = gcnew intervals_collection();
			bool domains_exists = cfg.value_exists("domains");
			xray::configs::lua_config_iterator d_b = cfg["knots"].begin();
			if(domains_exists)
				d_b = cfg["domains"].begin();

			R_ASSERT((cfg["knots"].size()%2 == 0));
			xray::configs::lua_config_iterator b = cfg["knots"].begin();
			for(; b!=cfg["knots"].end(); ++b)
			{
				float start_time = (float)(*b) * 1000.0f / m_panel_fps;
				++b;
				float interval_length = (float)(*b) * 1000.0f / m_panel_fps - start_time;
				if((old_max_time - start_time+interval_length) < xray::math::epsilon_5)
					continue;

				u32 interval_type = 0;
				if(domains_exists)
				{
					interval_type = (u32)(*d_b);
					++d_b;
					if(d_b!=cfg["domains"].end())
						++d_b;
				}

				intervals->Add(gcnew animation_channel_interval(ch, start_time, interval_length, interval_type));
			}

			ch->objects = intervals;
			break;
		}
	case animation_channel_type::partitions:
		{
			typedef ObjectModel::ObservableCollection<animation_channel_item^> partitions_collection;
			partitions_collection^ partitions = gcnew partitions_collection();
			bool domains_exists = cfg.value_exists("domains");
			xray::configs::lua_config_iterator d_b = cfg["knots"].begin();
			if(domains_exists)
				d_b = cfg["domains"].begin();

			xray::configs::lua_config_iterator b = cfg["knots"].begin();
			float previous_knot = (float)(*b) * 1000.0f / m_panel_fps;
			float current_knot = previous_knot;
			++b; // skip first zero
			for(; b!=cfg["knots"].end(); ++b)
			{
				current_knot = (float)(*b) * 1000.0f / m_panel_fps;
				float partition_length = current_knot - previous_knot;

				animation_channel_partition_type partition_type = animation_channel_partition_type::none;
				if(domains_exists)
				{
					partition_type = (animation_channel_partition_type)(u8(*d_b));
					++d_b;
				}

				if((current_knot - old_max_time) > xray::math::epsilon_3)
				{
					R_ASSERT_CMP(current_knot - old_max_time, >=, 0.0f);
					partitions->Insert(0, gcnew animation_channel_partition(ch, current_knot - old_max_time, partition_type));
					partition_length = old_max_time - previous_knot;
				}

				R_ASSERT_CMP(partition_length, >=, 0.0f);
				partitions->Add(gcnew animation_channel_partition(ch, partition_length, partition_type));
				previous_knot = current_knot;
			}

			if((m_max_time - old_max_time) > 500.0f / m_panel_fps)
			{
				R_ASSERT_CMP(m_max_time - old_max_time, >=, 0.0f);
				partitions->Add(gcnew animation_channel_partition(ch, m_max_time - old_max_time, animation_channel_partition_type::undefined));
			}
			else if((old_max_time - m_max_time) > 500.0f / m_panel_fps)
			{
				float new_length = safe_cast<float>(partitions[partitions->Count-1]->get_property("length")) - old_max_time + m_max_time;
				while(new_length<0.0f)
				{
					partitions->Remove(partitions[partitions->Count-1]);
					new_length += safe_cast<float>(partitions[partitions->Count-1]->get_property("length"));
				}

				R_ASSERT_CMP(new_length, >=, 0.0f);
				partitions[partitions->Count-1]->change_property("length", new_length);
			}

			ch->objects = partitions;
			break;
		}
	case animation_channel_type::events:
		{
			typedef ObjectModel::ObservableCollection<animation_channel_item^> events_collection;
			events_collection^ events = gcnew events_collection();
			if(!cfg.value_exists("knots"))
				break;

			xray::configs::lua_config_iterator b = cfg["knots"].begin();
			for(; b!=cfg["knots"].end(); ++b)
			{
				float time = (float)(*b) * 1000.0f / m_panel_fps;
				if(time>old_max_time)
					break;

				u32 event_type = 0;
				events->Add(gcnew animation_channel_event(ch, time, event_type));
			}

			ch->objects = events;
			break;
		}
	default:
		NODEFAULT();
	}

	m_host->panel->add_channel(ch);
}

void animation_setup_document::undo()
{
	m_command_engine->rollback(-1);
	Invalidate(false);
}

void animation_setup_document::redo()
{
	m_command_engine->rollback(1);
	Invalidate(false);
}

void animation_setup_document::select_all()
{
}

void animation_setup_document::copy(bool del)
{
	XRAY_UNREFERENCED_PARAMETER(del);
}

void animation_setup_document::paste()
{
}

void animation_setup_document::del()
{
}

void animation_setup_document::set_target()
{
	if(!m_clip)
		return;

	mutable_buffer buffer(ALLOCA(4096), 4096);
	animation_node_interpolator^ interpolator = safe_cast<animation_node_interpolator^>(m_clip->interpolator->owner);
	mixing::animation_lexeme l(
		mixing::animation_lexeme_parameters(
			buffer,
			m_clip->name(),
			m_clip->animation()
		).weight_interpolator( *interpolator->interpolator() )
	);
	m_setup_manager->set_target(l, (u32)System::Math::Round(m_current_time_in_ms));
}

void animation_setup_document::tick()
{
	if(!m_clip)
		return;

	if(!paused)
	{
		float elapsed_ms_delta = m_timer->get_elapsed_msec() * m_time_scale;
		current_time_in_ms += elapsed_ms_delta;
		m_timer->start();
	}
}

animation_channel^ animation_setup_document::find_channel(System::String^ name)
{
	for each(animation_channel^ ch in m_host->panel->channels)
	{
		if(ch->name==name)
			return ch;
	}

	return nullptr;
}

animation_channel_item^ animation_setup_document::find_item(animation_channel^ ch, System::Guid^ id)
{
	for each(animation_channel_item^ i in ch->objects)
	{
		if(i->id.Equals(id))
			return i;
	}

	return nullptr;
}

void animation_setup_document::update_properties()
{
	m_clip->container()->update_properties();
	m_setup_manager->show_properties(m_clip->container());
}

void animation_setup_document::on_channel_copy(System::Object^, setup_panel_copy_event_args^ e)
{
	xray::configs::lua_config_ptr cfg = xray::configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	xray::configs::lua_config_value val = cfg->get_root()["data"]["event_channels"][0];
	animation_channel^ ch = find_channel(e->channel_name);
	save_channel(ch, val);
	cfg->save(stream);
	stream.append("%c", 0);
	xray::os::copy_to_clipboard((pcstr)stream.get_buffer());
}

void animation_setup_document::on_channel_paste(System::Object^, System::EventArgs^)
{
	pstr string	= xray::os::paste_from_clipboard(*g_allocator);
	if(!string)
		return;

	R_ASSERT(m_clip);
	u32 frames = (UInt32)System::Math::Round(m_clip->animation_length()*m_clip->fps);
	is_saved &= !m_command_engine->run(gcnew animation_setup_paste_channel_command(this, string, frames));
	FREE(string);
}

void animation_setup_document::on_channel_cut(System::Object^ obj, setup_panel_copy_event_args^ e)
{
	on_channel_copy(obj, e);
	setup_panel_event_args^ new_e = gcnew setup_panel_event_args(e->channel_name, "", "", nullptr);
	is_saved &= !m_command_engine->run(gcnew animation_setup_channel_removed_command(this, new_e));
}