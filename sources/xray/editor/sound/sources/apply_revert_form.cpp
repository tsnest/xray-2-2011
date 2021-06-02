////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "apply_revert_form.h"
#include "sound_editor.h"

using xray::sound_editor::apply_revert_form;
using xray::sound_editor::sound_editor;

apply_revert_form::apply_revert_form(sound_editor^ ed)
:m_editor(ed)
{
	InitializeComponent();
}

apply_revert_form::~apply_revert_form()
{
	if (components)
		delete components;
}

void apply_revert_form::fill_list_box_items(List<String^>^ item_names)
{
	for each(String^ nm in item_names)
		m_list_box->Items->Add(nm);
}

void apply_revert_form::m_apply_button_Click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(String^ nm in m_list_box->Items)
		lst->Add(nm);

	m_editor->apply_changes(lst);
	this->DialogResult = System::Windows::Forms::DialogResult::Yes;
	Close();
}

void apply_revert_form::m_revert_button_Click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(String^ nm in m_list_box->Items)
		lst->Add(nm);

	m_editor->revert_changes(lst);
	this->DialogResult = System::Windows::Forms::DialogResult::No;
	Close();
}

void apply_revert_form::m_cancel_button_Click(Object^, EventArgs^)
{
	Close();
}

void apply_revert_form::applyChangesToolStripMenuItem_Click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(String^ nm in m_list_box->SelectedItems)
		lst->Add(nm);

	m_editor->apply_changes(lst);
	for each(String^ nm in lst)
		m_list_box->Items->Remove(nm);
}

void apply_revert_form::revertChangesToolStripMenuItem_Click(Object^, EventArgs^)
{
	List<String^>^ lst = gcnew List<String^>();
	for each(String^ nm in m_list_box->SelectedItems)
		lst->Add(nm);

	m_editor->revert_changes(lst);
	for each(String^ nm in lst)
		m_list_box->Items->Remove(nm);
}

void apply_revert_form::m_list_box_MouseUp(Object^, MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Right)
	{
		int index = m_list_box->IndexFromPoint(e->Location);
		if(index<0)
			return;

		if(!m_list_box->GetSelected(index))
		{
			m_list_box->SelectedItems->Clear();		
			m_list_box->SetSelected(index, true);
		}

		contextMenuStrip1->Show(m_list_box->PointToScreen(e->Location));
	}
}