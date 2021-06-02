////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_editor.h"
#include "particle_data_sources_panel.h"
#include "particle_contexts_manager.h"

namespace xray {
namespace particle_editor {

bool particle_contexts_manager::context_fit( System::String^ context )
{
	IDockContent^ active_content = m_editor->multidocument_base->main_dock_panel->ActiveContent;

/*	ASSERT(active_content->Focused == true);*/

	if (context == "Global")
		return true;
	else if (context == "ActiveDocument")
	{
		return active_content == m_editor->multidocument_base->active_document;
	}
	else if (context == "TreeView")
	{
		if (active_content != nullptr)
			return active_content == m_editor->particles_panel;
		return m_editor->particles_panel->IsActivated;		
	}
	else if (context == "View")
	{
		if (active_content != nullptr)
			return m_editor->view_window->context_fit();

		return m_editor->view_window->IsActivated;
	}
	return false;
}

List<String^>^	particle_contexts_manager::get_contexts_list	( )	
{
	List<String^>^ result_lst = gcnew List<String^>();

	result_lst->Add("Global");
	result_lst->Add("ActiveDocument");
	result_lst->Add("TreeView");
	result_lst->Add("View");

	return result_lst;
}


} // namespace particle_editor
} // namespace xray
