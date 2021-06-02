////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace editor {

	/// <summary>
	/// Summary for models_browser_dialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class models_browser_dialog : public System::Windows::Forms::Form
	{
	public:
		models_browser_dialog(void)
		{
			InitializeComponent();
			build_tree();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~models_browser_dialog()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
			void	build_tree				( );
			void	on_fs_iterator_ready	( resources::fs_iterator fs_it );
			void	process_fs				( resources::fs_iterator it, String^ file_path );

	private: System::Windows::Forms::TreeView^  trv_models;
	private: System::Windows::Forms::Panel^  pnl_buttons;
	private: System::Windows::Forms::Button^  btn_cancel;
	private: System::Windows::Forms::Button^  btn_ok;

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
			this->trv_models = (gcnew System::Windows::Forms::TreeView());
			this->pnl_buttons = (gcnew System::Windows::Forms::Panel());
			this->btn_ok = (gcnew System::Windows::Forms::Button());
			this->btn_cancel = (gcnew System::Windows::Forms::Button());
			this->pnl_buttons->SuspendLayout();
			this->SuspendLayout();
			// 
			// trv_models
			// 
			this->trv_models->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->trv_models->Dock = System::Windows::Forms::DockStyle::Fill;
			this->trv_models->HideSelection = false;
			this->trv_models->Location = System::Drawing::Point(0, 0);
			this->trv_models->Name = L"trv_models";
			this->trv_models->Size = System::Drawing::Size(284, 262);
			this->trv_models->TabIndex = 0;
			// 
			// pnl_buttons
			// 
			this->pnl_buttons->Controls->Add(this->btn_cancel);
			this->pnl_buttons->Controls->Add(this->btn_ok);
			this->pnl_buttons->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->pnl_buttons->Location = System::Drawing::Point(0, 221);
			this->pnl_buttons->Name = L"pnl_buttons";
			this->pnl_buttons->Size = System::Drawing::Size(284, 41);
			this->pnl_buttons->TabIndex = 1;
			// 
			// btn_ok
			// 
			this->btn_ok->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btn_ok->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->btn_ok->Location = System::Drawing::Point(116, 10);
			this->btn_ok->Name = L"btn_ok";
			this->btn_ok->Size = System::Drawing::Size(75, 23);
			this->btn_ok->TabIndex = 0;
			this->btn_ok->Text = L"OK";
			this->btn_ok->UseVisualStyleBackColor = true;
			// 
			// btn_cancel
			// 
			this->btn_cancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->btn_cancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->btn_cancel->Location = System::Drawing::Point(197, 10);
			this->btn_cancel->Name = L"btn_cancel";
			this->btn_cancel->Size = System::Drawing::Size(75, 23);
			this->btn_cancel->TabIndex = 1;
			this->btn_cancel->Text = L"Cancel";
			this->btn_cancel->UseVisualStyleBackColor = true;
			// 
			// models_browser_dialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->btn_cancel;
			this->ClientSize = System::Drawing::Size(284, 262);
			this->Controls->Add(this->pnl_buttons);
			this->Controls->Add(this->trv_models);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->KeyPreview = true;
			this->Name = L"models_browser_dialog";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Models Browser";
			this->TopMost = true;
			this->pnl_buttons->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	};

} // namespace editor
} // namespace xray