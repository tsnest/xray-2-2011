////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EDITOR_PANEL_H_INCLUDED
#define LOGIC_EDITOR_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

typedef xray::editor::wpf_controls::logic_view::logic_view_host wpf_logic_view;
typedef xray::editor::wpf_controls::logic_view::event_control wpf_logic_event;

namespace xray{
namespace editor {

	ref class level_editor;
	ref class object_logic;
	ref class object_scene;
	ref class object_job;

	/// <summary>
	/// Summary for logic_editor_panel
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class logic_editor_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		logic_editor_panel(level_editor^ editor)
			:m_editor(editor)
		{
			InitializeComponent();
			in_constructor();
		
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~logic_editor_panel()
		{
			if (components)
			{
				delete components;
			}
		}

	public:
		property wpf_logic_view^						cached_logic
		{
			wpf_logic_view^								get( ){ return m_cached_logic; }
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>		
		level_editor^									m_editor;
		object_scene^									m_selected_object;		
		wpf_logic_view^									m_cached_logic;

	private:
		System::ComponentModel::Container^				components;
		System::Windows::Forms::ToolStrip^				toolStrip1;
		System::Windows::Forms::ToolStripButton^		add_job_button;
		System::Windows::Forms::ToolStripButton^		add_sub_scene;
		System::Windows::Forms::ToolStripSeparator^		toolStripSeparator3;	
		System::Windows::Forms::ToolStripButton^		add_event_button;
		System::Windows::Forms::ToolStripSeparator^		toolStripSeparator4;
		System::Windows::Forms::ToolStripSeparator^		toolStripSeparator1;
		System::Windows::Forms::ToolStripComboBox^		add_job_type_combobox;
		System::Windows::Forms::ToolStripSeparator^		toolStripSeparator2;



#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(logic_editor_panel::typeid));
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->add_sub_scene = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->add_job_type_combobox = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->add_job_button = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->add_event_button = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// toolStrip1
			// 
			this->toolStrip1->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->add_sub_scene, this->toolStripSeparator3, 
				this->toolStripSeparator1, this->add_job_type_combobox, this->add_job_button, this->toolStripSeparator2, 
				this->toolStripSeparator4, this->add_event_button});
			this->toolStrip1->Location = System::Drawing::Point(0, 0);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->toolStrip1->Size = System::Drawing::Size(701, 25);
			this->toolStrip1->TabIndex = 0;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// add_sub_scene
			// 
			this->add_sub_scene->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->add_sub_scene->Enabled = false;
			this->add_sub_scene->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"add_sub_scene.Image")));
			this->add_sub_scene->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->add_sub_scene->Name = L"add_sub_scene";
			this->add_sub_scene->Size = System::Drawing::Size(62, 22);
			this->add_sub_scene->Text = L"Add Scene";
			this->add_sub_scene->Click += gcnew System::EventHandler(this, &logic_editor_panel::on_add_subscene_button_click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(6, 25);
			// 
			// add_job_type_combobox
			// 
			this->add_job_type_combobox->Name = L"add_job_type_combobox";
			this->add_job_type_combobox->Size = System::Drawing::Size(121, 25);
			//this->add_job_type_combobox->FlatStyle = ;

			// 
			// add_job_button
			// 
			this->add_job_button->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->add_job_button->Enabled = false;
			this->add_job_button->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"add_job_button.Image")));
			this->add_job_button->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->add_job_button->Name = L"add_job_button";
			this->add_job_button->Size = System::Drawing::Size(50, 22);
			this->add_job_button->Text = L"Add Job";
			this->add_job_button->Click += gcnew System::EventHandler(this, &logic_editor_panel::on_add_job_button_click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(6, 25);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(6, 25);
			// 
			// add_event_button
			// 
			this->add_event_button->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;

			this->add_event_button->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"add_event_button.Image")));
			this->add_event_button->Enabled = false;
			this->add_event_button->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->add_event_button->Name = L"add_event_button";
			this->add_event_button->Size = System::Drawing::Size(61, 22);
			this->add_event_button->Text = L"Add Event";
			this->add_event_button->Click += gcnew System::EventHandler(this, &logic_editor_panel::add_event_button_click);
			// 
			// toolStripSeparator4
			// 
			this->toolStripSeparator4->Name = L"toolStripSeparator4";
			this->toolStripSeparator4->Size = System::Drawing::Size(6, 25);
			// 
			// logic_editor_panel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->BackColor = System::Drawing::SystemColors::ControlDark;
			this->ClientSize = System::Drawing::Size(701, 264);
			this->Controls->Add(this->toolStrip1);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"logic_editor_panel";
			this->Text = L"Logic: ";
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


	public:
		void		on_add_job_button_click			( Object^ sender, System::EventArgs^ e );
		void		on_add_subscene_button_click	( Object^ sender, System::EventArgs^ e );
		void		add_event_button_click			( Object^ sender, System::EventArgs^ e );
		void		show_object_logic				( object_logic^ object );
		void		on_scene_child_added			( object_logic^ obj );
		void		remove_logic_view				( object_job^ job );

		System::String^ get_selected_job_type		( ) { return (System::String^)add_job_type_combobox->SelectedItem; };
	
	private:
		void		in_constructor					( );		


};
}//namespace xray
}//namespace editor 

#endif // #ifndef LOGIC_EDITOR_PANEL_H_INCLUDED