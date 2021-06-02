#include "pch.h"
#include "object_values_storage_edit_form.h"

namespace xray{
namespace editor{

void object_values_storage_edit_form::on_add_value_clicked(System::Object^  , System::EventArgs^  )
{
	if ( m_value_to_add_text_box->Text == "" )
		return;

	if ( m_values_list_box->Items->Contains( m_value_to_add_text_box->Text ) )
	{
		System::Windows::Forms::MessageBox::Show(this, "Value ["+m_value_to_add_text_box->Text+"] already exsists!", 
			"Value editor", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error);
		m_value_to_add_text_box->Text = "";
		return;
	}

	m_values_list_box->Items->Add( m_value_to_add_text_box->Text );
	m_parent_values_storage->add_value(  m_value_to_add_text_box->Text );
	m_value_to_add_text_box->Text = "";
	m_value_to_add_text_box->Focus();

}
void object_values_storage_edit_form::on_remove_value_clicked(System::Object^  , System::EventArgs^  )
{
	m_parent_values_storage->remove_value( m_values_list_box->SelectedItem->ToString( ) );
	m_values_list_box->Items->Remove( m_values_list_box->SelectedItem );
}
void object_values_storage_edit_form::on_lalues_list_selection_changed(System::Object^  , System::EventArgs^  ) 
{
	if ( m_values_list_box->SelectedItem != nullptr )
	{
		m_remove_value_button->Enabled = true;
	}
	else
	{
		m_remove_value_button->Enabled = false;
	}
}
}//namespace editor
}//namespace xray