////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_viewer_change_lexeme.h"
#include "animation_viewer_hypergraph.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_node_clip.h"
#include "animation_node_interval.h"
#include <xray/strings_stream.h>
#include "viewer_editor.h"
#include "viewer_document.h"
#include "animation_editor.h"

using xray::animation_editor::animation_viewer_hypergraph;
using xray::animation_editor::animation_viewer_change_lexeme;
using xray::animation_editor::animation_viewer_graph_node_base;
using xray::animation_editor::animation_node_interval;

animation_viewer_change_lexeme::animation_viewer_change_lexeme(animation_viewer_hypergraph^ h, animation_viewer_graph_node_base^ n, List<animation_node_interval^>^ lst, u32 cycle_index)
:m_hypergraph(h),
m_new_cycle_index(cycle_index),
m_node_id(n->id)
{

	animation_viewer_graph_node_animation^ anim = safe_cast<animation_viewer_graph_node_animation^>(n);
	m_old_cycle_index = anim->cycle_from_interval_id;
	configs::lua_config_ptr cfg_ptr = configs::create_lua_config();
	xray::strings::stream stream(g_allocator);
	for(int index=0; index<anim->intervals->Count; ++index)
	{
		cfg_ptr->get_root()["intervals"][index]["animation"] = anim->intervals[index]->parent->name();
		cfg_ptr->get_root()["intervals"][index]["id"] = anim->intervals[index]->id;
	}

	cfg_ptr->save(stream);
	stream.append("%c", 0);
	m_old_cfg_string = strings::duplicate(g_allocator, (pcstr)stream.get_buffer());

	configs::lua_config_ptr cfg_ptr2 = configs::create_lua_config();
	xray::strings::stream stream2(g_allocator);
	for(int index=0; index<lst->Count; ++index)
	{
		cfg_ptr2->get_root()["intervals"][index]["animation"] = lst[index]->parent->name();
		cfg_ptr2->get_root()["intervals"][index]["id"] = lst[index]->id;
	}

	cfg_ptr2->save(stream2);
	stream2.append("%c", 0);
	m_new_cfg_string = strings::duplicate(g_allocator, (pcstr)stream2.get_buffer());
}

animation_viewer_change_lexeme::~animation_viewer_change_lexeme()
{
	FREE(m_new_cfg_string);
	FREE(m_old_cfg_string);
}

bool animation_viewer_change_lexeme::commit()
{
	animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_id);
	animation_viewer_graph_node_animation^ anim = safe_cast<animation_viewer_graph_node_animation^>(n);
	anim->clear_intervals();
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_new_cfg_string);
	if(config_ptr && !config_ptr->empty()) 
	{
		for(u32 i=0; i<config_ptr->get_root()["intervals"].size(); ++i)
		{
			String^ clip_name = gcnew String((pcstr)config_ptr->get_root()["intervals"][i]["animation"]);
			u32 interval_id = (u32)config_ptr->get_root()["intervals"][i]["id"];
			animation_node_clip^ clip = m_hypergraph->get_editor()->get_animation_editor()->clip_by_name(clip_name);
			R_ASSERT(clip!=nullptr);
			anim->add_interval(clip->make_instance(), interval_id);
		}
	}

	m_hypergraph->get_owner()->reset_target();
	m_hypergraph->get_owner()->current_time = m_hypergraph->get_owner()->current_time;
	anim->cycle_from_interval_id = m_new_cycle_index;
	anim->init_properties(anim->sync_node);
	anim->Invalidate(false);
	m_hypergraph->get_editor()->show_properties(anim->container());
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();

	m_hypergraph->get_owner()->set_target(anim);
	return true;
}

void animation_viewer_change_lexeme::rollback()
{
	animation_viewer_graph_node_base^ n = animation_viewer_hypergraph::find_node_by_id(m_hypergraph->all_nodes(), m_node_id);
	animation_viewer_graph_node_animation^ anim = safe_cast<animation_viewer_graph_node_animation^>(n);
	anim->clear_intervals();
	configs::lua_config_ptr config_ptr = configs::create_lua_config_from_string(m_old_cfg_string);
	if(config_ptr && !config_ptr->empty())
	{
		for(u32 i=0; i<config_ptr->get_root()["intervals"].size(); ++i)
		{
			String^ clip_name = gcnew String((pcstr)config_ptr->get_root()["intervals"][i]["animation"]);
			u32 interval_id = (u32)config_ptr->get_root()["intervals"][i]["id"];
			animation_node_clip^ clip = m_hypergraph->get_editor()->get_animation_editor()->clip_by_name(clip_name);
			R_ASSERT(clip!=nullptr);
			anim->add_interval(clip->make_instance(), interval_id);
		}
	}

	m_hypergraph->get_owner()->reset_target();
	m_hypergraph->get_owner()->current_time = m_hypergraph->get_owner()->current_time;
	anim->cycle_from_interval_id = m_old_cycle_index;
	anim->init_properties(anim->sync_node);
	anim->Invalidate(false);
	m_hypergraph->get_editor()->show_properties(anim->container());
	if(m_hypergraph->get_owner()->target_locked)
		m_hypergraph->get_editor()->change_locked();

	m_hypergraph->get_owner()->set_target(anim);
}