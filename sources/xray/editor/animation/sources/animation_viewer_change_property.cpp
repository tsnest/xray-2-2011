////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_change_property.h"
#include "animation_viewer_hypergraph.h"
#include "animation_viewer_graph_node_base.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_node_clip_instance.h"
#include "animation_node_playing_instance.h"
#include "animation_node_interpolator.h"
#include "time_scale_instance.h"
#include "time_instance.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_timed.h"
#include "animation_viewer_graph_link_weighted.h"
#include "viewer_document.h"

using xray::animation_editor::animation_viewer_change_property;
using xray::animation_editor::animation_viewer_hypergraph;

animation_viewer_change_property::animation_viewer_change_property(animation_viewer_hypergraph^ h, 
																	wpf_property_container^ pc, 
																	System::String^ property_name, 
																	System::Object^ new_v, 
																	System::Object^ old_v)
:m_hypergraph(h),
m_changed_property_name(property_name),
m_new_value(new_v),
m_old_value(old_v),
m_src_id(u32(-1)),
m_dst_id(u32(-1)),
m_parent_is_anim_clip(false)
{
	if(m_changed_property_name->Contains("animation_clip/"))
	{
		m_parent_is_anim_clip = true;
		m_changed_property_name = m_changed_property_name->Remove(0,15);
	}
	else if(m_changed_property_name->Contains("time_scale/"))
		m_changed_property_name = m_changed_property_name->Remove(0,11);

	animation_viewer_graph_link^ lnk = dynamic_cast<animation_viewer_graph_link^>(pc->owner);
	if(lnk)
	{
		m_src_id = safe_cast<animation_viewer_graph_node_base^>(lnk->source_node)->id;
		m_dst_id = safe_cast<animation_viewer_graph_node_base^>(lnk->destination_node)->id;
		if(m_changed_property_name=="weight")
		{
			m_new_values = gcnew source_id_to_koef_type();
			m_old_values = gcnew source_id_to_koef_type();
			m_new_values->Add(m_src_id, (float)new_v);
			m_old_values->Add(m_src_id, (float)old_v);
			init_weight_values(safe_cast<animation_viewer_graph_link_weighted^>(lnk));
		}
	}
	else
	{
		m_src_id = dynamic_cast<animation_viewer_graph_node_base^>(pc->owner)->id;
	}
}

void animation_viewer_change_property::init_weight_values(xray::animation_editor::animation_viewer_graph_link_weighted^ initial_link)
{
	animation_viewer_graph_node_animation^ anim = dynamic_cast<animation_viewer_graph_node_animation^>(initial_link->source_node);
	if(anim && anim->additive())
		return;

	animation_viewer_graph_node_base^ parent = safe_cast<animation_viewer_graph_node_base^>(initial_link->destination_node);
	float sum = 0.0f;
	links^ lnks = gcnew links();
	for each(animation_viewer_graph_node_base^ child in parent->childs)
	{
		if(child==initial_link->source_node)
			continue;

		animation_viewer_graph_node_animation^ child_anim = dynamic_cast<animation_viewer_graph_node_animation^>(child);
		if(!child_anim || !(child_anim->additive()))
		{
			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), child->id, parent->id);
			animation_viewer_graph_link_weighted^ l = dynamic_cast<animation_viewer_graph_link_weighted^>(lnk);
			sum += l->weight;
			lnks->Add(l);
			m_old_values->Add(child->id, l->weight);
		}
	}

	if(!xray::math::is_similar(sum, 0.0f))
	{
		for each(animation_viewer_graph_link_weighted^ l in lnks)
		{
			float new_val = l->weight + ((float)m_old_value-(float)m_new_value)*(l->weight/sum);
			xray::math::clamp(new_val, 0.0f, 1.0f);
			new_val = (float)System::Math::Round(new_val, 3);
			m_new_values->Add(safe_cast<animation_viewer_graph_node_base^>(l->source_node)->id, new_val);
		}
	}
	else // if all weights equal to zero
	{
		for each(animation_viewer_graph_link_weighted^ l in lnks)
		{
			float new_val = ((float)m_old_value-(float)m_new_value)/lnks->Count;
			xray::math::clamp(new_val, 0.0f, 1.0f);
			new_val = (float)System::Math::Round(new_val, 3);
			m_new_values->Add(safe_cast<animation_viewer_graph_node_base^>(l->source_node)->id, new_val);
		}
	}
}

bool animation_viewer_change_property::commit()
{
	if(m_new_value->Equals(m_old_value))
	{
		m_hypergraph->Invalidate(false);
		return false;
	}

	if(m_changed_property_name=="weight")
	{
		for each(System::Collections::Generic::KeyValuePair<u32, float> p in m_new_values)
		{
			if(m_new_values->Count==1)
			{
				animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), p.Key);
				animation_viewer_graph_node_animation^ an = dynamic_cast<animation_viewer_graph_node_animation^>(n);
				if(an && !(an->additive()))
				{
					animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), p.Key, m_dst_id);
					R_ASSERT(lnk);
					animation_viewer_graph_link_weighted^ l = safe_cast<animation_viewer_graph_link_weighted^>(lnk);
					l->weight = m_old_values[p.Key];
					m_hypergraph->Invalidate(false);
					return false;
				}
			}

			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), p.Key, m_dst_id);
			R_ASSERT(lnk);
			animation_viewer_graph_link_weighted^ l = safe_cast<animation_viewer_graph_link_weighted^>(lnk);
			l->weight = p.Value;
		}

		m_hypergraph->get_owner()->reset_target();
		m_hypergraph->Invalidate(false);
		return true;
	}

	return set_value(m_new_value);
}

