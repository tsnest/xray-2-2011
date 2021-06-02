////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_precondition_action_editor.h"
#include "dialog_graph_link.h"
#include "dialog_link_precondition.h"
#include "dialog_link_action.h"
#include "dialog_operation.h"

using xray::dialog_editor::dialog_precondition_action_editor;
using xray::dialog_editor::precondition_action_editor_sorter;
using xray::dialog_editor::dialog_link_ui_type_editor;
using xray::dialog_editor::dialog_graph_link;
using xray::dialog_editor::dialog_link_precondition;
using xray::dialog_editor::dialog_link_action;
using xray::editor::controls::AlphanumComparator;
using namespace System::ComponentModel;

using xray::dialog_editor::dialog_expression;
////////////////////////////////////////////////////////////////////////////
//- class precondition_action_editor_sorter -------------------------------------------------
////////////////////////////////////////////////////////////////////////////
int precondition_action_editor_sorter::Compare(Object^ x, Object^ y)
{
	TreeNode^ left = safe_cast<TreeNode^>(x);
	TreeNode^ right = safe_cast<TreeNode^>(y);
	AlphanumComparator^ comparer = gcnew AlphanumComparator();
	if(left->Name=="preconditions")
		return -1;

	if(left->Name=="actions")
		return 1;

	return comparer->Compare(left->Name, right->Name);
}
////////////////////////////////////////////////////////////////////////////
//- class dialog_precondition_action_editor -------------------------------------------------
////////////////////////////////////////////////////////////////////////////
void dialog_precondition_action_editor::in_constructor()
{
	treeView1->TreeViewNodeSorter = gcnew precondition_action_editor_sorter();
	m_act_props = gcnew AttributeCollection(gcnew CategoryAttribute("Action properties"));
	m_prec_props = gcnew AttributeCollection(gcnew CategoryAttribute("Precondition properties"));
	m_type_props = gcnew AttributeCollection(gcnew dialog_link_precondition_attribute());
	m_changed = false;
	m_act_vis = true;
	m_prec_vis = true;
	m_prec_node = gcnew System::Windows::Forms::TreeNode(L"preconditions");
	m_prec_node->Name = L"preconditions";
	m_prec_node->Text = L"preconditions";
	m_act_node = gcnew System::Windows::Forms::TreeNode(L"actions");
	m_act_node->Name = L"actions";
	m_act_node->Text = L"actions";
}

void dialog_precondition_action_editor::on_form_closing(System::Object^ , System::Windows::Forms::FormClosingEventArgs^ e)
{
	if(e->CloseReason==System::Windows::Forms::CloseReason::UserClosing)
	{
		this->Hide();
		e->Cancel = true;
	}
}

void dialog_precondition_action_editor::make_nodes_from_preconditions(dialog_expression* n, TreeNodeCollection^ root)
{
	dialog_link_precondition^ pr = gcnew dialog_link_precondition(n);
	add_link_precondition_with_name(pr, root);
	if(pr->type!=dialog_link_precondition::e_type::Precondition)
	{
		dialog_operation* oper = dynamic_cast<dialog_operation*>(n);
		if(oper)
		{
			xray::dialog_editor::vector<dialog_expression*>::iterator b = oper->childs()->begin();
			for(; b!=oper->childs()->end(); ++b)
				make_nodes_from_preconditions(*b, pr->Nodes);
		}
	}
}

void dialog_precondition_action_editor::add_link_precondition_with_name(dialog_link_precondition^ pr, TreeNodeCollection^ root)
{
	String^ nm;
	if(pr->type==dialog_link_precondition::e_type::And)
		nm = "AND";
	else if(pr->type==dialog_link_precondition::e_type::Or)
		nm = "OR";
	else if(pr->type==dialog_link_precondition::e_type::Not)
		nm = "NOT";
	else
		nm = "FUNCTION";

	int i = 0;
	while(root->ContainsKey(nm+"_"+i))
		++i;

	pr->Name = nm+"_"+i;
	if(pr->type==dialog_link_precondition::e_type::And || pr->type==dialog_link_precondition::e_type::Or ||
		pr->type==dialog_link_precondition::e_type::Not)
		pr->Text = nm;
	else
		pr->Text = pr->func_names.ToString()+" ("+pr->param1+")";

	root->Add(pr);
	treeView1->Sort();
	select_node(pr);
	if(pr->Parent)
		pr->Parent->Expand();
}

