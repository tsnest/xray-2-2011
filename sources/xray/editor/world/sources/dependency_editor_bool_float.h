////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEPENDENCY_EDITOR_BOOL_FLOAT_H_INCLUDED
#define DEPENDENCY_EDITOR_BOOL_FLOAT_H_INCLUDED


#include "dependency_editor.h"
#include "dependency_editor_type.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
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
			/// Summary for dependency_editor_bool_float
			/// </summary>
			public ref class dependency_editor_bool_float : public dependency_editor
			{

			#pragma region | Initialize |

			public:
				dependency_editor_bool_float(void):
				  dependency_editor(dependency_editor_type::bool_to_float)
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
				~dependency_editor_bool_float()
				{
					if (components)
					{
						delete components;
					}
				}
			private: System::Windows::Forms::TextBox^  m_text_box_1;
			private: System::Windows::Forms::TextBox^  m_text_box_2;
			protected: 

			protected: 


			private:
				void in_constructor();
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
					this->m_text_box_1 = (gcnew System::Windows::Forms::TextBox());
					this->m_text_box_2 = (gcnew System::Windows::Forms::TextBox());
					this->SuspendLayout();
					// 
					// m_text_box_1
					// 
					this->m_text_box_1->Dock = System::Windows::Forms::DockStyle::Left;
					this->m_text_box_1->Location = System::Drawing::Point(0, 0);
					this->m_text_box_1->Name = L"m_text_box_1";
					this->m_text_box_1->Size = System::Drawing::Size(61, 20);
					this->m_text_box_1->TabIndex = 0;
					this->m_text_box_1->Text = "0";
					// 
					// m_text_box_2
					// 
					this->m_text_box_2->Dock = System::Windows::Forms::DockStyle::Fill;
					this->m_text_box_2->Location = System::Drawing::Point(61, 0);
					this->m_text_box_2->Name = L"m_text_box_2";
					this->m_text_box_2->Size = System::Drawing::Size(59, 20);
					this->m_text_box_2->TabIndex = 0;
					this->m_text_box_2->Text = "75";
					// 
					// dependency_editor_bool_float
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->Controls->Add(this->m_text_box_2);
					this->Controls->Add(this->m_text_box_1);
					this->Name = L"dependency_editor_bool_float";
					this->Size = System::Drawing::Size(120, 21);
					this->ResumeLayout(false);
					this->PerformLayout();

				}
			#pragma endregion

			#pragma endregion 

			#pragma region | Properties |

			protected:
				property list_of_float^	values
				{
					virtual	list_of_float^	get		() override;
					virtual	void			set		(list_of_float^ value) override; 
				}
			
			#pragma endregion

			};// class dependency_editor_bool_float
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DEPENDENCY_EDITOR_BOOL_FLOAT_H_INCLUDED