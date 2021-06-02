#include "pch.h"
#include "logic_objects_explorer.h"
#include "level_editor.h"
#include "object_base.h"
#include "project.h"
#include "command_select.h"

namespace xray{
namespace editor{

void logic_objects_explorer::treeView_selected_items_changed(System::Object^, System::EventArgs^ )
{
	
	if(m_level_editor->get_command_engine()->transaction_started())
		return;

	item_id_list^	new_selection = gcnew item_id_list;

	for each (TreeNode^ tn in treeView->selected_nodes)
	{
		if(tn->Tag==nullptr)
			continue;

		u32 ib		= dynamic_cast<object_base^>(tn->Tag)->id();

		new_selection->Add		( ib );
	}

	if(new_selection->Count!=0)
		m_level_editor->get_project()->select_ids(new_selection, enum_selection_method_set ) ;
	else
		if(treeView->selected_nodes->Count==0)
			m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project()) );

}


}
}