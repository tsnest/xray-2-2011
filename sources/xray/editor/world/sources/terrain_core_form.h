////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_CORE_FORM_H_INCLUDED
#define TERRAIN_CORE_FORM_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "terrain_quad.h"

namespace xray {
namespace editor {
	ref class tool_terrain;
	ref class terrain_core;
	ref class terrain_import_form;
	ref class project_background_item;
	value struct terrain_node_key;
	/// <summary>
	/// Summary for terrain_core_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class terrain_core_form : public System::Windows::Forms::Form
	{
	public:
		terrain_core_form(tool_terrain^ t)
		:m_tool(t)
		{
			InitializeComponent();
			in_constructor		();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~terrain_core_form()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Panel^  panel1;

	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Splitter^  splitter1;
	private: System::Windows::Forms::Panel^  global_map_panel;
	private: System::Windows::Forms::ContextMenuStrip^  global_map_menu;



	private: System::Windows::Forms::Button^  button_move_down;
	private: System::Windows::Forms::Button^  button_move_right;
	private: System::Windows::Forms::Button^  button_move_up;
	private: System::Windows::Forms::Button^  button_move_left;
	private: System::Windows::Forms::Button^  import_button;


	private: System::Windows::Forms::NumericUpDown^  m_scale_up_down;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  copy_nodes_button;


	private: System::Windows::Forms::Button^  edit_bkg_button;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  m_num_size_y;
	private: System::Windows::Forms::NumericUpDown^  m_num_pos_y;
	private: System::Windows::Forms::NumericUpDown^  m_num_size_x;
	private: System::Windows::Forms::NumericUpDown^  m_num_pos_x;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  current_y_label;

	private: System::Windows::Forms::Label^  current_x_label;
	private: System::Windows::Forms::Button^  focus_button;
	private: System::Windows::Forms::Panel^  map_bkg_info_panel;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::NumericUpDown^  bkg_scale_up_down;
	private: System::Windows::Forms::Button^  remove_bkg_button;
	private: System::Windows::Forms::Button^  insert_bkg_button;
	private: System::Windows::Forms::Button^  select_bkg_file_button;
	private: System::Windows::Forms::Label^  bkg_file_label;
	private: System::Windows::Forms::Button^  export_button;
	private: System::Windows::Forms::Button^  reset_colors_button;






















	private: System::ComponentModel::IContainer^  components;




	protected: 

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
			this->components = (gcnew System::ComponentModel::Container());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->map_bkg_info_panel = (gcnew System::Windows::Forms::Panel());
			this->select_bkg_file_button = (gcnew System::Windows::Forms::Button());
			this->bkg_file_label = (gcnew System::Windows::Forms::Label());
			this->remove_bkg_button = (gcnew System::Windows::Forms::Button());
			this->insert_bkg_button = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->bkg_scale_up_down = (gcnew System::Windows::Forms::NumericUpDown());
			this->focus_button = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->m_num_size_y = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_num_pos_y = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_num_size_x = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_num_pos_x = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->current_y_label = (gcnew System::Windows::Forms::Label());
			this->current_x_label = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->m_scale_up_down = (gcnew System::Windows::Forms::NumericUpDown());
			this->button_move_down = (gcnew System::Windows::Forms::Button());
			this->button_move_right = (gcnew System::Windows::Forms::Button());
			this->button_move_up = (gcnew System::Windows::Forms::Button());
			this->button_move_left = (gcnew System::Windows::Forms::Button());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->export_button = (gcnew System::Windows::Forms::Button());
			this->copy_nodes_button = (gcnew System::Windows::Forms::Button());
			this->edit_bkg_button = (gcnew System::Windows::Forms::Button());
			this->import_button = (gcnew System::Windows::Forms::Button());
			this->splitter1 = (gcnew System::Windows::Forms::Splitter());
			this->global_map_panel = (gcnew System::Windows::Forms::Panel());
			this->global_map_menu = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->reset_colors_button = (gcnew System::Windows::Forms::Button());
			this->panel1->SuspendLayout();
			this->map_bkg_info_panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->bkg_scale_up_down))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_size_y))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_pos_y))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_size_x))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_pos_x))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_scale_up_down))->BeginInit();
			this->panel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->map_bkg_info_panel);
			this->panel1->Controls->Add(this->focus_button);
			this->panel1->Controls->Add(this->groupBox1);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->current_y_label);
			this->panel1->Controls->Add(this->current_x_label);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->m_scale_up_down);
			this->panel1->Controls->Add(this->button_move_down);
			this->panel1->Controls->Add(this->button_move_right);
			this->panel1->Controls->Add(this->button_move_up);
			this->panel1->Controls->Add(this->button_move_left);
			this->panel1->Controls->Add(this->panel2);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Right;
			this->panel1->Location = System::Drawing::Point(476, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(157, 514);
			this->panel1->TabIndex = 0;
			// 
			// map_bkg_info_panel
			// 
			this->map_bkg_info_panel->Controls->Add(this->select_bkg_file_button);
			this->map_bkg_info_panel->Controls->Add(this->bkg_file_label);
			this->map_bkg_info_panel->Controls->Add(this->remove_bkg_button);
			this->map_bkg_info_panel->Controls->Add(this->insert_bkg_button);
			this->map_bkg_info_panel->Controls->Add(this->label5);
			this->map_bkg_info_panel->Controls->Add(this->bkg_scale_up_down);
			this->map_bkg_info_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->map_bkg_info_panel->Location = System::Drawing::Point(0, 99);
			this->map_bkg_info_panel->Name = L"map_bkg_info_panel";
			this->map_bkg_info_panel->Size = System::Drawing::Size(157, 100);
			this->map_bkg_info_panel->TabIndex = 22;
			// 
			// select_bkg_file_button
			// 
			this->select_bkg_file_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->select_bkg_file_button->Location = System::Drawing::Point(129, 33);
			this->select_bkg_file_button->Name = L"select_bkg_file_button";
			this->select_bkg_file_button->Size = System::Drawing::Size(21, 23);
			this->select_bkg_file_button->TabIndex = 16;
			this->select_bkg_file_button->Text = L"...";
			this->select_bkg_file_button->UseVisualStyleBackColor = true;
			this->select_bkg_file_button->Click += gcnew System::EventHandler(this, &terrain_core_form::select_bkg_file_button_Click);
			// 
			// bkg_file_label
			// 
			this->bkg_file_label->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->bkg_file_label->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->bkg_file_label->Location = System::Drawing::Point(4, 37);
			this->bkg_file_label->Name = L"bkg_file_label";
			this->bkg_file_label->Size = System::Drawing::Size(119, 14);
			this->bkg_file_label->TabIndex = 15;
			this->bkg_file_label->Text = L"label6";
			// 
			// remove_bkg_button
			// 
			this->remove_bkg_button->Location = System::Drawing::Point(69, 6);
			this->remove_bkg_button->Name = L"remove_bkg_button";
			this->remove_bkg_button->Size = System::Drawing::Size(56, 23);
			this->remove_bkg_button->TabIndex = 14;
			this->remove_bkg_button->Text = L"remove";
			this->remove_bkg_button->UseVisualStyleBackColor = true;
			this->remove_bkg_button->Click += gcnew System::EventHandler(this, &terrain_core_form::remove_bkg_button_Click);
			// 
			// insert_bkg_button
			// 
			this->insert_bkg_button->Location = System::Drawing::Point(7, 6);
			this->insert_bkg_button->Name = L"insert_bkg_button";
			this->insert_bkg_button->Size = System::Drawing::Size(56, 23);
			this->insert_bkg_button->TabIndex = 13;
			this->insert_bkg_button->Text = L"insert";
			this->insert_bkg_button->UseVisualStyleBackColor = true;
			this->insert_bkg_button->Click += gcnew System::EventHandler(this, &terrain_core_form::insert_bkg_button_Click);
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 61);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(114, 13);
			this->label5->TabIndex = 12;
			this->label5->Text = L"scale (meters per pixel)";
			// 
			// bkg_scale_up_down
			// 
			this->bkg_scale_up_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->bkg_scale_up_down->Location = System::Drawing::Point(5, 77);
			this->bkg_scale_up_down->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->bkg_scale_up_down->Name = L"bkg_scale_up_down";
			this->bkg_scale_up_down->Size = System::Drawing::Size(88, 20);
			this->bkg_scale_up_down->TabIndex = 11;
			this->bkg_scale_up_down->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->bkg_scale_up_down->ValueChanged += gcnew System::EventHandler(this, &terrain_core_form::bkg_scale_up_down_ValueChanged);
			// 
			// focus_button
			// 
			this->focus_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->focus_button->Location = System::Drawing::Point(4, 411);
			this->focus_button->Name = L"focus_button";
			this->focus_button->Size = System::Drawing::Size(131, 25);
			this->focus_button->TabIndex = 21;
			this->focus_button->Text = L"Focus on selection";
			this->focus_button->UseVisualStyleBackColor = true;
			this->focus_button->Click += gcnew System::EventHandler(this, &terrain_core_form::focus_button_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->m_num_size_y);
			this->groupBox1->Controls->Add(this->m_num_pos_y);
			this->groupBox1->Controls->Add(this->m_num_size_x);
			this->groupBox1->Controls->Add(this->m_num_pos_x);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->groupBox1->Location = System::Drawing::Point(0, 446);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(157, 68);
			this->groupBox1->TabIndex = 20;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Selection";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(0, 42);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(27, 13);
			this->label3->TabIndex = 22;
			this->label3->Text = L"Size";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(0, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(44, 13);
			this->label1->TabIndex = 21;
			this->label1->Text = L"Position";
			// 
			// m_num_size_y
			// 
			this->m_num_size_y->Location = System::Drawing::Point(104, 41);
			this->m_num_size_y->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, 0});
			this->m_num_size_y->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_num_size_y->Name = L"m_num_size_y";
			this->m_num_size_y->Size = System::Drawing::Size(46, 20);
			this->m_num_size_y->TabIndex = 20;
			this->m_num_size_y->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_num_size_y->ValueChanged += gcnew System::EventHandler(this, &terrain_core_form::spin_num_changed);
			// 
			// m_num_pos_y
			// 
			this->m_num_pos_y->Location = System::Drawing::Point(104, 15);
			this->m_num_pos_y->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, 0});
			this->m_num_pos_y->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, System::Int32::MinValue});
			this->m_num_pos_y->Name = L"m_num_pos_y";
			this->m_num_pos_y->Size = System::Drawing::Size(46, 20);
			this->m_num_pos_y->TabIndex = 19;
			this->m_num_pos_y->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_num_pos_y->ValueChanged += gcnew System::EventHandler(this, &terrain_core_form::spin_num_changed);
			// 
			// m_num_size_x
			// 
			this->m_num_size_x->Location = System::Drawing::Point(47, 41);
			this->m_num_size_x->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, 0});
			this->m_num_size_x->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_num_size_x->Name = L"m_num_size_x";
			this->m_num_size_x->Size = System::Drawing::Size(46, 20);
			this->m_num_size_x->TabIndex = 18;
			this->m_num_size_x->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_num_size_x->ValueChanged += gcnew System::EventHandler(this, &terrain_core_form::spin_num_changed);
			// 
			// m_num_pos_x
			// 
			this->m_num_pos_x->Location = System::Drawing::Point(48, 15);
			this->m_num_pos_x->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, 0});
			this->m_num_pos_x->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {400, 0, 0, System::Int32::MinValue});
			this->m_num_pos_x->Name = L"m_num_pos_x";
			this->m_num_pos_x->Size = System::Drawing::Size(45, 20);
			this->m_num_pos_x->TabIndex = 17;
			this->m_num_pos_x->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_num_pos_x->ValueChanged += gcnew System::EventHandler(this, &terrain_core_form::spin_num_changed);
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(1, 388);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(41, 13);
			this->label4->TabIndex = 19;
			this->label4->Text = L"Current";
			// 
			// current_y_label
			// 
			this->current_y_label->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->current_y_label->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->current_y_label->Location = System::Drawing::Point(95, 388);
			this->current_y_label->Name = L"current_y_label";
			this->current_y_label->Size = System::Drawing::Size(40, 20);
			this->current_y_label->TabIndex = 18;
			this->current_y_label->Text = L" ";
			// 
			// current_x_label
			// 
			this->current_x_label->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->current_x_label->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->current_x_label->Location = System::Drawing::Point(48, 388);
			this->current_x_label->Name = L"current_x_label";
			this->current_x_label->Size = System::Drawing::Size(40, 20);
			this->current_x_label->TabIndex = 17;
			this->current_x_label->Text = L" ";
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(9, 343);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 10;
			this->label2->Text = L"view scale";
			// 
			// m_scale_up_down
			// 
			this->m_scale_up_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->m_scale_up_down->Location = System::Drawing::Point(7, 362);
			this->m_scale_up_down->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {50, 0, 0, 0});
			this->m_scale_up_down->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_scale_up_down->Name = L"m_scale_up_down";
			this->m_scale_up_down->Size = System::Drawing::Size(56, 20);
			this->m_scale_up_down->TabIndex = 9;
			this->m_scale_up_down->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->m_scale_up_down->ValueChanged += gcnew System::EventHandler(this, &terrain_core_form::m_scale_up_down_ValueChanged);
			// 
			// button_move_down
			// 
			this->button_move_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_move_down->Location = System::Drawing::Point(95, 362);
			this->button_move_down->Name = L"button_move_down";
			this->button_move_down->Size = System::Drawing::Size(25, 25);
			this->button_move_down->TabIndex = 7;
			this->button_move_down->Text = L"|";
			this->button_move_down->UseVisualStyleBackColor = true;
			this->button_move_down->Click += gcnew System::EventHandler(this, &terrain_core_form::button_move_down_Click);
			// 
			// button_move_right
			// 
			this->button_move_right->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_move_right->Location = System::Drawing::Point(117, 337);
			this->button_move_right->Name = L"button_move_right";
			this->button_move_right->Size = System::Drawing::Size(25, 25);
			this->button_move_right->TabIndex = 6;
			this->button_move_right->Text = L">";
			this->button_move_right->UseVisualStyleBackColor = true;
			this->button_move_right->Click += gcnew System::EventHandler(this, &terrain_core_form::button_move_right_Click);
			// 
			// button_move_up
			// 
			this->button_move_up->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_move_up->Location = System::Drawing::Point(95, 314);
			this->button_move_up->Name = L"button_move_up";
			this->button_move_up->Size = System::Drawing::Size(25, 25);
			this->button_move_up->TabIndex = 5;
			this->button_move_up->Text = L"^";
			this->button_move_up->UseVisualStyleBackColor = true;
			this->button_move_up->Click += gcnew System::EventHandler(this, &terrain_core_form::button_move_up_Click);
			// 
			// button_move_left
			// 
			this->button_move_left->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_move_left->Location = System::Drawing::Point(74, 337);
			this->button_move_left->Name = L"button_move_left";
			this->button_move_left->Size = System::Drawing::Size(25, 25);
			this->button_move_left->TabIndex = 4;
			this->button_move_left->Text = L"<";
			this->button_move_left->UseVisualStyleBackColor = true;
			this->button_move_left->Click += gcnew System::EventHandler(this, &terrain_core_form::button_move_left_Click);
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->reset_colors_button);
			this->panel2->Controls->Add(this->export_button);
			this->panel2->Controls->Add(this->copy_nodes_button);
			this->panel2->Controls->Add(this->edit_bkg_button);
			this->panel2->Controls->Add(this->import_button);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel2->Location = System::Drawing::Point(0, 0);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(157, 99);
			this->panel2->TabIndex = 1;
			// 
			// export_button
			// 
			this->export_button->Location = System::Drawing::Point(64, 6);
			this->export_button->Name = L"export_button";
			this->export_button->Size = System::Drawing::Size(59, 25);
			this->export_button->TabIndex = 11;
			this->export_button->Text = L"Export";
			this->export_button->UseVisualStyleBackColor = true;
			this->export_button->Click += gcnew System::EventHandler(this, &terrain_core_form::export_button_Click);
			// 
			// copy_nodes_button
			// 
			this->copy_nodes_button->Location = System::Drawing::Point(4, 68);
			this->copy_nodes_button->Name = L"copy_nodes_button";
			this->copy_nodes_button->Size = System::Drawing::Size(75, 25);
			this->copy_nodes_button->TabIndex = 10;
			this->copy_nodes_button->Text = L"Copy nodes";
			this->copy_nodes_button->UseVisualStyleBackColor = true;
			this->copy_nodes_button->Click += gcnew System::EventHandler(this, &terrain_core_form::copy_nodes_button_Click);
			// 
			// edit_bkg_button
			// 
			this->edit_bkg_button->Location = System::Drawing::Point(4, 37);
			this->edit_bkg_button->Name = L"edit_bkg_button";
			this->edit_bkg_button->Size = System::Drawing::Size(102, 25);
			this->edit_bkg_button->TabIndex = 9;
			this->edit_bkg_button->Text = L"Edit bkg";
			this->edit_bkg_button->UseVisualStyleBackColor = true;
			this->edit_bkg_button->Click += gcnew System::EventHandler(this, &terrain_core_form::edit_bkg_button_Click);
			// 
			// import_button
			// 
			this->import_button->Location = System::Drawing::Point(6, 6);
			this->import_button->Name = L"import_button";
			this->import_button->Size = System::Drawing::Size(59, 25);
			this->import_button->TabIndex = 8;
			this->import_button->Text = L"Import";
			this->import_button->UseVisualStyleBackColor = true;
			this->import_button->Click += gcnew System::EventHandler(this, &terrain_core_form::import_button_Click);
			// 
			// splitter1
			// 
			this->splitter1->Dock = System::Windows::Forms::DockStyle::Right;
			this->splitter1->Location = System::Drawing::Point(473, 0);
			this->splitter1->Name = L"splitter1";
			this->splitter1->Size = System::Drawing::Size(3, 514);
			this->splitter1->TabIndex = 1;
			this->splitter1->TabStop = false;
			// 
			// global_map_panel
			// 
			this->global_map_panel->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->global_map_panel->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->global_map_panel->ContextMenuStrip = this->global_map_menu;
			this->global_map_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->global_map_panel->Location = System::Drawing::Point(0, 0);
			this->global_map_panel->Name = L"global_map_panel";
			this->global_map_panel->Size = System::Drawing::Size(473, 514);
			this->global_map_panel->TabIndex = 2;
			this->global_map_panel->MouseLeave += gcnew System::EventHandler(this, &terrain_core_form::global_map_panel_MouseLeave);
			this->global_map_panel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &terrain_core_form::global_map_panel_Paint);
			this->global_map_panel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &terrain_core_form::global_map_panel_MouseMove);
			this->global_map_panel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &terrain_core_form::global_map_panel_MouseDown);
			this->global_map_panel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &terrain_core_form::global_map_panel_MouseUp);
			// 
			// global_map_menu
			// 
			this->global_map_menu->Name = L"global_map_menu";
			this->global_map_menu->Size = System::Drawing::Size(61, 4);
			this->global_map_menu->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &terrain_core_form::global_map_menu_Opening);
			// 
			// reset_colors_button
			// 
			this->reset_colors_button->Location = System::Drawing::Point(79, 68);
			this->reset_colors_button->Name = L"reset_colors_button";
			this->reset_colors_button->Size = System::Drawing::Size(75, 25);
			this->reset_colors_button->TabIndex = 12;
			this->reset_colors_button->Text = L"Reset clr";
			this->reset_colors_button->UseVisualStyleBackColor = true;
			this->reset_colors_button->Click += gcnew System::EventHandler(this, &terrain_core_form::reset_colors_button_Click);
			// 
			// terrain_core_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(633, 514);
			this->Controls->Add(this->global_map_panel);
			this->Controls->Add(this->splitter1);
			this->Controls->Add(this->panel1);
			this->Name = L"terrain_core_form";
			this->ShowInTaskbar = false;
			this->Text = L"terrain_core_form";
			this->TopMost = true;
			this->Deactivate += gcnew System::EventHandler(this, &terrain_core_form::terrain_core_form_Deactivate);
			this->Activated += gcnew System::EventHandler(this, &terrain_core_form::terrain_core_form_Activated);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &terrain_core_form::terrain_core_form_FormClosing);
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->map_bkg_info_panel->ResumeLayout(false);
			this->map_bkg_info_panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->bkg_scale_up_down))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_size_y))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_pos_y))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_size_x))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_num_pos_x))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_scale_up_down))->EndInit();
			this->panel2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void global_map_panel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
	private: System::Void terrain_core_form_Activated(System::Object^  sender, System::EventArgs^  e);
	private: System::Void terrain_core_form_Deactivate(System::Object^  sender, System::EventArgs^  e);
	private: System::Void global_map_menu_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e);
	private: System::Void global_map_panel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void button_move_left_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void button_move_up_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void button_move_right_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void button_move_down_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void global_map_panel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void global_map_panel_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
	private: System::Void global_map_panel_MouseLeave(System::Object^  sender, System::EventArgs^  e);