void animation_viewer_change_property::rollback()
{
	if(m_changed_property_name=="weight")
	{
		for each(System::Collections::Generic::KeyValuePair<u32, float> p in m_old_values)
		{
			animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), p.Key, m_dst_id);
			R_ASSERT(lnk);
			animation_viewer_graph_link_weighted^ l = safe_cast<animation_viewer_graph_link_weighted^>(lnk);
			l->weight = p.Value;
		}
	
		m_hypergraph->get_owner()->reset_target();
		m_hypergraph->Invalidate(false);
		return;
	}

	R_ASSERT(set_value(m_old_value));
}

bool animation_viewer_change_property::set_value(System::Object^ value)
{
	wpf_property_container^ pc = nullptr;
	System::String^ property_name = "Node properties/";
	System::String^ changed_property_name = property_name+m_changed_property_name;
	if(m_dst_id==u32(-1))
	{
		animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_src_id);
		animation_viewer_graph_node_animation^ anim = safe_cast<animation_viewer_graph_node_animation^>(n);
		if(m_changed_property_name=="synchronization_group")
			pc = anim->animation_clip;
		else if(m_changed_property_name=="time_offset/in seconds" || m_changed_property_name=="time_offset/in frames")
		{
			pc = dynamic_cast<animation_node_playing_instance^>(anim->playing_instance->owner)->time_offset;
			changed_property_name = property_name+m_changed_property_name->Remove(0, 12);
		}
		else if(m_changed_property_name=="mixing_type")
			pc = anim->playing_instance;
		else if(m_changed_property_name=="value")
			pc = safe_cast<animation_node_playing_instance^>(anim->playing_instance->owner)->time_scale;
		else // m_changed_property_name=="type"||"time/time_s"||"time/time_f"||"epsilon"
		{
			if(m_parent_is_anim_clip)
			{
				animation_node_clip_instance^ clip = safe_cast<animation_node_clip_instance^>(anim->animation_clip->owner);
				pc = clip->interpolator;
				if(m_changed_property_name=="type")
				{
					pc->inner_value->value = value;
					pc->update_properties();
					m_hypergraph->set_selected_node(n);
					m_hypergraph->get_owner()->reset_target();
					m_hypergraph->Invalidate(false);
					return true;
				}
				else if(m_changed_property_name=="time/in seconds" || m_changed_property_name=="time/in frames")
				{
					pc = dynamic_cast<animation_node_interpolator^>(clip->interpolator->owner)->time;
					changed_property_name = property_name+m_changed_property_name->Remove(0, 5);
				}
			}
			else
			{
				animation_node_playing_instance^ pl_inst = safe_cast<animation_node_playing_instance^>(anim->playing_instance->owner);
				time_scale_instance^ ts_inst = safe_cast<time_scale_instance^>(pl_inst->time_scale->owner);
				pc = ts_inst->interpolator;
				if(m_changed_property_name=="type")
				{
					pc->inner_value->value = value;
					pc->update_properties();
					m_hypergraph->set_selected_node(n);
					m_hypergraph->get_owner()->reset_target();
					m_hypergraph->Invalidate(false);
					return true;
				}
				else if(m_changed_property_name=="time/in seconds" || m_changed_property_name=="time/in frames")
				{
					pc = dynamic_cast<animation_node_interpolator^>(ts_inst->interpolator->owner)->time;
					changed_property_name = property_name+m_changed_property_name->Remove(0, 5);
				}
			}
		}
	}
	else
	{
		animation_viewer_graph_link^ lnk = animation_viewer_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), m_src_id, m_dst_id);
		if(m_changed_property_name=="playing_time/in seconds" || m_changed_property_name=="playing_time/in frames")
		{
			pc = safe_cast<animation_viewer_graph_link_timed^>(lnk)->playing_time;
			changed_property_name = property_name+m_changed_property_name->Remove(0, 14);
		}
		else
		{
			animation_viewer_graph_link_weighted^ lnk_w = safe_cast<animation_viewer_graph_link_weighted^>(lnk);
			pc = lnk_w->interpolator;
			if(m_changed_property_name=="type")
			{
				pc->inner_value->value = value;
				pc->update_properties();
				m_hypergraph->get_owner()->reset_target();
				m_hypergraph->Invalidate(false);
				return true;
			}
			else if(m_changed_property_name=="time/in seconds" || m_changed_property_name=="time/in frames")
			{
				pc = dynamic_cast<animation_node_interpolator^>(lnk_w->interpolator->owner)->time;
				changed_property_name = property_name+m_changed_property_name->Remove(0, 5);
			}
		}
	}

	pc->properties[changed_property_name]->value = value;
	pc->update_properties();
	m_hypergraph->get_owner()->reset_target();
	m_hypergraph->Invalidate(false);
	return true;
}
