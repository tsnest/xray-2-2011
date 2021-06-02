////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_precondition.h"
#include "dialog_operation_and.h"
#include "dialog_operation_or.h"
#include "dialog_operation_not.h"

#pragma unmanaged
using xray::dialog_editor::dialog_operation_and;

u32 dialog_operation_and::m_created_counter = 0;

dialog_operation_and::dialog_operation_and()
{
	++m_created_counter;
}

dialog_operation_and::~dialog_operation_and()
{
	--m_created_counter;
}

bool dialog_operation_and::check()
{
	vector<dialog_expression*>::iterator b = m_childs.begin();
	for(; b!=m_childs.end(); ++b)
	{
		if(!((*b)->check()))
			return false;
	}

	return true;
}

void dialog_operation_and::load(xray::configs::lua_config_value const& cfg)
{
	if(cfg.value_exists("childs"))
	{
		configs::lua_config::const_iterator	i = cfg["childs"].begin();
		for(; i!=cfg["childs"].end(); ++i) 
		{
			int t = (int)((*i)["type"]);
			switch(t)
			{
			case 0: 
				{
					dialog_precondition* chld = NEW(dialog_precondition)();
					chld->load(*i);
					add_child(chld);
					break;
				}
			case 1: 
				{
					dialog_operation_and* chld = NEW(dialog_operation_and)();
					chld->load(*i);
					add_child(chld);
					break;
				}
			case 2: 
				{
					dialog_operation_or* chld = NEW(dialog_operation_or)();
					chld->load(*i);
					add_child(chld);
					break;
				}
			case 3: 
				{
					dialog_operation_not* chld = NEW(dialog_operation_not)();
					chld->load(*i);
					add_child(chld);
					break;
				}
			}
		}
	}
}

void dialog_operation_and::save(xray::configs::lua_config_value cfg)
{
	cfg["type"] = 1;
	vector<dialog_expression*>::iterator b = m_childs.begin();
	int i = 0;
	for(; b!=m_childs.end(); ++b)
		(*b)->save(cfg["childs"][i++]);
}