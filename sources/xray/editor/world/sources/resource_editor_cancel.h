////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITOR_CANCEL_H_INCLUDED
#define RESOURCE_EDITOR_CANCEL_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray
{
	namespace editor
	{
		ref class resource_editor_resource;
			
		public ref class resource_editor_cancel : public System::Windows::Forms::Form
		{

		#pragma region | Initialize |

		public:
			resource_editor_cancel(System::Collections::Generic::List<System::String^>^ changed_names)
			{
				InitializeComponent();
				//
				//TODO: Add the constructor code here
				//
				m_changed_names = changed_names;
				for each(System::String^ option_name in m_changed_names)
				{
					m_list_box->Items->Add(option_name);
				}
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~resource_editor_cancel()
			{
				if (components)
				{
					delete components;
				}
			}

		private:
			/// <summary>
			/// Required designer variable.
			/// </summary>
		private: System::Windows::Forms::Button^  m_cancel_button;
		protected: 
		private: System::Windows::Forms::Button^  m_ok_button;
		private: System::Windows::Forms::ListBox^  m_list_box;


		private: System::Windows::Forms::ToolStripMenuItem^  revertChangesToolStripMenuItem;

		private: System::Windows::Forms::Panel^  control_panel;
		private: System::Windows::Forms::GroupBox^  groupBox1;


		private: System::ComponentModel::IContainer^  components;
		private: System::Windows::Forms::Panel^  panel1;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::PictureBox^  pictureBox1;
		private: System::Windows::Forms::Panel^  panel2;
		private: System::Windows::Forms::Label^  label1;

		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->control_panel = (gcnew System::Windows::Forms::Panel());
				this->m_cancel_button = (gcnew System::Windows::Forms::Button());
				this->m_ok_button = (gcnew System::Windows::Forms::Button());
				this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->m_list_box = (gcnew System::Windows::Forms::ListBox());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->panel1 = (gcnew System::Windows::Forms::Panel());
				this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
				this->label2 = (gcnew System::Windows::Forms::Label());
				this->panel2 = (gcnew System::Windows::Forms::Panel());
				this->control_panel->SuspendLayout();
				this->panel1->SuspendLayout();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
				this->panel2->SuspendLayout();
				this->SuspendLayout();
				// 
				// control_panel
				// 
				this->control_panel->Controls->Add(this->m_cancel_button);
				this->control_panel->Controls->Add(this->m_ok_button);
				this->control_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->control_panel->Location = System::Drawing::Point(0, 193);
				this->control_panel->Name = L"control_panel";
				this->control_panel->Size = System::Drawing::Size(357, 41);
				this->control_panel->TabIndex = 0;
				// 
				// m_cancel_button
				// 
				this->m_cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_cancel_button->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->m_cancel_button->Location = System::Drawing::Point(270, 10);
				this->m_cancel_button->Name = L"m_cancel_button";
				this->m_cancel_button->Size = System::Drawing::Size(75, 23);
				this->m_cancel_button->TabIndex = 0;
				this->m_cancel_button->Text = L"Cancel";
				this->m_cancel_button->UseVisualStyleBackColor = true;
				// 
				// m_ok_button
				// 
				this->m_ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_ok_button->DialogResult = System::Windows::Forms::DialogResult::OK;
				this->m_ok_button->Location = System::Drawing::Point(189, 10);
				this->m_ok_button->Name = L"m_ok_button";
				this->m_ok_button->Size = System::Drawing::Size(75, 23);
				this->m_ok_button->TabIndex = 0;
				this->m_ok_button->Text = L"OK";
				this->m_ok_button->UseVisualStyleBackColor = true;
				// 
				// groupBox1
				// 
				this->groupBox1->Location = System::Drawing::Point(12, 192);
				this->groupBox1->Margin = System::Windows::Forms::Padding(33, 3, 3, 3);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Size = System::Drawing::Size(330, 3);
				this->groupBox1->TabIndex = 1;
				this->groupBox1->TabStop = false;
				// 
				// m_list_box
				// 
				this->m_list_box->BorderStyle = System::Windows::Forms::BorderStyle::None;
				this->m_list_box->FormattingEnabled = true;
				this->m_list_box->Location = System::Drawing::Point(5, 5);
				this->m_list_box->Name = L"m_list_box";
				this->m_list_box->SelectionMode = System::Windows::Forms::SelectionMode::None;
				this->m_list_box->Size = System::Drawing::Size(318, 117);
				this->m_list_box->TabIndex = 2;
				// 
				// label1
				// 
				this->label1->Dock = System::Windows::Forms::DockStyle::Fill;
				this->label1->Location = System::Drawing::Point(32, 0);
				this->label1->Name = L"label1";
				this->label1->Padding = System::Windows::Forms::Padding(5, 0, 0, 0);
				this->label1->Size = System::Drawing::Size(301, 41);
				this->label1->TabIndex = 3;
				this->label1->Text = L"Do you really want to cancel modifications in resources listed below\?";
				this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				// 
				// panel1
				// 
				this->panel1->Controls->Add(this->label1);
				this->panel1->Controls->Add(this->pictureBox1);
				this->panel1->Location = System::Drawing::Point(12, 12);
				this->panel1->Name = L"panel1";
				this->panel1->Size = System::Drawing::Size(333, 41);
				this->panel1->TabIndex = 4;
				// 
				// pictureBox1
				// 
				this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Left;
				this->pictureBox1->Location = System::Drawing::Point(0, 0);
				this->pictureBox1->Name = L"pictureBox1";
				this->pictureBox1->Size = System::Drawing::Size(32, 41);
				this->pictureBox1->TabIndex = 4;
				this->pictureBox1->TabStop = false;
				// 
				// label2
				// 
				this->label2->Location = System::Drawing::Point(0, 0);
				this->label2->Name = L"label2";
				this->label2->Size = System::Drawing::Size(100, 23);
				this->label2->TabIndex = 0;
				// 
				// panel2
				// 
				this->panel2->BackColor = System::Drawing::SystemColors::Window;
				this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				this->panel2->Controls->Add(this->m_list_box);
				this->panel2->Location = System::Drawing::Point(12, 56);
				this->panel2->Name = L"panel2";
				this->panel2->Size = System::Drawing::Size(330, 129);
				this->panel2->TabIndex = 5;
				// 
				// resource_editor_cancel
				// 
				this->AcceptButton = this->m_ok_button;
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->CancelButton = this->m_cancel_button;
				this->ClientSize = System::Drawing::Size(357, 234);
				this->ControlBox = false;
				this->Controls->Add(this->panel2);
				this->Controls->Add(this->groupBox1);
				this->Controls->Add(this->control_panel);
				this->Controls->Add(this->panel1);
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
				this->Name = L"resource_editor_cancel";
				this->ShowInTaskbar = false;
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
				this->Text = L"Cancel Modification";
				this->control_panel->ResumeLayout(false);
				this->panel1->ResumeLayout(false);
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
				this->panel2->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion

		#pragma endregion

		#pragma region |   Fileds   |

		private:
			System::Collections::Generic::List<System::String^>^		m_changed_names;

		#pragma endregion

		}; // class resource_editor_cancel
	} // namespace editor
} // namespace xray
#endif // #ifndef RESOURCE_EDITOR_CANCEL_H_INCLUDED