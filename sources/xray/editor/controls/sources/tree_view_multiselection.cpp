////////////////////////////////////////////////////////////////////////////
//	Created		: 22.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_view.h"
#include "tree_view_select_action.h"
#include "tree_node.h"

using namespace System::IO;
using namespace System::Windows::Forms;
using System::Reflection::BindingFlags;

namespace xray {
namespace editor {
namespace controls{

void		tree_view::select_node_impl( tree_node^ node )
{
	if( !m_selected_nodes->Contains( node ) )
	{
		node->m_fore_color	= node->ForeColor;
		node->m_back_color	= node->BackColor;
		node->BackColor		= Drawing::SystemColors::Highlight;
		node->ForeColor		= Drawing::SystemColors::HighlightText;
		m_selected_nodes->Add( node );
		Invalidate	( node->Bounds );
	}
	m_last_selected_node = node;
}

void		tree_view::select_sub_nodes(tree_node^ node)
{
	for each( tree_node^ sub_node in node->nodes )
	{
		select_node_impl( sub_node );
		select_sub_nodes( sub_node );
	}
}

void tree_view::deselect_node_impl(tree_node^ node)
{
	tree_node^ nod		= safe_cast<tree_node^>(node);

	if(m_selected_nodes->Contains(nod))
	{
		node->BackColor		= nod->m_back_color;
		node->ForeColor		= nod->m_fore_color;
		m_selected_nodes->Remove(node);
		Invalidate	(node->Bounds);
	}
}

void tree_view::deselect_nodes( List<tree_node^>^ nodes )
{
	for( int i = nodes->Count - 1; i >= 0; --i )
		deselect_node_impl( nodes[i] );

	m_last_selected_node = nullptr;
}

void		tree_view::OnMouseDown				( MouseEventArgs^ e )
{
	tree_node^ node_under_mouse = safe_cast<tree_node^>( GetNodeAt( e->Location ) );

	if( e->Button == Windows::Forms::MouseButtons::Right )
	{
		if( selected_nodes->Contains( node_under_mouse ) )
			m_b_right_clicked = true;
	}

	if( node_under_mouse != nullptr )
	{
		if( selected_nodes->Contains( node_under_mouse ) )
		{
			m_b_in_selection_mouse_down = true;
		}
		else
		{
			m_b_in_selection_mouse_down = false;
			if( on_before_select( node_under_mouse, tree_view_select_action::by_mouse ) )
			on_after_select( node_under_mouse );
		}
	}
	else
	{
		List< tree_node^ > old_list	( m_selected_nodes );
		deselect_nodes				( m_selected_nodes );
		on_selected_items_changed	( %old_list );
	}
	TreeView::OnMouseDown	( e );
}

void		tree_view::OnMouseUp				( MouseEventArgs^ e )
{
	tree_node^ node_under_mouse = safe_cast<tree_node^>( GetNodeAt( e->Location ) );

	if( node_under_mouse != nullptr )
	{
		if( m_b_in_selection_mouse_down )
		{
			m_b_in_selection_mouse_down = false;
			if( on_before_select( node_under_mouse, tree_view_select_action::by_mouse ) )
			on_after_select( node_under_mouse );
		}
	}

	m_b_right_clicked		= false;
	TreeView::OnMouseUp		( e );
}

void tree_view::select_node( tree_node^ node, bool value, bool exclusive )
{
	if(value)
	{
		if(exclusive)
			deselect_nodes			( m_selected_nodes );

		m_type_of_selected_nodes = node->m_node_type;
		select_node_impl		( node );
	}else
	{
		deselect_node_impl		( node );
	}

//.	on_selected_items_changed	( );
}

bool		tree_view::on_before_select			( tree_node^ node, tree_view_select_action action  )
{
	if( action == tree_view_select_action::by_keyboard && !keyboard_search_enabled )
		return false;

	if( m_b_right_clicked || ( !is_selectable_groups && node->m_node_type == tree_node_type::group_item ) )
		return false;

	if( is_multiselect )
	{
		if( ModifierKeys == Keys::Control && m_selected_nodes->Contains( node ) )
		{
			List< tree_node^ > old_list	( m_selected_nodes );
			deselect_node_impl			( node );
			on_selected_items_changed	( %old_list );
			return false;
		}
	}

	return true;
}

void		tree_view::on_after_select			( tree_node^ node )
{
	if( node == nullptr )
		return;

	if( !is_multiselect )
	{
		List< tree_node^ > old_list	( m_selected_nodes );
		deselect_nodes				( m_selected_nodes );
		select_node_impl			( node );
		on_selected_items_changed	( %old_list );
		return;
	}

	if( ModifierKeys == Keys::Control && !m_selected_nodes->Contains( node ) )
	{
		if(	is_selection_or_groups_or_items && m_selected_nodes->Count > 0 &&
			node->m_node_type != m_type_of_selected_nodes )
		return;
		
		m_type_of_selected_nodes	= node->m_node_type; 
		List< tree_node^ > old_list	( m_selected_nodes );
		select_node_impl			( node );
		on_selected_items_changed	( %old_list );
		return;
	}
	else if( !( ModifierKeys == Keys::Shift ) || ( ModifierKeys == Keys::Control && m_last_selected_node == nullptr ) )
	{
		List< tree_node^ > old_list	( m_selected_nodes );
		deselect_nodes				( m_selected_nodes );
		m_type_of_selected_nodes	= node->m_node_type;
		select_node_impl			( node );
		on_selected_items_changed	( %old_list );
		return;
	}

	List< tree_node^ > old_list	( m_selected_nodes );
	if( m_last_selected_node != nullptr )
	{
		//Shift pressed
		tree_node^ lower_node;
		tree_node^ upper_node;
		tree_node^ prev_lower_node;

		//Set upper - lower nodes
		if( m_last_selected_node->Bounds.Y < node->Bounds.Y )
		{
			upper_node 			= m_last_selected_node;
			lower_node 			= node;
		}
		else
		{
			upper_node 			= node;
			lower_node 			= m_last_selected_node;
		}
		prev_lower_node			= lower_node;

		while( lower_node != nullptr && lower_node != upper_node )
		{
			if( !(	( !is_selectable_groups && node->m_node_type == tree_node_type::group_item ) || 
					( is_selection_or_groups_or_items && node->m_node_type != m_type_of_selected_nodes ) ) )
				select_node_impl	( lower_node );

			prev_lower_node		= lower_node;
			lower_node			= safe_cast<tree_node^>( lower_node->PrevVisibleNode );
		}
		if( lower_node != nullptr )
			select_node_impl( lower_node );
		
		m_last_selected_node = node;
	}
	on_selected_items_changed( %old_list );
}

void		tree_view::OnBeforeSelect			(TreeViewCancelEventArgs^ e)
{
	e->Cancel = true;
	return;
}

void		tree_view::on_selected_items_changed( List<tree_node^>^ old_selection )
{
	selected_items_changed( this, gcnew tree_view_selection_event_args( old_selection, m_selected_nodes ) );
	if( m_selected_nodes->Count != 0 )
		OnAfterSelect( gcnew TreeViewEventArgs( m_selected_nodes[0] ) );
}

void		tree_view::select_all				( )
{
	if(nodes->Count==0)
		return;

	List< tree_node^ > old_list	( m_selected_nodes );

	for( TreeNode^ node = nodes[0]; node != nullptr; node = node->NextVisibleNode)
	{
		if(!(!is_selectable_groups && safe_cast<tree_node^>( node )->m_node_type == tree_node_type::group_item))
			select_node_impl	( safe_cast<tree_node^>( node ) );
	}

	on_selected_items_changed( %old_list );
}
void		tree_view::deselect_all				( )
{
	if( nodes->Count == 0 )
		return;

	List< tree_node^ > old_list	( m_selected_nodes );

	for( Int32 i = old_list.Count - 1; i >= 0; --i )
	{
		TreeNode^ node = old_list[i];
		if( node->TreeView != nullptr )
			deselect_node_impl	( safe_cast<tree_node^>( node ) );
		else
			old_list.RemoveAt( i );
	}

	m_selected_nodes->Clear		( );
	on_selected_items_changed	( %old_list );
}

}//namespace controls
}//namespace editor
}//namespace xray