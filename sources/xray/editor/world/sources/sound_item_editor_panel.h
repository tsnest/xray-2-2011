////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_ITEM_EDITOR_PANEL_H_INCLUDED
#define SOUND_ITEM_EDITOR_PANEL_H_INCLUDED

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
		ref class	sound_file_item;
		ref struct	sound_item_struct;

		/// <summary>
		/// Summary for sound_item_editor_panel
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class sound_item_editor_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
		{

		#pragma region | Initialize |

		public:
			sound_item_editor_panel(void)
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
			~sound_item_editor_panel()
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
				System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(sound_item_editor_panel::typeid));
				this->m_main_panel = (gcnew System::Windows::Forms::Panel());
				this->m_tool_strip = (gcnew System::Windows::Forms::ToolStrip());
				this->save_tool_strip_button = (gcnew System::Windows::Forms::ToolStripButton());
				this->m_tool_strip->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_main_panel
				// 
				this->m_main_panel->AllowDrop = true;
				this->m_main_panel->AutoScroll = true;
				this->m_main_panel->BackColor = System::Drawing::Color::White;
				this->m_main_panel->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
				this->m_main_panel->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_main_panel->Location = System::Drawing::Point(0, 25);
				this->m_main_panel->Name = L"m_main_panel";
				this->m_main_panel->Padding = System::Windows::Forms::Padding(5);
				this->m_main_panel->Size = System::Drawing::Size(504, 421);
				this->m_main_panel->TabIndex = 0;
				this->m_main_panel->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &sound_item_editor_panel::m_main_panel_drag_over);
				this->m_main_panel->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &sound_item_editor_panel::m_main_panel_drag_drop);
				// 
				// m_tool_strip
				// 
				this->m_tool_strip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->save_tool_strip_button});
				this->m_tool_strip->Location = System::Drawing::Point(0, 0);
				this->m_tool_strip->Name = L"m_tool_strip";
				this->m_tool_strip->Size = System::Drawing::Size(504, 25);
				this->m_tool_strip->TabIndex = 3;
				this->m_tool_strip->Text = L"toolStrip1";
				// 
				// save_tool_strip_button
				// 
				this->save_tool_strip_button->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
				this->save_tool_strip_button->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"save_tool_strip_button.Image")));
				this->save_tool_strip_button->ImageTransparentColor = System::Drawing::Color::Magenta;
				this->save_tool_strip_button->Name = L"save_tool_strip_button";
				this->save_tool_strip_button->Size = System::Drawing::Size(34, 22);
				this->save_tool_strip_button->Text = L"save";
				this->save_tool_strip_button->TextImageRelation = System::Windows::Forms::TextImageRelation::TextBeforeImage;
				this->save_tool_strip_button->Click += gcnew System::EventHandler(this, &sound_item_editor_panel::save_tool_strip_button_Click);
				// 
				// sound_item_editor_panel
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(504, 446);
				this->Controls->Add(this->m_main_panel);
				this->Controls->Add(this->m_tool_strip);
				this->DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(((((WeifenLuo::WinFormsUI::Docking::DockAreas::Float | WeifenLuo::WinFormsUI::Docking::DockAreas::DockLeft) 
					| WeifenLuo::WinFormsUI::Docking::DockAreas::DockRight) 
					| WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop) 
					| WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom));
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->HideOnClose = true;
				this->Name = L"sound_item_editor_panel";
				this->Text = L"sound_item_editor_panel";
				this->m_tool_strip->ResumeLayout(false);
				this->m_tool_strip->PerformLayout();
				this->ResumeLayout(false);
				this->PerformLayout();

			}
		
#pragma endregion
		
		#pragma endregion

		#pragma region |   Events   |

		public:
			//event EventHandler<sound_file_selection_event_args^>^ raw_file_selected;
			//event EventHandler<sound_file_selection_event_args^>^ raw_file_deselected;
			
		#pragma endregion

		#pragma region |   Fields   |

		private:
			System::ComponentModel::IContainer^			components;
			System::Windows::Forms::Panel^				m_main_panel;

			System::Windows::Forms::ToolStrip^			m_tool_strip;
private: System::Windows::Forms::ToolStripButton^  save_tool_strip_button;






			sound_item_struct^	m_selected_item;

		#pragma endregion

		#pragma region | Properties |
		
		public:
			property sound_item_struct^	selected_item
			{
				sound_item_struct^		get		();
				void					set		(sound_item_struct^ value);
			}

		#pragma endregion 

		#pragma region |   Methods  |

		private:

			void m_main_panel_drag_over					(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
			void m_main_panel_drag_drop					(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e);
			void save_tool_strip_button_Click			(System::Object^  sender, System::EventArgs^  e);

		#pragma endregion

		};//class sound_item_editor_panel
	}//namespace editor
}//namespace xray

#endif // #ifndef SOUND_ITEM_EDITOR_PANEL_H_INCLUDED