////////////////////////////////////////////////////////////////////////////
//	Created		: 20.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_tab.h"
#include "project.h"
#include "object_base.h"
#include "project_items.h"
#include "tool_base.h"
#include "level_editor.h"
#include "command_select.h"
#include "editor_world.h"
#include "command_add_library_object.h"
#include "window_ide.h"

namespace xray{
namespace editor{

void project_tab::in_constructor()
{
	treeView->ImageList	= m_level_editor->get_editor_world().images16x16();
	font_normal			= treeView->Font;
	font_ignored		= gcnew System::Drawing::Font(font_normal, System::Drawing::FontStyle::Strikeout);
}

void project_tab::global_view_menu_Opening(System::Object^, System::ComponentModel::CancelEventArgs^ )
{
	newFilterToolStripMenuItem->Enabled			= true;
	
	bool b_add_to_list		= false;
	bool b_remove_from_list	= false;
	bool b_delete_object	= false;

	project^ p = m_level_editor->get_project();
	for each (System::Windows::Forms::TreeNode^ node in treeView->selected_nodes)
	{
		if(node->Tag==nullptr)
			continue;

		project_item_base^ h				= safe_cast<project_item_base^>(node->Tag);

		if(h->get_persistent())
			continue;

		if(p->is_object_in_ignore_list(h->id()))
			b_remove_from_list	= true;
		else
			b_add_to_list		= true;

		b_delete_object			= true;
	}
	
	addToIgnoreListToolStripMenuItem->Visible		= b_add_to_list;
	removeFromIgnoreListToolStripMenuItem->Visible	= b_remove_from_list;
	removeToolStripMenuItem->Enabled				= b_delete_object;
}

void project_tab::newFilterToolStripMenuItem_Click(System::Object^, System::EventArgs^)
{
	m_level_editor->get_command_engine()->run( gcnew command_add_project_folder(m_level_editor, nullptr) );
}

void project_tab::removeToolStripMenuItem_Click(System::Object^, System::EventArgs^)
{
	m_level_editor->delete_selected				( );
}

void project_tab::add_remove_IgnoreListToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^)
{
	bool b_add = (sender==addToIgnoreListToolStripMenuItem);

	project^ p						= m_level_editor->get_project();

	System::Collections::Generic::List<tree_node^>^ sel = gcnew System::Collections::Generic::List<tree_node^>(treeView->selected_nodes);
	for each (System::Windows::Forms::TreeNode^ node in sel )
	{
		if(node->Tag==nullptr)
			continue;

		project_item_base^ h				= safe_cast<project_item_base^>(node->Tag);
		if(b_add == !p->is_object_in_ignore_list(h->id()))
		{
			p->set_object_ignore_list(h->id(), b_add);
			h->m_tree_node_->NodeFont = (b_add) ? font_ignored : font_normal;
		}
	}
}

void project_tab::treeView_BeforeLabelEdit(System::Object^, System::Windows::Forms::NodeLabelEditEventArgs^ e)
{
	if(e->Node->Tag==nullptr)
	{
		e->CancelEdit = true;
		return;
	}

	project_item_base^ ib		= safe_cast<project_item_base^>(e->Node->Tag);
	if(ib->m_parent==nullptr || ib->get_object()) // root group or object(name changed from object inspector)
		e->CancelEdit = true;
}

void project_tab::treeView_AfterLabelEdit(System::Object^, System::Windows::Forms::NodeLabelEditEventArgs^  e)
{
	if(e->Label==nullptr || e->Label->Length==0)
	{
		e->CancelEdit = true;
		return;
	}
	project_item_base^ w		= safe_cast<project_item_base^>(e->Node->Tag);
	System::String^ text		= e->Label;
	e->CancelEdit				= true;
	w->after_name_edit			( text );
}

void project_tab::treeView_selected_items_changed(System::Object^, tree_view_selection_event_args^ e )
{

	if(m_level_editor->get_command_engine()->transaction_started())
		return;

	if(m_level_editor->get_project()->get_pin_selection())
		return;

	project_items_list^	new_selection = gcnew project_items_list;

	/// bad temporary solution
	System::String^ keys	= gcnew System::String("");
	m_level_editor->ide()->get_current_keys_string(keys);
	bool recursive		= keys->Contains("Alt");
	
	for each ( TreeNode^ tn in e->new_selection )
	{
		if(tn->Tag==nullptr)
			continue;

		project_item_base^ ib		= safe_cast<project_item_base^>(tn->Tag);
		project_items_list			childs;
		if(recursive)
			ib->get_all_items		( %childs, true );
		else
			childs.Add				( ib );

		for each( project_item_base^ pi in childs)
		{
			if(pi->get_selectable() && !new_selection->Contains(pi))
				new_selection->Add		( pi );
		}
	}

	if(new_selection->Count!=0)
		m_level_editor->get_command_engine()->run( gcnew command_select( m_level_editor->get_project(), new_selection, enum_selection_method_set ) );
	else
		m_level_editor->get_command_engine()->run( gcnew command_select(m_level_editor->get_project()) );
}

System::Void project_tab::treeView_node_double_click(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^  e)
{
	m_level_editor->on_tree_view_node_double_click( e->Node );
}

void project_tab::set_total_objects_count( u32 value )
{
	totalStatusLabel1->Text = System::String::Format("Total:{0}", value);
}

void project_tab::set_selected_objects_count( u32 value )
{
	selectedStatusLabel1->Text = System::String::Format("Selected:{0}", value);
}

} // namespace editor
} // namespace xray
