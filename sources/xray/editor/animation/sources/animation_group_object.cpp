////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_group_object.h"
#include "animation_node_clip.h"
#include "animation_node_clip_instance.h"
#include "animation_node_interval.h"
#include "animation_node_interval_instance.h"
#include "animation_groups_document.h"
#include "animation_groups_editor.h"
#include "animation_editor.h"

using xray::animation_editor::animation_group_object;
using xray::animation_editor::animation_groups_document;
using xray::animation_editor::animation_groups_gait;
using xray::animation_editor::animation_groups_flag;
using xray::animation_editor::animation_node_interval;
using xray::animation_editor::animation_node_interval_instance;

animation_group_object::animation_group_object(animation_groups_document^ d)
{
	m_parent = d;
	m_id = Guid::NewGuid();
	m_name = "new_group";
	m_is_expanded = true;
	m_path = m_name+extension;
	m_view = nullptr;
	m_gait = animation_groups_gait::stand;
	m_flag = animation_groups_flag::idle;
	m_position = System::Windows::Point(0.0f, 0.0f);
	m_intervals = gcnew intervals_list();
	m_intervals->CollectionChanged += gcnew System::Collections::Specialized::NotifyCollectionChangedEventHandler(this, &animation_group_object::intervals_collection_changed);
	m_request = gcnew intervals_request_type();
}

animation_group_object::~animation_group_object()
{
	m_view = nullptr;
	delete m_intervals;
	m_request->Clear();
	delete m_request;
}

void animation_group_object::save(xray::configs::lua_config_value& cfg)
{
	String^ id_str = m_id.ToString();
	unmanaged_string id_unm_str(id_str);
	cfg["id"] = id_unm_str.c_str();
	unmanaged_string name_unm_str(m_name);
	cfg["name"] = name_unm_str.c_str();
	cfg["position"] = float2((float)position.X, (float)position.Y);
	cfg["is_expanded"] = m_view?m_view->is_expanded:m_is_expanded;
}

void animation_group_object::save_group_settings(xray::configs::lua_config_value& cfg)
{
	unmanaged_string flag_unm_str(m_flag.ToString());
	cfg["flags"] = flag_unm_str.c_str();
	unmanaged_string gait_unm_str(m_gait.ToString());
	cfg["gait"] = gait_unm_str.c_str();
	cfg["mix_time"] = m_mix_time;
	groups_editor_node_above_header_content^ cont = safe_cast<groups_editor_node_above_header_content^>(m_view->above_header_content);
	cfg["value"] = float(cont->up_down->Value);
	for(int index=0; index<m_intervals->Count; ++index)
	{
		String^ anim_path = animation_editor::single_animations_path + "/" + gcnew String( get_interval( index )->parent->name( ) );
		unmanaged_string path(anim_path);
		cfg["animations"][index]["animation"] = path.c_str();
		cfg["animations"][index]["id"] = get_interval(index)->id;
		if(!xray::math::is_similar(m_intervals[index]->max_weight, 1.0f))
			cfg["animations"][index]["max_weight"] = m_intervals[index]->max_weight;
		if(!xray::math::is_similar(m_intervals[index]->min_weight, 0.0f))
			cfg["animations"][index]["min_weight"] = m_intervals[index]->min_weight;
	}
}

void animation_group_object::load(xray::configs::lua_config_value const& cfg)
{
	String^ id_str = gcnew String((pcstr)cfg["id"]);
	m_id = Guid::Guid(id_str);
	m_name = gcnew String((pcstr)cfg["name"]);
	m_path = m_name+extension;
	float2 pos = (float2)cfg["position"];
	m_position.X = pos.x;
	m_position.Y = pos.y;
	m_is_expanded = (cfg.value_exists("is_expanded"))?(bool)cfg["is_expanded"]:true;
}

void animation_group_object::load_group_settings(xray::configs::lua_config_value const& cfg)
{
	String^ flag_str = gcnew String((pcstr)cfg["flags"]);
	m_flag = safe_cast<animation_groups_flag>(System::Enum::Parse(animation_groups_flag::typeid, flag_str));
	String^ gait_str = gcnew String((pcstr)cfg["gait"]);
	m_gait = safe_cast<animation_groups_gait>(System::Enum::Parse(animation_groups_gait::typeid, gait_str));
	m_mix_time = (float)cfg["mix_time"];
	if(m_view!=nullptr && cfg.value_exists("value"))
	{
		m_parent->disable_events_handling = true;
		groups_editor_node_above_header_content^ cont = safe_cast<groups_editor_node_above_header_content^>(m_view->above_header_content);
		cont->up_down->Value = (float)cfg["value"];
		m_parent->disable_events_handling = false;
	}

	for(u32 i=0; i<cfg["animations"].size(); ++i)
	{
		String^ clip_name = gcnew String((pcstr)cfg["animations"][i]["animation"]);
		if( clip_name->Contains( animation_editor::single_animations_path ) )
			clip_name = clip_name->Remove(0, animation_editor::single_animations_path->Length + 1 );

		intervals_request rq;
		rq.cfg_interval_id = (u32)cfg["animations"][i]["id"];
		rq.max_weight = (cfg["animations"][i].value_exists("max_weight"))?(float)cfg["animations"][i]["max_weight"]:1.0f;
		rq.min_weight = (cfg["animations"][i].value_exists("min_weight"))?(float)cfg["animations"][i]["min_weight"]:0.0f;
		rq.interval_id = i;
		if(!m_request->ContainsKey(clip_name))
			m_request->Add(clip_name, gcnew List<intervals_request>());

		m_request[clip_name]->Add(rq);
	}

	if(m_request->Count>0)
	{
		animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_group_object::on_animation_loaded);
		intervals_request_type^ r = gcnew intervals_request_type(m_request);
		for each(KeyValuePair<String^, List<intervals_request>^> p in r)
			m_parent->get_editor()->get_animation_editor()->request_animation_clip(p.Key, cb);
	}
}

