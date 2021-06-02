////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SELECTOR_H_INCLUDED
#define EFFECT_SELECTOR_H_INCLUDED


using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray
{
	namespace editor
	{

		public ref class effect_selector : public System::Windows::Forms::Form
		{
		public:
			effect_selector()
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
			~effect_selector()
			{
				if (components)
				{
					delete components;
				}
			}
		private: xray::editor::controls::tree_view^		m_tree_view;
		private: System::Windows::Forms::Panel^  panel1;
		private: System::Windows::Forms::Button^  ok_button;
		private: System::Windows::Forms::Button^  m_cancel_button;

		private: System::ComponentModel::IContainer^	components;

		private:
			/// <summary>
			/// Required designer variable.
			/// </summary>


		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent()
			{
				this->components = (gcnew System::ComponentModel::Container());
				this->m_tree_view = (gcnew xray::editor::controls::tree_view());
				this->panel1 = (gcnew System::Windows::Forms::Panel());
				this->m_cancel_button = (gcnew System::Windows::Forms::Button());
				this->ok_button = (gcnew System::Windows::Forms::Button());
				this->panel1->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_tree_view
				// 
				this->m_tree_view->auto_expand_on_filter = false;
				this->m_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_tree_view->filter_visible = false;
				this->m_tree_view->is_multiselect = false;
				this->m_tree_view->is_selectable_groups = true;
				this->m_tree_view->is_selection_or_groups_or_items = false;
				this->m_tree_view->Location = System::Drawing::Point(8, 8);
				this->m_tree_view->Name = L"m_tree_view";
				this->m_tree_view->PathSeparator = L"/";
				this->m_tree_view->Size = System::Drawing::Size(268, 208);
				this->m_tree_view->source = nullptr;
				this->m_tree_view->TabIndex = 0;
				// 
				// panel1
				// 
				this->panel1->Controls->Add(this->m_cancel_button);
				this->panel1->Controls->Add(this->ok_button);
				this->panel1->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->panel1->Location = System::Drawing::Point(8, 216);
				this->panel1->Name = L"panel1";
				this->panel1->Size = System::Drawing::Size(268, 38);
				this->panel1->TabIndex = 1;
				// 
				// m_cancel_button
				// 
				this->m_cancel_button->Anchor = System::Windows::Forms::AnchorStyles::Right;
				this->m_cancel_button->Location = System::Drawing::Point(190, 12);
				this->m_cancel_button->Name = L"m_cancel_button";
				this->m_cancel_button->Size = System::Drawing::Size(75, 23);
				this->m_cancel_button->TabIndex = 0;
				this->m_cancel_button->Text = L"Cancel";
				this->m_cancel_button->UseVisualStyleBackColor = true;
				this->m_cancel_button->Click += gcnew System::EventHandler(this, &effect_selector::m_cancel_button_Click);
				// 
				// ok_button
				// 
				this->ok_button->Anchor = System::Windows::Forms::AnchorStyles::Right;
				this->ok_button->Location = System::Drawing::Point(109, 12);
				this->ok_button->Name = L"ok_button";
				this->ok_button->Size = System::Drawing::Size(75, 23);
				this->ok_button->TabIndex = 0;
				this->ok_button->Text = L"OK";
				this->ok_button->UseVisualStyleBackColor = true;
				this->ok_button->Click += gcnew System::EventHandler(this, &effect_selector::ok_button_Click);
				// 
				// effect_selector
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(284, 262);
				this->Controls->Add(this->m_tree_view);
				this->Controls->Add(this->panel1);
				this->Name = L"effect_selector";
				this->Padding = System::Windows::Forms::Padding(8);
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
				this->Text = L"Effect Selector";
				this->Load += gcnew System::EventHandler(this, &effect_selector::effect_selector_Load);
				this->panel1->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion

		public:
			property	HashSet<System::String^>^	existing;
			property	bool						is_for_texture;
			property	System::String^				selected_item;
			property	System::String^				selecting_stage;

		private:
			void	effect_selector_Load				(Object^  sender, System::EventArgs^  e);
			void	effect_items_loaded					(Object^  sender, System::EventArgs^  e);
			void	m_tree_view_NodeMouseDoubleClick	(Object^  sender, TreeNodeMouseClickEventArgs^  e);
			void	ok_button_Click						(Object^  sender, System::EventArgs^  e);
			void	m_cancel_button_Click				(Object^  sender, System::EventArgs^  e);

		};// ref class effect_selector
	
	}// namespace editor
}// namespace xray

#endif // #ifndef EFFECT_SELECTOR_H_INCLUDED