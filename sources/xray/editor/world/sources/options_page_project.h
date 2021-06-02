////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OPTIONS_PAGE_PROJECT_H_INCLUDED
#define OPTIONS_PAGE_PROJECT_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	class editor_world;
	/// <summary>
	/// Summary for options_page_project
	/// </summary>
	public ref class options_page_project : public System::Windows::Forms::UserControl, public editor_base::options_page
	{
	public:
		options_page_project( editor_world& world ):m_editor_world( world )
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
		~options_page_project()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  track_active_item_check;
	protected: 
	private: System::Windows::Forms::Label^  label1;

	protected: 

	protected: 

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
			this->track_active_item_check = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// track_active_item_check
			// 
			this->track_active_item_check->AutoSize = true;
			this->track_active_item_check->Location = System::Drawing::Point(190, 13);
			this->track_active_item_check->Name = L"track_active_item_check";
			this->track_active_item_check->Size = System::Drawing::Size(15, 14);
			this->track_active_item_check->TabIndex = 0;
			this->track_active_item_check->UseVisualStyleBackColor = true;
			this->track_active_item_check->CheckedChanged += gcnew System::EventHandler(this, &options_page_project::track_active_item_check_CheckedChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(4, 14);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(156, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Track active item in project tree";
			// 
			// options_page_project
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->label1);
			this->Controls->Add(this->track_active_item_check);
			this->Name = L"options_page_project";
			this->Size = System::Drawing::Size(236, 283);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		System::Collections::Hashtable		m_backup_values;
		editor_world&						m_editor_world;

	public:
		virtual bool	accept_changes		( );
		virtual void	cancel_changes		( );
		virtual void	activate_page		( );
		virtual void	deactivate_page		( );
		virtual bool	changed				( );
		virtual Control^ get_control		( ) { return this; }

	private: System::Void track_active_item_check_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	};
}
}

#endif // #ifndef OPTIONS_PAGE_PROJECT_H_INCLUDED