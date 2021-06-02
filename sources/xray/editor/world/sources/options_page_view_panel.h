////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OPTIONS_PAGE_VIEW_PANEL_H_INCLUDED
#define OPTIONS_PAGE_VIEW_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {


	/// <summary>
	/// Summary for options_page_view_panel
	/// </summary>
	public ref class options_page_view_panel : public System::Windows::Forms::UserControl, public editor_base::options_page
	{
	public:
		options_page_view_panel( editor_base::scene_view_panel^ view_window )
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_view_window = view_window;
		}
		virtual bool	accept_changes		( );
		virtual void	cancel_changes		( );
		virtual void	activate_page		( );
		virtual void	deactivate_page		( );
		virtual bool	changed				( );
		virtual System::Windows::Forms::Control^	get_control	( ) { return this; }


	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~options_page_view_panel()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label2;
	protected: 
	private: System::Windows::Forms::TrackBar^  gizmo_size_track;
	private: System::Windows::Forms::NumericUpDown^  m_far_plane_box;
	private: System::Windows::Forms::NumericUpDown^  m_near_plane_box;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TrackBar^  mouse_sens_track;
	private: System::Windows::Forms::NumericUpDown^  grid_step_UpDown;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::NumericUpDown^  grid_size_numericUpDown;
	private: System::Windows::Forms::Label^  label4;

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
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->gizmo_size_track = (gcnew System::Windows::Forms::TrackBar());
			this->m_far_plane_box = (gcnew System::Windows::Forms::NumericUpDown());
			this->m_near_plane_box = (gcnew System::Windows::Forms::NumericUpDown());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->mouse_sens_track = (gcnew System::Windows::Forms::TrackBar());
			this->grid_step_UpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->grid_size_numericUpDown = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gizmo_size_track))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_far_plane_box))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_near_plane_box))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->mouse_sens_track))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->grid_step_UpDown))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->grid_size_numericUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(8, 65);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(57, 13);
			this->label2->TabIndex = 28;
			this->label2->Text = L"Gizmo size";
			// 
			// gizmo_size_track
			// 
			this->gizmo_size_track->LargeChange = 20;
			this->gizmo_size_track->Location = System::Drawing::Point(105, 65);
			this->gizmo_size_track->Maximum = 100;
			this->gizmo_size_track->Name = L"gizmo_size_track";
			this->gizmo_size_track->Size = System::Drawing::Size(128, 45);
			this->gizmo_size_track->SmallChange = 10;
			this->gizmo_size_track->TabIndex = 27;
			this->gizmo_size_track->TickFrequency = 10;
			this->gizmo_size_track->Value = 50;
			this->gizmo_size_track->ValueChanged += gcnew System::EventHandler(this, &options_page_view_panel::gizmo_size_track_ValueChanged);
			// 
			// m_far_plane_box
			// 
			this->m_far_plane_box->DecimalPlaces = 1;
			this->m_far_plane_box->Location = System::Drawing::Point(111, 36);
			this->m_far_plane_box->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->m_far_plane_box->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {5, 0, 0, 0});
			this->m_far_plane_box->Name = L"m_far_plane_box";
			this->m_far_plane_box->Size = System::Drawing::Size(120, 20);
			this->m_far_plane_box->TabIndex = 32;
			this->m_far_plane_box->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {3005, 0, 0, 65536});
			// 
			// m_near_plane_box
			// 
			this->m_near_plane_box->DecimalPlaces = 1;
			this->m_near_plane_box->Location = System::Drawing::Point(111, 9);
			this->m_near_plane_box->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->m_near_plane_box->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->m_near_plane_box->Name = L"m_near_plane_box";
			this->m_near_plane_box->Size = System::Drawing::Size(120, 20);
			this->m_near_plane_box->TabIndex = 31;
			this->m_near_plane_box->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {4, 0, 0, 0});
			this->m_near_plane_box->ValueChanged += gcnew System::EventHandler(this, &options_page_view_panel::m_near_plane_box_ValueChanged);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(6, 36);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(51, 13);
			this->label7->TabIndex = 30;
			this->label7->Text = L"Far plane";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 9);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(59, 13);
			this->label6->TabIndex = 29;
			this->label6->Text = L"Near plane";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 97);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(101, 13);
			this->label1->TabIndex = 34;
			this->label1->Text = L"Camera Z sensitivity";
			// 
			// mouse_sens_track
			// 
			this->mouse_sens_track->LargeChange = 20;
			this->mouse_sens_track->Location = System::Drawing::Point(103, 97);
			this->mouse_sens_track->Maximum = 100;
			this->mouse_sens_track->Minimum = 5;
			this->mouse_sens_track->Name = L"mouse_sens_track";
			this->mouse_sens_track->Size = System::Drawing::Size(128, 45);
			this->mouse_sens_track->SmallChange = 10;
			this->mouse_sens_track->TabIndex = 33;
			this->mouse_sens_track->TickFrequency = 10;
			this->mouse_sens_track->Value = 50;
			this->mouse_sens_track->ValueChanged += gcnew System::EventHandler(this, &options_page_view_panel::mouse_sens_track_ValueChanged);
			// 
			// grid_step_UpDown
			// 
			this->grid_step_UpDown->DecimalPlaces = 2;
			this->grid_step_UpDown->Location = System::Drawing::Point(111, 135);
			this->grid_step_UpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->grid_step_UpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->grid_step_UpDown->Name = L"grid_step_UpDown";
			this->grid_step_UpDown->Size = System::Drawing::Size(120, 20);
			this->grid_step_UpDown->TabIndex = 36;
			this->grid_step_UpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 65536});
			this->grid_step_UpDown->ValueChanged += gcnew System::EventHandler(this, &options_page_view_panel::grid_step_UpDown_ValueChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(6, 135);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(66, 13);
			this->label3->TabIndex = 35;
			this->label3->Text = L"Grid step (m)";
			// 
			// grid_size_numericUpDown
			// 
			this->grid_size_numericUpDown->Location = System::Drawing::Point(111, 162);
			this->grid_size_numericUpDown->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {500, 0, 0, 0});
			this->grid_size_numericUpDown->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->grid_size_numericUpDown->Name = L"grid_size_numericUpDown";
			this->grid_size_numericUpDown->Size = System::Drawing::Size(120, 20);
			this->grid_size_numericUpDown->TabIndex = 38;
			this->grid_size_numericUpDown->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->grid_size_numericUpDown->ValueChanged += gcnew System::EventHandler(this, &options_page_view_panel::grid_size_numericUpDown_ValueChanged);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(6, 162);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(47, 13);
			this->label4->TabIndex = 37;
			this->label4->Text = L"Grid size";
			// 
			// options_page_view_panel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->grid_size_numericUpDown);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->grid_step_UpDown);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->mouse_sens_track);
			this->Controls->Add(this->m_far_plane_box);
			this->Controls->Add(this->m_near_plane_box);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->gizmo_size_track);
			this->Name = L"options_page_view_panel";
			this->Size = System::Drawing::Size(236, 283);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gizmo_size_track))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_far_plane_box))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_near_plane_box))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->mouse_sens_track))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->grid_step_UpDown))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->grid_size_numericUpDown))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private:
		System::Collections::Hashtable		m_backup_values;
		editor_base::scene_view_panel^		m_view_window;
	private: System::Void m_near_plane_box_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void mouse_sens_track_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void gizmo_size_track_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void grid_step_UpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void grid_size_numericUpDown_ValueChanged(System::Object^  sender, System::EventArgs^  e);
};
}
}

#endif // #ifndef OPTIONS_PAGE_VIEW_PANEL_H_INCLUDED