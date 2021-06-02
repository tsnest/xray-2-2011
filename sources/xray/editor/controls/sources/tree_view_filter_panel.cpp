////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_view_filter_panel.h"
#include "tree_view.h"
#include "tree_node.h"

namespace xray {
namespace editor {
namespace controls{


void	tree_view_filter_panel::in_constructor()
{
	m_invisible_nodes		= gcnew Collections::Generic::Dictionary<String^, tree_node^>();
	m_invisible_folders		= gcnew Collections::Generic::Dictionary<String^, String^>();
}

static void	expand_parent_line(TreeNode^ node)
{
	while(node->Parent != nullptr)
	{
		node = node->Parent;
		node->Expand();
	}
}

void		tree_view_filter_panel::filter_nodes		(TreeNodeCollection^ nodes, String^ filter)
{
	tree_node^ node;

	for (int i=0;i<nodes->Count;++i)
	{
		node = safe_cast<tree_node^>(nodes[i]);
		if(safe_cast<tree_node^>(node)->m_node_type == tree_node_type::single_item)
		{
			if(!node->Text->ToLower()->Contains(filter))
			{
				m_invisible_nodes->Add(node->FullPath, node);
				nodes->RemoveAt(i--);
			}
			else
				if(m_auto_expand_on_filter)expand_parent_line(node);
		}
		else
		{
			filter_nodes(node->nodes, filter);
			if(node->nodes->Count == 0)
			{
				if(!node->Text->ToLower()->Contains(filter))
				{
					m_invisible_folders->Add(node->FullPath, node->Text);
					nodes->RemoveAt(i--);
					node->Collapse();
				}
			}
		}
	}
}

void		tree_view_filter_panel::filter_invisible_nodes(String^ filter)
{
	array<String^>^ keys				= gcnew array<String^>(m_invisible_nodes->Count);
	m_invisible_nodes->Keys->CopyTo		(keys, 0);
	array<String^>^ segments;

	for each(String^ key in keys)
	{
		segments = key->Split('/');
		if(m_invisible_nodes[key]->Text->ToLower()->Contains(filter))
		{			
			segments[segments->Length-1]			= "";
			m_parent_tree_view->process_item_path(segments)->Nodes->Add	(m_invisible_nodes[key]);
			if(m_auto_expand_on_filter && filter != String::Empty)
				expand_parent_line(m_invisible_nodes[key]);
			m_invisible_nodes->Remove				(key);
		}
	}

	keys								= gcnew array<String^>(m_invisible_folders->Count);
	m_invisible_folders->Keys->CopyTo	(keys, 0);

	for each(String^ key in keys)
	{
		if(m_invisible_folders[key]->ToLower()->Contains(filter))
		{			
			TreeNode^ new_node		=  m_parent_tree_view->process_item_path(key->Split('/'));
			if(m_auto_expand_on_filter && filter != String::Empty)
				expand_parent_line(new_node);
			m_invisible_folders->Remove	(key);
		}
	}
}

void		tree_view_filter_panel::filter_text_changed	(System::Object^ sender, EventArgs^  e)
{
	String^ filter = safe_cast<TextBox^>(sender)->Text->ToLower();

	if(filter!=String::Empty)
		filter_nodes(m_parent_tree_view->nodes, filter);
	filter_invisible_nodes(filter);
	
	m_parent_tree_view->Sort();
}


}//namespace controls
}//namespace editor
}//namespace xray