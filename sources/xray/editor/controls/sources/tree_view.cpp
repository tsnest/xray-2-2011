////////////////////////////////////////////////////////////////////////////
//	Created		: 14.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_view.h"
#include "tree_node.h"
#include "tree_view_source.h"
#include "tree_view_search_panel.h"
#include "tree_view_filter_panel.h"

using namespace System::IO;
using namespace System::Windows::Forms;

using System::Reflection::BindingFlags;

namespace xray{
namespace editor{
namespace controls{

tree_view::~tree_view()
{
	if(m_source)
		delete m_source;
}

tree_view_source^		tree_view::source::get					()												
{
	return m_source;
}

void					tree_view::source::set					(tree_view_source^ value)						
{
	m_source = value;
	if(value != nullptr)
		value->parent = this;
}

Boolean					tree_view::auto_expand_on_filter::get	()												
{
	return m_filter_panel->m_auto_expand_on_filter;
}
void					tree_view::auto_expand_on_filter::set	(Boolean value)									
{
	m_filter_panel->m_auto_expand_on_filter = value;
}

Boolean					tree_view::filter_visible::get			()												
{
	return m_filter_panel->Visible;
}
void					tree_view::filter_visible::set			(Boolean value)									
{
	m_filter_panel->Visible = value;
}

void					tree_view::refresh						()												
{
	nodes->Clear();
	if(m_source != nullptr)
		m_source->refresh();
}

TreeNode^				tree_view::process_item_path			(array<String^>^ segments, Boolean create_path)	
{
	return process_item_path	(segments, create_path, -1, -1);
}

void					tree_view::on_items_loaded				()												
{
	items_loaded(this, EventArgs::Empty);
}

TreeNode^				tree_view::process_item_path			(array<String^>^ segments, Boolean create_path, Int32 image_index_collapsed, Int32 image_index_expanded)
{
	TreeNode^ node = root;

	for each(String^ segment in segments)
	{
		if(segment == "") continue;
		
		if(node->Nodes->ContainsKey(segment))
		{
			node							= safe_cast<tree_node^>(node->Nodes[segment]);
			continue;
		}
		else
		{
			if(!create_path)
				return nullptr;

			tree_node^ new_node			= gcnew tree_node(segment);

			new_node->m_image_index_collapsed	= image_index_collapsed;
			new_node->m_image_index_expanded	= image_index_expanded;
			new_node->Name						= segment;
			if(image_index_collapsed == -1)
			{
				new_node->ImageIndex			= 0;
				new_node->SelectedImageIndex	= 0;
			}
			else
			{
				new_node->ImageIndex			= image_index_collapsed;
				new_node->SelectedImageIndex	= image_index_collapsed;
			}
			new_node->m_node_type			= tree_node_type::group_item;
			//new_node->ContextMenuStrip		= m_nodes_context_menu;
			
			node->Nodes->Add				(new_node);
			node							= new_node;
		}
	}

	return node;
}

TreeNode^				tree_view::process_item_path			(array<String^>^ segments)						
{
	return process_item_path(segments, true);
}

TreeNode^				tree_view::process_item_path			(String^ segments, String^ separator)			
{
	return process_item_path(segments->Split(gcnew array<String^>{separator}, StringSplitOptions::RemoveEmptyEntries), true);
}

TreeNode^				tree_view::process_item_path			(String^ segments)								
{
	return process_item_path(segments, PathSeparator);
}

tree_node^ tree_view::add_item( String^ file_path )
{
	return add_item(file_path, -1);
}

tree_node^ tree_view::add_item( String^ file_path, Int32 image_index )
{
	array<String^>^		segments		= file_path->Split('/');
	String^				file_name		= segments[segments->Length-1];
	
	segments[segments->Length-1]		= "";
	TreeNode^	  nod					= nullptr;

	if(image_index == -1)
		nod			= process_item_path(segments);
	else
		nod			= process_item_path(segments, false);

	return add_item( nod, file_name, image_index );
}

tree_node^				tree_view::add_item						( tree_node^ destination, String^ item_name )
{
	return add_item( destination, item_name, -1 );
}

tree_node^				tree_view::add_item						( tree_node^ destination, String^ item_name, Int32 image_index )
{
	return add_item( safe_cast<TreeNode^>( destination ), item_name, image_index );
}

tree_node^				tree_view::add_group					( String^ folder_path )							
{
	return safe_cast<tree_node^>(process_item_path(folder_path->Split('/')));
}

tree_node^				tree_view::add_group					(String^ folder_path, Int32 image_index_collapsed, Int32 image_index_expanded)
{
	return safe_cast<tree_node^>(process_item_path(folder_path->Split('/'), true, image_index_collapsed, image_index_expanded));
}

tree_node^				tree_view::add_group					( tree_node^ destination, String^ group_name )
{
	return add_group( destination, group_name, -1, -1 );
}

tree_node^				tree_view::add_group					( tree_node^ destination, String^ group_name, Int32 image_index_collapsed, Int32 image_index_expanded )
{
	tree_node^ new_node			= gcnew tree_node( group_name );

	new_node->m_image_index_collapsed	= image_index_collapsed;
	new_node->m_image_index_expanded	= image_index_expanded;
	new_node->Name						= group_name;
	if( image_index_collapsed == -1 )
	{
		new_node->ImageIndex			= 0;
		new_node->SelectedImageIndex	= 0;
	}
	else
	{
		new_node->ImageIndex			= image_index_collapsed;
		new_node->SelectedImageIndex	= image_index_collapsed;
	}
	new_node->m_node_type			= tree_node_type::group_item;
	//new_node->ContextMenuStrip		= m_nodes_context_menu;
	
	destination->nodes->Add				( new_node );

	return new_node;
}

void					tree_view::remove_item					(String^ folder_path)							
{
	TreeNode^ node = process_item_path(folder_path->Split('/'), false);
	if(node)
		node->Remove();
}

void					tree_view::remove_group					(String^ group_path)							
{
	remove_item	( group_path);
}

void					tree_view::remove_item					( tree_node^ item_node )
{
	item_node->Remove	( );
}

void					tree_view::remove_group					( tree_node^ group_node )
{
	group_node->Remove	( );
}

void					tree_view::change_item_context			(String^ file_path, Object^ context)			
{
	array<String^>^		segments		= file_path->Split('/');
	String^				file_name		= segments[segments->Length-1];

	segments[segments->Length-1]		= "";

	TreeNode^ dir = process_item_path(segments, false);
	if(dir && dir->Nodes->ContainsKey(file_name))
		dir->Nodes[file_name]->Tag = context;
}

void					tree_view::OnAfterExpand				(System::Windows::Forms::TreeViewEventArgs^  e)	
{
	tree_node^ node = safe_cast<tree_node^>(e->Node);
	if(node->m_image_index_collapsed != -1)
	{
		e->Node->ImageIndex				= node->m_image_index_expanded;
		e->Node->SelectedImageIndex		= node->m_image_index_expanded;		
	}

	TreeView::OnAfterExpand(e);
}

void					tree_view::OnAfterCollapse				(System::Windows::Forms::TreeViewEventArgs^  e)	
{
	tree_node^ node = safe_cast<tree_node^>(e->Node);
	if(node->m_image_index_collapsed != -1)
	{
		e->Node->ImageIndex				= node->m_image_index_collapsed;
		e->Node->SelectedImageIndex		= node->m_image_index_collapsed;		
	}

	TreeView::OnAfterCollapse(e);
}

void					tree_view::track_active_node			(System::String^ full_path)						
{
	tree_node^	node = get_node(full_path);
	if(node != nullptr)
	{
		List< tree_node^ > old_list	( m_selected_nodes );
		deselect_nodes( m_selected_nodes );
		select_node_impl( node );
		node->EnsureVisible( );
		on_selected_items_changed( %old_list );
	}
}

void					tree_view::track_active_nodes			(List<System::String^>^ full_paths)						
{
	deselect_nodes( m_selected_nodes );
	for each(System::String^ node_name in full_paths)
	{
		tree_node^ node = get_node(node_name);
		if(node != nullptr)
		{
			List< tree_node^ > old_list( m_selected_nodes );
			select_node_impl( node );
			node->EnsureVisible( );
			on_selected_items_changed( %old_list );
			if(!is_multiselect)
				return;
		}
	}
}

tree_node^				tree_view::get_node						(String^ full_path)								
{
	array<System::String^ >^ segments = full_path->Split('/');
	TreeNode^ node = root;
	for each(String^ segment in segments)
	{
		if(segment == "") 
			continue;

		if(node->Nodes->ContainsKey(segment))
		{
			node						= node->Nodes[segment];
			continue;
		}
		return nullptr;
	}
	if(node == root)
		return nullptr;
	return safe_cast<tree_node^>(node);
}

void					tree_view::OnNodeMouseClick				(System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	if( e->Button == System::Windows::Forms::MouseButtons::Right && !selected_nodes->Contains( safe_cast<tree_node^>( e->Node ) ) )
		SelectedNode = e->Node;
	TreeView::OnNodeMouseClick(e);
}

//void					tree_view::create_item					(System::Object^ sender, System::EventArgs^ e)	
//{
//	item_create(this, gcnew tree_view_event_args());
//}

//void					tree_view::create_group					(System::Object^ sender, System::EventArgs^ e)	
//{
//	group_create(this, gcnew tree_view_event_args());
//}

//void					tree_view::remove_item					(System::Object^ sender, System::EventArgs^ e)	
//{
//	item_remove(this, gcnew tree_view_event_args());
//}

void					tree_view::add_items					( GObjIList^ items )				
{
	for each(String^ item in items)
	{
		add_item(item);
	}
}
void					tree_view::add_items					(array<Object^>^ items)							
{
	for each(String^ item in items)
	{
		add_item(item);
	}
}

void					tree_view::OnKeyDown					(KeyEventArgs^  e)								
{
	if(e->KeyCode == Keys::F && e->Control)
	{
		m_search_panel->Visible							= true;
		m_search_panel->Enabled							= true;
		m_search_panel->m_search_text_box->Text			= "";
		m_search_panel->m_search_text_box->Focus		();
		m_search_panel->m_search_next->Enabled			= false;
		m_search_panel->m_search_next->Enabled			= false;
		e->Handled										= true;
	}
	else if( is_multiselect && e->KeyCode == Keys::A && e->Control )
	{
			select_all();
	}
	else if( m_last_selected_node != nullptr )
	{
		if( e->KeyCode == Keys::Up )
			on_after_select( safe_cast<tree_node^>( m_last_selected_node->PrevVisibleNode ) );
		else if( e->KeyCode == Keys::Down )
			on_after_select( safe_cast<tree_node^>( m_last_selected_node->NextVisibleNode ) );
		else if( e->KeyCode == Keys::Left )
			m_last_selected_node->Collapse( );
		else if( e->KeyCode == Keys::Right )
			m_last_selected_node->Expand( );
	}
	else if( m_last_selected_node == nullptr )
	{
		if( ( e->KeyCode == Keys::Up || e->KeyCode == Keys::Down ) && nodes->Count > 0 )
			select_node_impl( safe_cast<tree_node^>( nodes[0] ) );
	}

	if( keyboard_search_enabled )
		TreeView::OnKeyDown( e );
}

void					tree_view::OnKeyUp						(KeyEventArgs^  e)
{
	TreeView::OnKeyUp(e);
}


void					tree_view::clear						()
{
	nodes->Clear();
	m_selected_nodes->Clear();
}

tree_node^				tree_view::add_item						( TreeNode^ destination, String^ item_name, Int32 image_index )
{
	tree_node^ node				= gcnew tree_node( item_name );
	node->Tag					= nullptr;
	if( image_index == -1 )
	{
		node->ImageIndex			= 2;
		node->SelectedImageIndex	= 2;
	}
	else
	{
		node->ImageIndex				= image_index;
		node->SelectedImageIndex		= image_index;
		node->m_image_index_expanded	= image_index;
		node->m_image_index_collapsed	= image_index;
	}
	node->m_node_type					= tree_node_type::single_item;
	//node->ContextMenuStrip				= m_nodes_context_menu;

	destination->Nodes->Add				( node );
	return								node;
}


}//namespace controls	
}//namespace editor
}//namespace xray