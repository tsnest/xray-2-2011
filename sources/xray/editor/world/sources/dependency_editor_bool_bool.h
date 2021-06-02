////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEPENDENCY_EDITOR_BOOL_BOOL_H_INCLUDED
#define DEPENDENCY_EDITOR_BOOL_BOOL_H_INCLUDED

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
			/// Summary for dependency_editor_bool_bool
			/// </summary>
			public ref class dependency_editor_bool_bool : public dependency_editor
				
			{

			#pragma region | Initialize |

			public:
				dependency_editor_bool_bool(void):
					dependency_editor(dependency_editor_type::bool_to_bool)
				{
					InitializeComponent();
					//
					//TODO: Add the constructor code here
					//
					in_constructor();
					Dock = DockStyle::Fill;
				}

			protected:
				/// <summary>
				/// Clean up any resources being used.
				/// </summary>
				~dependency_editor_bool_bool()
				{
					if (components)
					{
						delete components;
					}
				}
			private: System::Windows::Forms::CheckBox^  m_check_box;
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
					this->m_check_box = (gcnew System::Windows::Forms::CheckBox());
					this->SuspendLayout();
					// 
					// m_check_box
					// 
					this->m_check_box->AutoSize = true;
					this->m_check_box->Dock = System::Windows::Forms::DockStyle::Fill;
					this->m_check_box->Location = System::Drawing::Point(0, 0);
					this->m_check_box->Name = L"m_check_box";
					this->m_check_box->Size = System::Drawing::Size(100, 21);
					this->m_check_box->TabIndex = 0;
					this->m_check_box->Text = L"not";
					this->m_check_box->UseVisualStyleBackColor = true;
					// 
					// dependency_editor_bool_bool
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->Controls->Add(this->m_check_box);
					this->Name = L"dependency_editor_bool_bool";
					this->Size = System::Drawing::Size(100, 21);
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

			};//class dependency_editor_bool_bool
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DEPENDENCY_EDITOR_BOOL_BOOL_H_INCLUDED