////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_editor_contexts_manager.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"

using xray::sound_editor::sound_editor_contexts_manager;

bool sound_editor_contexts_manager::context_fit(String^ context)
{
	IDockContent^ active_content = m_editor->multidocument_base->main_dock_panel->ActiveContent;

	if(context=="Global" || context=="sound_editor")
		return true;
	else if(context=="Active_document")
		return active_content==m_editor->multidocument_base->active_document;

	return false;
}

List<String^>^ sound_editor_contexts_manager::get_contexts_list()	
{
	return gcnew List<String^>();
}
