////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_IMPORT_FORM_H_INCLUDED
#define TERRAIN_IMPORT_FORM_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {
	ref struct terrain_import_export_settings;

	/// <summary>
	/// Summary for terrain_import_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class terrain_import_form : public System::Windows::Forms::Form
	{
	public:
		terrain_import_form()
		{
			InitializeComponent	( );
			in_constructor		();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~terrain_import_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected: 
	private: System::Windows::Forms::Button^  select_source_button;
	private: System::Windows::Forms::TextBox^  source_text_box;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Button^  ok_button;
	private: System::Windows::Forms::Button^  cancel_button;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::RadioButton^  diffuse_rgb_radio_;

	private: System::Windows::Forms::RadioButton^  diffuse_rgba_radio_;


	private: System::Windows::Forms::CheckBox^  create_cell_if_empty_check;
	private: System::Windows::Forms::RadioButton^  diffuse_rgb_height_a_radio_;

	private: System::Windows::Forms::RadioButton^  height_a_radio_;


	private: System::Windows::Forms::Label^  file_desc_label;



	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NumericUpDown^  up_down_lt_x;
	private: System::Windows::Forms::NumericUpDown^  up_down_lt_z;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::NumericUpDown^  up_down_size_x;
	private: System::Windows::Forms::NumericUpDown^  up_down_size_z;


	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::NumericUpDown^  max_height_up_down;
	private: System::Windows::Forms::NumericUpDown^  min_height_up_down;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::RadioButton^  height_grayscale_radio;







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
			this->file_desc_label = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->height_grayscale_radio = (gcnew System::Windows::Forms::RadioButton());
			this->max_height_up_down = (gcnew System::Windows::Forms::NumericUpDown());
			this->min_height_up_down = (gcnew System::Windows::Forms::NumericUpDown());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->height_a_radio_ = (gcnew System::Windows::Forms::RadioButton());
			this->diffuse_rgb_height_a_radio_ = (gcnew System::Windows::Forms::RadioButton());
			this->diffuse_rgb_radio_ = (gcnew System::Windows::Forms::RadioButton());
			this->diffuse_rgba_radio_ = (gcnew System::Windows::Forms::RadioButton());
			this->select_source_button = (gcnew System::Windows::Forms::Button());
			this->source_text_box = (gcnew System::Windows::Forms::TextBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->up_down_size_x = (gcnew System::Windows::Forms::NumericUpDown());
			this->up_down_size_z = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->up_down_lt_x = (gcnew System::Windows::Forms::NumericUpDown());
			this->up_down_lt_z = (gcnew System::Windows::Forms::NumericUpDown());
			this->create_cell_if_empty_check = (gcnew System::Windows::Forms::CheckBox());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->max_height_up_down))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->min_height_up_down))->BeginInit();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_size_x))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_size_z))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_lt_x))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_lt_z))->BeginInit();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->file_desc_label);
			this->groupBox1->Controls->Add(this->groupBox3);
			this->groupBox1->Controls->Add(this->select_source_button);
			this->groupBox1->Controls->Add(this->source_text_box);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(472, 141);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"source";
			// 
			// file_desc_label
			// 
			this->file_desc_label->AutoSize = true;
			this->file_desc_label->Location = System::Drawing::Point(10, 42);
			this->file_desc_label->Name = L"file_desc_label";
			this->file_desc_label->Size = System::Drawing::Size(78, 13);
			this->file_desc_label->TabIndex = 3;
			this->file_desc_label->Text = L"no file selected";
			// 
			// groupBox3
			// 
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->height_grayscale_radio);
			this->groupBox3->Controls->Add(this->max_height_up_down);
			this->groupBox3->Controls->Add(this->min_height_up_down);
			this->groupBox3->Controls->Add(this->label8);
			this->groupBox3->Controls->Add(this->label1);
			this->groupBox3->Controls->Add(this->height_a_radio_);
			this->groupBox3->Controls->Add(this->diffuse_rgb_height_a_radio_);
			this->groupBox3->Controls->Add(this->diffuse_rgb_radio_);
			this->groupBox3->Controls->Add(this->diffuse_rgba_radio_);
			this->groupBox3->Location = System::Drawing::Point(6, 56);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(461, 79);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			// 
			// height_grayscale_radio
			// 
			this->height_grayscale_radio->AutoSize = true;
			this->height_grayscale_radio->Checked = true;
			this->height_grayscale_radio->Location = System::Drawing::Point(174, 42);
			this->height_grayscale_radio->Name = L"height_grayscale_radio";
			this->height_grayscale_radio->Size = System::Drawing::Size(109, 17);
			this->height_grayscale_radio->TabIndex = 10;
			this->height_grayscale_radio->TabStop = true;
			this->height_grayscale_radio->Text = L"Height(Grayscale)";
			this->height_grayscale_radio->UseVisualStyleBackColor = true;
			// 
			// max_height_up_down
			// 
			this->max_height_up_down->DecimalPlaces = 3;
			this->max_height_up_down->Location = System::Drawing::Point(359, 42);
			this->max_height_up_down->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->max_height_up_down->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, System::Int32::MinValue});
			this->max_height_up_down->Name = L"max_height_up_down";
			this->max_height_up_down->Size = System::Drawing::Size(95, 20);
			this->max_height_up_down->TabIndex = 9;
			this->max_height_up_down->ThousandsSeparator = true;
			this->max_height_up_down->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, 0});
			this->max_height_up_down->ValueChanged += gcnew System::EventHandler(this, &terrain_import_form::min_height_up_down_ValueChanged);
			// 
			// min_height_up_down
			// 
			this->min_height_up_down->DecimalPlaces = 3;
			this->min_height_up_down->Location = System::Drawing::Point(359, 16);
			this->min_height_up_down->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->min_height_up_down->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, System::Int32::MinValue});
			this->min_height_up_down->Name = L"min_height_up_down";
			this->min_height_up_down->Size = System::Drawing::Size(95, 20);
			this->min_height_up_down->TabIndex = 8;
			this->min_height_up_down->ThousandsSeparator = true;
			this->min_height_up_down->ValueChanged += gcnew System::EventHandler(this, &terrain_import_form::min_height_up_down_ValueChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(289, 44);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(61, 13);
			this->label8->TabIndex = 7;
			this->label8->Text = L"Max Height";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(289, 18);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(58, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Min Height";
			// 
			// height_a_radio_
			// 
			this->height_a_radio_->AutoSize = true;
			this->height_a_radio_->Enabled = false;
			this->height_a_radio_->Location = System::Drawing::Point(93, 42);
			this->height_a_radio_->Name = L"height_a_radio_";
			this->height_a_radio_->Size = System::Drawing::Size(69, 17);
			this->height_a_radio_->TabIndex = 3;
			this->height_a_radio_->Text = L"Height(A)";
			this->height_a_radio_->UseVisualStyleBackColor = true;
			// 
			// diffuse_rgb_height_a_radio_
			// 
			this->diffuse_rgb_height_a_radio_->AutoSize = true;
			this->diffuse_rgb_height_a_radio_->Enabled = false;
			this->diffuse_rgb_height_a_radio_->Location = System::Drawing::Point(109, 19);
			this->diffuse_rgb_height_a_radio_->Name = L"diffuse_rgb_height_a_radio_";
			this->diffuse_rgb_height_a_radio_->Size = System::Drawing::Size(134, 17);
			this->diffuse_rgb_height_a_radio_->TabIndex = 2;
			this->diffuse_rgb_height_a_radio_->Text = L"Diffuse(RGB) Height(A)";
			this->diffuse_rgb_height_a_radio_->UseVisualStyleBackColor = true;
			// 
			// diffuse_rgb_radio_
			// 
			this->diffuse_rgb_radio_->AutoSize = true;
			this->diffuse_rgb_radio_->Enabled = false;
			this->diffuse_rgb_radio_->Location = System::Drawing::Point(6, 42);
			this->diffuse_rgb_radio_->Name = L"diffuse_rgb_radio_";
			this->diffuse_rgb_radio_->Size = System::Drawing::Size(87, 17);
			this->diffuse_rgb_radio_->TabIndex = 1;
			this->diffuse_rgb_radio_->Text = L"Diffuse(RGB)";
			this->diffuse_rgb_radio_->UseVisualStyleBackColor = true;
			// 
			// diffuse_rgba_radio_
			// 
			this->diffuse_rgba_radio_->AutoSize = true;
			this->diffuse_rgba_radio_->Enabled = false;
			this->diffuse_rgba_radio_->Location = System::Drawing::Point(6, 19);
			this->diffuse_rgba_radio_->Name = L"diffuse_rgba_radio_";
			this->diffuse_rgba_radio_->Size = System::Drawing::Size(97, 17);
			this->diffuse_rgba_radio_->TabIndex = 0;
			this->diffuse_rgba_radio_->Text = L"Diffuse (RGBA)";
			this->diffuse_rgba_radio_->UseVisualStyleBackColor = true;
			// 
			// select_source_button
			// 
			this->select_source_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->select_source_button->Location = System::Drawing::Point(444, 19);
			this->select_source_button->Name = L"select_source_button";
			this->select_source_button->Size = System::Drawing::Size(23, 23);
			this->select_source_button->TabIndex = 1;
			this->select_source_button->Text = L"...";
			this->select_source_button->UseVisualStyleBackColor = true;
			this->select_source_button->Click += gcnew System::EventHandler(this, &terrain_import_form::select_source_button_Click);
			// 
			// source_text_box
			// 
			this->source_text_box->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->source_text_box->Location = System::Drawing::Point(6, 19);
			this->source_text_box->Name = L"source_text_box";
			this->source_text_box->ReadOnly = true;
			this->source_text_box->Size = System::Drawing::Size(432, 20);
			this->source_text_box->TabIndex = 0;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->label7);
			this->groupBox2->Controls->Add(this->up_down_size_x);
			this->groupBox2->Controls->Add(this->up_down_size_z);
			this->groupBox2->Controls->Add(this->label4);
			this->groupBox2->Controls->Add(this->label3);
			this->groupBox2->Controls->Add(this->label2);
			this->groupBox2->Controls->Add(this->up_down_lt_x);
			this->groupBox2->Controls->Add(this->up_down_lt_z);
			this->groupBox2->Controls->Add(this->create_cell_if_empty_check);
			this->groupBox2->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox2->Location = System::Drawing::Point(0, 141);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(472, 72);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"size";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(351, 45);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(14, 13);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Z";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(351, 21);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(14, 13);
			this->label6->TabIndex = 9;
			this->label6->Text = L"X";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(295, 21);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(61, 13);
			this->label7->TabIndex = 8;
			this->label7->Text = L"Size (x64m)";
			// 
			// up_down_size_x
			// 
			this->up_down_size_x->Location = System::Drawing::Point(371, 19);
			this->up_down_size_x->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			this->up_down_size_x->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->up_down_size_x->Name = L"up_down_size_x";
			this->up_down_size_x->Size = System::Drawing::Size(95, 20);
			this->up_down_size_x->TabIndex = 7;
			this->up_down_size_x->ThousandsSeparator = true;
			this->up_down_size_x->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// up_down_size_z
			// 
			this->up_down_size_z->Location = System::Drawing::Point(371, 43);
			this->up_down_size_z->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			this->up_down_size_z->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->up_down_size_z->Name = L"up_down_size_z";
			this->up_down_size_z->Size = System::Drawing::Size(95, 20);
			this->up_down_size_z->TabIndex = 6;
			this->up_down_size_z->ThousandsSeparator = true;
			this->up_down_size_z->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(174, 45);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(14, 13);
			this->label4->TabIndex = 5;
			this->label4->Text = L"Z";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(174, 21);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(14, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"X";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(90, 26);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(78, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"LeftTop (x64m)";
			// 
			// up_down_lt_x
			// 
			this->up_down_lt_x->Location = System::Drawing::Point(194, 19);
			this->up_down_lt_x->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			this->up_down_lt_x->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, System::Int32::MinValue});
			this->up_down_lt_x->Name = L"up_down_lt_x";
			this->up_down_lt_x->Size = System::Drawing::Size(95, 20);
			this->up_down_lt_x->TabIndex = 2;
			this->up_down_lt_x->ThousandsSeparator = true;
			// 
			// up_down_lt_z
			// 
			this->up_down_lt_z->Location = System::Drawing::Point(194, 43);
			this->up_down_lt_z->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, 0});
			this->up_down_lt_z->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {200, 0, 0, System::Int32::MinValue});
			this->up_down_lt_z->Name = L"up_down_lt_z";
			this->up_down_lt_z->Size = System::Drawing::Size(95, 20);
			this->up_down_lt_z->TabIndex = 1;
			this->up_down_lt_z->ThousandsSeparator = true;
			// 
			// create_cell_if_empty_check
			// 
			this->create_cell_if_empty_check->AutoSize = true;
			this->create_cell_if_empty_check->Checked = true;
			this->create_cell_if_empty_check->CheckState = System::Windows::Forms::CheckState::Checked;
			this->create_cell_if_empty_check->Location = System::Drawing::Point(6, 46);
			this->create_cell_if_empty_check->Name = L"create_cell_if_empty_check";
			this->create_cell_if_empty_check->Size = System::Drawing::Size(115, 17);
			this->create_cell_if_empty_check->TabIndex = 0;
			this->create_cell_if_empty_check->Text = L"Create cell if empty";
			this->create_cell_if_empty_check->UseVisualStyleBackColor = true;
			// 
			// ok_button
			// 
			this->ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ok_button->Location = System::Drawing::Point(310, 239);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(75, 24);
			this->ok_button->TabIndex = 2;
			this->ok_button->Text = L"Ok";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &terrain_import_form::ok_button_Click);
			// 
			// cancel_button
			// 
			this->cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancel_button->Location = System::Drawing::Point(391, 239);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(75, 24);
			this->cancel_button->TabIndex = 3;
			this->cancel_button->Text = L"Cancel";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &terrain_import_form::cancel_button_Click);
			// 
			// terrain_import_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(472, 268);
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->ok_button);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = L"terrain_import_form";
			this->Text = L"Import";
			this->TopMost = true;
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->max_height_up_down))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->min_height_up_down))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_size_x))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_size_z))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_lt_x))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->up_down_lt_z))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void select_source_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ok_button_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void cancel_button_Click(System::Object^  sender, System::EventArgs^  e);
	public:
		terrain_import_export_settings^			m_settings;
	private:
		void								in_constructor();
public:
	void init_layout();
	System::Windows::Forms::NumericUpDown^  get_up_down_lt_x() {return up_down_lt_x;}
	System::Windows::Forms::NumericUpDown^  get_up_down_lt_z() {return up_down_lt_z;}

	System::Windows::Forms::NumericUpDown^  get_up_down_size_x() {return up_down_size_x;}
	System::Windows::Forms::NumericUpDown^  get_up_down_size_z() {return up_down_size_z;}
private: System::Void min_height_up_down_ValueChanged(System::Object^  sender, System::EventArgs^  e);
};

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_IMPORT_FORM_H_INCLUDED