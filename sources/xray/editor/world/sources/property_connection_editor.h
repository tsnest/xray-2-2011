////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_CONNECTION_EDITOR_H_INCLUDED
#define PROPERTY_CONNECTION_EDITOR_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
	namespace editor_base { ref class command_engine;}
namespace editor {
	ref class level_editor;
	ref class project;
	ref class link_storage;
	ref class object_base;

//	ref class command_engine;
	/// <summary>
	/// Summary for property_connection_editor
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
public ref class property_connection_editor : public System::Windows::Forms::Form, public controls::hypergraph::connection_editor
	{
	public:
		property_connection_editor(xray::editor::level_editor^ le):m_level_editor(le)
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
		~property_connection_editor()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  top_menu;
	protected: 

	private: System::Windows::Forms::Panel^  top_panel;
	private: System::Windows::Forms::Panel^  bottom_panel;
	protected: 


	private: System::Windows::Forms::Panel^  left_panel;


	private: System::Windows::Forms::Splitter^  splitter1;
	private: System::Windows::Forms::Panel^  right_panel;
	private: System::Windows::Forms::ListView^  left_props_view;






	private: System::Windows::Forms::TableLayoutPanel^  bottom_buttons_layout;
	private: System::Windows::Forms::Button^  close_button;


	private: System::Windows::Forms::Button^  make_connection_button;
	private: System::Windows::Forms::Button^  disconnect_button;


	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ToolStripMenuItem^  enuToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  viewTypeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  listViewMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  detailsViewMenuItem;
	private: System::Windows::Forms::ListView^  right_props_view;
	private: System::Windows::Forms::Label^  right_label;
	private: System::Windows::Forms::Label^  left_label;
	private: System::Windows::Forms::ToolStripMenuItem^  leftToolStripMenuItem;


	private: System::Windows::Forms::ToolStripMenuItem^  rightToolStripMenuItem;

	private: System::Windows::Forms::Button^  btn_swap;
	private: System::Windows::Forms::Button^  btn_grab_right;
	private: System::Windows::Forms::Button^  btn_grab_left;








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
			System::Windows::Forms::ColumnHeader^  columnHeader1;
			System::Windows::Forms::ColumnHeader^  columnHeader2;
			System::Windows::Forms::ColumnHeader^  columnHeader3;
			System::Windows::Forms::ColumnHeader^  columnHeader4;
			this->top_menu = (gcnew System::Windows::Forms::MenuStrip());
			this->enuToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->viewTypeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->detailsViewMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->listViewMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->leftToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->rightToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->top_panel = (gcnew System::Windows::Forms::Panel());
			this->btn_grab_right = (gcnew System::Windows::Forms::Button());
			this->btn_grab_left = (gcnew System::Windows::Forms::Button());
			this->btn_swap = (gcnew System::Windows::Forms::Button());
			this->right_label = (gcnew System::Windows::Forms::Label());
			this->left_label = (gcnew System::Windows::Forms::Label());
			this->bottom_panel = (gcnew System::Windows::Forms::Panel());
			this->bottom_buttons_layout = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->close_button = (gcnew System::Windows::Forms::Button());
			this->make_connection_button = (gcnew System::Windows::Forms::Button());
			this->disconnect_button = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->left_panel = (gcnew System::Windows::Forms::Panel());
			this->left_props_view = (gcnew System::Windows::Forms::ListView());
			this->splitter1 = (gcnew System::Windows::Forms::Splitter());
			this->right_panel = (gcnew System::Windows::Forms::Panel());
			this->right_props_view = (gcnew System::Windows::Forms::ListView());
			columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			columnHeader2 = (gcnew System::Windows::Forms::ColumnHeader());
			columnHeader3 = (gcnew System::Windows::Forms::ColumnHeader());
			columnHeader4 = (gcnew System::Windows::Forms::ColumnHeader());
			this->top_menu->SuspendLayout();
			this->top_panel->SuspendLayout();
			this->bottom_panel->SuspendLayout();
			this->bottom_buttons_layout->SuspendLayout();
			this->left_panel->SuspendLayout();
			this->right_panel->SuspendLayout();
			this->SuspendLayout();
			// 
			// columnHeader1
			// 
			columnHeader1->Text = L"Name";
			columnHeader1->Width = 100;
			// 
			// columnHeader2
			// 
			columnHeader2->Text = L"Data Type";
			columnHeader2->Width = 138;
			// 
			// columnHeader3
			// 
			columnHeader3->Text = L"Name";
			columnHeader3->Width = 100;
			// 
			// columnHeader4
			// 
			columnHeader4->Text = L"Data Type";
			columnHeader4->Width = 138;
			// 
			// top_menu
			// 
			this->top_menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->enuToolStripMenuItem, 
				this->leftToolStripMenuItem, this->rightToolStripMenuItem});
			this->top_menu->Location = System::Drawing::Point(0, 0);
			this->top_menu->Name = L"top_menu";
			this->top_menu->Size = System::Drawing::Size(542, 24);
			this->top_menu->TabIndex = 0;
			this->top_menu->Text = L"menuStrip1";
			// 
			// enuToolStripMenuItem
			// 
			this->enuToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->viewTypeToolStripMenuItem});
			this->enuToolStripMenuItem->Name = L"enuToolStripMenuItem";
			this->enuToolStripMenuItem->Size = System::Drawing::Size(50, 20);
			this->enuToolStripMenuItem->Text = L"Menu";
			// 
			// viewTypeToolStripMenuItem
			// 
			this->viewTypeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->detailsViewMenuItem, 
				this->listViewMenuItem});
			this->viewTypeToolStripMenuItem->Name = L"viewTypeToolStripMenuItem";
			this->viewTypeToolStripMenuItem->Size = System::Drawing::Size(133, 22);
			this->viewTypeToolStripMenuItem->Text = L"View Mode";
			// 
			// detailsViewMenuItem
			// 
			this->detailsViewMenuItem->Checked = true;
			this->detailsViewMenuItem->CheckOnClick = true;
			this->detailsViewMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
			this->detailsViewMenuItem->Name = L"detailsViewMenuItem";
			this->detailsViewMenuItem->Size = System::Drawing::Size(109, 22);
			this->detailsViewMenuItem->Text = L"Details";
			this->detailsViewMenuItem->Click += gcnew System::EventHandler(this, &property_connection_editor::detailsViewMenuItem_Click);
			// 
			// listViewMenuItem
			// 
			this->listViewMenuItem->CheckOnClick = true;
			this->listViewMenuItem->Name = L"listViewMenuItem";
			this->listViewMenuItem->Size = System::Drawing::Size(109, 22);
			this->listViewMenuItem->Text = L"List";
			this->listViewMenuItem->Click += gcnew System::EventHandler(this, &property_connection_editor::listViewMenuItem_Click);
			// 
			// leftToolStripMenuItem
			// 
			this->leftToolStripMenuItem->Name = L"leftToolStripMenuItem";
			this->leftToolStripMenuItem->Size = System::Drawing::Size(39, 20);
			this->leftToolStripMenuItem->Text = L"Left";
			// 
			// rightToolStripMenuItem
			// 
			this->rightToolStripMenuItem->Name = L"rightToolStripMenuItem";
			this->rightToolStripMenuItem->Size = System::Drawing::Size(47, 20);
			this->rightToolStripMenuItem->Text = L"Right";
			// 
			// top_panel
			// 
			this->top_panel->Controls->Add(this->btn_grab_right);
			this->top_panel->Controls->Add(this->btn_grab_left);
			this->top_panel->Controls->Add(this->btn_swap);
			this->top_panel->Controls->Add(this->right_label);
			this->top_panel->Controls->Add(this->left_label);
			this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->top_panel->Location = System::Drawing::Point(0, 24);
			this->top_panel->Name = L"top_panel";
			this->top_panel->Size = System::Drawing::Size(542, 22);
			this->top_panel->TabIndex = 1;
			// 
			// btn_grab_right
			// 
			this->btn_grab_right->Location = System::Drawing::Point(298, 0);
			this->btn_grab_right->Name = L"btn_grab_right";
			this->btn_grab_right->Size = System::Drawing::Size(75, 23);
			this->btn_grab_right->TabIndex = 8;
			this->btn_grab_right->Text = L"grab right";
			this->btn_grab_right->UseVisualStyleBackColor = true;
			this->btn_grab_right->Click += gcnew System::EventHandler(this, &property_connection_editor::btn_grab_right_Click);
			// 
			// btn_grab_left
			// 
			this->btn_grab_left->Location = System::Drawing::Point(136, 0);
			this->btn_grab_left->Name = L"btn_grab_left";
			this->btn_grab_left->Size = System::Drawing::Size(75, 23);
			this->btn_grab_left->TabIndex = 7;
			this->btn_grab_left->Text = L"grab left";
			this->btn_grab_left->UseVisualStyleBackColor = true;
			this->btn_grab_left->Click += gcnew System::EventHandler(this, &property_connection_editor::btn_grab_left_Click);
			// 
			// btn_swap
			// 
			this->btn_swap->Location = System::Drawing::Point(217, 0);
			this->btn_swap->Name = L"btn_swap";
			this->btn_swap->Size = System::Drawing::Size(75, 23);
			this->btn_swap->TabIndex = 6;
			this->btn_swap->Text = L"<-->";
			this->btn_swap->UseVisualStyleBackColor = true;
			this->btn_swap->Click += gcnew System::EventHandler(this, &property_connection_editor::btn_swap_Click);
			// 
			// right_label
			// 
			this->right_label->AutoSize = true;
			this->right_label->Dock = System::Windows::Forms::DockStyle::Right;
			this->right_label->Location = System::Drawing::Point(502, 0);
			this->right_label->Name = L"right_label";
			this->right_label->Padding = System::Windows::Forms::Padding(0, 5, 5, 0);
			this->right_label->Size = System::Drawing::Size(40, 18);
			this->right_label->TabIndex = 1;
			this->right_label->Text = L"label1";
			// 
			// left_label
			// 
			this->left_label->AutoSize = true;
			this->left_label->Dock = System::Windows::Forms::DockStyle::Left;
			this->left_label->Location = System::Drawing::Point(0, 0);
			this->left_label->Name = L"left_label";
			this->left_label->Padding = System::Windows::Forms::Padding(5, 5, 0, 0);
			this->left_label->Size = System::Drawing::Size(40, 18);
			this->left_label->TabIndex = 0;
			this->left_label->Text = L"label1";
			// 
			// bottom_panel
			// 
			this->bottom_panel->Controls->Add(this->bottom_buttons_layout);
			this->bottom_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->bottom_panel->Location = System::Drawing::Point(0, 485);
			this->bottom_panel->Name = L"bottom_panel";
			this->bottom_panel->Size = System::Drawing::Size(542, 34);
			this->bottom_panel->TabIndex = 2;
			// 
			// bottom_buttons_layout
			// 
			this->bottom_buttons_layout->ColumnCount = 5;
			this->bottom_buttons_layout->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				20)));
			this->bottom_buttons_layout->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				20)));
			this->bottom_buttons_layout->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				20)));
			this->bottom_buttons_layout->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				20)));
			this->bottom_buttons_layout->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				20)));
			this->bottom_buttons_layout->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 
				20)));
			this->bottom_buttons_layout->Controls->Add(this->close_button, 4, 0);
			this->bottom_buttons_layout->Controls->Add(this->make_connection_button, 3, 0);
			this->bottom_buttons_layout->Controls->Add(this->disconnect_button, 2, 0);
			this->bottom_buttons_layout->Controls->Add(this->button2, 1, 0);
			this->bottom_buttons_layout->Controls->Add(this->button1, 0, 0);
			this->bottom_buttons_layout->Dock = System::Windows::Forms::DockStyle::Fill;
			this->bottom_buttons_layout->Location = System::Drawing::Point(0, 0);
			this->bottom_buttons_layout->Name = L"bottom_buttons_layout";
			this->bottom_buttons_layout->RowCount = 1;
			this->bottom_buttons_layout->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 
				100)));
			this->bottom_buttons_layout->Size = System::Drawing::Size(542, 34);
			this->bottom_buttons_layout->TabIndex = 0;
			// 
			// close_button
			// 
			this->close_button->Dock = System::Windows::Forms::DockStyle::Fill;
			this->close_button->Location = System::Drawing::Point(435, 3);
			this->close_button->Name = L"close_button";
			this->close_button->Size = System::Drawing::Size(104, 28);
			this->close_button->TabIndex = 4;
			this->close_button->Text = L"Close";
			this->close_button->UseVisualStyleBackColor = true;
			this->close_button->Click += gcnew System::EventHandler(this, &property_connection_editor::close_button_Click);
			// 
			// make_connection_button
			// 
			this->make_connection_button->Dock = System::Windows::Forms::DockStyle::Fill;
			this->make_connection_button->Location = System::Drawing::Point(327, 3);
			this->make_connection_button->Name = L"make_connection_button";
			this->make_connection_button->Size = System::Drawing::Size(102, 28);
			this->make_connection_button->TabIndex = 3;
			this->make_connection_button->Text = L"Connect";
			this->make_connection_button->UseVisualStyleBackColor = true;
			this->make_connection_button->Click += gcnew System::EventHandler(this, &property_connection_editor::make_connection_button_Click);
			// 
			// disconnect_button
			// 
			this->disconnect_button->Dock = System::Windows::Forms::DockStyle::Fill;
			this->disconnect_button->Location = System::Drawing::Point(219, 3);
			this->disconnect_button->Name = L"disconnect_button";
			this->disconnect_button->Size = System::Drawing::Size(102, 28);
			this->disconnect_button->TabIndex = 2;
			this->disconnect_button->Text = L"Disconnect";
			this->disconnect_button->UseVisualStyleBackColor = true;
			this->disconnect_button->Click += gcnew System::EventHandler(this, &property_connection_editor::disconnect_button_Click);
			// 
			// button2
			// 
			this->button2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button2->Location = System::Drawing::Point(111, 3);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(102, 28);
			this->button2->TabIndex = 1;
			this->button2->Text = L"not implemented";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// button1
			// 
			this->button1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->button1->Location = System::Drawing::Point(3, 3);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(102, 28);
			this->button1->TabIndex = 0;
			this->button1->Text = L"not implemented";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// left_panel
			// 
			this->left_panel->Controls->Add(this->left_props_view);
			this->left_panel->Dock = System::Windows::Forms::DockStyle::Left;
			this->left_panel->Location = System::Drawing::Point(0, 46);
			this->left_panel->Name = L"left_panel";
			this->left_panel->Size = System::Drawing::Size(250, 439);
			this->left_panel->TabIndex = 3;
			// 
			// left_props_view
			// 
			this->left_props_view->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {columnHeader1, columnHeader2});
			this->left_props_view->Dock = System::Windows::Forms::DockStyle::Fill;
			this->left_props_view->ForeColor = System::Drawing::SystemColors::ControlText;
			this->left_props_view->FullRowSelect = true;
			this->left_props_view->GridLines = true;
			this->left_props_view->HideSelection = false;
			this->left_props_view->Location = System::Drawing::Point(0, 0);
			this->left_props_view->MultiSelect = false;
			this->left_props_view->Name = L"left_props_view";
			this->left_props_view->Size = System::Drawing::Size(250, 439);
			this->left_props_view->TabIndex = 2;
			this->left_props_view->UseCompatibleStateImageBehavior = false;
			this->left_props_view->View = System::Windows::Forms::View::Details;
			this->left_props_view->SelectedIndexChanged += gcnew System::EventHandler(this, &property_connection_editor::left_props_view_SelectedIndexChanged);
			// 
			// splitter1
			// 
			this->splitter1->Location = System::Drawing::Point(250, 46);
			this->splitter1->Name = L"splitter1";
			this->splitter1->Size = System::Drawing::Size(2, 439);
			this->splitter1->TabIndex = 4;
			this->splitter1->TabStop = false;
			// 
			// right_panel
			// 
			this->right_panel->Controls->Add(this->right_props_view);
			this->right_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->right_panel->Location = System::Drawing::Point(252, 46);
			this->right_panel->Name = L"right_panel";
			this->right_panel->Size = System::Drawing::Size(290, 439);
			this->right_panel->TabIndex = 5;
			// 
			// right_props_view
			// 
			this->right_props_view->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {columnHeader3, columnHeader4});
			this->right_props_view->Dock = System::Windows::Forms::DockStyle::Fill;
			this->right_props_view->ForeColor = System::Drawing::SystemColors::ControlText;
			this->right_props_view->FullRowSelect = true;
			this->right_props_view->GridLines = true;
			this->right_props_view->HideSelection = false;
			this->right_props_view->Location = System::Drawing::Point(0, 0);
			this->right_props_view->Name = L"right_props_view";
			this->right_props_view->Size = System::Drawing::Size(290, 439);
			this->right_props_view->TabIndex = 3;
			this->right_props_view->UseCompatibleStateImageBehavior = false;
			this->right_props_view->View = System::Windows::Forms::View::Details;
			this->right_props_view->DrawColumnHeader += gcnew System::Windows::Forms::DrawListViewColumnHeaderEventHandler(this, &property_connection_editor::right_props_view_DrawColumnHeader);
			this->right_props_view->DrawItem += gcnew System::Windows::Forms::DrawListViewItemEventHandler(this, &property_connection_editor::right_props_view_DrawItem);
			this->right_props_view->ItemSelectionChanged += gcnew System::Windows::Forms::ListViewItemSelectionChangedEventHandler(this, &property_connection_editor::right_props_view_ItemSelectionChanged);
			this->right_props_view->DrawSubItem += gcnew System::Windows::Forms::DrawListViewSubItemEventHandler(this, &property_connection_editor::right_props_view_DrawSubItem);
			// 
			// property_connection_editor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(542, 519);
			this->Controls->Add(this->right_panel);
			this->Controls->Add(this->splitter1);
			this->Controls->Add(this->left_panel);
			this->Controls->Add(this->bottom_panel);
			this->Controls->Add(this->top_panel);
			this->Controls->Add(this->top_menu);
			this->MainMenuStrip = this->top_menu;
			this->Name = L"property_connection_editor";
			this->Text = L"property_connection_editor";
			this->TopMost = true;
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &property_connection_editor::property_connection_editor_FormClosing);
			this->top_menu->ResumeLayout(false);
			this->top_menu->PerformLayout();
			this->top_panel->ResumeLayout(false);
			this->top_panel->PerformLayout();
			this->bottom_panel->ResumeLayout(false);
			this->bottom_buttons_layout->ResumeLayout(false);
			this->left_panel->ResumeLayout(false);
			this->right_panel->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	public:
		virtual void	execute(controls::hypergraph::node^ source, controls::hypergraph::node^ destinastion);

		void					set_src_object(object_base^ o);
		void					set_dst_object(object_base^ o);
	private:
		level_editor^			m_level_editor;
		project^				m_project;
		link_storage^			m_link_storage;
		object_base^			m_object_src;
		object_base^			m_object_dst;
		
		ref struct item_style
		{
			System::Drawing::Font^	m_font;
			System::Drawing::Color	m_fore_color;
			System::Drawing::Color	m_back_color;
		};
		enum class item_style_id{e_connected, e_connected_current, e_not_connected, e_connection_enabled, e_connection_disabled};
		cli::array<item_style^>^		m_items_style;

		void					in_constructor		();
		void					fill_object_props	(System::Windows::Forms::ListView^ view, object_base^ o, bool b_in);
		void					colorize_items		(bool b_left);
		void					update_buttons		();

		private: System::Void listViewMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void detailsViewMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void close_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void disconnect_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void make_connection_button_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void right_props_view_ItemSelectionChanged(System::Object^  sender, System::Windows::Forms::ListViewItemSelectionChangedEventArgs^  e);
		private: System::Void right_props_view_DrawItem(System::Object^  sender, System::Windows::Forms::DrawListViewItemEventArgs^  e);
		private: System::Void right_props_view_DrawColumnHeader(System::Object^  sender, System::Windows::Forms::DrawListViewColumnHeaderEventArgs^  e);
		private: System::Void right_props_view_DrawSubItem(System::Object^  sender, System::Windows::Forms::DrawListViewSubItemEventArgs^  e);
		private: System::Void left_props_view_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void btn_grab_left_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void btn_swap_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void btn_grab_right_Click(System::Object^  sender, System::EventArgs^  e);
		private: System::Void property_connection_editor_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
}

#endif // #ifndef PROPERTY_CONNECTION_EDITOR_H_INCLUDED