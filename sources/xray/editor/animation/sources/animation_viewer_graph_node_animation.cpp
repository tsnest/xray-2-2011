////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_node_clip_instance.h"
#include "animation_node_clip.h"
#include "animation_node_interval.h"
#include "animation_node_playing_instance.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_hypergraph.h"
#include "viewer_editor.h"
#include "viewer_document.h"
#include "animation_editor.h"
#include "animation_setup_manager.h"

#include <xray/animation/anim_track_common.h>

using xray::animation_editor::animation_viewer_graph_node_animation;
using xray::editor::wpf_controls::property_descriptor;
using xray::editor::wpf_controls::property_editors::attributes::combo_box_items_attribute;
using xray::animation_editor::animation_ui_type_lexeme_editor;
using xray::animation_editor::animation_viewer_hypergraph;
using xray::animation_editor::animation_node_clip_instance;
////////////////////////////////////////////////////////////////////////////
// ref class animation_viewer_graph_node_animation /////////////////////////
////////////////////////////////////////////////////////////////////////////
void animation_viewer_graph_node_animation::initialize()
{
	super::initialize();
	m_name = "";
	m_intervals = gcnew intervals_list();
	m_request = gcnew intervals_request_type();
	m_playing_instance = gcnew animation_node_playing_instance(this);
	m_clips = gcnew instances_list();
	sync_node = nullptr;
	cycle_from_interval_id = u32(-1);

	m_container = gcnew wpf_property_container();
	m_container->owner = this;
}

animation_viewer_graph_node_animation::~animation_viewer_graph_node_animation()
{
	for each(animation_node_clip_instance^ inst in m_clips)
		delete inst;

	delete m_clips;
	delete m_intervals;
	delete m_container;
	delete m_playing_instance;
}

void animation_viewer_graph_node_animation::draw_caption(System::Windows::Forms::PaintEventArgs^ e)
{
	System::Drawing::Size sz = m_style->get_max_point_size();
	sz.Height = int(Math::Round(sz.Height*m_style->scale));
	sz.Width = int(Math::Round(sz.Width*m_style->scale));
	System::Drawing::Rectangle client = ClientRectangle;
	if(m_has_top)
	{
		client.Height -= int(Math::Round(VERTICAL_TAB_HEIGHT*m_style->scale));
		client.Y += int(Math::Round(VERTICAL_TAB_HEIGHT*m_style->scale));
	}

	if(m_has_bottom)
		client.Height -= int(Math::Round(VERTICAL_TAB_HEIGHT*m_style->scale));

	System::Drawing::Rectangle rect = Rectangle::Inflate(client, -sz.Width/2, -sz.Height/2);
	rect.Height /= 2;
	m_style->border_pen->Width = 1;

	m_style->brush->Color = Drawing::Color::Green;
	System::String^ interval = (m_intervals->Count>0)? m_intervals[0]->type.ToString() : "";
	e->Graphics->DrawString(interval, m_style->font, m_style->brush, rect, m_style->fmt);

	rect.Location = System::Drawing::Point(rect.Location.X, rect.Location.Y+rect.Height);
	m_style->brush->Color = Drawing::Color::Black;
	e->Graphics->DrawString(get_text(), m_style->font, m_style->brush, rect, m_style->fmt);
}

void animation_viewer_graph_node_animation::save(xray::configs::lua_config_value cfg)
{
	super::save(cfg);
	cfg["type"] = 1;
	for(int index=0; index<m_intervals->Count; ++index)
	{
		animation_node_interval^ interval = m_intervals[index];
		cfg["intervals"][index]["animation"] = interval->parent->name();
		if(interval->id!=u32(-1))
			cfg["intervals"][index]["interval_id"] = interval->id;
		else
		{
			cfg["intervals"][index]["offset"] = m_intervals[index]->offset;
			cfg["intervals"][index]["length"] = m_intervals[index]->length;
		}
	}
	
	cfg["cycle_from_interval_id"] = cycle_from_interval_id;
	for each(animation_node_clip_instance^ inst in m_clips)
		inst->save();

	m_playing_instance->save(cfg);
}

