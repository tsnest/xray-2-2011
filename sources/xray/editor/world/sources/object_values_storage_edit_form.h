#pragma once

#include "object_values_storage.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	/// <summary>
	/// Summary for object_values_storage_edit_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class object_values_storage_edit_form : public System::Windows::Forms::Form
	{
	public:
		object_values_storage_edit_form( object_values_storage^ parent_values_storage )
		{
			InitializeComponent();
			for each( System::String^ val in parent_values_storage->get_values_by_type("strings") )
			{
				m_values_list_box->Items->Add( val );
			}
			m_parent_values_storage = parent_values_storage;
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~object_values_storage_edit_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  m_values_list_box;

	protected: 
	private: 


	private: System::Windows::Forms::Button^	m_add_value_button;
	private: System::Windows::Forms::Button^	m_remove_value_button;
	private: System::Windows::Forms::TextBox^	m_value_to_add_text_box;
			 object_values_storage^				m_parent_values_storage;



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->m_values_list_box = (gcnew System::Windows::Forms::ListBox());
			this->m_value_to_add_text_box = (gcnew System::Windows::Forms::TextBox());
			this->m_add_value_button = (gcnew System::Windows::Forms::Button());
			this->m_remove_value_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// m_values_list_box
			// 
			this->m_values_list_box->FormattingEnabled = true;
			this->m_values_list_box->Location = System::Drawing::Point(13, 39);
			this->m_values_list_box->Name = L"m_values_list_box";
			this->m_values_list_box->Size = System::Drawing::Size(210, 446);
			this->m_values_list_box->TabIndex = 0;
			this->m_values_list_box->SelectedIndexChanged += gcnew System::EventHandler(this, &object_values_storage_edit_form::on_lalues_list_selection_changed);
			// 
			// m_value_to_add_text_box
			// 
			this->m_value_to_add_text_box->Location = System::Drawing::Point(13, 13);
			this->m_value_to_add_text_box->Name = L"m_value_to_add_text_box";
			this->m_value_to_add_text_box->Size = System::Drawing::Size(210, 20);
			this->m_value_to_add_text_box->TabIndex = 1;
			// 
			// m_add_value_button
			// 
			this->m_add_value_button->Location = System::Drawing::Point(229, 13);
			this->m_add_value_button->Name = L"m_add_value_button";
			this->m_add_value_button->Size = System::Drawing::Size(109, 20);
			this->m_add_value_button->TabIndex = 2;
			this->m_add_value_button->Text = L"Add value";
			this->m_add_value_button->UseVisualStyleBackColor = true;
			this->m_add_value_button->Click += gcnew System::EventHandler(this, &object_values_storage_edit_form::on_add_value_clicked);
			// 
			// m_remove_value_button
			// 
			this->m_remove_value_button->Enabled = false;
			this->m_remove_value_button->Location = System::Drawing::Point(230, 39);
			this->m_remove_value_button->Name = L"m_remove_value_button";
			this->m_remove_value_button->Size = System::Drawing::Size(108, 21);
			this->m_remove_value_button->TabIndex = 3;
			this->m_remove_value_button->Text = L"Remove value";
			this->m_remove_value_button->UseVisualStyleBackColor = true;
			this->m_remove_value_button->Click += gcnew System::EventHandler(this, &object_values_storage_edit_form::on_remove_value_clicked);
			// 
			// object_values_storage_edit_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(345, 495);
			this->Controls->Add(this->m_remove_value_button);
			this->Controls->Add(this->m_add_value_button);
			this->Controls->Add(this->m_value_to_add_text_box);
			this->Controls->Add(this->m_values_list_box);
			this->Name = L"object_values_storage_edit_form";
			this->Text = L"values editor";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
private: 
	void on_add_value_clicked(System::Object^  sender, System::EventArgs^  e);
	void on_remove_value_clicked(System::Object^  sender, System::EventArgs^  e);
	void on_lalues_list_selection_changed(System::Object^  sender, System::EventArgs^  e) ;
};
}
}
