////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPLACE_COMBO_BOX_H_INCLUDED
#define INPLACE_COMBO_BOX_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

typedef ComboBox::ObjectCollection ObjectCollection;

namespace xray
{
	namespace editor
	{
		/// <summary>
		/// Summary for inplace_combo_box
		/// </summary>
		public ref class inplace_combo_box : public System::Windows::Forms::UserControl
		{

		#pragma region | Initialize |

		public:
			inplace_combo_box(void)
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
			~inplace_combo_box()
			{
				if (components)
				{
					delete components;
				}
			}
		
		private:
		
		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->m_combo_box = (gcnew System::Windows::Forms::ComboBox());
				this->m_label = (gcnew System::Windows::Forms::Label());
				this->SuspendLayout();
				// 
				// m_combo_box
				// 
				this->m_combo_box->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_combo_box->FormattingEnabled = true;
				this->m_combo_box->Location = System::Drawing::Point(0, 0);
				this->m_combo_box->Name = L"m_combo_box";
				this->m_combo_box->Size = System::Drawing::Size(150, 21);
				this->m_combo_box->TabIndex = 0;
				this->m_combo_box->Visible = false;
				this->m_combo_box->SelectedIndexChanged += gcnew System::EventHandler(this, &inplace_combo_box::m_combo_box_SelectedIndexChanged);
				this->m_combo_box->DropDownClosed += gcnew System::EventHandler(this, &inplace_combo_box::m_combo_box_DropDownClosed);
				// 
				// m_label
				// 
				this->m_label->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_label->Location = System::Drawing::Point(0, 0);
				this->m_label->Name = L"m_label";
				this->m_label->Size = System::Drawing::Size(150, 21);
				this->m_label->TabIndex = 0;
				this->m_label->Text = L"new";
				this->m_label->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				this->m_label->Click += gcnew System::EventHandler(this, &inplace_combo_box::m_label_Click);
				// 
				// inplace_combo_box
				// 
				this->Controls->Add(this->m_combo_box);
				this->Controls->Add(this->m_label);
				this->Name = L"inplace_combo_box";
				this->Size = System::Drawing::Size(150, 21);
				this->ResumeLayout(false);

			}
	#pragma endregion

		#pragma endregion

		#pragma region |   Events   |

		public:
			event EventHandler<EventArgs^>^			SelectedIndexChanged;
			
		#pragma endregion

		#pragma region |   Fields   |

		private:
			System::ComponentModel::IContainer^		components;
			System::Windows::Forms::ComboBox^		m_combo_box;
			System::Windows::Forms::Label^			m_label;

		#pragma endregion

		#pragma region | Properties |

		public:
			property ObjectCollection^				Items
			{
				ObjectCollection^	get();
			}

			property Object^						SelectedItem
			{
				Object^				get();
				void				set(Object^ value);
			}

			property Int32							SelectedIndex
			{
				Int32				get();
				void				set(Int32 index);
			}

			property String^						SelectedText
			{
				String^				get();
			}

		#pragma endregion 

		#pragma region |   Methods  |

		private:
			void m_combo_box_SelectedIndexChanged	(System::Object^  sender, System::EventArgs^  e);
			void m_label_Click						(System::Object^  sender, System::EventArgs^  e);
			void m_combo_box_DropDownClosed			(System::Object^  sender, System::EventArgs^  e);

		public:
		
		#pragma endregion

		}; // class inplace_combo_box
	}//namespace editor
}//namespace xray
#endif // #ifndef INPLACE_COMBO_BOX_H_INCLUDED#pragma once
