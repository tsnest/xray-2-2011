////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OPTIONS_PAGE_GENERAL_H_INCLUDED
#define OPTIONS_PAGE_GENERAL_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {	
namespace editor {

	class editor_world;

	/// <summary>
	/// Summary for options_page_general
	/// </summary>
	public ref class options_page_general : public System::Windows::Forms::UserControl, public editor_base::options_page
	{
	public:
		options_page_general	( editor_world& world ):
		  m_editor_world		( world )
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		virtual bool	accept_changes		( );
		virtual void	cancel_changes		( );
		virtual void	activate_page		( );
		virtual void	deactivate_page		( );
		virtual bool	changed				( );

		virtual System::Windows::Forms::Control^	get_control	( ) { return this; }

	protected:

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~options_page_general()
		{
			if (components)
			{
				delete components;
			}
		}







	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::CheckBox^  draw_collision_check_box;




	private: System::Windows::Forms::NumericUpDown^  m_focus_distance_box;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::ColorDialog^  colorDialog1;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Button^  button1;

	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;



	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::NumericUpDown^  cells_depth_numericUpDown;









	protected: 

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
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->draw_collision_check_box = (gcnew System::Windows::Forms::CheckBox());
			this->m_focus_distance_box = (gcnew System::Windows::Forms::NumericUpDown());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->colorDialog1 = (gcnew System::Windows::Forms::ColorDialog());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->cells_depth_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_focus_distance_box))->BeginInit();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->cells_depth_numericUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(72, 13);
			this->label5->TabIndex = 9;
			this->label5->Text = L"Draw collision";
			// 
			// draw_collision_check_box
			// 
			this->draw_collision_check_box->AutoSize = true;
			this->draw_collision_check_box->Location = System::Drawing::Point(108, -1);
			this->draw_collision_check_box->Name = L"draw_collision_check_box";
			this->draw_collision_check_box->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->draw_collision_check_box->Size = System::Drawing::Size(15, 14);
			this->draw_collision_check_box->TabIndex = 8;
			this->draw_collision_check_box->UseVisualStyleBackColor = true;
			this->draw_collision_check_box->CheckedChanged += gcnew System::EventHandler(this, &options_page_general::draw_collision_check_box_CheckedChanged);
			// 
			// m_focus_distance_box
			// 
			this->m_focus_distance_box->DecimalPlaces = 1;
			this->m_focus_distance_box->Location = System::Drawing::Point(108, 29);
			this->m_focus_distance_box->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->m_focus_distance_box->Name = L"m_focus_distance_box";
			this->m_focus_distance_box->Size = System::Drawing::Size(120, 20);
			this->m_focus_distance_box->TabIndex = 17;
			this->m_focus_distance_box->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {100, 0, 0, 0});
			this->m_focus_distance_box->ValueChanged += gcnew System::EventHandler(this, &options_page_general::m_focus_distance_box_ValueChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(3, 29);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(79, 13);
			this->label8->TabIndex = 16;
			this->label8->Text = L"Focus distance";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->numericUpDown1);
			this->groupBox1->Controls->Add(this->panel1);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->button1);
			this->groupBox1->Location = System::Drawing::Point(6, 201);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(222, 74);
			this->groupBox1->TabIndex = 20;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Selection";
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->DecimalPlaces = 1;
			this->numericUpDown1->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDown1->Location = System::Drawing::Point(110, 14);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, 0});
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(105, 20);
			this->numericUpDown1->TabIndex = 24;
			this->numericUpDown1->ValueChanged += gcnew System::EventHandler(this, &options_page_general::numericUpDown1_ValueChanged);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(30)), static_cast<System::Int32>(static_cast<System::Byte>(10)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->panel1->Location = System::Drawing::Point(11, 43);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(91, 23);
			this->panel1->TabIndex = 23;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(6, 22);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(97, 13);
			this->label9->TabIndex = 22;
			this->label9->Text = L"Selection blink rate";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(109, 43);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(106, 23);
			this->button1->TabIndex = 21;
			this->button1->Text = L"Set selection color";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &options_page_general::button1_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(3, 60);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(85, 13);
			this->label3->TabIndex = 23;
			this->label3->Text = L"Cells draw depth";
			// 
			// cells_depth_numericUpDown
			// 
			this->cells_depth_numericUpDown->Location = System::Drawing::Point(108, 58);
			this->cells_depth_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, 0});
			this->cells_depth_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {2, 0, 0, 0});
			this->cells_depth_numericUpDown->Name = L"cells_depth_numericUpDown";
			this->cells_depth_numericUpDown->Size = System::Drawing::Size(120, 20);
			this->cells_depth_numericUpDown->TabIndex = 24;
			this->cells_depth_numericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, 0});
			this->cells_depth_numericUpDown->ValueChanged += gcnew System::EventHandler(this, &options_page_general::cells_depth_numericUpDown_ValueChanged);
			// 
			// options_page_general
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->cells_depth_numericUpDown);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->m_focus_distance_box);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->draw_collision_check_box);
			this->Name = L"options_page_general";
			this->Size = System::Drawing::Size(236, 283);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_focus_distance_box))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->cells_depth_numericUpDown))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Collections::Hashtable		m_backup_values;
		editor_world&						m_editor_world;
	private: System::Void draw_collision_check_box_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void m_focus_distance_box_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void numericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void cells_depth_numericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
};

} //	namespace editor
} //	namespace xray

#endif // #ifndef OPTIONS_PAGE_GENERAL_H_INCLUDED