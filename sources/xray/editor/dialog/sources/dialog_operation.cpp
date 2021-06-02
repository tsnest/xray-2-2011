////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_operation.h"
#include "dialog_expression.h"

#pragma unmanaged
using xray::dialog_editor::dialog_operation;
using xray::dialog_editor::dialog_expression;

dialog_operation::dialog_operation()
{
	m_childs = vector<dialog_expression*>();
}

dialog_operation::~dialog_operation()
{

}

void dialog_operation::destroy()
{
	vector<dialog_expression*>::iterator it = m_childs.begin();
	for(; it!=m_childs.end(); ++it)
		(*it)->destroy();

	DELETE(this);
}
