////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROJECT_BROWSER_DIALOG_H_INCLUDED
#define PROJECT_BROWSER_DIALOG_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace editor {
	/// <summary>
	/// Summary for project_browser_dialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class project_browser_dialog : public System::Windows::Forms::Form
	{
	public:
		project_browser_dialog()
		{
			InitializeComponent	();
			in_constructor		();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~project_browser_dialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListView^  projects_list_view;
	private: System::Windows::Forms::Label^  current_path_label;
	private: System::Windows::Forms::Button^  ok_button;
	private: System::Windows::Forms::Button^  cancel_button;
	private: System::Windows::Forms::TextBox^  selected_text_box;

	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::ColumnHeader^  columnHeader2;
	private: System::Windows::Forms::ColumnHeader^  columnHeader3;
	protected: 

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
			System::Windows::Forms::ListViewItem^  listViewItem1 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(3) {L"itm1", 
				L"sda", L"ggg"}, -1));
			System::Windows::Forms::ListViewItem^  listViewItem2 = (gcnew System::Windows::Forms::ListViewItem(L"itm2"));
			System::Windows::Forms::ListViewItem^  listViewItem3 = (gcnew System::Windows::Forms::ListViewItem(L"itm3"));
			System::Windows::Forms::ListViewItem^  listViewItem4 = (gcnew System::Windows::Forms::ListViewItem(L"itm4"));
			this->projects_list_view = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader3 = (gcnew System::Windows::Forms::ColumnHeader());
			this->current_path_label = (gcnew System::Windows::Forms::Label());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->selected_text_box = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// projects_list_view
			// 
			this->projects_list_view->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->projects_list_view->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(3) {this->columnHeader1, 
				this->columnHeader2, this->columnHeader3});
			this->projects_list_view->FullRowSelect = true;
			this->projects_list_view->HideSelection = false;
			this->projects_list_view->Items->AddRange(gcnew cli::array< System::Windows::Forms::ListViewItem^  >(4) {listViewItem1, listViewItem2, 
				listViewItem3, listViewItem4});
			this->projects_list_view->Location = System::Drawing::Point(12, 25);
			this->projects_list_view->MultiSelect = false;
			this->projects_list_view->Name = L"projects_list_view";
			this->projects_list_view->Size = System::Drawing::Size(392, 293);
			this->projects_list_view->TabIndex = 0;
			this->projects_list_view->UseCompatibleStateImageBehavior = false;
			this->projects_list_view->View = System::Windows::Forms::View::Details;
			this->projects_list_view->ItemActivate += gcnew System::EventHandler(this, &project_browser_dialog::projects_list_view_ItemActivate);
			this->projects_list_view->SelectedIndexChanged += gcnew System::EventHandler(this, &project_browser_dialog::projects_list_view_SelectedIndexChanged);
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"Name";
			this->columnHeader1->Width = 120;
			// 
			// columnHeader2
			// 
			this->columnHeader2->Text = L"Modification time";
			this->columnHeader2->Width = 140;
			// 
			// columnHeader3
			// 
			this->columnHeader3->Text = L"Type";
			this->columnHeader3->Width = 120;
			// 
			// current_path_label
			// 
			this->current_path_label->AutoSize = true;
			this->current_path_label->Location = System::Drawing::Point(12, 9);
			this->current_path_label->Name = L"current_path_label";
			this->current_path_label->Size = System::Drawing::Size(35, 13);
			this->current_path_label->TabIndex = 1;
			this->current_path_label->Text = L"label1";
			// 
			// ok_button
			// 
			this->ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ok_button->Location = System::Drawing::Point(254, 354);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(75, 23);
			this->ok_button->TabIndex = 2;
			this->ok_button->Text = L"OK";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &project_browser_dialog::ok_clicked);
			// 
			// cancel_button
			// 
			this->cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancel_button->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->cancel_button->Location = System::Drawing::Point(335, 354);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(75, 23);
			this->cancel_button->TabIndex = 3;
			this->cancel_button->Text = L"Cancel";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &project_browser_dialog::cancel_clicked);
			// 
			// selected_text_box
			// 
			this->selected_text_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->selected_text_box->Location = System::Drawing::Point(12, 324);
			this->selected_text_box->Name = L"selected_text_box";
			this->selected_text_box->Size = System::Drawing::Size(243, 20);
			this->selected_text_box->TabIndex = 4;
			this->selected_text_box->TextChanged += gcnew System::EventHandler(this, &project_browser_dialog::selected_text_box_TextChanged);
			this->selected_text_box->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &project_browser_dialog::selected_text_box_KeyDown);
			// 
			// project_browser_dialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->cancel_button;
			this->ClientSize = System::Drawing::Size(416, 389);
			this->Controls->Add(this->selected_text_box);
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->ok_button);
			this->Controls->Add(this->current_path_label);
			this->Controls->Add(this->projects_list_view);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->KeyPreview = true;
			this->Name = L"project_browser_dialog";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Project browser";
			this->TopMost = true;
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		public:
		property System::String^	selected_project;

			::DialogResult			SelectProjectToLoad			( System::String^ resources_path, System::String^ initial );
			::DialogResult			SelectProjectToSave			( System::String^ resources_path, System::String^ initial );

		private:
			ImageList^				m_image_list_large;
			ImageList^				m_image_list_small;
			bool					m_b_save_project_mode;
			System::String^			m_root_path;
			System::String^			m_current_path;

			void					browse_down					( System::String^ folder );
			void					browse_back					( );
			void					initialize_dialog			( bool b_save_mode, System::String^ resources_root_path, System::String^ focused_item );
			void					fill_current_path			( );
			void					in_constructor				( );
			void					projects_list_view_ItemActivate( System::Object^ , System::EventArgs^ );
			void					projects_list_view_SelectedIndexChanged( System::Object^ , System::EventArgs^ );
			void					selected_text_box_TextChanged( System::Object^ , System::EventArgs^ );
			void					ok_clicked					( System::Object^, System::EventArgs^);
			void					cancel_clicked				( System::Object^, System::EventArgs^);
			ListViewItem^			selected_list_item			( );
			void					selected_text_box_KeyDown	(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
};

} // namespace editor
} // namespace xray

#endif // #ifndef PROJECT_BROWSER_DIALOG_H_INCLUDED