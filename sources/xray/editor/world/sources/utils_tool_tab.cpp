#include "pch.h"
#include "utils_tool_tab.h"
#include "tool_utils.h"

using xray::editor::utils_tool_tab;

System::Void utils_tool_tab::on_after_select(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e)
{
	if(e->Node==nullptr || (String^)e->Node->Tag == "group")
		m_tool.select_library_name(nullptr);
	else
		m_tool.select_library_name(e->Node->FullPath);
}
