////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_COPY_NODES_FORM_H_INCLUDED
#define TERRAIN_COPY_NODES_FORM_H_INCLUDED

namespace xray{
namespace editor {

	ref class terrain_core;
	/// <summary>
	/// Summary for terrain_copy_nodes_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class terrain_copy_nodes_form : public System::Windows::Forms::Form
	{
	public:
		terrain_copy_nodes_form(void)
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
		~terrain_copy_nodes_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  l1;
	public: System::Windows::Forms::NumericUpDown^  from_x_numericUpDown;
	private: 
	public: System::Windows::Forms::NumericUpDown^  from_y_numericUpDown;
	public: System::Windows::Forms::NumericUpDown^  to_y_numericUpDown;
	public: System::Windows::Forms::NumericUpDown^  to_x_numericUpDown;
	protected: 




	private: System::Windows::Forms::Label^  label1;
	public: System::Windows::Forms::NumericUpDown^  size_y_numericUpDown;
	private: 
	public: System::Windows::Forms::NumericUpDown^  size_x_numericUpDown;


	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Label^  status_label;

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
			this->l1 = (gcnew System::Windows::Forms::Label());
			this->from_x_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->from_y_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->to_y_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->to_x_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->size_y_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->size_x_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->status_label = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->from_x_numericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->from_y_numericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->to_y_numericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->to_x_numericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->size_y_numericUpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->size_x_numericUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// l1
			// 
			this->l1->AutoSize = true;
			this->l1->Location = System::Drawing::Point(12, 22);
			this->l1->Name = L"l1";
			this->l1->Size = System::Drawing::Size(30, 13);
			this->l1->TabIndex = 0;
			this->l1->Text = L"From";
			// 
			// from_x_numericUpDown
			// 
			this->from_x_numericUpDown->Location = System::Drawing::Point(64, 20);
			this->from_x_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->from_x_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, System::Int32::MinValue});
			this->from_x_numericUpDown->Name = L"from_x_numericUpDown";
			this->from_x_numericUpDown->Size = System::Drawing::Size(58, 20);
			this->from_x_numericUpDown->TabIndex = 1;
			// 
			// from_y_numericUpDown
			// 
			this->from_y_numericUpDown->Location = System::Drawing::Point(64, 46);
			this->from_y_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->from_y_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, System::Int32::MinValue});
			this->from_y_numericUpDown->Name = L"from_y_numericUpDown";
			this->from_y_numericUpDown->Size = System::Drawing::Size(58, 20);
			this->from_y_numericUpDown->TabIndex = 2;
			// 
			// to_y_numericUpDown
			// 
			this->to_y_numericUpDown->Location = System::Drawing::Point(64, 98);
			this->to_y_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->to_y_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, System::Int32::MinValue});
			this->to_y_numericUpDown->Name = L"to_y_numericUpDown";
			this->to_y_numericUpDown->Size = System::Drawing::Size(58, 20);
			this->to_y_numericUpDown->TabIndex = 5;
			// 
			// to_x_numericUpDown
			// 
			this->to_x_numericUpDown->Location = System::Drawing::Point(64, 72);
			this->to_x_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->to_x_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, System::Int32::MinValue});
			this->to_x_numericUpDown->Name = L"to_x_numericUpDown";
			this->to_x_numericUpDown->Size = System::Drawing::Size(58, 20);
			this->to_x_numericUpDown->TabIndex = 4;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 74);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(20, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"To";
			// 
			// size_y_numericUpDown
			// 
			this->size_y_numericUpDown->Location = System::Drawing::Point(64, 150);
			this->size_y_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->size_y_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->size_y_numericUpDown->Name = L"size_y_numericUpDown";
			this->size_y_numericUpDown->Size = System::Drawing::Size(58, 20);
			this->size_y_numericUpDown->TabIndex = 8;
			this->size_y_numericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// size_x_numericUpDown
			// 
			this->size_x_numericUpDown->Location = System::Drawing::Point(64, 124);
			this->size_x_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {300, 0, 0, 0});
			this->size_x_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->size_x_numericUpDown->Name = L"size_x_numericUpDown";
			this->size_x_numericUpDown->Size = System::Drawing::Size(58, 20);
			this->size_x_numericUpDown->TabIndex = 7;
			this->size_x_numericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 126);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(27, 13);
			this->label2->TabIndex = 6;
			this->label2->Text = L"Size";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 200);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 9;
			this->button1->Text = L"Ok";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &terrain_copy_nodes_form::button1_Click);
			// 
			// button2
			// 
			this->button2->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button2->Location = System::Drawing::Point(93, 200);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 10;
			this->button2->Text = L"Cancel";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// status_label
			// 
			this->status_label->AutoSize = true;
			this->status_label->Location = System::Drawing::Point(12, 184);
			this->status_label->Name = L"status_label";
			this->status_label->Size = System::Drawing::Size(0, 13);
			this->status_label->TabIndex = 11;
			// 
			// terrain_copy_nodes_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->CancelButton = this->button2;
			this->ClientSize = System::Drawing::Size(209, 226);
			this->Controls->Add(this->status_label);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->size_y_numericUpDown);
			this->Controls->Add(this->size_x_numericUpDown);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->to_y_numericUpDown);
			this->Controls->Add(this->to_x_numericUpDown);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->from_y_numericUpDown);
			this->Controls->Add(this->from_x_numericUpDown);
			this->Controls->Add(this->l1);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"terrain_copy_nodes_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Hide;
			this->Text = L"terrain_copy_nodes_form";
			this->TopMost = true;
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->from_x_numericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->from_y_numericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->to_y_numericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->to_x_numericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->size_y_numericUpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->size_x_numericUpDown))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
		 public:
			 terrain_core^	m_terrain_core;
};
}
}

#endif // #ifndef TERRAIN_COPY_NODES_FORM_H_INCLUDED