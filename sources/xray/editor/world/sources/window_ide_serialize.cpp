////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_tab.h"
#include "tools_tab.h"
#include "editor_world.h"
#include "window_ide.h"

namespace xray{
namespace editor{


template <typename T>
inline static T registry_value				(RegistryKey ^key, System::String^ value_id, const T &default_value)
{
	array<System::String^>^		names = key->GetValueNames();
	if (names->IndexOf(names,value_id) >= 0)
		return			((T)key->GetValue(value_id));

	return				(default_value);
}

WeifenLuo::WinFormsUI::Docking::IDockContent^ window_ide::reload_content	(System::String^ persist_string)
{
	WeifenLuo::WinFormsUI::Docking::IDockContent^	result = nullptr;

	if( persist_string == "xray.editor_base.scene_view_panel" ||
		 persist_string == "xray.editor.window_view" )// remove next commit
	{
		result			= m_view_window;
		return			result;
	}

	result = get_editor_world().find_dock_content(persist_string);

	return				(result);
}

} //namespace editor
} //namespace xray
