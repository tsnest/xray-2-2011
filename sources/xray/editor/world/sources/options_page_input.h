#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "options_page.h"
#include "input_keys_holder.h"

namespace xray {
namespace editor {

	class editor_world;

	ref class shortcut_edit_box;

	/// <summary>
	/// Summary for options_page_input
	/// </summary>
	public ref class options_page_input : public System::Windows::Forms::UserControl, public options_page
	{
	public:

		enum class	enum_action_chage_id { enum_action_chage_add, enum_action_chage_remove };
		ref class action_item
		{
		public:
			action_item( String^ n , String^ k ):
			name		( n ),
			keys		( k )
			{ }

			String^ name;
			String^ keys;
		};

		ref class action_item_change : public action_item
		{
		public:
			action_item_change( enum_action_chage_id c, String^ n , String^ k ): 
			action_item ( n, k ),
			change_id	( c )
			{ }	

			enum_action_chage_id change_id;
		};


	public:
		options_page_input	( editor_world& world ):
		m_world				( world ),
		m_shortcut_bar_first_select	( true ),
		m_initialized		( false )
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}


		virtual void	activate_page		( );
		virtual void	deactivate_page		( );
		virtual bool	accept_changes		( );
		virtual void	cancel_changes		( );
		virtual bool	changed				( );

		virtual System::Windows::Forms::Control^	get_control	( ) { return this; }


	protected:

	private: System::Windows::Forms::Label^		label1;
	private: System::Windows::Forms::TextBox^	filter_text_box;
	private: System::Windows::Forms::ListBox^	actions_list_box;
	private: System::Windows::Forms::Label^		label4;
	private: System::Windows::Forms::Label^  shortcut_for_selected_label;

	private: System::Windows::Forms::TextBox^	shortcut_text_box;
	private: System::Windows::Forms::Button^	assign_button;
	private: System::Windows::Forms::Button^	remove_button;

