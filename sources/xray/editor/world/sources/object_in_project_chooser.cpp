////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_in_project_chooser.h"
#include "object_in_project_chooser_form.h"
#include "project.h"

namespace xray {
namespace editor {


object_in_project_chooser::object_in_project_chooser( project^ p)
:m_project(p)
{}

System::String^	object_in_project_chooser::id( )
{
	return "project_item";
}

bool object_in_project_chooser::choose( System::String^ current, System::String^ filter, System::String^% result, bool read_only )
{
	XRAY_UNREFERENCED_PARAMETERS		( read_only );
	object_in_project_chooser_form^ frm = gcnew object_in_project_chooser_form;
	frm->initialize						( m_project, current, filter );
	
	if(frm->ShowDialog()==System::Windows::Forms::DialogResult::OK)
	{
		result	= frm->selected_path;
		return true;
	}else
		return false;
}

} // namespace editor
} // namespace xray
