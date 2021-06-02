////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_link_action.h"
#include "dialog_action.h"
using xray::dialog_editor::dialog_link_action;
using xray::dialog_editor::dialog_action;

dialog_link_action::dialog_link_action()
{
	m_action = NEW(dialog_action)();
}

dialog_link_action::dialog_link_action(dialog_action* act)
{
	m_action = NEW(dialog_action)();
	m_action->m_function = act->m_function;
	if(act->m_param)
		m_action->m_param = strings::duplicate(g_allocator, act->m_param);
}

dialog_link_action::~dialog_link_action()
{
	DELETE(m_action);
}

void dialog_link_action::param1::set(String^ nv)
{
	if(m_action->m_param)
		FREE(m_action->m_param);

	m_action->m_param = strings::duplicate(g_allocator, unmanaged_string(nv).c_str());
};

dialog_action* dialog_link_action::action_copy()
{
	dialog_action* new_action = NEW(dialog_action)();
	new_action->m_function = m_action->m_function;
	if(m_action->m_param)
		new_action->m_param = strings::duplicate(g_allocator, m_action->m_param);

	return new_action;
}