void animation_viewer_graph_node_animation::load(xray::configs::lua_config_value const& cfg, animation_viewer_hypergraph^ h)
{
	super::load(cfg, h);
	if(cfg.value_exists("intervals"))
	{
		for(u32 i=0; i<cfg["intervals"].size(); ++i)
		{
			String^ clip_name = gcnew String((pcstr)cfg["intervals"][i]["animation"]);
			if( clip_name->Contains( animation_editor::single_animations_path ) )
				clip_name = clip_name->Remove( 0, animation_editor::single_animations_path->Length + 1 );

			intervals_request rq;
			rq.cfg_interval_id = u32(-1);
			if(cfg["intervals"][i].value_exists("interval_id"))
				rq.cfg_interval_id = (u32)cfg["intervals"][i]["interval_id"];
			else
			{
				rq.offset = (float)cfg["intervals"][i]["offset"];
				rq.length = (float)cfg["intervals"][i]["length"];
			}

			rq.interval_id = i;
			if(!m_request->ContainsKey(clip_name))
				m_request->Add(clip_name, gcnew List<intervals_request>());

			m_request[clip_name]->Add(rq);
		}
	}

	if(cfg.value_exists("cycle_from_interval_id"))
		cycle_from_interval_id = (u32)cfg["cycle_from_interval_id"];

	m_playing_instance->load(cfg);
}

void animation_viewer_graph_node_animation::on_added(xray::editor::controls::hypergraph::hypergraph_control^ parent)
{
	super::on_added(parent);
	request_animation();
	init_properties(sync_node);
}

void animation_viewer_graph_node_animation::request_animation()
{
	if(m_request->Count>0)
	{
		animation_viewer_hypergraph^ h = safe_cast<animation_viewer_hypergraph^>(m_parent);
		if(h!=nullptr) 
		{
			animation_clip_request_callback^ cb = gcnew animation_clip_request_callback(this, &animation_viewer_graph_node_animation::on_animation_loaded);
			intervals_request_type^ r = gcnew intervals_request_type(m_request);
			for each(KeyValuePair<String^, List<intervals_request>^> p in r)
				h->get_editor()->get_animation_editor()->request_animation_clip(p.Key, cb);
		}
	}
	else
		init_properties(nullptr);
}

void animation_viewer_graph_node_animation::on_animation_loaded(xray::animation_editor::animation_node_clip_instance^ new_clip)
{
	if(new_clip==nullptr)
		return;

	m_clips->Add(new_clip);
	String^ clip_name = gcnew String(new_clip->name());
	R_ASSERT(m_request->ContainsKey(clip_name));
	for each(intervals_request p in m_request[clip_name])
	{
		animation_node_interval^ interval = nullptr;
		if(p.cfg_interval_id==u32(-1))
			interval = new_clip->get_interval_by_offset_and_length(p.offset, p.length);
		else
		{
			R_ASSERT((u32)new_clip->intervals->Count>p.cfg_interval_id);
			interval = new_clip->intervals[p.cfg_interval_id];
		}

		if(interval!=nullptr)
		{
			if(p.interval_id>(u32)m_intervals->Count)
				m_intervals->Add(interval);
			else
				m_intervals->Insert(p.interval_id, interval);
		}
	}
	
	m_request->Remove(clip_name);
	Invalidate(false);
}

void animation_viewer_graph_node_animation::init_properties(animation_viewer_graph_node_animation^ new_sync_node)
{
	sync_node = new_sync_node;
	animation_viewer_hypergraph^ h = safe_cast<animation_viewer_hypergraph^>(m_parent);
	h->get_editor()->show_properties(nullptr);
	m_container->properties->clear();
	m_container->properties->add_from_obj(this, "animation_clip");
	m_container->properties->add_from_obj(this, "intervals");
	m_container->properties->add_from_obj(this, "playing_instance");

	property_descriptor^ desc = m_container->properties["Node properties/animation_clip"];
	desc->is_expanded = true;

	desc = m_container->properties["Node properties/playing_instance"];
	desc->is_expanded = true;
	desc->is_browsable = (sync_node==nullptr);

	if(h->get_owner()->is_loaded() && h->selected_nodes->Count!=0 && h->selected_nodes[m_parent->selected_nodes->Count-1]==this)
		h->get_editor()->show_properties(m_container);
}

