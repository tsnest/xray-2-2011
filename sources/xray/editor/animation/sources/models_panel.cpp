////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "models_panel.h"

using xray::animation_editor::models_panel;
using xray::animation_editor::models_panel_mode;

void models_panel::in_constructor(String^ name, Generic::List<String^>^ lst, models_panel_mode mode)
{
	m_mode = mode;
	textBox1->Text = name;
	for each(String^ name in lst)
		listBox1->Items->Add(name);

	if(m_mode==models_panel_mode::add)
	{
		listBox1->Enabled = false;
		listBox1->Visible = false;
		label1->Text = "Enter model name:";
		button1->Location = System::Drawing::Point(59, 51);
		ClientSize = System::Drawing::Size(193, 82);
	}
	else if(m_mode==models_panel_mode::remove)
	{
		textBox1->Enabled = false;
		textBox1->Visible = false;
		label1->Text = L"Select models to remove:";
	}
	else
	{
		if(listBox1->Items->Count>0)
			listBox1->SelectedItem = listBox1->Items[0];

		listBox1->SelectionMode = System::Windows::Forms::SelectionMode::One;
		textBox1->Enabled = false;
		textBox1->Visible = false;
		label1->Text = L"Select new active model:";
	}
}

String^ models_panel::get_name()
{
	return textBox1->Text;
}

void models_panel::get_selected_list(Generic::List<String^>% lst)
{
	for each(String^ name in listBox1->SelectedItems)
		lst.Add(name);
}