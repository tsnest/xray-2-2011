////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logic_event_link_chooser_form.h"
#include "project.h"
#include "project_tab.h"
#include "logic_event.h"
#include "object_scene.h"

namespace xray{
namespace editor{

extern void copy_nodes( TreeNodeCollection^ dest, TreeNodeCollection^ src, System::String^ filter );

void logic_event_link_chooser_form::initialize( project^ p, System::String^ initial, System::String^ f )
{
	xray::editor::controls::tree_view^ tree	= p->ui()->treeView;
	treeView->ImageList						= tree->ImageList;
	copy_nodes								( treeView->nodes, tree->nodes, f );

	if(initial)
	{
		tree_node^ n		= find_hierrarchy( tree->root->nodes, initial );
		if(n!=nullptr)
		{
			n->Selected				= true;
			n->EnsureVisible		( );
		}
	}

	treeView->is_multiselect				= false;
	treeView->is_selectable_groups			= false;
	ok_button->Enabled						= false;
}

void logic_event_link_chooser_form::treeView_selected_items_changed(System::Object^, controls::tree_view_selection_event_args^ )
{
	selected_path					= nullptr;
	selected_event					= nullptr;
	ok_button->Enabled				= false;

	m_events_list_box->Items->Clear();

	if(treeView->selected_node)
	{
		project_item_object^ itm	= dynamic_cast<project_item_object^>(treeView->selected_node->Tag);
		if ( itm )
		{
			selected_path		= itm->get_full_name( );
			for each( System::String^ event_name in ( safe_cast<object_scene^>( itm->get_object( ) ) )->get_event_names_list() )
			{
				m_events_list_box->Items->Add(event_name) ;
			}			
		}
	}
}

void logic_event_link_chooser_form::ok_button_Click(System::Object^, System::EventArgs^ )
{
	DialogResult = System::Windows::Forms::DialogResult::OK;
}

void logic_event_link_chooser_form::cancel_button_Click(System::Object^, System::EventArgs^ )
{
	DialogResult = System::Windows::Forms::DialogResult::Cancel;
}

void logic_event_link_chooser_form::m_events_list_box_SelectedIndexChanged(System::Object^  , System::EventArgs^ ) 
{
	selected_event = m_events_list_box->SelectedItem->ToString();
	ok_button->Enabled = (selected_event!=nullptr);
}



} //namespace editor
}//namespace xray