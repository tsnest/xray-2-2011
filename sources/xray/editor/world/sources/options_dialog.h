#pragma once

#include	"options_page.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

interface class options_page;

	/// <summary>
	/// Summary for options_dialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class options_dialog : public System::Windows::Forms::Form, public options_manager
	{
	public:
		options_dialog	(void):
			m_active_page	(nullptr),
			m_initialized	(false)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~options_dialog()
		{
			if (components)
			{
				delete components;
			}
		}


	protected: 

	private: System::Windows::Forms::Button^  ok_button;
	private: System::Windows::Forms::Button^  cancel_button;

	private: System::Windows::Forms::Panel^  page_platform_panel;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label1;




	private: System::Windows::Forms::TreeView^  tree_view;

	public: 

	virtual	void	register_page	( System::String^ full_name, options_page^ page );
	virtual	void	unregister_page	( System::String^ full_name );

	private:
		void		initialize_all	( TreeNodeCollection^ nodes );
		void		make_changes	( bool b_accept, TreeNodeCollection^ nodes );

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
			this->tree_view = (gcnew System::Windows::Forms::TreeView());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->page_platform_panel = (gcnew System::Windows::Forms::Panel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->page_platform_panel->SuspendLayout();
			this->SuspendLayout();
			// 
			// tree_view
			// 
			this->tree_view->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->tree_view->HideSelection = false;
			this->tree_view->Location = System::Drawing::Point(6, 8);
			this->tree_view->Name = L"tree_view";
			this->tree_view->Size = System::Drawing::Size(180, 294);
			this->tree_view->TabIndex = 1;
			this->tree_view->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &options_dialog::tree_view_AfterSelect);
			// 
			// ok_button
			// 
			this->ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ok_button->Location = System::Drawing::Point(426, 306);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(75, 23);
			this->ok_button->TabIndex = 3;
			this->ok_button->Text = L"OK";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &options_dialog::ok_button_Click);
			// 
			// cancel_button
			// 
			this->cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancel_button->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancel_button->Location = System::Drawing::Point(507, 306);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(75, 23);
			this->cancel_button->TabIndex = 4;
			this->cancel_button->Text = L"Cancel";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &options_dialog::cancel_button_Click);
			// 
			// page_platform_panel
			// 
			this->page_platform_panel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->page_platform_panel->BackColor = System::Drawing::SystemColors::Control;
			this->page_platform_panel->Controls->Add(this->label1);
			this->page_platform_panel->Location = System::Drawing::Point(192, 8);
			this->page_platform_panel->Name = L"page_platform_panel";
			this->page_platform_panel->Size = System::Drawing::Size(389, 292);
			this->page_platform_panel->TabIndex = 7;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(105, 219);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(169, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"<no options avialable for this item>";
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Location = System::Drawing::Point(203, 294);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(366, 8);
			this->groupBox1->TabIndex = 11;
			this->groupBox1->TabStop = false;
			// 
			// options_dialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(594, 335);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->page_platform_panel);
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->ok_button);
			this->Controls->Add(this->tree_view);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(600, 360);
			this->Name = L"options_dialog";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Options";
			this->Shown += gcnew System::EventHandler(this, &options_dialog::options_dialog_Shown);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &options_dialog::options_dialog_FormClosed);
			this->page_platform_panel->ResumeLayout(false);
			this->page_platform_panel->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

		private:
			options_page^	m_active_page;
			bool			m_initialized;
			
		private: 
			System::Void tree_view_AfterSelect	(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
			System::Void options_dialog_Shown	(System::Object^  sender, System::EventArgs^  e);
			System::Void ok_button_Click		(System::Object^  sender, System::EventArgs^  e);
			System::Void options_dialog_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
			System::Void cancel_button_Click	(System::Object^  sender, System::EventArgs^  e);
};

} // namespace editor 
} // namespace xray