String^ animation_group_object::source_path::get( )
{
	return animation_editor::animation_controllers_path + "/groups/";
};

void animation_group_object::on_animation_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	if(new_clip==nullptr)
		return;

	String^ clip_name = gcnew String(new_clip->name());
	R_ASSERT(m_request->ContainsKey(clip_name));
	for each(intervals_request p in m_request[clip_name])
	{
		animation_node_interval^ interval = new_clip->intervals[p.cfg_interval_id];
		animation_node_interval_instance^ inst = gcnew animation_node_interval_instance(interval, p.max_weight, p.min_weight);
		inst->property_changed += gcnew Action<animation_node_interval_instance^, String^, Object^, Object^>(this, &animation_group_object::interval_property_changed);
		if(p.interval_id>(u32)m_intervals->Count)
			m_intervals->Add(inst);
		else
			m_intervals->Insert(p.interval_id, inst);
	}
	
	m_request->Remove(clip_name);
	refresh_view();
}

void animation_group_object::interval_property_changed(animation_node_interval_instance^ interval, System::String^ property_name, System::Object^ new_val, System::Object^ old_val)
{
	if(!m_parent->disable_events_handling)
		m_parent->change_interval_property(m_id.ToString(), m_intervals->IndexOf(interval), property_name, new_val, old_val);
}

System::Windows::Point animation_group_object::position::get()
{
	if(m_view!=nullptr)
		return m_view->position;
	else
		return m_position;
}

void animation_group_object::position::set(System::Windows::Point new_val)
{
	if(m_view!=nullptr)
		m_view->position = new_val;
	else
		m_position = new_val;
}

void animation_group_object::name::set(String^ new_val)
{
	if(m_parent->disable_events_handling)
	{
		m_name = new_val;
		path = new_val;
		if(m_view)
		{
			m_view->name = m_name;
			refresh_view();
		}
	}
	else
		m_parent->change_group_property(m_id.ToString(), "name", new_val, m_name);
}

void animation_group_object::gait::set(animation_groups_gait new_val) 
{
	if(m_parent->disable_events_handling)
		m_gait = new_val;
	else
		m_parent->change_group_property(m_id.ToString(), "gait", new_val, m_gait);
}

void animation_group_object::flag::set(animation_groups_flag new_val) 
{
	if(m_parent->disable_events_handling)
		m_flag = new_val;
	else
		m_parent->change_group_property(m_id.ToString(), "flag", new_val, m_flag);
}

void animation_group_object::mix_time::set(float new_val)
{
	if(m_parent->disable_events_handling)
		m_mix_time = new_val;
	else
		m_parent->change_group_property(m_id.ToString(), "mix_time", new_val, m_mix_time);
}

void animation_group_object::view::set(properties_node^ n)
{
	m_view = n;
	m_view->id = m_id.ToString();
	m_view->name = m_name;
	m_view->is_expanded = m_is_expanded;
	refresh_view();
	m_view->PropertyChanged += gcnew System::ComponentModel::PropertyChangedEventHandler(this, &animation_group_object::view_position_changed);
}

void animation_group_object::view_position_changed(System::Object^, System::ComponentModel::PropertyChangedEventArgs^ e)
{
	if(e->PropertyName=="position")
		refresh_view();
}

void animation_group_object::refresh_view()
{
	if(m_view!=nullptr)
		m_view->update_properties();
}

void animation_group_object::intervals_collection_changed(System::Object^, NotifyCollectionChangedEventArgs^ e)
{
	if(e->Action==NotifyCollectionChangedAction::Remove && m_view && !m_parent->disable_events_handling)
		m_parent->remove_group_intervals(m_view, e->OldItems);
}

animation_node_interval^ animation_group_object::get_interval(u32 index)
{
	if(index<(u32)m_intervals->Count)
		return m_intervals[index]->interval;

	return nullptr;
}
