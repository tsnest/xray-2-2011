////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEPENDENCY_EDITOR_FLOAT_BOOL_H_INCLUDED
#define DEPENDENCY_EDITOR_FLOAT_BOOL_H_INCLUDED

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
			/// Summary for dependency_editor_float_bool
			/// </summary>
			public ref class dependency_editor_float_bool : public dependency_editor
			{
			
			#pragma region | Initialize |

			public:
				dependency_editor_float_bool(void):
					dependency_editor(dependency_editor_type::float_to_bool)
				{
					InitializeComponent();
					//
					//TODO: Add the constructor code here
					//
					Dock = DockStyle::Fill;
					in_constructor();
				}

			protected:
				/// <summary>
				/// Clean up any resources being used.
				/// </summary>
				~dependency_editor_float_bool()
				{
					if (components)
					{
						delete components;
					}
				}
			private: System::Windows::Forms::TextBox^  m_text_box;
			private: System::Windows::Forms::ComboBox^  m_combo_box;
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
					this->m_text_box = (gcnew System::Windows::Forms::TextBox());
					this->m_combo_box = (gcnew System::Windows::Forms::ComboBox());
					this->SuspendLayout();
					// 
					// m_text_box
					// 
					this->m_text_box->Dock = System::Windows::Forms::DockStyle::Fill;
					this->m_text_box->Location = System::Drawing::Point(43, 0);
					this->m_text_box->Name = L"m_text_box";
					this->m_text_box->Size = System::Drawing::Size(93, 20);
					this->m_text_box->TabIndex = 0;
					this->m_text_box->Text = "50";
					// 
					// m_combo_box
					// 
					this->m_combo_box->Dock = System::Windows::Forms::DockStyle::Left;
					this->m_combo_box->FormattingEnabled = true;
					this->m_combo_box->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L">", L"<", L"=", L"!=", L">=", L"<="});
					this->m_combo_box->Location = System::Drawing::Point(0, 0);
					this->m_combo_box->Name = L"m_combo_box";
					this->m_combo_box->Size = System::Drawing::Size(43, 21);
					this->m_combo_box->TabIndex = 1;
					this->m_combo_box->SelectedIndex = 0;
					// 
					// dependency_editor_float_bool
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->Controls->Add(this->m_text_box);
					this->Controls->Add(this->m_combo_box);
					this->Name = L"dependency_editor_float_bool";
					this->Size = System::Drawing::Size(136, 21);
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

			};// class dependency_editor_float_bool
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DEPENDENCY_EDITOR_FLOAT_BOOL_H_INCLUDED