////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_precondition.h"
#include "dialog_expression.h"

#pragma unmanaged
using xray::dialog_editor::dialog_precondition;
using xray::dialog_editor::e_precondition_functions;
using xray::dialog_editor::dialog_expression;

u32 dialog_precondition::m_created_counter = 0;

dialog_precondition::dialog_precondition(int func, pstr par)
:m_function(e_precondition_functions(func)),
m_param(par)
{
	++m_created_counter;
}

dialog_precondition::~dialog_precondition()
{
	if(m_param)
		FREE(m_param);

	--m_created_counter;
}

void dialog_precondition::load(xray::configs::lua_config_value const& cfg)
{
	m_function = e_precondition_functions(int(cfg["func"]));
	if(cfg.value_exists("param1"))
		m_param = strings::duplicate(g_allocator, cfg["param1"]);
	else
		m_param = NULL;
}

void dialog_precondition::save(xray::configs::lua_config_value cfg)
{
	cfg["type"] = 0;
	cfg["func"] = (int)m_function;
	cfg["param1"] = m_param;
}

bool dialog_precondition::check()
{
	switch(m_function)
	{
	case has_info: return predicate_has_info(m_param);
	case has_item: return predicate_has_item(m_param);
	}
	return true;
}

bool dialog_precondition::predicate_has_info(pcstr info)
{
	XRAY_UNREFERENCED_PARAMETER(info);
	return true;
}

bool dialog_precondition::predicate_has_item(pcstr item_name)
{
	XRAY_UNREFERENCED_PARAMETER(item_name);
	return true;
}

void dialog_precondition::destroy()
{
	DELETE(this);
}
