////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_action.h"

#pragma unmanaged
using xray::dialog_editor::dialog_action;
using xray::dialog_editor::e_action_functions;

u32 dialog_action::m_created_counter = 0;

dialog_action::dialog_action(u8 func, pstr par)
:m_function(e_action_functions(func)),
m_param(par)
{
	++m_created_counter;
}

dialog_action::~dialog_action()
{
	if(m_param)
		FREE(m_param);

	--m_created_counter;
}

void dialog_action::load(xray::configs::lua_config_value const& cfg)
{
	m_function = e_action_functions(int(cfg["func"]));
	if(cfg.value_exists("param1"))
		m_param = strings::duplicate(g_allocator, cfg["param1"]);
	else
		m_param = NULL;
}

void dialog_action::save(xray::configs::lua_config_value cfg)
{
	cfg["func"] = (int)m_function;
	cfg["param1"] = m_param;
}

void dialog_action::do_action()
{
	switch(m_function)
	{
	case give_info: return action_give_info(m_param);
	case give_item: return action_give_item(m_param);
	}
}

void dialog_action::action_give_info(pcstr info)
{
	XRAY_UNREFERENCED_PARAMETER(info);
}

void dialog_action::action_give_item(pcstr item_name)
{
	XRAY_UNREFERENCED_PARAMETER(item_name);
}