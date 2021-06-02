////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project_tab.h"
#include "project_items.h"
#include "project.h"
#include "level_editor.h"
#include "command_reparent_items.h"

using xray::editor::project_tab;

void project_tab::treeView_ItemDrag(System::Object^, System::Windows::Forms::ItemDragEventArgs^ e)
{
	if( e->Button == ::MouseButtons::Left || e->Button == ::MouseButtons::Middle)
	{
		string_list^ names_list = gcnew string_list;
		bool op_result			= (treeView->selected_nodes->Count!=0);

		for each (TreeNode^ tn in treeView->selected_nodes)
		{
			project_item_base^ ib	= safe_cast<project_item_base^>(tn->Tag);
			names_list->Add			( ib->get_full_name() );
		}

		if(op_result)
			DoDragDrop( names_list, DragDropEffects::Move );
	}
}

void project_tab::treeView_DragEnter(System::Object^, System::Windows::Forms::DragEventArgs^  e)
{
	string_list^ pi	= dynamic_cast<string_list^>(e->Data->GetData(string_list::typeid));
	if(pi)
		e->Effect = DragDropEffects::Move;
	else
		e->Effect = DragDropEffects::None;
}

void project_tab::treeView_DragLeave(System::Object^, System::EventArgs^)
{}

void project_tab::treeView_DragOver(System::Object^ sender, System::Windows::Forms::DragEventArgs^ e)
{
	System::Windows::Forms::Control^ c = safe_cast<System::Windows::Forms::Control^>(sender);
	System::Drawing::Point pt(e->X, e->Y);
	pt									= c->PointToClient(pt);


	if(pt.Y < 30)
	{
		scroll_direction = 30;
		scroll_timer->Enabled = true;
	}else
	if(pt.Y > c->Height-30)
	{
		scroll_direction = -30;
		scroll_timer->Enabled = true;
	}else
		scroll_timer->Enabled = false;
}

void project_tab::scroll_timer_Tick(System::Object^, System::EventArgs^)
{
	int	int_handle	= treeView->Handle.ToInt32( );
	HWND hwnd		=	( *( HWND* )&int_handle  );

	if(scroll_direction < 0 )
	{
		++scroll_direction;
		::SendMessage(hwnd, 277, 1, 0);
	}else
	if(scroll_direction > 0)
	{
		--scroll_direction;
		::SendMessage( hwnd, 277, 0, 0);
	}

	if(scroll_direction==0)
		scroll_timer->Enabled = false;
}

void project_tab::treeView_DragDrop(System::Object^, System::Windows::Forms::DragEventArgs^  e)
{
	scroll_timer->Enabled = false;

	string_list^ src_names	= dynamic_cast<string_list^>(e->Data->GetData(string_list::typeid));

	TreeNode^ dest_node		= treeView->GetNodeAt(treeView->PointToClient(System::Drawing::Point(e->X, e->Y)));
	if(!dest_node)
		return;

	project_item_folder^ dst_gi	= dynamic_cast<project_item_folder^>(dest_node->Tag);

	if(!dst_gi)
	{
		project_item_object^ dst_oi	= dynamic_cast<project_item_object^>(dest_node->Tag);
		if(dst_oi)
			dst_gi = dynamic_cast<project_item_folder^>(dst_oi->m_parent);

		if(!dst_gi)
			return;
	}

// check
	for each(System::String^ src_name in src_names)
	{
		project_item_base^ src = m_level_editor->get_project()->find_by_path( src_name, true );
		if(src==dst_gi || (dst_gi->is_child_of(src)) )
			return;
	}

	string_list^	to_move = gcnew string_list;
	m_level_editor->get_project()->extract_topmost_items( src_names, to_move );

	System::String^ dst_name = dst_gi->get_full_name();

	m_level_editor->get_command_engine()->run( 
		gcnew command_reparent_items( m_level_editor->get_project(),
										to_move,
										dst_name)
											);

}

