////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WPF_CONTROLS_PAGE_H_INCLUDED
#define WPF_CONTROLS_PAGE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;

namespace xray 
{
	namespace editor_base 
	{
		/// <summary>
		/// Summary for wpf_controls_page
		/// </summary>
		public ref class wpf_controls_page : public UserControl, options_page
		{
		public:
			wpf_controls_page(void)
			{
				InitializeComponent	( );

				in_constructor		( );
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~wpf_controls_page()
			{
				if (components)
				{
					delete components;
				}
			}
		
		private:
			Boolean		m_changes_made;
			Boolean		m_initialized;

			Boolean		m_curve_editor_lock_zoom;
			Double		m_hierarchical_item_indent;

		private: System::Windows::Forms::GroupBox^  groupBox1;
		private: System::Windows::Forms::RadioButton^  unlock_radio_button;
		private: System::Windows::Forms::RadioButton^  lock_radio_button;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::TextBox^  m_hierarchical_item_indent_text_box;



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
				this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->unlock_radio_button = (gcnew System::Windows::Forms::RadioButton());
				this->lock_radio_button = (gcnew System::Windows::Forms::RadioButton());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->m_hierarchical_item_indent_text_box = (gcnew System::Windows::Forms::TextBox());
				this->groupBox1->SuspendLayout();
				this->SuspendLayout();
				// 
				// groupBox1
				// 
				this->groupBox1->Controls->Add(this->unlock_radio_button);
				this->groupBox1->Controls->Add(this->lock_radio_button);
				this->groupBox1->Location = System::Drawing::Point(3, 3);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Size = System::Drawing::Size(366, 64);
				this->groupBox1->TabIndex = 0;
				this->groupBox1->TabStop = false;
				this->groupBox1->Text = L"Curve Editor";
				// 
				// unlock_radio_button
				// 
				this->unlock_radio_button->AutoSize = true;
				this->unlock_radio_button->Location = System::Drawing::Point(12, 41);
				this->unlock_radio_button->Name = L"unlock_radio_button";
				this->unlock_radio_button->Size = System::Drawing::Size(114, 17);
				this->unlock_radio_button->TabIndex = 0;
				this->unlock_radio_button->TabStop = true;
				this->unlock_radio_button->Text = L"Unlock zoom axies";
				this->unlock_radio_button->UseVisualStyleBackColor = true;
				this->unlock_radio_button->CheckedChanged += gcnew System::EventHandler(this, &wpf_controls_page::unlock_radio_button_checked_changed);
				// 
				// lock_radio_button
				// 
				this->lock_radio_button->AutoSize = true;
				this->lock_radio_button->Location = System::Drawing::Point(12, 19);
				this->lock_radio_button->Name = L"lock_radio_button";
				this->lock_radio_button->Size = System::Drawing::Size(104, 17);
				this->lock_radio_button->TabIndex = 0;
				this->lock_radio_button->TabStop = true;
				this->lock_radio_button->Text = L"Lock zoom axies";
				this->lock_radio_button->UseVisualStyleBackColor = true;
				this->lock_radio_button->CheckedChanged += gcnew System::EventHandler(this, &wpf_controls_page::unlock_radio_button_checked_changed);
				// 
				// label1
				// 
				this->label1->AutoSize = true;
				this->label1->Location = System::Drawing::Point(12, 76);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(122, 13);
				this->label1->TabIndex = 1;
				this->label1->Text = L"Hierarchical Item Indent:";
				// 
				// m_hierarchical_item_indent_text_box
				// 
				this->m_hierarchical_item_indent_text_box->Location = System::Drawing::Point(269, 73);
				this->m_hierarchical_item_indent_text_box->Name = L"m_hierarchical_item_indent_text_box";
				this->m_hierarchical_item_indent_text_box->Size = System::Drawing::Size(100, 20);
				this->m_hierarchical_item_indent_text_box->TabIndex = 2;
				this->m_hierarchical_item_indent_text_box->TextChanged += gcnew System::EventHandler(this, &wpf_controls_page::hierarchical_item_indent_changed);
				// 
				// wpf_controls_page
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->Controls->Add(this->m_hierarchical_item_indent_text_box);
				this->Controls->Add(this->label1);
				this->Controls->Add(this->groupBox1);
				this->Name = L"wpf_controls_page";
				this->Size = System::Drawing::Size(372, 395);
				this->groupBox1->ResumeLayout(false);
				this->groupBox1->PerformLayout();
				this->ResumeLayout(false);
				this->PerformLayout();

			}
		#pragma endregion

		private: 
			void				in_constructor						( );
			void				unlock_radio_button_checked_changed	( Object^ sender, EventArgs^ e );
			void				hierarchical_item_indent_changed	( Object^ sender, EventArgs^ e );

		public:
			virtual void		activate_page		( );
			virtual void		deactivate_page		( );
			virtual bool		accept_changes		( );
			virtual void		cancel_changes		( );
			virtual bool		changed				( );

			virtual Control^	get_control			( );


};

	} // namespace editor_base
} // namespace xray

#endif // #ifndef WPF_CONTROLS_PAGE_H_INCLUDED