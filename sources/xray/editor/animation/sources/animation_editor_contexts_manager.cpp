////////////////////////////////////////////////////////////////////////////
//	Created		: 01.11.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_editor_contexts_manager.h"
#include "animation_editor_form.h"
#include "animation_editor.h"
#include "viewer_editor.h"
#include "animation_setup_manager.h"
#include "animation_groups_editor.h"
#include "animation_collections_editor.h"
#include "channels_editor.h"

using xray::animation_editor::animation_editor_contexts_manager;

bool animation_editor_contexts_manager::context_fit(String^ context)
{
	IDockContent^ active_content = m_editor->form->active_content;

	if(context=="Global")
		return true;
	else if(context=="viewer_editor")
		return active_content==m_editor->viewer;
	else if(context=="animation_setup_manager")
		return active_content==m_editor->setup_manager;
	else if(context=="groups_editor")
		return active_content==m_editor->groups;
	else if(context=="collections_editor")
		return active_content==m_editor->collections_editor;
	else if(context=="viewer_editor_active_doc")
	{
		if(active_content==m_editor->viewer)
		{
			IDockContent^ ac = m_editor->viewer->multidocument_base->main_dock_panel->ActiveContent;
			return ac==m_editor->viewer->multidocument_base->active_document;
		}
	}
	else if(context=="setup_manager_active_doc")
	{
		if(active_content==m_editor->setup_manager)
		{
			IDockContent^ ac = m_editor->setup_manager->multidocument_base->main_dock_panel->ActiveContent;
			return ac==m_editor->setup_manager->multidocument_base->active_document;
		}
	}
	else if(context=="groups_editor_active_doc")
	{
		if(active_content==m_editor->groups)
		{
			IDockContent^ ac = m_editor->groups->multidocument_base->main_dock_panel->ActiveContent;
			return ac==m_editor->groups->multidocument_base->active_document;
		}
	}
	else if(context=="collections_editor_active_doc")
	{
		if(active_content==m_editor->collections_editor)
		{
			IDockContent^ ac = m_editor->collections_editor->multidocument_base->main_dock_panel->ActiveContent;
			return ac==m_editor->collections_editor->multidocument_base->active_document;
		}
	}
	else if(context=="View")
		return (m_editor->form->main_dock_panel->ActiveContent==m_editor->viewport && m_editor->viewport->context_fit());
	else if(context=="navigation_mode")
		return m_editor->is_in_navigation_mode();

	return false;
}

List<String^>^ animation_editor_contexts_manager::get_contexts_list()	
{
	return gcnew List<String^>();
}
