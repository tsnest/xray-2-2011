////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tree_view_search_panel.h"


using namespace System;
using namespace System::Windows::Forms;

namespace xray {
namespace editor {
namespace controls{

void		tree_view_search_panel::in_constructor				()
{
	m_finded_nodes			= gcnew Collections::Generic::List<TreeNode^>();
	m_search_next->Enabled	= false;
	m_search_prev->Enabled	= false;
}

static void	expand_parent_line										(TreeNode^ node)
{
	while(node->Parent != nullptr)
	{
		node = node->Parent;
		node->Expand();
	}
}

void		tree_view_search_panel::m_search_text_box_key_down	(System::Object^ sender , KeyEventArgs^  e)
{
	if(e->KeyCode == Keys::Enter)
	{
		m_finded_nodes->Clear();
		String^ search_str = safe_cast<TextBox^>(sender)->Text->ToLower();
		if(search_str != "")
			search_nodes(m_parent_tree_view->Nodes, search_str);
		m_parent_tree_view->SelectedNode		= nullptr;

		if(m_finded_nodes->Count == 0)
			MessageBox::Show("There is no matches!");
		else if(m_finded_nodes->Count == 1)
			m_result_label->Text		="Result: 1 / 1";
		else if(m_finded_nodes->Count > 1)
		{
			m_result_label->Enabled		= true;
			m_search_next->Enabled		= true;
			m_search_result_index		= 0;

			m_result_label->Text		="Result: 1 / "+m_finded_nodes->Count.ToString();

			m_finded_nodes[0]->EnsureVisible();
			m_selected_node_prev_color		= m_finded_nodes[0]->BackColor;
			m_finded_nodes[0]->BackColor	= Drawing::Color::Coral;
		}
		else
		{
			m_result_label->Enabled		= false;
			m_search_next->Enabled		= false;
			m_search_prev->Enabled		= false;
			m_finded_nodes->Clear		();
			m_search_result_index		= -1;
		}
	}
	if(Visible == true && e->KeyCode == Keys::Escape)
	{
		close();
		e->Handled = true;
	}
}

void		tree_view_search_panel::search_button_click		(System::Object^ sender, EventArgs^ )
{
	m_search_text_box_key_down(m_search_text_box, gcnew KeyEventArgs(Keys::Enter));
}

void		tree_view_search_panel::search_nodes			(TreeNodeCollection^ nodes, String^ search_str)
{
	for each(TreeNode^ node in nodes)
	{
		node->BackColor = Drawing::Color::White;
		node->Collapse();
		
		if( node->Text->ToLower()->Contains( search_str ) || node->FullPath->ToLower()->Contains( search_str ) )
		{
			node->BackColor = Drawing::Color::FromArgb(220, 255, 220);
			m_finded_nodes->Add(node);
			expand_parent_line(node);
		}
		search_nodes(node->Nodes, search_str);
	}
}

static void colorize_nodes(TreeNodeCollection^ nodes, Color fore_color, Color back_color)
{
	for each(TreeNode ^ node in nodes)
	{
		node->ForeColor = fore_color;
		node->BackColor = back_color;
		colorize_nodes(node->Nodes, fore_color, back_color);
	}
}

void		tree_view_search_panel::search_close_click		(System::Object^ sender , EventArgs^  e)
{
	close();
}

void		tree_view_search_panel::search_prev_click		(System::Object^ sender , EventArgs^  e)
{
	m_search_next->Enabled									= true;
	if(m_finded_nodes[m_search_result_index]->BackColor == Drawing::Color::Coral)
		m_finded_nodes[m_search_result_index]->BackColor		= m_selected_node_prev_color;
	m_selected_node_prev_color								= m_finded_nodes[--m_search_result_index]->BackColor;
	m_finded_nodes[m_search_result_index]->BackColor		= Drawing::Color::Coral;
	m_finded_nodes[m_search_result_index]->EnsureVisible	();
	m_parent_tree_view->SelectedNode						= nullptr;
	m_result_label->Text									= "Result: "+(m_search_result_index+1).ToString()+" / "+m_finded_nodes->Count.ToString();
	if(m_search_result_index == 0) m_search_prev->Enabled	= false;
}	

void		tree_view_search_panel::search_next_click		(System::Object^ sender , EventArgs^  e)
{
	m_search_prev->Enabled									= true;	
	if(m_finded_nodes[m_search_result_index]->BackColor == Drawing::Color::Coral)
		m_finded_nodes[m_search_result_index]->BackColor		= m_selected_node_prev_color;
	m_selected_node_prev_color								= m_finded_nodes[++m_search_result_index]->BackColor;
	m_finded_nodes[m_search_result_index]->BackColor		= Drawing::Color::Coral;
	m_finded_nodes[m_search_result_index]->EnsureVisible	();
	m_parent_tree_view->SelectedNode						= nullptr;
	m_result_label->Text									= "Result: "+(m_search_result_index+1).ToString()+" / "+m_finded_nodes->Count.ToString();
	if(m_search_result_index == m_finded_nodes->Count-1)	m_search_next->Enabled	= false;
}

void		tree_view_search_panel::close					()
{
	if(Visible)
	{
		Visible = false;
		colorize_nodes(m_parent_tree_view->Nodes, m_parent_tree_view->ForeColor, m_parent_tree_view->BackColor);
	}

	m_result_label->Enabled		= false;
	m_search_next->Enabled		= false;
	m_search_prev->Enabled		= false;
	m_finded_nodes->Clear		();
	m_search_result_index		= -1;
	m_result_label->Text		= "Results: - / -";
}

}//namespace controls
}//namespace editor
}//namespace xray