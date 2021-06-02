#pragma once

#pragma managed(push,off)
#include <xray/render/world.h>
#pragma managed(pop)

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

ref class property_restrictor;

namespace xray {
namespace editor_base { class property_holder; }
namespace editor {
	ref class tool_base;
	ref struct lib_item;
	ref class object_base;
	ref class level_editor;
	/// <summary>
	/// Summary for library_object_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	enum enum_current_mode{mode_new, mode_edit, mode_none};
	public ref class library_object_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		library_object_tab(level_editor^ le):m_level_editor(le),
			m_tool(nullptr), 
			m_library_item(nullptr),
			m_current_object(nullptr)
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
		~library_object_tab()
		{
			in_destructor();
			if (components)
			{
				delete components;
			}
		}

	protected: 

	private: System::Windows::Forms::MenuStrip^  menuStrip1;


	private: System::Windows::Forms::ToolStripMenuItem^  menuToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::Panel^  bottom_panel;
	private: System::Windows::Forms::Panel^  top_panel;
	private: System::Windows::Forms::ComboBox^  raw_objects_selector_cb;
	private: System::Windows::Forms::TabPage^  tabPage2;

	private: System::Windows::Forms::ToolStripMenuItem^  newTabToolStripMenuItem;



	private: System::Windows::Forms::Label^  label2;






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
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->menuToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newTabToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->bottom_panel = (gcnew System::Windows::Forms::Panel());
			this->top_panel = (gcnew System::Windows::Forms::Panel());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->raw_objects_selector_cb = (gcnew System::Windows::Forms::ComboBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->menuStrip1->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->top_panel->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->menuToolStripMenuItem, 
				this->newTabToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(331, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// menuToolStripMenuItem
			// 
			this->menuToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->saveToolStripMenuItem, 
				this->exitToolStripMenuItem});
			this->menuToolStripMenuItem->Name = L"menuToolStripMenuItem";
			this->menuToolStripMenuItem->Size = System::Drawing::Size(54, 20);
			this->menuToolStripMenuItem->Text = L"Object";
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(165, 22);
			this->saveToolStripMenuItem->Text = L"Commit changes";
			this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &library_object_tab::on_commit);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(165, 22);
			this->exitToolStripMenuItem->Text = L"Discard changes";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &library_object_tab::on_discard);
			// 
			// newTabToolStripMenuItem
			// 
			this->newTabToolStripMenuItem->Name = L"newTabToolStripMenuItem";
			this->newTabToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->newTabToolStripMenuItem->Text = L"new tab";
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 24);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(331, 483);
			this->tabControl1->TabIndex = 5;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->bottom_panel);
			this->tabPage1->Controls->Add(this->top_panel);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(323, 457);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Main";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// bottom_panel
			// 
			this->bottom_panel->BackColor = System::Drawing::SystemColors::Control;
			this->bottom_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->bottom_panel->Location = System::Drawing::Point(3, 31);
			this->bottom_panel->Name = L"bottom_panel";
			this->bottom_panel->Size = System::Drawing::Size(317, 423);
			this->bottom_panel->TabIndex = 5;
			// 
			// top_panel
			// 
			this->top_panel->BackColor = System::Drawing::SystemColors::Control;
			this->top_panel->Controls->Add(this->label2);
			this->top_panel->Controls->Add(this->raw_objects_selector_cb);
			this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->top_panel->Location = System::Drawing::Point(3, 3);
			this->top_panel->Name = L"top_panel";
			this->top_panel->Size = System::Drawing::Size(317, 28);
			this->top_panel->TabIndex = 3;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Dock = System::Windows::Forms::DockStyle::Left;
			this->label2->Location = System::Drawing::Point(0, 0);
			this->label2->Name = L"label2";
			this->label2->Padding = System::Windows::Forms::Padding(5);
			this->label2->Size = System::Drawing::Size(68, 23);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Base class";
			// 
			// raw_objects_selector_cb
			// 
			this->raw_objects_selector_cb->BackColor = System::Drawing::SystemColors::Window;
			this->raw_objects_selector_cb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->raw_objects_selector_cb->FormattingEnabled = true;
			this->raw_objects_selector_cb->Location = System::Drawing::Point(69, 3);
			this->raw_objects_selector_cb->Name = L"raw_objects_selector_cb";
			this->raw_objects_selector_cb->Size = System::Drawing::Size(243, 21);
			this->raw_objects_selector_cb->TabIndex = 0;
			this->raw_objects_selector_cb->SelectedIndexChanged += gcnew System::EventHandler(this, &library_object_tab::raw_objects_selector_cb_TextChanged);
			// 
			// tabPage2
			// 
			this->tabPage2->BackColor = System::Drawing::Color::Transparent;
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(323, 457);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Restrictions";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// library_object_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(331, 507);
			this->CloseButton = false;
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->menuStrip1);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"library_object_tab";
			this->TabText = L"library object";
			this->Text = L"library object";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->top_panel->ResumeLayout(false);
			this->top_panel->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
			protected:
					xray::editor::controls::property_grid^	m_property_grid;
					xray::editor::controls::property_grid^	m_property_grid_restrictions;
//					xray::editor_base::property_holder*	m_library_object_ph;
					xray::editor_base::property_holder_wrapper^	m_library_object_ph;
					level_editor^							m_level_editor;
					tool_base^								m_tool;
					lib_item^								m_library_item;
					object_base^							m_current_object;
					enum_current_mode						m_current_mode;

					void			create_current_object	();
					void			destroy_current_object	();

			private: void 			in_destructor			();
			private: void 			in_constructor			();
			private: void 			clear					();
			public:	void			create_new				(tool_base^ tool);
			public:	void			edit_existing			(tool_base^ tool, System::String^ name);
			public:	void			on_render				();
			private: System::Void raw_objects_selector_cb_TextChanged(System::Object^  sender, System::EventArgs^  e);
			private: System::Void on_commit(System::Object^  sender, System::EventArgs^  e);
			private: System::Void on_discard(System::Object^  sender, System::EventArgs^  e);
};
}
}
