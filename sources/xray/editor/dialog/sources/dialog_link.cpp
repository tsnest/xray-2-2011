////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_link.h"
#include "dialog_expression.h"
#include "dialog_action.h"
#include "dialog_precondition.h"
#include "dialog_operation_and.h"
#include "dialog_operation_or.h"
#include "dialog_operation_not.h"

#pragma unmanaged
using xray::dialog_editor::dialog_link;
using xray::dialog_editor::dialog_node_base;
using xray::dialog_editor::dialog_expression;
using xray::dialog_editor::dialog_action;
using xray::dialog_editor::dialog_precondition;
using xray::dialog_editor::dialog_operation_and;
using xray::dialog_editor::dialog_operation_or;
using xray::dialog_editor::dialog_operation_not;

u32 dialog_link::m_created_counter = 0;

dialog_link::dialog_link(dialog_node_base* chld)
:m_child(chld)
{
	m_root_precondition = NULL;
	++m_created_counter;
}

dialog_link::~dialog_link()
{
	m_root_precondition = NULL;
	--m_created_counter;
}

bool dialog_link::check_preconditions()
{
	if(m_root_precondition!=NULL)
		return m_root_precondition->check();

	return true;
}

void dialog_link::do_actions()
{
	vector<dialog_action*>::iterator b = m_actions_list.begin();
	for(; b!=m_actions_list.end(); ++b)
		(*b)->do_action();
}

void dialog_link::load(xray::configs::lua_config_value const& cfg)
{
	if(cfg.value_exists("preconditions"))
	{
		int t = (int)cfg["preconditions"]["type"];
		switch(t)
		{
		case 0: 
			{
				dialog_precondition* root = NEW(dialog_precondition)();
				root->load(cfg["preconditions"]);
				set_root_precondition(root);
				break;
			}
		case 1: 
			{
				dialog_operation_and* root = NEW(dialog_operation_and)();
				root->load(cfg["preconditions"]);
				set_root_precondition(root);
				break;
			}
		case 2: 
			{
				dialog_operation_or* root = NEW(dialog_operation_or)();
				root->load(cfg["preconditions"]);
				set_root_precondition(root);
				break;
			}
		case 3: 
			{
				dialog_operation_not* root = NEW(dialog_operation_not)();
				root->load(cfg["preconditions"]);
				set_root_precondition(root);
				break;
			}
		}
	}

	if(cfg.value_exists("actions"))
	{
		configs::lua_config::const_iterator	i = cfg["actions"].begin();
		for(; i!=cfg["actions"].end(); ++i) 
		{
			dialog_action* act = NEW(dialog_action)();
			act->load(*i);
			add_action(act);
		}
	}
}

void dialog_link::save(xray::configs::lua_config_value cfg)
{
	xray::configs::lua_config_value precond_table = cfg["preconditions"];
	if(m_root_precondition!=NULL)
		m_root_precondition->save(cfg["preconditions"]);

	xray::configs::lua_config_value action_table = cfg["actions"];
	int i = 0;
	vector<dialog_action*>::iterator b = m_actions_list.begin();
	for(; b!=m_actions_list.end(); ++b)
	{
		xray::configs::lua_config_value cur_action = action_table[i];
		(*b)->save(cur_action);
		++i;
	}
}
