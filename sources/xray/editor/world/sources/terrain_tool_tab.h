////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_TOOL_TAB_H_INCLUDED
#define TERRAIN_TOOL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "tool_tab.h"
#include <xray/sound/sound.h>
using namespace xray::editor::wpf_controls;

namespace xray {
namespace editor {
ref class tool_terrain;
ref class terrain_heightmap_control_tab;
ref class terrain_core_form;
	/// <summary>
	/// Summary for light_tool_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class terrain_tool_tab :	public System::Windows::Forms::UserControl, 
									public tool_tab
	{
	public:
		terrain_tool_tab(tool_terrain^ t):m_tool(t)
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
		~terrain_tool_tab()
		{
			in_destructor();
			if (components)
			{
				delete components;
			}
		}
	void		in_constructor();
	void		in_destructor();
	private: tool_terrain^	m_tool;
			 terrain_core_form^			m_terrain_core_form;
			terrain_heightmap_control_tab^	m_terrain_heightmap_control_tab;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	protected: 

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ToolStrip^  top_toolStrip;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Panel^  active_control_property_grid;
	private: System::Windows::Forms::Splitter^  splitter1;
	private: System::Windows::Forms::Panel^  control_parameters_panel;
			property_grid::host^		m_property_grid_host;



	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;
	private: 
	private: System::ComponentModel::IContainer^  components;
	public: 
	private: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->splitter1 = (gcnew System::Windows::Forms::Splitter());
			this->control_parameters_panel = (gcnew System::Windows::Forms::Panel());
			this->active_control_property_grid = (gcnew System::Windows::Forms::Panel());
			this->top_toolStrip = (gcnew System::Windows::Forms::ToolStrip());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// statusStrip1
			// 
			this->statusStrip1->Location = System::Drawing::Point(0, 548);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(239, 22);
			this->statusStrip1->TabIndex = 2;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->splitter1);
			this->groupBox1->Controls->Add(this->control_parameters_panel);
			this->groupBox1->Controls->Add(this->active_control_property_grid);
			this->groupBox1->Controls->Add(this->top_toolStrip);
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(239, 490);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"tools";
			// 
			// splitter1
			// 
			this->splitter1->Dock = System::Windows::Forms::DockStyle::Top;
			this->splitter1->Location = System::Drawing::Point(3, 254);
			this->splitter1->Name = L"splitter1";
			this->splitter1->Size = System::Drawing::Size(233, 3);
			this->splitter1->TabIndex = 12;
			this->splitter1->TabStop = false;
			// 
			// control_parameters_panel
			// 
			this->control_parameters_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->control_parameters_panel->Location = System::Drawing::Point(3, 254);
			this->control_parameters_panel->Name = L"control_parameters_panel";
			this->control_parameters_panel->Size = System::Drawing::Size(233, 233);
			this->control_parameters_panel->TabIndex = 11;
			// 
			// active_control_property_grid
			// 
			this->active_control_property_grid->Dock = System::Windows::Forms::DockStyle::Top;
			this->active_control_property_grid->Location = System::Drawing::Point(3, 41);
			this->active_control_property_grid->Name = L"active_control_property_grid";
			this->active_control_property_grid->Size = System::Drawing::Size(233, 213);
			this->active_control_property_grid->TabIndex = 8;
			// 
			// top_toolStrip
			// 
			this->top_toolStrip->Location = System::Drawing::Point(3, 16);
			this->top_toolStrip->Name = L"top_toolStrip";
			this->top_toolStrip->Size = System::Drawing::Size(233, 25);
			this->top_toolStrip->TabIndex = 6;
			this->top_toolStrip->Text = L"top_toolStrip";
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button1->Location = System::Drawing::Point(3, 522);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(65, 23);
			this->button1->TabIndex = 14;
			this->button1->Text = L"CORE";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &terrain_tool_tab::button1_Click_1);
			// 
			// button2
			// 
			this->button2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button2->Location = System::Drawing::Point(3, 493);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(65, 23);
			this->button2->TabIndex = 18;
			this->button2->Text = L"Export";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &terrain_tool_tab::button2_Click);
			// 
			// button3
			// 
			this->button3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button3->Location = System::Drawing::Point(83, 493);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(115, 23);
			this->button3->TabIndex = 19;
			this->button3->Text = L"Activate collision";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &terrain_tool_tab::button3_Click);
			// 
			// button4
			// 
			this->button4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button4->Location = System::Drawing::Point(83, 522);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(115, 23);
			this->button4->TabIndex = 20;
			this->button4->Text = L"Deactivate collision";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &terrain_tool_tab::button4_Click);

			this->button5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button5->Location = System::Drawing::Point(83, 545);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(115, 23);
			this->button5->TabIndex = 20;
			this->button5->Text = L"Generate Navmesh";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &terrain_tool_tab::button5_Click);
			// 
			// terrain_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->statusStrip1);
			this->Name = L"terrain_tool_tab";
			this->Size = System::Drawing::Size(239, 570);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	public:
	virtual System::Windows::Forms::UserControl^	frame () {return this;}
	virtual void		on_activate						( );
	virtual void		on_deactivate					( );

	private: 
	void 			button1_Click_1				( System::Object^  sender, System::EventArgs^  e );
	void 			on_control_activated		( editor_base::editor_control_base^ );
	void 			on_control_deactivated		( editor_base::editor_control_base^ );
	void 			on_control_property_changed	( editor_base::editor_control_base^ );
	void 			core_form_closing			( System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
	void 			button2_Click				( System::Object^  sender, System::EventArgs^  e);
	void			button3_Click				(System::Object^  sender, System::EventArgs^  e);
	void			button4_Click				(System::Object^  sender, System::EventArgs^  e);
	void			button5_Click				(System::Object^  sender, System::EventArgs^  e);
};
}
}

#endif // #ifndef TERRAIN_TOOL_TAB_H_INCLUDED