private:
		void				in_constructor();
		tool_terrain^		m_tool;
		terrain_core^		m_terrain_core;

		Point				m_first_node;
		int					m_current_view_scale;
		bool				m_edit_backgroungs_mode;
		bool				m_edit_nodes_mode;

		bool				m_mouse_rect_capture;
		bool				m_ext_value;
//		terrain_node_key	m_selected_node_lt;
//		terrain_node_key	m_selected_node_rb;

		void				set_current_view_scale_impl	(int value);
		terrain_node_key	get_node_key_mouse			();
		void				add_node					(System::Object^ sender, System::EventArgs^);
		void				clear_node					(System::Object^ sender, System::EventArgs^);
		void				focus_to_node				(System::Object^ sender, System::EventArgs^);
		void				add_backgroung_image		(System::Object^ sender, System::EventArgs^);
		void				remove_backgroung_image		(System::Object^ sender, System::EventArgs^);
		void				update_status_string		();
		void				draw_background				(System::Drawing::Graphics^);
		System::Drawing::Point point_to_client			(float3 p );
public:
	property int					current_scale{
		int							get	()								{return m_current_view_scale;}
		void						set	(int value)						{set_current_view_scale_impl(value);}
	}
	property int	num_pos_x{
		int				get	()					{ return System::Decimal::ToInt32(m_num_pos_x->Value); }
		void			set	(int value)			{ m_ext_value=true; m_num_pos_x->Value = value; m_ext_value=false; }
	}
	property int	num_pos_y{
		int				get	()					{ return System::Decimal::ToInt32(m_num_pos_y->Value); }
		void			set	(int value)			{ m_ext_value=true; m_num_pos_y->Value = value; m_ext_value=false; }
	}
	property int	num_size_x{
		int				get	()					{ return System::Decimal::ToInt32(m_num_size_x->Value); }
		void			set	(int value)			{ m_ext_value=true; m_num_size_x->Value = value; m_ext_value=false; }
	}
	property int	num_size_y{
		int				get	()					{ return System::Decimal::ToInt32(m_num_size_y->Value); }
		void			set	(int value)			{ m_ext_value=true; m_num_size_y->Value = value; m_ext_value=false; }
	}
	
	property terrain_node_key selected_nodes_pos{
		terrain_node_key	get()				{ return terrain_node_key(num_pos_x, num_pos_y);}
		void				set(terrain_node_key value)	{num_pos_x = value.x; num_pos_y=value.z; update_bkg_panel();}
	}
	
	property terrain_node_key selected_nodes_size{
		terrain_node_key	get()				{ return terrain_node_key(num_size_x, num_size_y);}
		void				set(terrain_node_key value)	{num_size_x = value.x; num_size_y=value.z;}
	}
	terrain_import_form^		m_import_form;

	private: System::Void import_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void export_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_scale_up_down_ValueChanged(System::Object^  sender, System::EventArgs^  e);
			 System::Drawing::Brush^					m_node_brush;
	private: System::Void terrain_core_form_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
	private: System::Void num_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void edit_bkg_button_Click(System::Object^  sender, System::EventArgs^  e);
	void spin_num_changed	(System::Object^, System::EventArgs^);
	void do_edit_bkg_mode			( bool what_to_do );

	void focus_button_Click					( System::Object^, System::EventArgs^ );
	void insert_bkg_button_Click			( System::Object^, System::EventArgs^ );
	void remove_bkg_button_Click			( System::Object^, System::EventArgs^ );
	void select_bkg_file_button_Click		( System::Object^, System::EventArgs^ );
	void copy_nodes_button_Click			( System::Object^, System::EventArgs^ );
	void bkg_scale_up_down_ValueChanged		( System::Object^, System::EventArgs^ );
	void update_bkg_panel					( );
	project_background_item^ focused_bkg_item ( );
	bool  select_bkg_texture_name			( System::String^% selected );
	void reset_colors_button_Click			(System::Object^  sender, System::EventArgs^  e);
};



} // editor namespace
} // xray namespace

#endif // #ifndef TERRAIN_CORE_FORM_H_INCLUDED