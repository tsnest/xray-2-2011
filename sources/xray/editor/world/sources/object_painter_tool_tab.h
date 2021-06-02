////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PAINTER_TOOL_TAB_H_INCLUDED
#define OBJECT_PAINTER_TOOL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "tool_tab.h"

namespace xray{
namespace editor {
ref class level_editor;
ref class object_painter_control;
ref class painted_object;

	/// <summary>
	/// Summary for object_painter_tool_tab
	/// </summary>
	public ref class object_painter_tool_tab : public System::Windows::Forms::UserControl,
												public tool_tab
	{
	public:
		object_painter_tool_tab(level_editor^ le):m_level_editor( le )
		{
			InitializeComponent();
			post_constructor	();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~object_painter_tool_tab();

	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ToolStrip^  top_tool_strip;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Button^  add_from_selection_button;
	private: System::Windows::Forms::ListView^  m_snap_list_view;


	private: System::Windows::Forms::Button^  clear_button;
	private: System::Windows::Forms::Button^  focus_button;

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
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->top_tool_strip = (gcnew System::Windows::Forms::ToolStrip());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->focus_button = (gcnew System::Windows::Forms::Button());
			this->clear_button = (gcnew System::Windows::Forms::Button());
			this->add_from_selection_button = (gcnew System::Windows::Forms::Button());
			this->m_snap_list_view = (gcnew System::Windows::Forms::ListView());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// statusStrip1
			// 
			this->statusStrip1->Location = System::Drawing::Point(0, 349);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(203, 22);
			this->statusStrip1->TabIndex = 1;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->top_tool_strip);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(203, 41);
			this->groupBox1->TabIndex = 2;
			this->groupBox1->TabStop = false;
			// 
			// top_tool_strip
			// 
			this->top_tool_strip->Location = System::Drawing::Point(3, 16);
			this->top_tool_strip->Name = L"top_tool_strip";
			this->top_tool_strip->Size = System::Drawing::Size(197, 25);
			this->top_tool_strip->TabIndex = 0;
			this->top_tool_strip->Text = L"toolStrip1";
			// 
			// groupBox3
			// 
			this->groupBox3->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox3->Location = System::Drawing::Point(0, 41);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(203, 94);
			this->groupBox3->TabIndex = 4;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"properties";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->focus_button);
			this->groupBox2->Controls->Add(this->clear_button);
			this->groupBox2->Controls->Add(this->add_from_selection_button);
			this->groupBox2->Controls->Add(this->m_snap_list_view);
			this->groupBox2->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox2->Location = System::Drawing::Point(0, 135);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(203, 150);
			this->groupBox2->TabIndex = 5;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Snap list";
			// 
			// focus_button
			// 
			this->focus_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->focus_button->Location = System::Drawing::Point(6, 121);
			this->focus_button->Name = L"focus_button";
			this->focus_button->Size = System::Drawing::Size(56, 23);
			this->focus_button->TabIndex = 4;
			this->focus_button->Text = L"Focus";
			this->focus_button->UseVisualStyleBackColor = true;
			this->focus_button->Click += gcnew System::EventHandler(this, &object_painter_tool_tab::focus_button_Click);
			// 
			// clear_button
			// 
			this->clear_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->clear_button->Location = System::Drawing::Point(68, 121);
			this->clear_button->Name = L"clear_button";
			this->clear_button->Size = System::Drawing::Size(53, 23);
			this->clear_button->TabIndex = 3;
			this->clear_button->Text = L"Reset";
			this->clear_button->UseVisualStyleBackColor = true;
			this->clear_button->Click += gcnew System::EventHandler(this, &object_painter_tool_tab::clear_button_Click);
			// 
			// add_from_selection_button
			// 
			this->add_from_selection_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->add_from_selection_button->Location = System::Drawing::Point(6, 97);
			this->add_from_selection_button->Name = L"add_from_selection_button";
			this->add_from_selection_button->Size = System::Drawing::Size(56, 23);
			this->add_from_selection_button->TabIndex = 1;
			this->add_from_selection_button->Text = L"Add sel";
			this->add_from_selection_button->UseVisualStyleBackColor = true;
			this->add_from_selection_button->Click += gcnew System::EventHandler(this, &object_painter_tool_tab::add_from_selection_button_Click);
			// 
			// m_snap_list_view
			// 
			this->m_snap_list_view->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_snap_list_view->Enabled = false;
			this->m_snap_list_view->Location = System::Drawing::Point(6, 19);
			this->m_snap_list_view->MultiSelect = false;
			this->m_snap_list_view->Name = L"m_snap_list_view";
			this->m_snap_list_view->ShowGroups = false;
			this->m_snap_list_view->Size = System::Drawing::Size(191, 72);
			this->m_snap_list_view->TabIndex = 0;
			this->m_snap_list_view->UseCompatibleStateImageBehavior = false;
			this->m_snap_list_view->View = System::Windows::Forms::View::List;
			// 
			// object_painter_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->statusStrip1);
			this->Name = L"object_painter_tool_tab";
			this->Size = System::Drawing::Size(203, 371);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
public:
	virtual System::Windows::Forms::UserControl^	frame () {return this;}
	virtual void		on_activate						( );
	virtual void		on_deactivate					( );
	void				load_settings					( RegistryKey^ key );
	void				save_settings					( RegistryKey^ key );

private:
	level_editor^				m_level_editor;
	object_painter_control^		m_object_painter_control;
	painted_object^				m_dest_object;

	void				post_constructor				( );
	void				add_from_selection_button_Click	( System::Object^ sender, System::EventArgs^ e );
	void				remove_button_Click				( System::Object^ sender, System::EventArgs^ e );
	void				clear_button_Click				( System::Object^  sender, System::EventArgs^  e );
	void				focus_button_Click				( System::Object^ , System::EventArgs^ );
};

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PAINTER_TOOL_TAB_H_INCLUDED