////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COMPOUND_TOOL_TAB_H_INCLUDED
#define COMPOUND_TOOL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "tool_tab.h"
#include "project_defines.h"

namespace xray {
namespace editor {
	ref class tool_composite;
	ref class object_composite;
	ref class library_objects_tree;
	/// <summary>
	/// Summary for compound_tool_tab
	/// </summary>
	public ref class compound_tool_tab :	public System::Windows::Forms::UserControl,
											public tool_tab
	{
	public:
		compound_tool_tab(tool_composite^ t):m_tool(t)
		{
			InitializeComponent	( );
			post_constructor	( );
			switch_edit_mode	( false );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~compound_tool_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	private: tool_composite^	m_tool;

	protected: 

	private: 

	public: 






	private: 








	public: 






	private: System::Windows::Forms::ListView^  object_details_list;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::ColumnHeader^  columnHeader2;
	private: System::Windows::Forms::Splitter^  splitter1;
	private: System::Windows::Forms::Panel^  objects_tree_place;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Panel^  panel1;

	private: 



	public: 

	private: 
	private: System::ComponentModel::IContainer^  components;
	public: 

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
			this->object_details_list = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->splitter1 = (gcnew System::Windows::Forms::Splitter());
			this->objects_tree_place = (gcnew System::Windows::Forms::Panel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// object_details_list
			// 
			this->object_details_list->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->columnHeader1, 
				this->columnHeader2});
			this->object_details_list->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->object_details_list->FullRowSelect = true;
			this->object_details_list->GridLines = true;
			this->object_details_list->Location = System::Drawing::Point(0, 332);
			this->object_details_list->Name = L"object_details_list";
			this->object_details_list->Size = System::Drawing::Size(250, 192);
			this->object_details_list->TabIndex = 23;
			this->object_details_list->UseCompatibleStateImageBehavior = false;
			this->object_details_list->View = System::Windows::Forms::View::Details;
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"Name";
			this->columnHeader1->Width = 120;
			// 
			// columnHeader2
			// 
			this->columnHeader2->Text = L"Type";
			this->columnHeader2->Width = 105;
			// 
			// splitter1
			// 
			this->splitter1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->splitter1->Location = System::Drawing::Point(0, 540);
			this->splitter1->Name = L"splitter1";
			this->splitter1->Size = System::Drawing::Size(250, 3);
			this->splitter1->TabIndex = 24;
			this->splitter1->TabStop = false;
			// 
			// objects_tree_place
			// 
			this->objects_tree_place->Dock = System::Windows::Forms::DockStyle::Fill;
			this->objects_tree_place->Location = System::Drawing::Point(0, 0);
			this->objects_tree_place->Name = L"objects_tree_place";
			this->objects_tree_place->Size = System::Drawing::Size(250, 540);
			this->objects_tree_place->TabIndex = 25;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(61, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(54, 13);
			this->label1->TabIndex = 20;
			this->label1->Text = L"<nothing>";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(52, 13);
			this->label2->TabIndex = 24;
			this->label2->Text = L"Selected:";
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->panel1->Location = System::Drawing::Point(0, 524);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(250, 16);
			this->panel1->TabIndex = 26;
			// 
			// compound_tool_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->object_details_list);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->objects_tree_place);
			this->Controls->Add(this->splitter1);
			this->Name = L"compound_tool_tab";
			this->Size = System::Drawing::Size(250, 543);
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
	library_objects_tree^	m_library_objects_tree;
	void					library_current_path_changed( System::String^ library_object_path, System::String^ prev_path );
	public:
	virtual System::Windows::Forms::UserControl^	frame () {return this;}

	virtual void		on_activate						( );
	virtual void		on_deactivate					( );

	void				post_constructor				( );
	void				switch_edit_mode				( bool b );
	void				add_to_current_compound			( object_base_list^ list );
	void				on_project_clear				( );
	void				fill_objects_list				( );
	void 				edit_selected_compound_button_Click		();
	void 				add_selected_to_compound_button_Click	();
	void 				cancel_selected_compound_button_Click	();
	void 				new_from_selection_button_Click			();
	void 				ungroup_button_Click					();
	void				on_add_logic_button_clicked				();

	bool				is_in_edit_mode							();
	bool				is_not_in_edit_mode						();

};

} // namespace editor
} // namespace xray

#endif // #ifndef COMPOUND_TOOL_TAB_H_INCLUDED