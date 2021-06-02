////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editors_helper.h"

using WeifenLuo::WinFormsUI::Docking::FloatWindow;
using System::Windows::Forms::Form;
using xray::editor::wpf_controls::dot_net_helpers;

namespace xray{
namespace editor_base{

Boolean		editors_helper::is_active_form_belong_to( Form^ form )
{
	Form^ active_form = Form::ActiveForm;
	if( active_form == form )
		return true;
	else 
	{
		FloatWindow^ float_window;
		if( ( float_window = dot_net_helpers::as_type<FloatWindow^>( active_form  ) ) != nullptr )
		{
			 Form^ parent_form = float_window->DockPanel->FindForm( );
			 if( parent_form == form )
				 return true;
		}
	}
	return false;
}

} // namespace editor_base
} // namespace xray