void dialog_precondition_action_editor::add_link_action_with_name(dialog_link_action^ act, TreeNodeCollection^ root)
{
	String^ nm = "ACTION_";
	int i = 0;
	while(root->ContainsKey(nm+i))
		++i;

	act->Name = nm+i;
	act->Text = act->func_names.ToString()+" ("+act->param1+")";
	root->Add(act);
	treeView1->Sort();
	select_node(act);
	if(act->Parent)
		act->Parent->Expand();
}

void dialog_precondition_action_editor::show(dialog_graph_link^ lnk, bool act_vis, bool prec_vis)
{
	m_link = lnk;
	treeView1->Nodes->Clear();
	m_prec_node->Nodes->Clear();
	m_act_node->Nodes->Clear();
	m_act_vis = act_vis;
	m_prec_vis = prec_vis;
	TreeNodeCollection^ action_root = treeView1->Nodes;
	TreeNodeCollection^ precondition_root = treeView1->Nodes;
	if(m_act_vis && m_prec_vis)
	{
		treeView1->Nodes->Add(m_act_node);
		action_root = m_act_node->Nodes;
		treeView1->Nodes->Add(m_prec_node);
		precondition_root = m_prec_node->Nodes;
	}
	else if(m_act_vis)
		precondition_root = m_prec_node->Nodes;
	else if(m_prec_vis)
		action_root = m_act_node->Nodes;

	vector<dialog_action*>::iterator b = m_link->m_link->actions()->begin();
	for(; b!=m_link->m_link->actions()->end(); ++b)
	{
		dialog_link_action^ act = gcnew dialog_link_action(*b);
		add_link_action_with_name(act, action_root);
	}

	dialog_expression* expr = m_link->m_link->root_precondition();
	if(expr!=NULL)
		make_nodes_from_preconditions(expr, precondition_root);

	if(treeView1->Nodes->Count>0)
	{
		select_node(treeView1->Nodes[0]);
		bool b = (treeView1->SelectedNode==m_prec_node);
		button1->Enabled = b;
		button2->Enabled = b;
		button3->Enabled = b;
		button4->Enabled = b;
		button5->Enabled = !b;
	}
	else
	{
		button1->Enabled = prec_vis;
		button2->Enabled = prec_vis;
		button3->Enabled = prec_vis;
		button4->Enabled = prec_vis;
		button5->Enabled = m_act_vis;
	}

	show_properties(treeView1->SelectedNode);
	this->ShowDialog();
}

