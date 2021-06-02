////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "select_library_class.h"
#include "select_library_class_form.h"
#include "tool_base.h"

namespace xray {
namespace editor {

library_class_chooser::library_class_chooser( tool_base^ tool )
:m_tool	( tool )
{}

System::String^	library_class_chooser::id( )
{
	return m_tool->name();
}

bool library_class_chooser::choose( System::String^ current, 
									System::String^ filter, 
									System::String^% result,
									bool read_only)
{
	select_library_class_form	frm( m_tool );
	frm.initialize				( current, read_only, filter );

	if(::DialogResult::OK==frm.ShowDialog())
	{
		result			= frm.selected_path;
		return true;
	}else
		return false;
}


} // namespace editor
} // namespace xray
