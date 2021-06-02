////////////////////////////////////////////////////////////////////////////
//	Created		: 25.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_FILE_ITEM_DEPENDENCY_H_INCLUDED
#define SOUND_FILE_ITEM_DEPENDENCY_H_INCLUDED

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
		ref class	inplace_combo_box;
		ref struct	sound_item_dependency_struct;
		ref class	sound_file_item_property;

		/// <summary>
		/// Summary for sound_file_item_dependency
		/// </summary>
		public ref class sound_file_item_dependency : public System::Windows::Forms::UserControl
		{

		#pragma region | Initialize |

		public:
			sound_file_item_dependency(void)
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
			~sound_file_item_dependency()
			{
				if (components)
				{
					delete components;
				}
			}

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
				this->m_split_container->Panel1->SuspendLayout();
				this->m_split_container->SuspendLayout();
				this->m_split_container_inner->Panel1->SuspendLayout();
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
				this->m_split_container->Size = System::Drawing::Size(452, 21);
				this->m_split_container->SplitterDistance = 203;
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
				this->m_split_container_inner->Size = System::Drawing::Size(203, 21);
				this->m_split_container_inner->SplitterDistance = 33;
				this->m_split_container_inner->TabIndex = 0;
				// 
				// m_delete_button
				// 
				this->m_delete_button->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_delete_button->Enabled = false;
				this->m_delete_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
				this->m_delete_button->Location = System::Drawing::Point(0, 0);
				this->m_delete_button->Name = L"m_delete_button";
				this->m_delete_button->Size = System::Drawing::Size(33, 21);
				this->m_delete_button->TabIndex = 0;
				this->m_delete_button->Text = L"X";
				this->m_delete_button->UseVisualStyleBackColor = true;
				this->m_delete_button->Click += gcnew System::EventHandler(this, &sound_file_item_dependency::m_delete_button_Click);
				// 
				// sound_file_item_dependency
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->Controls->Add(this->m_split_container);
				this->Name = L"sound_file_item_dependency";
				this->Size = System::Drawing::Size(452, 21);
				this->m_split_container->Panel1->ResumeLayout(false);
				this->m_split_container->ResumeLayout(false);
				this->m_split_container_inner->Panel1->ResumeLayout(false);
				this->m_split_container_inner->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion

		#pragma endregion 

		#pragma region |   Events   |

		public:
			event EventHandler<EventArgs^>^		SelectedIndexChanged;

		#pragma endregion 

		#pragma region |   Fields   |

		private:
			System::ComponentModel::IContainer^			components;
			System::Windows::Forms::SplitContainer^		m_split_container;
			inplace_combo_box^							m_dependencies_combo_box;
			sound_file_item_property^					m_parent_property;
private: System::Windows::Forms::SplitContainer^  m_split_container_inner;
private: System::Windows::Forms::Button^  m_delete_button;
		 sound_item_struct::property_dependency^		m_selected_dependency;

		#pragma endregion

		#pragma region | Properties |

		public:
			property ComboBox::ObjectCollection^	combobox_items
			{
				ComboBox::ObjectCollection^		get();
			}

			property Int32							selected_combobox_index
			{
				Int32							get();
				void							set(Int32 value);
			}

			property sound_item_dependency_struct^	selected_combobox_item
			{
				sound_item_dependency_struct^	get();
			}

			property Control^						editor_container
			{
				Control^						get();
			}

			property sound_item_struct::property_dependency^	selected_dependency
			{
				sound_item_struct::property_dependency^	get();
				void									set(sound_item_struct::property_dependency^ value);
			}

			property list_of_float^					values
			{
				list_of_float^					get();
				void							set(list_of_float^ value);
			}

			property sound_file_item_property^		parent_property
			{
				sound_file_item_property^		get();
				void							set(sound_file_item_property^ value);
			}
					
		#pragma endregion 

		#pragma region |  Methods  |

		private:
			void combobox_selected_index_changed	(Object^ sender, EventArgs^ e);
			void m_delete_button_Click				(Object^  sender, EventArgs^  e);

		public:
			void check_editor_type					();

		#pragma endregion
		};// class sound_file_item_dependency
	}//namespace editor
}//namespace xray
#endif // #ifndef SOUND_FILE_ITEM_DEPENDENCY_H_INCLUDED