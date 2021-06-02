////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TREE_VIEW_FILTER_PANEL_H_INCLUDED
#define TREE_VIEW_FILTER_PANEL_H_INCLUDED

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
			ref class tree_node_collection;
			ref class tree_view;
			ref class tree_node;
			/// <summary>
			/// Summary for ex_tree_view_filter_panel
			/// </summary>
			ref class tree_view_filter_panel : public System::Windows::Forms::UserControl
			{

			#pragma region | Initialize |

			public:
				tree_view_filter_panel(void)
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
				~tree_view_filter_panel()
				{
					if (components)
					{
						delete components;
					}
				}

			private:
				void	in_constructor();
			private: System::Windows::Forms::Label^  m_filter_label;
			private: System::Windows::Forms::TextBox^  m_filter_text_box;
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
					this->m_filter_label = (gcnew System::Windows::Forms::Label());
					this->m_filter_text_box = (gcnew System::Windows::Forms::TextBox());
					this->SuspendLayout();
					// 
					// m_filter_label
					// 
					this->m_filter_label->AutoSize = true;
					this->m_filter_label->Location = System::Drawing::Point(3, 7);
					this->m_filter_label->Name = L"m_filter_label";
					this->m_filter_label->Size = System::Drawing::Size(32, 13);
					this->m_filter_label->TabIndex = 0;
					this->m_filter_label->Text = L"Filter:";
					// 
					// m_filter_text_box
					// 
					this->m_filter_text_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
						| System::Windows::Forms::AnchorStyles::Right));
					this->m_filter_text_box->Location = System::Drawing::Point(41, 4);
					this->m_filter_text_box->Name = L"m_filter_text_box";
					this->m_filter_text_box->Size = System::Drawing::Size(197, 20);
					this->m_filter_text_box->TabIndex = 1;
					this->m_filter_text_box->TextChanged += gcnew System::EventHandler(this, &tree_view_filter_panel::filter_text_changed);
					// 
					// ex_tree_view_filter_panel
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->Controls->Add(this->m_filter_text_box);
					this->Controls->Add(this->m_filter_label);
					this->Name = L"ex_tree_view_filter_panel";
					this->Size = System::Drawing::Size(241, 27);
					this->ResumeLayout(false);
					this->PerformLayout();

				}
				#pragma endregion

			#pragma endregion

			#pragma region |   Fields   |

			private:
				Collections::Generic::Dictionary<String^, tree_node^>^	m_invisible_nodes;
				Collections::Generic::Dictionary<String^, String^>^		m_invisible_folders;

			public:
				tree_view^						m_parent_tree_view;
				Boolean								m_auto_expand_on_filter;

			#pragma endregion

			#pragma region |   Methods  |

			private:
				void			filter_text_changed			(System::Object^ sender, EventArgs^  e);
				void			filter_invisible_nodes		(String^ filter);
				void			filter_nodes				(TreeNodeCollection^ nodes, String^ filter);

			#pragma endregion

			};// class tree_view_filter_panel
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef TREE_VIEW_FILTER_PANEL_H_INCLUDED