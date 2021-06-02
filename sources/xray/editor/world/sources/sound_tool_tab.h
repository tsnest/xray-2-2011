#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "tool_tab.h"

namespace xray{
namespace editor {
ref class tool_base;
ref class library_objects_tree;
ref struct lib_item_base;
class editor_world;

	/// <summary>
	/// Summary for sound_tool_tab
	/// </summary>
	public ref class sound_tool_tab : public System::Windows::Forms::UserControl,
										public tool_tab
	{
	public:
		sound_tool_tab(tool_base^ tool, editor_world& world):
		  m_tool		( tool ),
		  m_editor_world( world )
		{
			InitializeComponent();
			post_constructor	( );
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~sound_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}

		tool_base^		m_tool;
	private: System::Windows::Forms::ToolStrip^  m_tool_strip;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  lbl_time_factor;
	private: System::Windows::Forms::NumericUpDown^  time_factor;
	private: System::Windows::Forms::Panel^  objects_tree_place;

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
			this->m_tool_strip = (gcnew System::Windows::Forms::ToolStrip());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->lbl_time_factor = (gcnew System::Windows::Forms::Label());
			this->time_factor = (gcnew System::Windows::Forms::NumericUpDown());
			this->objects_tree_place = (gcnew System::Windows::Forms::Panel());
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->time_factor))->BeginInit();
			this->SuspendLayout();
			// 
			// m_tool_strip
			// 
			this->m_tool_strip->Location = System::Drawing::Point(0, 0);
			this->m_tool_strip->Name = L"m_tool_strip";
			this->m_tool_strip->Size = System::Drawing::Size(236, 25);
			this->m_tool_strip->TabIndex = 1;
			this->m_tool_strip->Text = L"toolStrip1";
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->lbl_time_factor);
			this->panel1->Controls->Add(this->time_factor);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel1->Location = System::Drawing::Point(0, 25);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(236, 21);
			this->panel1->TabIndex = 6;
			// 
			// lbl_time_factor
			// 
			this->lbl_time_factor->AutoSize = true;
			this->lbl_time_factor->Location = System::Drawing::Point(3, 0);
			this->lbl_time_factor->Name = L"lbl_time_factor";
			this->lbl_time_factor->Size = System::Drawing::Size(59, 13);
			this->lbl_time_factor->TabIndex = 6;
			this->lbl_time_factor->Text = L"time factor:";
			// 
			// time_factor
			// 
			this->time_factor->DecimalPlaces = 2;
			this->time_factor->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->time_factor->Location = System::Drawing::Point(67, -2);
			this->time_factor->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, 65536});
			this->time_factor->Name = L"time_factor";
			this->time_factor->Size = System::Drawing::Size(66, 20);
			this->time_factor->TabIndex = 5;
			this->time_factor->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 65536});
			this->time_factor->ValueChanged += gcnew System::EventHandler(this, &sound_tool_tab::time_factor_ValueChanged);
			// 
			// objects_tree_place
			// 
			this->objects_tree_place->Dock = System::Windows::Forms::DockStyle::Fill;
			this->objects_tree_place->Location = System::Drawing::Point(0, 46);
			this->objects_tree_place->Name = L"objects_tree_place";
			this->objects_tree_place->Size = System::Drawing::Size(236, 212);
			this->objects_tree_place->TabIndex = 7;
			// 
			// sound_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->objects_tree_place);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->m_tool_strip);
			this->Name = L"sound_tool_tab";
			this->Size = System::Drawing::Size(236, 258);
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->time_factor))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	protected:
	void	post_constructor							( );
	library_objects_tree^	m_library_objects_tree;
	public:
	void	fill_objects_list							( System::String^ initial );
	void	item_double_clicked							( lib_item_base^ item );
	virtual System::Windows::Forms::UserControl^	frame ( ) {return this;}
	virtual void		on_activate						( );
	virtual void		on_deactivate					( );
	private:
		controls::mode_strip_button^					m_debug_options_button;
		editor_world&									m_editor_world;
	private: 
			System::Void	time_factor_ValueChanged	(System::Object^  sender, System::EventArgs^  e);
};

	
}
}
