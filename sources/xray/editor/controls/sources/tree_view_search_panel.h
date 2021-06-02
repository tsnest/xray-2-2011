////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EX_TREE_VIEW_SEARCH_PANEL_H_INCLUDED
#define EX_TREE_VIEW_SEARCH_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray
{
	namespace editor
	{
		namespace controls
		{

			/// <summary>
			/// Summary for tree_view_search_panel
			/// </summary>
			ref class tree_view_search_panel : public System::Windows::Forms::UserControl
			{

			#pragma region | Initialize |

			public:
				tree_view_search_panel(void)
				{
					InitializeComponent();
					//
					//TODO: Add the constructor code here
					//
					in_constructor();
				}

			protected:
				/// <summary>
				/// Clean up any resources being used.
				/// </summary>
				~tree_view_search_panel()
				{
					if (components)
					{
						delete components;
					}
				}
			
			private:
				void	in_constructor();
			internal: System::Windows::Forms::Button^  m_search_button;
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
					this->m_search_text_box = (gcnew System::Windows::Forms::TextBox());
					this->m_search_label = (gcnew System::Windows::Forms::Label());
					this->m_result_label = (gcnew System::Windows::Forms::Label());
					this->m_search_next = (gcnew System::Windows::Forms::Button());
					this->m_search_prev = (gcnew System::Windows::Forms::Button());
					this->m_search_close = (gcnew System::Windows::Forms::Button());
					this->m_search_button = (gcnew System::Windows::Forms::Button());
					this->SuspendLayout();
					// 
					// m_search_text_box
					// 
					this->m_search_text_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
						| System::Windows::Forms::AnchorStyles::Right));
					this->m_search_text_box->Location = System::Drawing::Point(53, 3);
					this->m_search_text_box->Name = L"m_search_text_box";
					this->m_search_text_box->Size = System::Drawing::Size(121, 20);
					this->m_search_text_box->TabIndex = 0;
					this->m_search_text_box->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &tree_view_search_panel::m_search_text_box_key_down);
					// 
					// m_search_label
					// 
					this->m_search_label->AutoSize = true;
					this->m_search_label->Location = System::Drawing::Point(3, 6);
					this->m_search_label->Name = L"m_search_label";
					this->m_search_label->Size = System::Drawing::Size(44, 13);
					this->m_search_label->TabIndex = 1;
					this->m_search_label->Text = L"Search:";
					// 
					// m_result_label
					// 
					this->m_result_label->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
					this->m_result_label->AutoSize = true;
					this->m_result_label->Location = System::Drawing::Point(3, 31);
					this->m_result_label->Name = L"m_result_label";
					this->m_result_label->Size = System::Drawing::Size(65, 13);
					this->m_result_label->TabIndex = 2;
					this->m_result_label->Text = L"Results: - / -";
					// 
					// m_search_next
					// 
					this->m_search_next->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
					this->m_search_next->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
					this->m_search_next->Location = System::Drawing::Point(146, 26);
					this->m_search_next->Name = L"m_search_next";
					this->m_search_next->Size = System::Drawing::Size(39, 23);
					this->m_search_next->TabIndex = 3;
					this->m_search_next->Text = L"next";
					this->m_search_next->UseVisualStyleBackColor = true;
					this->m_search_next->Click += gcnew System::EventHandler(this, &tree_view_search_panel::search_next_click);
					// 
					// m_search_prev
					// 
					this->m_search_prev->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
					this->m_search_prev->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
					this->m_search_prev->Location = System::Drawing::Point(101, 26);
					this->m_search_prev->Name = L"m_search_prev";
					this->m_search_prev->Size = System::Drawing::Size(39, 23);
					this->m_search_prev->TabIndex = 3;
					this->m_search_prev->Text = L"prev";
					this->m_search_prev->UseVisualStyleBackColor = true;
					this->m_search_prev->Click += gcnew System::EventHandler(this, &tree_view_search_panel::search_prev_click);
					// 
					// m_search_close
					// 
					this->m_search_close->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
					this->m_search_close->Location = System::Drawing::Point(191, 26);
					this->m_search_close->Name = L"m_search_close";
					this->m_search_close->Size = System::Drawing::Size(42, 23);
					this->m_search_close->TabIndex = 3;
					this->m_search_close->Text = L"close";
					this->m_search_close->UseVisualStyleBackColor = true;
					this->m_search_close->Click += gcnew System::EventHandler(this, &tree_view_search_panel::search_close_click);
					// 
					// m_search_button
					// 
					this->m_search_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
					this->m_search_button->Location = System::Drawing::Point(180, 3);
					this->m_search_button->Name = L"m_search_button";
					this->m_search_button->Size = System::Drawing::Size(53, 23);
					this->m_search_button->TabIndex = 3;
					this->m_search_button->Text = L"search";
					this->m_search_button->UseVisualStyleBackColor = true;
					this->m_search_button->Click += gcnew System::EventHandler(this, &tree_view_search_panel::search_button_click);
					// 
					// tree_view_search_panel
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->Controls->Add(this->m_search_button);
					this->Controls->Add(this->m_search_close);
					this->Controls->Add(this->m_search_prev);
					this->Controls->Add(this->m_search_next);
					this->Controls->Add(this->m_result_label);
					this->Controls->Add(this->m_search_label);
					this->Controls->Add(this->m_search_text_box);
					this->Enabled = false;
					this->Name = L"tree_view_search_panel";
					this->Size = System::Drawing::Size(236, 53);
					this->ResumeLayout(false);
					this->PerformLayout();

				}
		#pragma endregion

			#pragma endregion

			#pragma region |   Fields   |

			private:
				Collections::Generic::List<TreeNode^>^	m_finded_nodes;
				Int32									m_search_result_index;
				Color									m_selected_node_prev_color;

			internal:
				System::Windows::Forms::TextBox^	m_search_text_box;
				System::Windows::Forms::Label^		m_search_label;
				System::Windows::Forms::Label^		m_result_label;
				System::Windows::Forms::Button^		m_search_next;
				System::Windows::Forms::Button^		m_search_prev;
				System::Windows::Forms::Button^		m_search_close;

			public:
				TreeView^							m_parent_tree_view;

			#pragma endregion

			#pragma region |   Methods  |

			private:
				void		m_search_text_box_key_down		(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
				void		search_close_click				(System::Object^ sender , EventArgs^  e);
				void		search_prev_click				(System::Object^ sender , EventArgs^  e);
				void		search_next_click				(System::Object^ sender , EventArgs^  e);
				void		search_button_click				(System::Object^ sender , EventArgs^  e);
				void		search_nodes					(TreeNodeCollection^ nodes, String^ search_str);

			public:
				void		close							();

			#pragma endregion
};// class tree_view_search_panel

		}//namespace controls
	}//namespace editor
}//namespace xray

#endif // #ifndef EX_TREE_VIEW_SEARCH_PANEL_H_INCLUDED