////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_viewer_detailed_options_panel.h"
#include "viewer_editor.h"
#include <xray/animation/anim_track_common.h>

using xray::animation_editor::animation_viewer_detailed_options_panel;
using xray::animation_editor::viewer_editor;

animation_viewer_detailed_options_panel::animation_viewer_detailed_options_panel(viewer_editor^ ed)
:m_editor(ed)
{
	m_numeric_is_updating = false;
	checkBox1 = gcnew CheckBox();
	checkBox2 = gcnew CheckBox();
	checkBox3 = gcnew CheckBox();
	label1 = gcnew Label();
	label2 = gcnew Label();
	numericUpDown1 = gcnew NumericUpDown();
	numericUpDown2 = gcnew NumericUpDown();
	button1 = gcnew Button();
	button2 = gcnew Button();
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(numericUpDown1)->BeginInit();
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(numericUpDown2)->BeginInit();
	SuspendLayout();

	// checkBox1
	checkBox1->AutoSize = true;
	checkBox1->Location = System::Drawing::Point(12, 12);
	checkBox1->Name = L"checkBox1";
	checkBox1->Size = System::Drawing::Size(195, 17);
	checkBox1->TabIndex = 0;
	checkBox1->Text = L"Use detailed weights and time scale";
	checkBox1->UseVisualStyleBackColor = true;
	checkBox1->CheckedChanged += gcnew System::EventHandler(this, &animation_viewer_detailed_options_panel::on_checkbox1_checked_changed);

	// checkBox2
	checkBox2->AutoSize = true;
	checkBox2->Location = System::Drawing::Point(12, 35);
	checkBox2->Name = L"checkBox2";
	checkBox2->Size = System::Drawing::Size(132, 17);
	checkBox2->TabIndex = 1;
	checkBox2->Text = L"Show detailed weights";
	checkBox2->UseVisualStyleBackColor = true;

	// checkBox3
	checkBox3->AutoSize = true;
	checkBox3->Location = System::Drawing::Point(12, 58);
	checkBox3->Name = L"checkBox3";
	checkBox3->Size = System::Drawing::Size(148, 17);
	checkBox3->TabIndex = 2;
	checkBox3->Text = L"Show detailed time scales";
	checkBox3->UseVisualStyleBackColor = true;

	// label1
	label1->AutoSize = true;
	label1->Location = System::Drawing::Point(9, 78);
	label1->Name = L"label1";
	label1->Size = System::Drawing::Size(135, 13);
	label1->TabIndex = 4;
	label1->Text = L"Graphics detalization in ms:";

	// label2
	label2->AutoSize = true;
	label2->Location = System::Drawing::Point(92, 117);
	label2->Name = L"label2";
	label2->Size = System::Drawing::Size(52, 13);
	label2->TabIndex = 7;
	label2->Text = L"in frames:";

	// numericUpDown1
	numericUpDown1->Location = System::Drawing::Point(12, 94);
	numericUpDown1->Minimum = System::Decimal(1);
	numericUpDown1->Name = L"numericUpDown1";
	numericUpDown1->Size = System::Drawing::Size(120, 20);
	numericUpDown1->TabIndex = 3;
	numericUpDown1->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_detailed_options_panel::on_numeric1_value_changed);

	// numericUpDown2
	numericUpDown2->Location = System::Drawing::Point(12, 133);
	numericUpDown2->DecimalPlaces = 3;
	numericUpDown2->Minimum = System::Decimal(0.033f);
	numericUpDown2->Name = L"numericUpDown2";
	numericUpDown2->Size = System::Drawing::Size(120, 20);
	numericUpDown2->TabIndex = 6;
	numericUpDown2->ValueChanged += gcnew System::EventHandler(this, &animation_viewer_detailed_options_panel::on_numeric2_value_changed);

	// button1
	button1->Location = System::Drawing::Point(12, 159);
	button1->Name = L"button1";
	button1->Size = System::Drawing::Size(94, 23);
	button1->TabIndex = 5;
	button1->Text = L"OK";
	button1->UseVisualStyleBackColor = true;
	button1->Click += gcnew System::EventHandler(this, &animation_viewer_detailed_options_panel::on_ok_clicked); 

	// button2
	button2->Location = System::Drawing::Point(113, 159);
	button2->Name = L"button2";
	button2->Size = System::Drawing::Size(94, 23);
	button2->TabIndex = 8;
	button2->Text = L"Cancel";
	button2->UseVisualStyleBackColor = true;

	// animation_viewer_detailed_options_panel
	AcceptButton = button1;
	AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	CancelButton = button2;
	ClientSize = System::Drawing::Size(218, 192);
	Controls->Add(button2);
	Controls->Add(button1);
	Controls->Add(numericUpDown2);
	Controls->Add(numericUpDown1);
	Controls->Add(label2);
	Controls->Add(label1);
	Controls->Add(checkBox3);
	Controls->Add(checkBox2);
	Controls->Add(checkBox1);
	FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	MaximizeBox = false;
	MinimizeBox = false;
	Name = L"animation_viewer_detailed_options_panel";
	ShowIcon = false;
	ShowInTaskbar = false;
	Text = L"Detaled options";
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(numericUpDown1)->EndInit();
	cli::safe_cast<System::ComponentModel::ISupportInitialize^ >(numericUpDown2)->EndInit();
	ResumeLayout(false);
	PerformLayout();
}

