////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_EDITOR_FORM_H_INCLUDED
#define DIALOG_EDITOR_FORM_H_INCLUDED

#include "dialog_editor.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {

	ref class dialog_editor_impl;

	public ref class dialog_editor_form : public System::Windows::Forms::Form
	{
	public:
		dialog_editor_form(dialog_editor_impl^ ed)
		:m_editor(ed)
		{
			InitializeComponent();
			in_constructor();
		}
		property xray::editor::controls::document_editor_base^	multidocument_base
		{
			xray::editor::controls::document_editor_base^	get () {return m_multidocument_base;};
		}
		property System::Windows::Forms::SaveFileDialog^ save_file_dialog
		{
			System::Windows::Forms::SaveFileDialog^ get () {return m_save_file_dialog;};
		}

	protected:
		~dialog_editor_form()
		{
			if (components)
				delete components;
		}

	private:
		void in_constructor								();
		void on_view_menu_item_select					(Object^ , EventArgs^ );
		void on_language_panel_menu_select				(Object^ , EventArgs^ );
		void on_file_view_panel_menu_select				(Object^ , EventArgs^ );
		void on_properties_panel_menu_select			(Object^ , EventArgs^ );
		void on_control_panel_menu_select				(Object^ , EventArgs^ );

	private:
		System::ComponentModel::Container^				components;
		dialog_editor_impl^								m_editor;
		xray::editor::controls::document_editor_base^	m_multidocument_base;
		System::Windows::Forms::ToolStripMenuItem^		localizations_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		add_language_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		remove_language_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		export_to_xls_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		import_from_xls_menu_item;

		System::Windows::Forms::ToolStripMenuItem^		view_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		language_panel_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		file_view_panel_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		properties_panel_menu_item;
		System::Windows::Forms::ToolStripMenuItem^		control_panel_menu_item;
		System::Windows::Forms::SaveFileDialog^			m_save_file_dialog;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_save_file_dialog = (gcnew System::Windows::Forms::SaveFileDialog());
			this->localizations_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->add_language_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->remove_language_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->export_to_xls_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->import_from_xls_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->view_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->control_panel_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->file_view_panel_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->properties_panel_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->language_panel_menu_item = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SuspendLayout();
			// 
			// m_save_file_dialog
			// 
			this->m_save_file_dialog->DefaultExt = L"dlg";
			this->m_save_file_dialog->Filter = L"Dialog files|*.dlg";
			this->m_save_file_dialog->RestoreDirectory = true;
			this->m_save_file_dialog->Title = L"New document";
			// 
			// localizations_menu_item
			// 
			this->localizations_menu_item->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->add_language_menu_item, 
				this->remove_language_menu_item, this->export_to_xls_menu_item, this->import_from_xls_menu_item});
			this->localizations_menu_item->Name = L"localizations_menu_item";
			this->localizations_menu_item->Size = System::Drawing::Size(67, 20);
			this->localizations_menu_item->Text = L"&Localizations";
			// 
			// add_language_menu_item
			// 
			this->add_language_menu_item->Name = L"add_language_menu_item";
			this->add_language_menu_item->Size = System::Drawing::Size(169, 22);
			this->add_language_menu_item->Text = L"&Add language";
			// 
			// remove_language_menu_item
			// 
			this->remove_language_menu_item->Name = L"remove_language_menu_item";
			this->remove_language_menu_item->Size = System::Drawing::Size(169, 22);
			this->remove_language_menu_item->Text = L"&Remove language";
			// 
			// export_to_xls_menu_item
			// 
			this->export_to_xls_menu_item->Name = L"export_to_xls_menu_item";
			this->export_to_xls_menu_item->Size = System::Drawing::Size(169, 22);
			this->export_to_xls_menu_item->Text = L"&Export to *.xlsx";
			// 
			// import_from_xls_menu_item
			// 
			this->import_from_xls_menu_item->Name = L"import_from_xls_menu_item";
			this->import_from_xls_menu_item->Size = System::Drawing::Size(169, 22);
			this->import_from_xls_menu_item->Text = L"&Import from *.xlsx";
			// 
			// view_menu_item
			// 
			this->view_menu_item->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->control_panel_menu_item, 
				this->file_view_panel_menu_item, this->properties_panel_menu_item, this->language_panel_menu_item});
			this->view_menu_item->Name = L"view_menu_item";
			this->view_menu_item->Size = System::Drawing::Size(67, 20);
			this->view_menu_item->Text = L"&View";
			this->view_menu_item->Click += gcnew System::EventHandler(this, &dialog_editor_form::on_view_menu_item_select);
			// 
			// control_panel_menu_item
			// 
			this->control_panel_menu_item->Checked = true;
			this->control_panel_menu_item->CheckOnClick = true;
			this->control_panel_menu_item->CheckState = System::Windows::Forms::CheckState::Checked;
			this->control_panel_menu_item->Name = L"control_panel_menu_item";
			this->control_panel_menu_item->Size = System::Drawing::Size(164, 22);
			this->control_panel_menu_item->Text = L"Toolbox";
			this->control_panel_menu_item->Click += gcnew System::EventHandler(this, &dialog_editor_form::on_control_panel_menu_select);
			// 
			// file_view_panel_menu_item
			// 
			this->file_view_panel_menu_item->Checked = true;
			this->file_view_panel_menu_item->CheckOnClick = true;
			this->file_view_panel_menu_item->CheckState = System::Windows::Forms::CheckState::Checked;
			this->file_view_panel_menu_item->Name = L"file_view_panel_menu_item";
			this->file_view_panel_menu_item->Size = System::Drawing::Size(164, 22);
			this->file_view_panel_menu_item->Text = L"File viewer";
			this->file_view_panel_menu_item->Click += gcnew System::EventHandler(this, &dialog_editor_form::on_file_view_panel_menu_select);
			// 
			// properties_panel_menu_item
			// 
			this->properties_panel_menu_item->Checked = true;
			this->properties_panel_menu_item->CheckOnClick = true;
			this->properties_panel_menu_item->CheckState = System::Windows::Forms::CheckState::Checked;
			this->properties_panel_menu_item->Name = L"properties_panel_menu_item";
			this->properties_panel_menu_item->Size = System::Drawing::Size(164, 22);
			this->properties_panel_menu_item->Text = L"Properties viewer";
			this->properties_panel_menu_item->Click += gcnew System::EventHandler(this, &dialog_editor_form::on_properties_panel_menu_select);
			// 
			// language_panel_menu_item
			// 
			this->language_panel_menu_item->Checked = true;
			this->language_panel_menu_item->CheckOnClick = true;
			this->language_panel_menu_item->CheckState = System::Windows::Forms::CheckState::Checked;
			this->language_panel_menu_item->Name = L"language_panel_menu_item";
			this->language_panel_menu_item->Size = System::Drawing::Size(164, 22);
			this->language_panel_menu_item->Text = L"Languages panel";
			this->language_panel_menu_item->Click += gcnew System::EventHandler(this, &dialog_editor_form::on_language_panel_menu_select);
			// 
			// dialog_editor_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 264);
			this->Name = L"dialog_editor_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Dialog editor";
			this->ResumeLayout(false);

		}
#pragma endregion
};
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef DIALOG_EDITOR_FORM_H_INCLUDED
