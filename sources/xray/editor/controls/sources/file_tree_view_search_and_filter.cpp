////////////////////////////////////////////////////////////////////////////
//	Created		: 22.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "file_tree_view.h"

using namespace System::IO;
using namespace System::Windows::Forms;

using System::Reflection::BindingFlags;

namespace xray
{
namespace editor
{
namespace controls
{

static void	expand_parent_line(TreeNode^ node)
{
	while(node->Parent != nullptr)
	{
		node = node->Parent;
		node->Expand();
	}
}

void		file_tree_view::filter_nodes		(TreeNodeCollection^ nodes, String^ filter)
{
	TreeNode^ node;

	for (int i=0;i<nodes->Count;++i)
	{
		node = nodes[i];
		if(node->ImageIndex == safe_cast<Int32>(file_tree_view::image_state::file))
		{
			if(!node->Text->ToLower()->Contains(filter))
			{
				m_invisible_nodes->Add(node->FullPath, node);
				nodes->RemoveAt(i--);
			}
			else
				if(auto_expand_on_filter)expand_parent_line(node);
		}
		else
		{
			filter_nodes(node->Nodes, filter);
			if(node->Nodes->Count == 0)
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

void		file_tree_view::filter_invisible_nodes(String^ filter)
{
	array<String^>^ keys				= gcnew array<String^>(m_invisible_nodes->Count);
	m_invisible_nodes->Keys->CopyTo		(keys, 0);
	array<String^>^ segments;

	for each(String^ key in keys)
	{
		segments = key->Split('\\');
		if(m_invisible_nodes[key]->Text->ToLower()->Contains(filter))
		{			
			segments[segments->Length-1]			= "";
			process_file_path(segments)->Nodes->Add	(m_invisible_nodes[key]);
			if(auto_expand_on_filter && filter != String::Empty)
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
			TreeNode^ new_node			=  process_file_path(key->Split('\\'));
			if(auto_expand_on_filter && filter != String::Empty)
				expand_parent_line(new_node);
			m_invisible_folders->Remove	(key);
		}
	}
}

void		file_tree_view::filter_text_changed	(System::Object^ sender, EventArgs^  e)
{
	String^ filter = safe_cast<TextBox^>(sender)->Text->ToLower()->Replace("/", "\\");

	if(filter!=String::Empty)
		filter_nodes(m_tree_view->Nodes, filter);
	filter_invisible_nodes(filter);
}

void		file_tree_view::tree_view_key_down		(System::Object^ sender , KeyEventArgs^  e)
{
	if(e->KeyCode == Keys::ControlKey)	m_b_control	= true;
	if(e->KeyCode == Keys::ShiftKey)	m_b_shift	= true;

	if(e->KeyCode == Keys::F && e->Control)
	{
		m_search_panel->Visible = true;
		m_search_textbox->Text	= "";
		m_search_textbox->Focus	();
	}
}

void		file_tree_view::tree_view_key_up		(System::Object^ sender , KeyEventArgs^  e)
{
	if(e->KeyCode == Keys::ControlKey)	m_b_control	= false;
	if(e->KeyCode == Keys::ShiftKey)	m_b_shift	= false;
}

void		file_tree_view::search_textbox_key_down	(System::Object^ sender , KeyEventArgs^  e)
{
	if(e->KeyCode == Keys::Enter)
	{
		m_finded_nodes->Clear();
		search_nodes(m_tree_view->Nodes, safe_cast<TextBox^>(sender)->Text->ToLower()->Replace("/", "\\"));
		m_tree_view->SelectedNode		= nullptr;

		if(m_finded_nodes->Count > 0)
		{
			m_result_label->Enabled		= true;
			m_search_next->Enabled		= true;
			m_search_result_index		= 0;

			m_result_label->Text		="Result: 1 / "+m_finded_nodes->Count.ToString();

			m_finded_nodes[0]->EnsureVisible();
			m_finded_nodes[0]->BackColor = Drawing::Color::Coral;
		}
		else
		{
			m_result_label->Enabled		= false;
			m_search_next->Enabled		=false;
			m_search_prev->Enabled		=false;
			m_finded_nodes->Clear		();
			m_search_result_index		= -1;
		}
	}
}

void		file_tree_view::search_nodes			(TreeNodeCollection^ nodes, String^ search_str)
{
	for each(TreeNode^ node in nodes)
	{
		node->BackColor = Drawing::Color::White;
		node->Collapse();
		
		if(node->Text->ToLower()->Contains(search_str))
		{
			node->BackColor = Drawing::Color::FromArgb(220, 255, 220);
			m_finded_nodes->Add(node);
			expand_parent_line(node);
		}
		search_nodes(node->Nodes, search_str);
	}
}

static void colorize_nodes(TreeNodeCollection^ nodes)
{
	for each(TreeNode ^ node in nodes)
	{
		node->BackColor = Drawing::Color::White;
		colorize_nodes(node->Nodes);
	}
}

void		file_tree_view::search_close_click		(System::Object^ sender , EventArgs^  e)
{
	if(m_search_panel->Visible)
	{
		m_search_panel->Visible = false;
		colorize_nodes(m_tree_view->Nodes);
	}

	m_result_label->Enabled		= false;
	m_search_next->Enabled		=false;
	m_search_prev->Enabled		=false;
	m_finded_nodes->Clear		();
	m_search_result_index		= -1;
}

void		file_tree_view::search_prev_click		(System::Object^ sender , EventArgs^  e)
{
	m_search_next->Enabled									= true;	
	m_finded_nodes[m_search_result_index]->BackColor		= Drawing::Color::FromArgb(220, 255, 220);
	m_finded_nodes[--m_search_result_index]->BackColor		= Drawing::Color::Coral;
	m_finded_nodes[m_search_result_index]->EnsureVisible	();
	m_tree_view->SelectedNode								= nullptr;
	m_result_label->Text									= "Result: "+(m_search_result_index+1).ToString()+" / "+m_finded_nodes->Count.ToString();
	if(m_search_result_index == 0) m_search_prev->Enabled	= false;
}	

void		file_tree_view::search_next_click		(System::Object^ sender , EventArgs^  e)
{
	m_search_prev->Enabled									= true;	
	m_finded_nodes[m_search_result_index]->BackColor		= Drawing::Color::FromArgb(220, 255, 220);
	m_finded_nodes[++m_search_result_index]->BackColor		= Drawing::Color::Coral;
	m_finded_nodes[m_search_result_index]->EnsureVisible	();
	m_tree_view->SelectedNode								= nullptr;
	m_result_label->Text									= "Result: "+(m_search_result_index+1).ToString()+" / "+m_finded_nodes->Count.ToString();
	if(m_search_result_index == m_finded_nodes->Count-1)	m_search_next->Enabled	= false;
}

}//namespace controls
}//namespace editor
}//namespace xray