////////////////////////////////////////////////////////////////////////////
//	Created		: 01.11.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_editor_contexts_manager.h"
#include "dialog_editor.h"

using xray::dialog_editor::dialog_editor_contexts_manager;

bool dialog_editor_contexts_manager::context_fit(String^ context)
{
	IDockContent^ active_content = m_editor->multidocument_base->main_dock_panel->ActiveContent;

	if(context=="dialog_editor")
		return true;
	else if(context=="dialog_editor_active_document")
		return active_content==m_editor->multidocument_base->active_document;

	return false;
}

List<String^>^ dialog_editor_contexts_manager::get_contexts_list()	
{
	return gcnew List<String^>();
}