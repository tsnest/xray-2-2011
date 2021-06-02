////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_in_compound_chooser_form.h"
#include "project.h"
#include "object_base.h"
#include "project_tab.h"

namespace xray{
namespace editor{

extern void copy_nodes( TreeNodeCollection^ dest, TreeNodeCollection^ src, System::String^ filter );
// {
// 	for each (TreeNode^ n in src)
// 	{
// 		xray::editor::controls::tree_node^ src_node = safe_cast<xray::editor::controls::tree_node^>(n);
// 		xray::editor::controls::tree_node^ dst_node = gcnew xray::editor::controls::tree_node(src_node->Name);
// 
// 		if(filter)
// 		{
// 			project_item_base^ itm			= dynamic_cast<project_item_base^>(src_node->Tag);
// 			if(itm)
// 			{
// 				object_base^ o			= itm->get_object();
// 				if( o )
// 				{
// 					System::String^ object_type	= o->GetType()->ToString();
// 					if(object_type!= filter)
// 						continue;
// 				}
// 			}
// 		}
// 
// 		dst_node->Text					= src_node->Text;
// 		dst_node->ImageIndex			= src_node->ImageIndex;
// 		dst_node->SelectedImageIndex	= src_node->SelectedImageIndex;
// 		dst_node->Tag					= src_node->Tag;
// 		dest->Add						( dst_node );
// 		copy_nodes						( dst_node->nodes, src_node->nodes, filter );
// 	}
// }

void object_in_compound_chooser_form::initialize( project^ p, System::String^ initial, System::String^ f )
{
	cli::array<System::String^>^ initial_values = initial->Split(',');

	System::String^ root_path = initial_values[0];
	initial = initial_values[1];

	xray::editor::controls::tree_view^ tree	= p->ui()->treeView;

	tree_node^ n		= p->find_by_path(root_path, true)->m_tree_node_;

	
	treeView->ImageList						= tree->ImageList;
	copy_nodes								( treeView->nodes, n->nodes, f );

	if(initial)
	{
		tree_node^ n		= find_hierrarchy( tree->nodes, initial );
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

void object_in_compound_chooser_form::treeView_selected_items_changed(System::Object^, controls::tree_view_selection_event_args^ )
{
	selected_path					= nullptr;

	if(treeView->selected_node)
	{
		project_item_object^ itm	= dynamic_cast<project_item_object^>(treeView->selected_node->Tag);
		if(itm)
			selected_path		= itm->get_full_name();
	}

	ok_button->Enabled = (selected_path!=nullptr);
}

void object_in_compound_chooser_form::ok_button_Click(System::Object^, System::EventArgs^ )
{
	DialogResult = System::Windows::Forms::DialogResult::OK;
}

void object_in_compound_chooser_form::cancel_button_Click(System::Object^, System::EventArgs^ )
{
	DialogResult = System::Windows::Forms::DialogResult::Cancel;
}

}
}


