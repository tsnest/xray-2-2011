////////////////////////////////////////////////////////////////////////////
//	Created		: 18.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITOR_COMMIT_H_INCLUDED
#define RESOURCE_EDITOR_COMMIT_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "resource_editor_resource.h"

namespace xray {
namespace editor {
		ref class resource_editor;

		public ref class resource_editor_commit : public System::Windows::Forms::Form
		{

		#pragma region | Initialize |

		public:
			resource_editor_commit( resource_editor^ re )
			:m_resource_editor( re )
			{
				InitializeComponent		( );
				fill_changes_list		( );
				//
				//TODO: Add the constructor code here
				//
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~resource_editor_commit()
			{
				if (components)
				{
					delete components;
				}
			}
		private: System::Windows::Forms::Button^  m_cancel_button;
		protected: 
		private: System::Windows::Forms::Button^  m_ok_button;
		private: System::Windows::Forms::ListBox^  m_list_box;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::ContextMenuStrip^  m_options_context_menu;
		private: System::Windows::Forms::ToolStripMenuItem^  revertChangesToolStripMenuItem;
		private: System::Windows::Forms::Button^  m_revert_selection_button;
		private: System::Windows::Forms::Panel^  panel1;


		private: System::ComponentModel::IContainer^  components;


		private:
			/// <summary>
			/// Required designer variable.
			/// </summary>


		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->components = (gcnew System::ComponentModel::Container());
				System::Windows::Forms::Panel^  control_panel;
				System::Windows::Forms::GroupBox^  groupBox1;
				this->m_cancel_button = (gcnew System::Windows::Forms::Button());
				this->m_revert_selection_button = (gcnew System::Windows::Forms::Button());
				this->m_ok_button = (gcnew System::Windows::Forms::Button());
				this->m_list_box = (gcnew System::Windows::Forms::ListBox());
				this->m_options_context_menu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
				this->revertChangesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->panel1 = (gcnew System::Windows::Forms::Panel());
				control_panel = (gcnew System::Windows::Forms::Panel());
				groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				control_panel->SuspendLayout();
				this->m_options_context_menu->SuspendLayout();
				this->panel1->SuspendLayout();
				this->SuspendLayout();
				// 
				// control_panel
				// 
				control_panel->Controls->Add(this->m_cancel_button);
				control_panel->Controls->Add(this->m_revert_selection_button);
				control_panel->Controls->Add(this->m_ok_button);
				control_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
				control_panel->Location = System::Drawing::Point(0, 176);
				control_panel->Name = L"control_panel";
				control_panel->Size = System::Drawing::Size(357, 41);
				control_panel->TabIndex = 0;
				// 
				// m_cancel_button
				// 
				this->m_cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_cancel_button->Location = System::Drawing::Point(270, 10);
				this->m_cancel_button->Name = L"m_cancel_button";
				this->m_cancel_button->Size = System::Drawing::Size(75, 23);
				this->m_cancel_button->TabIndex = 0;
				this->m_cancel_button->Text = L"Cancel";
				this->m_cancel_button->UseVisualStyleBackColor = true;
				this->m_cancel_button->Click += gcnew System::EventHandler(this, &resource_editor_commit::m_cancel_button_Click);
				// 
				// m_revert_selection_button
				// 
				this->m_revert_selection_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
				this->m_revert_selection_button->Location = System::Drawing::Point(12, 10);
				this->m_revert_selection_button->Name = L"m_revert_selection_button";
				this->m_revert_selection_button->Size = System::Drawing::Size(110, 23);
				this->m_revert_selection_button->TabIndex = 0;
				this->m_revert_selection_button->Text = L"Revert Selection";
				this->m_revert_selection_button->UseVisualStyleBackColor = true;
				this->m_revert_selection_button->Click += gcnew System::EventHandler(this, &resource_editor_commit::revertChanges_Click);
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
				groupBox1->Location = System::Drawing::Point(12, 175);
				groupBox1->Name = L"groupBox1";
				groupBox1->Size = System::Drawing::Size(333, 3);
				groupBox1->TabIndex = 1;
				groupBox1->TabStop = false;
				// 
				// m_list_box
				// 
				this->m_list_box->BorderStyle = System::Windows::Forms::BorderStyle::None;
				this->m_list_box->FormattingEnabled = true;
				this->m_list_box->Location = System::Drawing::Point(5, 5);
				this->m_list_box->Margin = System::Windows::Forms::Padding(3, 3, 3, 0);
				this->m_list_box->Name = L"m_list_box";
				this->m_list_box->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
				this->m_list_box->Size = System::Drawing::Size(321, 117);
				this->m_list_box->TabIndex = 2;
				this->m_list_box->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &resource_editor_commit::m_list_box_MouseUp);
				// 
				// m_options_context_menu
				// 
				this->m_options_context_menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->revertChangesToolStripMenuItem});
				this->m_options_context_menu->Name = L"m_options_context_menu";
				this->m_options_context_menu->Size = System::Drawing::Size(108, 26);
				// 
				// revertChangesToolStripMenuItem
				// 
				this->revertChangesToolStripMenuItem->Name = L"revertChangesToolStripMenuItem";
				this->revertChangesToolStripMenuItem->Size = System::Drawing::Size(107, 22);
				this->revertChangesToolStripMenuItem->Text = L"&Revert";
				this->revertChangesToolStripMenuItem->Click += gcnew System::EventHandler(this, &resource_editor_commit::revertChanges_Click);
				// 
				// label1
				// 
				this->label1->Location = System::Drawing::Point(12, 0);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(345, 35);
				this->label1->TabIndex = 3;
				this->label1->Text = L"Do you really want to commit changes in resources listed below\?";
				this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				// 
				// panel1
				// 
				this->panel1->BackColor = System::Drawing::SystemColors::Window;
				this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				this->panel1->Controls->Add(this->m_list_box);
				this->panel1->Location = System::Drawing::Point(12, 37);
				this->panel1->Name = L"panel1";
				this->panel1->Size = System::Drawing::Size(333, 128);
				this->panel1->TabIndex = 4;
				// 
				// resource_editor_commit
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(357, 217);
				this->ControlBox = false;
				this->Controls->Add(this->panel1);
				this->Controls->Add(groupBox1);
				this->Controls->Add(control_panel);
				this->Controls->Add(this->label1);
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
				this->Name = L"resource_editor_commit";
				this->ShowInTaskbar = false;
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
				this->Text = L"Commit Resources";
				control_panel->ResumeLayout(false);
				this->m_options_context_menu->ResumeLayout(false);
				this->panel1->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion
		#pragma endregion

		#pragma region |   Fileds   |

			private:
				resource_editor^					m_resource_editor;
		#pragma endregion

		#pragma region |   Methods  |

		private:
			void 				m_cancel_button_Click			( Object^ sender,System::EventArgs^ e );
			void 				revertChanges_Click				( Object^ sender,System::EventArgs^ e );
			void 				m_list_box_MouseUp				( Object^ sender, MouseEventArgs^ e ); 
			void				fill_changes_list				( );

		#pragma endregion

		}; // class resource_editor_commit
	} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_EDITOR_COMMIT_H_INCLUDED