////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_in_compound_chooser.h"
#include "object_in_compound_chooser_form.h"
#include "project.h"

namespace xray {
namespace editor {

object_in_compound_chooser::object_in_compound_chooser( project^ p)
	:m_project(p)
{}

System::String^	object_in_compound_chooser::id( )
{
	return "compound_item";
}

bool object_in_compound_chooser::choose( System::String^ current, System::String^ filter, System::String^% result, bool read_only )
{
	XRAY_UNREFERENCED_PARAMETERS		( read_only );
	object_in_compound_chooser_form^ frm = gcnew object_in_compound_chooser_form;
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