	private: System::Windows::Forms::ComboBox^	shortcuts_combo_box;
	private: System::Windows::Forms::ComboBox^  assigned_to_comboBox;
	private: System::Windows::Forms::Label^  asigned_to_label;




	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~options_page_input()
		{
			if (components)
			{
				delete components;
			}
		}

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
			this->asigned_to_label = (gcnew System::Windows::Forms::Label());
			this->shortcuts_combo_box = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->filter_text_box = (gcnew System::Windows::Forms::TextBox());
			this->actions_list_box = (gcnew System::Windows::Forms::ListBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->shortcut_for_selected_label = (gcnew System::Windows::Forms::Label());
			this->shortcut_text_box = (gcnew System::Windows::Forms::TextBox());
			this->assign_button = (gcnew System::Windows::Forms::Button());
			this->remove_button = (gcnew System::Windows::Forms::Button());
			this->assigned_to_comboBox = (gcnew System::Windows::Forms::ComboBox());
			this->SuspendLayout();
			// 
			// asigned_to_label
			// 
			this->asigned_to_label->AutoSize = true;
			this->asigned_to_label->Enabled = false;
			this->asigned_to_label->Location = System::Drawing::Point(14, 223);
			this->asigned_to_label->Name = L"asigned_to_label";
			this->asigned_to_label->Size = System::Drawing::Size(160, 13);
			this->asigned_to_label->TabIndex = 20;
			this->asigned_to_label->Text = L"Shortcut is currently assigned to:";
			// 
			// shortcuts_combo_box
			// 
			this->shortcuts_combo_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->shortcuts_combo_box->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->shortcuts_combo_box->Enabled = false;
			this->shortcuts_combo_box->FormattingEnabled = true;
			this->shortcuts_combo_box->Location = System::Drawing::Point(12, 149);
			this->shortcuts_combo_box->Name = L"shortcuts_combo_box";
			this->shortcuts_combo_box->Size = System::Drawing::Size(268, 21);
			this->shortcuts_combo_box->TabIndex = 18;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(14, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(121, 13);
			this->label1->TabIndex = 14;
			this->label1->Text = L"Filter actions containing:";
			// 
			// filter_text_box
			// 
			this->filter_text_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->filter_text_box->Location = System::Drawing::Point(12, 17);
			this->filter_text_box->Name = L"filter_text_box";
			this->filter_text_box->Size = System::Drawing::Size(348, 20);
			this->filter_text_box->TabIndex = 10;
			this->filter_text_box->TextChanged += gcnew System::EventHandler(this, &options_page_input::filter_text_box_TextChanged);
			// 
			// actions_list_box
			// 
			this->actions_list_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->actions_list_box->FormattingEnabled = true;
			this->actions_list_box->Location = System::Drawing::Point(12, 43);
			this->actions_list_box->Name = L"actions_list_box";
			this->actions_list_box->Size = System::Drawing::Size(348, 82);
			this->actions_list_box->TabIndex = 11;
			this->actions_list_box->SelectedIndexChanged += gcnew System::EventHandler(this, &options_page_input::actionslistBox_SelectedIndexChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(14, 177);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(102, 13);
			this->label4->TabIndex = 16;
			this->label4->Text = L"Press shortcut keys:";
			// 
			// shortcut_for_selected_label
			// 
			this->shortcut_for_selected_label->AutoSize = true;
			this->shortcut_for_selected_label->Enabled = false;
			this->shortcut_for_selected_label->Location = System::Drawing::Point(14, 133);
			this->shortcut_for_selected_label->Name = L"shortcut_for_selected_label";
			this->shortcut_for_selected_label->Size = System::Drawing::Size(145, 13);
			this->shortcut_for_selected_label->TabIndex = 17;
			this->shortcut_for_selected_label->Text = L"Shortcuts for selected action:";
			// 
			// shortcut_text_box
			// 
			this->shortcut_text_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->shortcut_text_box->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->shortcut_text_box->ImeMode = System::Windows::Forms::ImeMode::Off;
			this->shortcut_text_box->Location = System::Drawing::Point(12, 194);
			this->shortcut_text_box->Name = L"shortcut_text_box";
			this->shortcut_text_box->ReadOnly = true;
			this->shortcut_text_box->ShortcutsEnabled = false;
			this->shortcut_text_box->Size = System::Drawing::Size(268, 20);
			this->shortcut_text_box->TabIndex = 12;
			this->shortcut_text_box->TextChanged += gcnew System::EventHandler(this, &options_page_input::shortcut_text_box_TextChanged);
			this->shortcut_text_box->MouseCaptureChanged += gcnew System::EventHandler(this, &options_page_input::shortcut_text_box_MouseCaptureChanged);
			this->shortcut_text_box->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &options_page_input::shortcut_text_box_PreviewKeyDown);
			this->shortcut_text_box->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &options_page_input::shortcut_text_box_KeyDown);
			this->shortcut_text_box->Leave += gcnew System::EventHandler(this, &options_page_input::shortcut_text_box_Leave);
			this->shortcut_text_box->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &options_page_input::shortcut_text_box_KeyUp);
			this->shortcut_text_box->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &options_page_input::shortcut_text_box_MouseDown);
			this->shortcut_text_box->Enter += gcnew System::EventHandler(this, &options_page_input::shortcut_text_box_Enter);
			this->shortcut_text_box->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &options_page_input::shortcut_text_box_MouseUp);
			// 
			// assign_button
			// 
			this->assign_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->assign_button->Location = System::Drawing::Point(286, 194);
			this->assign_button->Name = L"assign_button";
			this->assign_button->Size = System::Drawing::Size(75, 23);
			this->assign_button->TabIndex = 8;
			this->assign_button->Text = L"A&ssign";
			this->assign_button->UseVisualStyleBackColor = true;
			this->assign_button->Click += gcnew System::EventHandler(this, &options_page_input::assign_button_Click);
			// 
			// remove_button
			// 
			this->remove_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->remove_button->Location = System::Drawing::Point(285, 149);
			this->remove_button->Name = L"remove_button";
			this->remove_button->Size = System::Drawing::Size(75, 23);
			this->remove_button->TabIndex = 9;
			this->remove_button->Text = L"Re&move";
			this->remove_button->UseVisualStyleBackColor = true;
			this->remove_button->Click += gcnew System::EventHandler(this, &options_page_input::remove_button_Click);
			// 
			// assigned_to_comboBox
			// 
			this->assigned_to_comboBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->assigned_to_comboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->assigned_to_comboBox->Enabled = false;
			this->assigned_to_comboBox->FormattingEnabled = true;
			this->assigned_to_comboBox->Location = System::Drawing::Point(12, 239);
			this->assigned_to_comboBox->Name = L"assigned_to_comboBox";
			this->assigned_to_comboBox->Size = System::Drawing::Size(349, 21);
			this->assigned_to_comboBox->TabIndex = 21;
			// 
			// options_page_input
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->assigned_to_comboBox);
			this->Controls->Add(this->asigned_to_label);
			this->Controls->Add(this->shortcuts_combo_box);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->filter_text_box);
			this->Controls->Add(this->actions_list_box);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->shortcut_for_selected_label);
			this->Controls->Add(this->shortcut_text_box);
			this->Controls->Add(this->assign_button);
			this->Controls->Add(this->remove_button);
			this->Name = L"options_page_input";
			this->Size = System::Drawing::Size(372, 395);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:
		editor_world&		m_world;
		System::String^		m_active_action;
		System::String^		m_curr_shortcut_str;
		int					m_keys_count;
		int					m_pass;
		bool				m_shortcut_bar_first_select;
		bool				m_initialized;
		input_keys_holder	m_input_keys_holder;

		typedef System::Collections::Generic::List < action_item^ > action_item_list;
		typedef System::Collections::Generic::SortedDictionary<String^, action_item_list^ > name_to_action_list;
		typedef System::Collections::Generic::SortedDictionary<String^, action_item_list^ >	key_to_action_list;

		System::Collections::Generic::SortedDictionary<String^, action_item_list^ > m_name_to_action;
		System::Collections::Generic::SortedDictionary<String^, action_item_list^ > m_key_to_action;

		typedef System::Collections::Generic::List < action_item_change^ > changes_list;
	
		changes_list m_changes_list;

	private: 
		System::Void actionslistBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void remove_button_Click				(System::Object^  sender, System::EventArgs^  e);
		System::Void shortcut_text_box_TextChanged		(System::Object^  sender, System::EventArgs^  e);
		System::Void shortcut_text_box_KeyDown			(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
		System::Void shortcut_text_box_KeyUp			(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
		System::Void execute_key_down					();
		System::Void execute_key_up						();

		System::Void assign_button_Click				(System::Object^  sender, System::EventArgs^  e);
		System::Void filter_text_box_TextChanged		(System::Object^  sender, System::EventArgs^  e);

		System::Void update_assegned_to_combo			();
		System::Void update_actions_list				();
		System::Void remove_action_shortcut				( System::String^ name, System::String^  keys );
				bool add_action_shortcut				( System::String^ name, System::String^  keys );
				void clear_shortcut_text_box			( );
				void initialize							( );

inline System::String^	exctract_first_keys				(System::String^ keys);

		System::Void shortcut_text_box_MouseDown		(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		System::Void shortcut_text_box_MouseUp			(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
		System::Void shortcut_text_box_Leave(System::Object^  sender, System::EventArgs^  e) 
		{
			XRAY_UNREFERENCED_PARAMETERS	( sender, e );
			m_shortcut_bar_first_select = true;
		}
		System::Void shortcut_text_box_Enter(System::Object^  sender, System::EventArgs^  e);
private: System::Void shortcut_text_box_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) 
		 {
			 XRAY_UNREFERENCED_PARAMETER	( sender );
			 e->IsInputKey = true;
		 }
private: System::Void shortcut_text_box_MouseCaptureChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 XRAY_UNREFERENCED_PARAMETERS	( sender, e );
			 shortcut_text_box->Capture = false;
		 }
};


} // namespace xray
} // namespace editor
