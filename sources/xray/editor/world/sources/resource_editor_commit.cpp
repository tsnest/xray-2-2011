////////////////////////////////////////////////////////////////////////////
//	Created		: 18.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_editor_commit.h"
#include "resource_editor.h"


namespace xray{
namespace editor{

void resource_editor_commit::fill_changes_list( )
{
	m_list_box->Items->Clear( );
	for each ( resource_editor_resource^ r in m_resource_editor->changed_resources )
		m_list_box->Items->Add( r->m_name );
}

void resource_editor_commit::m_cancel_button_Click( Object^ ,System::EventArgs^ )
{
	this->Close();
}

void resource_editor_commit::revertChanges_Click( Object^ ,System::EventArgs^ )
{
	resources_list^ lst = gcnew resources_list;

	for each (int i in m_list_box->SelectedIndices)
		lst->Add( m_resource_editor->changed_resources[i] );

	m_resource_editor->revert_changes( lst );

	fill_changes_list				( );
}

void resource_editor_commit::m_list_box_MouseUp( Object^ , MouseEventArgs^ e )
{
	if(e->Button == System::Windows::Forms::MouseButtons::Right)
	{
		if(!m_list_box->GetSelected(m_list_box->IndexFromPoint(e->Location)))
		{
			m_list_box->SelectedItems->Clear();		
			m_list_box->SetSelected		(m_list_box->IndexFromPoint(e->Location), true);
		}

		m_options_context_menu->Show	(m_list_box->PointToScreen(e->Location));
	}
}

} // namespace editor
} // namespace xray