animation_viewer_detailed_options_panel::~animation_viewer_detailed_options_panel()
{
	if (components)
		delete components;
}

void animation_viewer_detailed_options_panel::show()
{
	bool val = m_editor->use_detailed_weights_and_scales;
	checkBox1->Checked = val;
	checkBox2->Checked = m_editor->show_detailed_weights;
	checkBox3->Checked = m_editor->show_detailed_scales;
	checkBox2->Enabled = val;
	checkBox3->Enabled = val;
	numericUpDown1->Value = System::Decimal(m_editor->detalization_delta);
	numericUpDown2->Value = System::Decimal(m_editor->detalization_delta/1000.f*animation::default_fps);
	numericUpDown1->Enabled = val;
	numericUpDown2->Enabled = val;
	label1->Enabled = val;
	label2->Enabled = val;
	this->ShowDialog();
}

void animation_viewer_detailed_options_panel::on_checkbox1_checked_changed(System::Object^, System::EventArgs^)
{
	bool val = checkBox1->Checked;
	numericUpDown2->Enabled = val;
	numericUpDown1->Enabled = val;
	label2->Enabled = val;
	label1->Enabled = val;
	checkBox3->Enabled = val;
	checkBox2->Enabled = val;
}

void animation_viewer_detailed_options_panel::on_numeric1_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_numeric_is_updating)
	{
		m_numeric_is_updating = true;
		numericUpDown2->Value = System::Decimal((float)numericUpDown1->Value/1000.f*animation::default_fps);
		m_numeric_is_updating = false;
	}
}

void animation_viewer_detailed_options_panel::on_numeric2_value_changed(System::Object^, System::EventArgs^)
{
	if(!m_numeric_is_updating)
	{
		m_numeric_is_updating = true;
		numericUpDown1->Value = System::Decimal((float)numericUpDown2->Value*1000.f/animation::default_fps);
		m_numeric_is_updating = false;
	}
}

void animation_viewer_detailed_options_panel::on_ok_clicked(System::Object^, System::EventArgs^)
{
	m_editor->show_detailed_weights = checkBox2->Checked;
	m_editor->show_detailed_scales = checkBox3->Checked;
	m_editor->detalization_delta = (u32)numericUpDown1->Value;
	m_editor->use_detailed_weights_and_scales = checkBox1->Checked;
	this->Hide();
}