#include "pch.h"
#include "learning_properties_form.h"
#include "animation_editor.h"

using xray::animation_editor::learning_properties_form;
using xray::animation_editor::animation_editor;

learning_properties_form::learning_properties_form(animation_editor^ ed, String^ controller_name)
:m_editor(ed),
m_controller_name(controller_name)
{
	InitializeComponent();
	numericUpDown1->Value = System::Decimal(m_editor->learning_max_residual);
	numericUpDown2->Value = System::Decimal(m_editor->learning_max_iterations_count);
}

learning_properties_form::~learning_properties_form()
{
	if(components)
		delete components;
}

void learning_properties_form::ok_clicked(System::Object^, System::EventArgs^)
{
	m_editor->learning_max_residual = (float)numericUpDown1->Value;
	m_editor->learning_max_iterations_count = (u32)numericUpDown2->Value;
	m_editor->start_learning_controller(m_controller_name);
	Close();
}

void learning_properties_form::cancel_clicked(System::Object^, System::EventArgs^)
{
	Close();
}