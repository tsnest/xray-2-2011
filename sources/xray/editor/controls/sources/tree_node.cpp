////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_node.h"
#include "tree_view.h"

namespace xray {
namespace editor {
namespace controls{

tree_node^		tree_node::add_node_single			(String^ name)
{
	return add_node_single(name, -1);
}

tree_node^		tree_node::add_node_single			(String^ name, Int32 image_index)
{
	tree_node^ node				= gcnew tree_node(name);
	if(image_index == -1)
	{
		node->ImageIndex			= 2;
		node->SelectedImageIndex	= 2;
	}
	else
	{
		node->ImageIndex			= image_index;
		node->SelectedImageIndex	= image_index;
	}
	node->m_node_type				= tree_node_type::single_item;
	node->ContextMenuStrip			= ContextMenuStrip;

	nodes->Add						(node);
	return node;
}

tree_node^		tree_node::add_node_file_part			(String^ name)
{
	tree_node^ node				= gcnew tree_node(name);
	node->ImageIndex			= 2;
	node->SelectedImageIndex	= 2;
	
	node->m_node_type				= tree_node_type::file_part_item;
	
	nodes->Add						(node);
	return node;
}

tree_node^		tree_node::add_node_group			(String^ name)
{
	return add_node_group(name, -1, -1);
}

tree_node^ tree_node::add_node_group			(String^ name, Int32 image_index_expanded, Int32 image_index_collapsed)
{
	tree_node^ new_node					= gcnew tree_node(name);

	new_node->m_image_index_collapsed	= image_index_collapsed;
	new_node->m_image_index_expanded	= image_index_expanded;
	new_node->Name						= name;

	if(image_index_collapsed == -1)
	{
		new_node->ImageIndex			= static_cast<int>(0);
		new_node->SelectedImageIndex	= static_cast<int>(0);
	}
	else
	{
		new_node->ImageIndex			= image_index_collapsed;
		new_node->SelectedImageIndex	= image_index_collapsed;
	}
	new_node->m_node_type			= tree_node_type::group_item;
	new_node->ContextMenuStrip		= ContextMenuStrip;
	
	nodes->Add						(new_node);
	return new_node;
}

bool tree_node::get_selected_impl( )
{
	if ( TreeView == nullptr )
		return false;
	tree_view^ tv = safe_cast<tree_view^>(TreeView);
	return tv->selected_nodes->Contains( this );
}

void tree_node::set_selected_impl( bool b )
{
	if(get_selected_impl()==b)
		return;

	tree_view^ tv	= safe_cast<tree_view^>(TreeView);
	tv->select_node	( this, b, false );
}

}//namespace controls
}//namespace editor
}//namespace xray