TreeNodeCollection^ dialog_precondition_action_editor::get_root_and_check_prereq(System::String^ nm)
{
	TreeNodeCollection^ root;
	if(treeView1->Nodes->Count==0)
	{
		root = treeView1->Nodes;
		return root;
	}

	if(treeView1->SelectedNode==nullptr)
		treeView1->SelectedNode = treeView1->Nodes[0];

	dialog_link_precondition^ n = dynamic_cast<dialog_link_precondition^>(treeView1->SelectedNode);
	root = treeView1->SelectedNode->Nodes;

	if(treeView1->SelectedNode==m_prec_node && root->Count>0)
	{
		System::Windows::Forms::MessageBox::Show(this, "Can't add more than one node inside root 'preconditions'!", 
			"Preconditions and actions editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return nullptr;
	}

	if(n && n->type==dialog_link_precondition::e_type::Precondition)
	{
		System::Windows::Forms::MessageBox::Show(this, "Can't make '"+nm+"' node inside root 'FUNCTION'!", 
			"Preconditions and actions editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return nullptr;
	}

	if(n && n->type==dialog_link_precondition::e_type::Not && root->Count>0)
	{
		System::Windows::Forms::MessageBox::Show(this, "Can't add more than one node to root 'NOT'!", 
			"Preconditions and actions editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		return nullptr;
	}
	return root;
}

void dialog_precondition_action_editor::on_new_and_clicked(System::Object^ , System::EventArgs^ )
{
	TreeNodeCollection^ root = get_root_and_check_prereq("AND");
	if(root!=nullptr)
	{
		dialog_link_precondition^ pr = gcnew dialog_link_precondition();
		pr->type = dialog_link_precondition::e_type::And;
		add_link_precondition_with_name(pr, root);
		m_changed = true;
	}
}

void dialog_precondition_action_editor::on_new_or_clicked(System::Object^ , System::EventArgs^ )
{
	TreeNodeCollection^ root = get_root_and_check_prereq("OR");
	if(root!=nullptr)
	{
		dialog_link_precondition^ pr = gcnew dialog_link_precondition();
		pr->type = dialog_link_precondition::e_type::Or;
		add_link_precondition_with_name(pr, root);
		m_changed = true;
	}
}

void dialog_precondition_action_editor::on_new_not_clicked(System::Object^ , System::EventArgs^ )
{
	TreeNodeCollection^ root = get_root_and_check_prereq("NOT");
	if(root!=nullptr)
	{
		dialog_link_precondition^ pr = gcnew dialog_link_precondition();
		pr->type = dialog_link_precondition::e_type::Not;
		add_link_precondition_with_name(pr, root);
		m_changed = true;
	}
}

void dialog_precondition_action_editor::on_new_function_clicked(System::Object^ , System::EventArgs^ )
{
	TreeNodeCollection^ root = get_root_and_check_prereq("FUNCTION");
	if(root!=nullptr)
	{
		dialog_link_precondition^ pr = gcnew dialog_link_precondition();
		pr->type = dialog_link_precondition::e_type::Precondition;
		add_link_precondition_with_name(pr, root);
		m_changed = true;
	}
}

void dialog_precondition_action_editor::on_new_action_clicked(System::Object^ , System::EventArgs^ )
{
	dialog_link_action^ new_act = gcnew dialog_link_action();
	if(!m_prec_vis)
		add_link_action_with_name(new_act, treeView1->Nodes);
	else
		add_link_action_with_name(new_act, m_act_node->Nodes);

	m_changed = true;
}

void dialog_precondition_action_editor::on_ok_clicked(System::Object^ , System::EventArgs^ )
{
	if(m_changed)
	{
		Generic::List<dialog_link_action^>^ act_lst = gcnew Generic::List<dialog_link_action^>();
		TreeNodeCollection^ action_root = treeView1->Nodes;
		TreeNodeCollection^ precondition_root = treeView1->Nodes;
		if(m_act_vis && m_prec_vis)
		{
			action_root = m_act_node->Nodes;
			precondition_root = m_prec_node->Nodes;
		}
		else if(m_act_vis)
			precondition_root = m_prec_node->Nodes;
		else if(m_prec_vis)
			action_root = m_act_node->Nodes;


		for each(dialog_link_action^ n in action_root)
			act_lst->Add(n);
		
		if(precondition_root->Count>0)
			m_link->set_preds_and_acts(safe_cast<dialog_link_precondition^>(precondition_root[0]), act_lst);
		else
			m_link->set_preds_and_acts(nullptr, act_lst);

		act_lst->Clear();
	}

	m_changed = false;
	for each(TreeNode^ child in m_act_node->Nodes)
		destroy_node(child);

	m_act_node->Nodes->Clear();
	for each(TreeNode^ child in m_prec_node->Nodes)
		destroy_node(child);

	m_prec_node->Nodes->Clear();
	treeView1->Nodes->Clear();
	this->Hide();
}

void dialog_precondition_action_editor::on_cancel_clicked(System::Object^ , System::EventArgs^ )
{
	m_changed = false;
	for each(TreeNode^ child in m_act_node->Nodes)
		destroy_node(child);

	m_act_node->Nodes->Clear();
	for each(TreeNode^ child in m_prec_node->Nodes)
		destroy_node(child);

	m_prec_node->Nodes->Clear();
	treeView1->Nodes->Clear();
	this->Hide();
}

void dialog_precondition_action_editor::on_node_select(System::Object^ , System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
{
	select_node(e->Node);
	show_properties(e->Node);
}

void dialog_precondition_action_editor::on_delete_clicked(System::Object^ , System::EventArgs^ )
{
	if(treeView1->SelectedNode!=nullptr)
		delete_node_with_childs(treeView1->SelectedNode);
}

void dialog_precondition_action_editor::destroy_node(TreeNode^ n)
{
	if(n->Nodes->Count>0)
	{
		for each(TreeNode^ child in n->Nodes)
			destroy_node(child);
	}

	delete n;
}
void dialog_precondition_action_editor::delete_node_with_childs(TreeNode^ selected_node)
{
	R_ASSERT(selected_node!=nullptr);
	select_node(nullptr);
	show_properties(nullptr);
	m_changed = true;
	if(selected_node==m_act_node)
	{
		for each(TreeNode^ child in m_act_node->Nodes)
			destroy_node(child);

		m_act_node->Nodes->Clear();
		return;
	}
	else if(selected_node==m_prec_node)
	{
		for each(TreeNode^ child in m_prec_node->Nodes)
			destroy_node(child);

		m_prec_node->Nodes->Clear();
		return;
	}

	TreeNodeCollection^ nodes = nullptr;
	if(!selected_node->Parent)
		nodes = treeView1->Nodes;
	else
		nodes = selected_node->Parent->Nodes;
	nodes->Remove(selected_node);
	destroy_node(selected_node);
}

void dialog_precondition_action_editor::on_tree_view_key_down(System::Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Delete && treeView1->SelectedNode!=nullptr)
		delete_node_with_childs(treeView1->SelectedNode);
}

void dialog_precondition_action_editor::on_tree_view_key_up(System::Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode==Keys::Down || e->KeyCode==Keys::Up || e->KeyCode==Keys::Left || e->KeyCode==Keys::Right)
	{
		select_node(treeView1->SelectedNode);
		show_properties(treeView1->SelectedNode);
	}
}

void dialog_precondition_action_editor::show_properties(TreeNode^ n)
{
	if(n!=nullptr && n!=m_prec_node && n!=m_act_node)
	{
		if(m_act_node->Nodes->Contains(n) || !m_prec_vis)
			propertyGrid1->BrowsableAttributes = m_act_props;
		else if(n->Text!="AND" && n->Text!="OR" && n->Text!="NOT")
			propertyGrid1->BrowsableAttributes = m_prec_props;
		else
			propertyGrid1->BrowsableAttributes = m_type_props;

		propertyGrid1->SelectedObject = n;
	}
	else
		propertyGrid1->SelectedObject = nullptr;
}

void dialog_precondition_action_editor::on_node_property_changed(System::Object^ , System::Windows::Forms::PropertyValueChangedEventArgs^ e)
{
	if(e->ChangedItem->Value!=e->OldValue)
	{
		if(e->ChangedItem->Label=="Node type")
		{
			dialog_link_precondition::e_type val = (dialog_link_precondition::e_type)e->ChangedItem->Value;
			switch(val)
			{
			case dialog_link_precondition::e_type::And:
				{
					String^ nm = "AND";
					int i = 0;
					TreeNodeCollection^ nodes = nullptr;
					if(!treeView1->SelectedNode->Parent)
						nodes = treeView1->Nodes;
					else
						nodes = treeView1->SelectedNode->Parent->Nodes;

					while(nodes->ContainsKey(nm+i))
						++i;

					treeView1->SelectedNode->Text = nm;
					treeView1->SelectedNode->Name = nm+"_"+i;
					break;
				}
			case dialog_link_precondition::e_type::Or:
				{
					String^ nm = "OR";
					int i = 0;
					TreeNodeCollection^ nodes = nullptr;
					if(!treeView1->SelectedNode->Parent)
						nodes = treeView1->Nodes;
					else
						nodes = treeView1->SelectedNode->Parent->Nodes;

					while(nodes->ContainsKey(nm+i))
						++i;

					treeView1->SelectedNode->Text = nm;
					treeView1->SelectedNode->Name = nm+"_"+i;
					break;
				}
			case dialog_link_precondition::e_type::Not:
				{
					String^ nm = "NOT";
					int i = 0;
					TreeNodeCollection^ nodes = nullptr;
					if(!treeView1->SelectedNode->Parent)
						nodes = treeView1->Nodes;
					else
						nodes = treeView1->SelectedNode->Parent->Nodes;

					while(nodes->ContainsKey(nm+i))
						++i;

					treeView1->SelectedNode->Text = nm;
					treeView1->SelectedNode->Name = nm+"_"+i;
					break;
				}
			case dialog_link_precondition::e_type::Precondition:
				{
					dialog_link_precondition^ n = safe_cast<dialog_link_precondition^>(treeView1->SelectedNode);
					if(n->Nodes->Count!=0)
					{
						n->GetType()->GetProperty("type")->SetValue(n, e->OldValue, nullptr);
						System::Windows::Forms::MessageBox::Show(this, "Can't change node type because of childs!", 
							"Preconditions and actions editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
						return;
					}
						
					String^ nm = "FUNCTION";
					int i = 0;
					TreeNodeCollection^ nodes = nullptr;
					if(!n->Parent)
						nodes = treeView1->Nodes;
					else
						nodes = n->Parent->Nodes;

					while(nodes->ContainsKey(nm+i))
						++i;

					n->Text = n->func_names.ToString()+" ("+n->param1+")";
					n->Name = nm+"_"+i;
					break;
				}
			default: NODEFAULT();
			}
			TreeNode^ sel = treeView1->SelectedNode;
			treeView1->Sort();
			select_node(sel);
			show_properties(sel);
		}
		else if(e->ChangedItem->Label=="Function")
		{
			if(dynamic_cast<dialog_link_action^>(treeView1->SelectedNode))
			{
				dialog_link_action^ n = safe_cast<dialog_link_action^>(treeView1->SelectedNode);
				n->Text = e->ChangedItem->Value->ToString()+" ("+n->param1+")";
			}
			else if(dynamic_cast<dialog_link_precondition^>(treeView1->SelectedNode))
			{
				dialog_link_precondition^ n = safe_cast<dialog_link_precondition^>(treeView1->SelectedNode);
				n->Text = e->ChangedItem->Value->ToString()+" ("+n->param1+")";
			}
		}
		else if(e->ChangedItem->Label=="Parameter")
		{
			if(dynamic_cast<dialog_link_action^>(treeView1->SelectedNode))
			{
				dialog_link_action^ n = safe_cast<dialog_link_action^>(treeView1->SelectedNode);
				n->Text = n->func_names.ToString()+" ("+e->ChangedItem->Value+")";
			}
			else if(dynamic_cast<dialog_link_precondition^>(treeView1->SelectedNode))
			{
				dialog_link_precondition^ n = safe_cast<dialog_link_precondition^>(treeView1->SelectedNode);
				n->Text = n->func_names.ToString()+" ("+e->ChangedItem->Value+")";
			}
		}
		m_changed = true;
	}
}

void dialog_precondition_action_editor::select_node(TreeNode^ n)
{
	treeView1->SelectedNode = n;
	if(n==m_act_node || dynamic_cast<dialog_link_action^>(n))
	{
		button1->Enabled = false;
		button2->Enabled = false;
		button3->Enabled = false;
		button4->Enabled = false;
		button5->Enabled = true;
		return;
	}
	else if(n==m_prec_node || dynamic_cast<dialog_link_precondition^>(n))
	{
		button5->Enabled = false;
		dialog_link_precondition^ pr = dynamic_cast<dialog_link_precondition^>(n);
		if(pr && pr->type==dialog_link_precondition::e_type::Precondition)
		{
			button1->Enabled = false;
			button2->Enabled = false;
			button3->Enabled = false;
			button4->Enabled = false;
		}
		else
		{
			button1->Enabled = true;
			button2->Enabled = true;
			button3->Enabled = true;
			button4->Enabled = true;
		}
		return;
	}

	if(m_act_vis)
	{
		button1->Enabled = false;
		button2->Enabled = false;
		button3->Enabled = false;
		button4->Enabled = false;
		button5->Enabled = true;
	}

	if(m_prec_vis)
	{
		button1->Enabled = true;
		button2->Enabled = true;
		button3->Enabled = true;
		button4->Enabled = true;
	}
}

////////////////////////////////////////////////////////////////////////////
//- class dialog_link_ui_type_editor ----------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
#include "dialog_hypergraph.h"
#include "dialog_document.h"
#include "dialog_editor.h"
#include "dialog_graph_node_base.h"

using xray::dialog_editor::dialog_editor_impl;
using xray::dialog_editor::dialog_hypergraph;
using xray::dialog_editor::dialog_document;

void	dialog_link_ui_type_editor::run_editor	( xray::editor::wpf_controls::property_editors::property^ prop )
{
	bool is_action = (prop->descriptors[0]->Name=="act");
	dialog_graph_link^ lnk = safe_cast<dialog_graph_link^>(prop->property_owners[0]);
	dialog_graph_node_base^ n = safe_cast<dialog_graph_node_base^>(lnk->source_node);
	dialog_hypergraph^ h = safe_cast<dialog_hypergraph^>(n->get_parent());
	dialog_document^ d = safe_cast<dialog_document^>(h->get_owner());
	d->get_editor()->show_prec_act_editor(lnk, is_action, !is_action);
}