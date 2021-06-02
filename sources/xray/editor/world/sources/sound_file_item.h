////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_FILE_ITEM_H_INCLUDED
#define SOUND_FILE_ITEM_H_INCLUDED

#include "sound_item_struct.h"

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
		ref class sound_item_editor_panel;
		/// <summary>
		/// Summary for sound_file_item
		/// </summary>
		public ref class sound_file_item : public System::Windows::Forms::UserControl
		{

		#pragma region | Initialize |

		public:
			sound_file_item(void)
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
			~sound_file_item()
			{
				if (components)
				{
					delete components;
				}
			}
	
		protected: 

		private:
			void in_constructor();

		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->m_split_container = (gcnew System::Windows::Forms::SplitContainer());
				this->m_split_container_inner = (gcnew System::Windows::Forms::SplitContainer());
				this->m_delete_button = (gcnew System::Windows::Forms::Button());
				this->m_file_name_label = (gcnew System::Windows::Forms::Label());
				this->m_split_container->Panel1->SuspendLayout();
				this->m_split_container->SuspendLayout();
				this->m_split_container_inner->Panel1->SuspendLayout();
				this->m_split_container_inner->Panel2->SuspendLayout();
				this->m_split_container_inner->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_split_container
				// 
				this->m_split_container->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_split_container->Location = System::Drawing::Point(0, 0);
				this->m_split_container->Name = L"m_split_container";
				// 
				// m_split_container.Panel1
				// 
				this->m_split_container->Panel1->Controls->Add(this->m_split_container_inner);
				// 
				// m_split_container.Panel2
				// 
				this->m_split_container->Panel2->Padding = System::Windows::Forms::Padding(0, 2, 2, 0);
				this->m_split_container->Panel2->ControlAdded += gcnew System::Windows::Forms::ControlEventHandler(this, &sound_file_item::m_split_container_Panel2_ControlAdded);
				this->m_split_container->Panel2->ControlRemoved += gcnew System::Windows::Forms::ControlEventHandler(this, &sound_file_item::m_split_container_Panel2_ControlRemoved);
				this->m_split_container->Size = System::Drawing::Size(376, 48);
				this->m_split_container->SplitterDistance = 135;
				this->m_split_container->TabIndex = 0;
				// 
				// m_split_container_inner
				// 
				this->m_split_container_inner->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_split_container_inner->Location = System::Drawing::Point(0, 0);
				this->m_split_container_inner->Name = L"m_split_container_inner";
				// 
				// m_split_container_inner.Panel1
				// 
				this->m_split_container_inner->Panel1->Controls->Add(this->m_delete_button);
				// 
				// m_split_container_inner.Panel2
				// 
				this->m_split_container_inner->Panel2->Controls->Add(this->m_file_name_label);
				this->m_split_container_inner->Size = System::Drawing::Size(135, 48);
				this->m_split_container_inner->SplitterDistance = 32;
				this->m_split_container_inner->TabIndex = 1;
				// 
				// m_delete_button
				// 
				this->m_delete_button->Dock = System::Windows::Forms::DockStyle::Top;
				this->m_delete_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
				this->m_delete_button->Location = System::Drawing::Point(0, 0);
				this->m_delete_button->Name = L"m_delete_button";
				this->m_delete_button->Size = System::Drawing::Size(32, 21);
				this->m_delete_button->TabIndex = 0;
				this->m_delete_button->Text = L"X";
				this->m_delete_button->UseVisualStyleBackColor = true;
				this->m_delete_button->Click += gcnew System::EventHandler(this, &sound_file_item::m_delete_button_Click);
				// 
				// m_file_name_label
				// 
				this->m_file_name_label->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_file_name_label->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, 
					System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
				this->m_file_name_label->Location = System::Drawing::Point(0, 0);
				this->m_file_name_label->Name = L"m_file_name_label";
				this->m_file_name_label->Size = System::Drawing::Size(99, 48);
				this->m_file_name_label->TabIndex = 0;
				this->m_file_name_label->Text = L"Some Sound";
				// 
				// sound_file_item
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->BackColor = System::Drawing::SystemColors::ControlLight;
				this->Controls->Add(this->m_split_container);
				this->Name = L"sound_file_item";
				this->Size = System::Drawing::Size(376, 48);
				this->m_split_container->Panel1->ResumeLayout(false);
				this->m_split_container->ResumeLayout(false);
				this->m_split_container_inner->Panel1->ResumeLayout(false);
				this->m_split_container_inner->Panel2->ResumeLayout(false);
				this->m_split_container_inner->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion

		#pragma endregion 

		#pragma region |   Fields   |

		private:
			System::ComponentModel::IContainer^			components;
			System::Windows::Forms::SplitContainer^		m_split_container;
			System::Windows::Forms::Label^				m_file_name_label;
			System::Windows::Forms::SplitContainer^		m_split_container_inner;
			System::Windows::Forms::Button^				m_delete_button;
			sound_item_editor_panel^					m_parent_editor;

			sound_item_struct::sound^					m_selected_sound;

		#pragma endregion

		#pragma region | Properties |

		public:
			property String^					sound_name
			{
				String^							get		();
				void							set		(String^ value);
			}

			property sound_item_editor_panel^	parent_editor
			{
				sound_item_editor_panel^		get		();
				void							set		(sound_item_editor_panel^ value);
			}

			property sound_item_struct::sound^	selected_sound
			{
				sound_item_struct::sound^		get		();
				void							set		(sound_item_struct::sound^ value);
			}

			property ControlCollection^	properties
			{
				ControlCollection^				get		(){return m_split_container->Panel2->Controls;}
			}
					
		#pragma endregion 

		#pragma region |   Methods  |

		private:
			void m_split_container_Panel2_ControlAdded			(System::Object^  sender, ControlEventArgs^  e);
			void m_split_container_Panel2_ControlRemoved		(System::Object^  sender, ControlEventArgs^  e);
			void m_split_container_Panel2_ControlSizeChanged	(System::Object^  sender, EventArgs^  e);
			void m_delete_button_Click							(System::Object^  sender, System::EventArgs^  e);

		public:
		
		#pragma endregion

		private: 
				 
}; // class sound_file_item
	}//namespace editor
}//namespace xray

#endif // #ifndef SOUND_FILE_ITEM_H_INCLUDED