System::String^ animation_viewer_graph_node_animation::get_text()
{
	if(m_clips->Count>0)
	{
		System::String^ name = gcnew String(m_clips[0]->name());
		cli::array<System::String^>^ tokens = name->Split('/');
		int count = tokens->Length;
		if(count>2)
			return tokens[count-2]+"/"+tokens[count-1];
		else
			return name;
	}
	else
		return "none animation";
}

bool animation_viewer_graph_node_animation::synchronized_with_node(animation_viewer_graph_node_animation^ start_node)
{
	if(!sync_node)
		return false;

	if(sync_node==start_node)
		return true;

	return sync_node->synchronized_with_node(start_node);
}

animation_viewer_graph_node_animation::wpf_property_container^ animation_viewer_graph_node_animation::animation_clip::get()
{
	if(m_clips->Count>0)
		return m_clips[0]->container();
	else
		return nullptr;
}

animation_viewer_graph_node_animation::wpf_property_container^ animation_viewer_graph_node_animation::playing_instance::get()
{
	return (m_playing_instance)?m_playing_instance->container():nullptr;
}

xray::animation::skeleton_animation_ptr animation_viewer_graph_node_animation::clip()
{
	if(m_clips->Count>0)
		return m_clips[0]->animation();
	else
		return NULL;
}

bool animation_viewer_graph_node_animation::additive()
{
	if(m_clips->Count>0)
		return m_clips[0]->additive;
	else
		return false;
}

u32 animation_viewer_graph_node_animation::synchronization_group()
{
	if(m_clips->Count>0)
		return m_clips[0]->synchronization_group;
	else
		return u32(-1);
}

void animation_viewer_graph_node_animation::on_double_click(System::Object^, System::EventArgs^)
{
	animation_viewer_hypergraph^ h = safe_cast<animation_viewer_hypergraph^>(get_parent());
	R_ASSERT(h!=nullptr);
	viewer_editor^ ed = h->get_editor();
	if((ModifierKeys & Keys::Control) == Keys::Control)
	{
		if(m_clips->Count>0)
		{
			System::String^ name = gcnew String(m_clips[0]->name());
			ed->get_animation_editor()->setup_manager->Activate();
			ed->get_animation_editor()->setup_manager->multidocument_base->load_document(name);
		}
	}
	else
		ed->run_lexeme_editor(this);
}

void animation_viewer_graph_node_animation::clear_intervals()
{
	m_intervals->Clear();
	for each(animation_node_clip_instance^ inst in m_clips)
		delete inst;

	m_clips->Clear();
}

void animation_viewer_graph_node_animation::add_interval(animation_node_clip_instance^ inst, u32 interval_index)
{
	m_intervals->Add(inst->intervals[interval_index]);
	for each(animation_node_clip_instance^ instance in m_clips)
	{
		if(instance->base==inst->base)
			return;
	}

	m_clips->Add(inst);
}

u32 animation_viewer_graph_node_animation::lexeme_length_in_frames()
{
	if(m_intervals->Count>0)
	{
		u32 res = 0;
		for each(animation_node_interval^ interval in m_intervals)
			res += (u32)System::Math::Round(interval->length*animation::default_fps);

		return res;
	}
		
	return u32(-1);
}
////////////////////////////////////////////////////////////////////////////
// ref class animation_ui_type_lexeme_editor ///////////////////////////////
////////////////////////////////////////////////////////////////////////////
void animation_ui_type_lexeme_editor::run_editor(xray::editor::wpf_controls::property_editors::property^ prop)
{
	property_container_base^ cont = safe_cast<property_container_base^>(prop->property_owners[0]);
	animation_viewer_graph_node_animation^ n = safe_cast<animation_viewer_graph_node_animation^>(cont->owner);
	animation_viewer_hypergraph^ h = safe_cast<animation_viewer_hypergraph^>(n->get_parent());
	R_ASSERT(h!=nullptr);
	viewer_editor^ ed = h->get_editor();
	ed->run_lexeme_editor(n);
}