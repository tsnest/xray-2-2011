////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "contexts_manager.h"
#include "model_editor.h"

namespace xray{
namespace model_editor{

contexts_manager::contexts_manager( model_editor^ ed )
:m_model_editor	( ed )
{
}

bool contexts_manager::context_fit( System::String^ context_str )
{
 	if(context_str=="Global")
	{
		return true;
	}else
 	if(context_str=="View")
	{
		return m_model_editor->m_view_window->context_fit();
	}else
		return false;
}

string_list^ contexts_manager::get_contexts_list( )
{
	return gcnew string_list( gcnew cli::array<System::String^>(4){"Global", "View", "Hierrarchy", "Properties"} );
}

} //namespace model_editor
} //namespace xray
