#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace model_editor {

	/// <summary>
	/// Summary for bsp_tree_control
	/// </summary>
	public ref class bsp_tree_control : public System::Windows::Forms::UserControl
	{
	public:
		bsp_tree_control(void)
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
		~bsp_tree_control()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 
	private: System::Windows::Forms::CheckBox^  chb_generated_portals;
	private: System::Windows::Forms::CheckBox^  chb_outer_edges;
	private: System::Windows::Forms::TrackBar^  slimness_threshold_trackbar;
	private: System::Windows::Forms::CheckBox^  chb_portals;
	private: System::Windows::Forms::CheckBox^  chb_portal_blanks;
	private: System::Windows::Forms::CheckBox^  chb_geometry;
	private: System::Windows::Forms::CheckBox^  chb_draw_divider;
	private: System::Windows::Forms::CheckBox^  chb_double_sided;
	private: System::Windows::Forms::CheckBox^  chb_solid;
	private: System::Windows::Forms::CheckBox^  chb_aabb;
	private: System::Windows::Forms::TrackBar^  min_square_trackbar;
	private: System::Windows::Forms::Label^  lbl_slimness_threshold;
	private: System::Windows::Forms::Label^  lbl_min_square;

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
			this->chb_generated_portals = (gcnew System::Windows::Forms::CheckBox());
			this->chb_outer_edges = (gcnew System::Windows::Forms::CheckBox());
			this->slimness_threshold_trackbar = (gcnew System::Windows::Forms::TrackBar());
			this->chb_portals = (gcnew System::Windows::Forms::CheckBox());
			this->chb_portal_blanks = (gcnew System::Windows::Forms::CheckBox());
			this->chb_geometry = (gcnew System::Windows::Forms::CheckBox());
			this->chb_draw_divider = (gcnew System::Windows::Forms::CheckBox());
			this->chb_double_sided = (gcnew System::Windows::Forms::CheckBox());
			this->chb_solid = (gcnew System::Windows::Forms::CheckBox());
			this->chb_aabb = (gcnew System::Windows::Forms::CheckBox());
			this->min_square_trackbar = (gcnew System::Windows::Forms::TrackBar());
			this->lbl_slimness_threshold = (gcnew System::Windows::Forms::Label());
			this->lbl_min_square = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->slimness_threshold_trackbar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->min_square_trackbar))->BeginInit();
			this->SuspendLayout();
			// 
			// chb_generated_portals
			// 
			this->chb_generated_portals->AutoSize = true;
			this->chb_generated_portals->Location = System::Drawing::Point(14, 203);
			this->chb_generated_portals->Name = L"chb_generated_portals";
			this->chb_generated_portals->Size = System::Drawing::Size(136, 17);
			this->chb_generated_portals->TabIndex = 46;
			this->chb_generated_portals->Text = L"Draw generated portals";
			this->chb_generated_portals->UseVisualStyleBackColor = true;
			this->chb_generated_portals->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_generated_portals_CheckedChanged);
			// 
			// chb_outer_edges
			// 
			this->chb_outer_edges->AutoSize = true;
			this->chb_outer_edges->Location = System::Drawing::Point(14, 179);
			this->chb_outer_edges->Name = L"chb_outer_edges";
			this->chb_outer_edges->Size = System::Drawing::Size(87, 17);
			this->chb_outer_edges->TabIndex = 45;
			this->chb_outer_edges->Text = L"Draw circuits";
			this->chb_outer_edges->UseVisualStyleBackColor = true;
			this->chb_outer_edges->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_outer_edges_CheckedChanged);
			// 
			// slimness_threshold_trackbar
			// 
			this->slimness_threshold_trackbar->Location = System::Drawing::Point(14, 62);
			this->slimness_threshold_trackbar->Maximum = 1000;
			this->slimness_threshold_trackbar->Name = L"slimness_threshold_trackbar";
			this->slimness_threshold_trackbar->Size = System::Drawing::Size(213, 42);
			this->slimness_threshold_trackbar->TabIndex = 44;
			this->slimness_threshold_trackbar->TickFrequency = 50;
			this->slimness_threshold_trackbar->Scroll += gcnew System::EventHandler(this, &bsp_tree_control::slimness_threshold_trackbar_Scroll);
			// 
			// chb_portals
			// 
			this->chb_portals->AutoSize = true;
			this->chb_portals->Location = System::Drawing::Point(150, 182);
			this->chb_portals->Name = L"chb_portals";
			this->chb_portals->Size = System::Drawing::Size(85, 17);
			this->chb_portals->TabIndex = 43;
			this->chb_portals->Text = L"Draw portals";
			this->chb_portals->UseVisualStyleBackColor = true;
			this->chb_portals->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_portals_CheckedChanged);
			// 
			// chb_portal_blanks
			// 
			this->chb_portal_blanks->AutoSize = true;
			this->chb_portal_blanks->Location = System::Drawing::Point(150, 158);
			this->chb_portal_blanks->Name = L"chb_portal_blanks";
			this->chb_portal_blanks->Size = System::Drawing::Size(114, 17);
			this->chb_portal_blanks->TabIndex = 42;
			this->chb_portal_blanks->Text = L"Draw portal blanks";
			this->chb_portal_blanks->UseVisualStyleBackColor = true;
			this->chb_portal_blanks->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_portal_blanks_CheckedChanged);
			// 
			// chb_geometry
			// 
			this->chb_geometry->AutoSize = true;
			this->chb_geometry->Checked = true;
			this->chb_geometry->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chb_geometry->Location = System::Drawing::Point(14, 110);
			this->chb_geometry->Name = L"chb_geometry";
			this->chb_geometry->Size = System::Drawing::Size(97, 17);
			this->chb_geometry->TabIndex = 41;
			this->chb_geometry->Text = L"Draw geometry";
			this->chb_geometry->UseVisualStyleBackColor = true;
			this->chb_geometry->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_geometry_CheckedChanged);
			// 
			// chb_draw_divider
			// 
			this->chb_draw_divider->AutoSize = true;
			this->chb_draw_divider->Location = System::Drawing::Point(150, 134);
			this->chb_draw_divider->Name = L"chb_draw_divider";
			this->chb_draw_divider->Size = System::Drawing::Size(85, 17);
			this->chb_draw_divider->TabIndex = 40;
			this->chb_draw_divider->Text = L"Draw divider";
			this->chb_draw_divider->UseVisualStyleBackColor = true;
			this->chb_draw_divider->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_draw_divider_CheckedChanged);
			// 
			// chb_double_sided
			// 
			this->chb_double_sided->AutoSize = true;
			this->chb_double_sided->Location = System::Drawing::Point(13, 156);
			this->chb_double_sided->Name = L"chb_double_sided";
			this->chb_double_sided->Size = System::Drawing::Size(88, 17);
			this->chb_double_sided->TabIndex = 39;
			this->chb_double_sided->Text = L"Double sided";
			this->chb_double_sided->UseVisualStyleBackColor = true;
			this->chb_double_sided->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_double_sided_CheckedChanged);
			// 
			// chb_solid
			// 
			this->chb_solid->AutoSize = true;
			this->chb_solid->Location = System::Drawing::Point(14, 133);
			this->chb_solid->Name = L"chb_solid";
			this->chb_solid->Size = System::Drawing::Size(49, 17);
			this->chb_solid->TabIndex = 38;
			this->chb_solid->Text = L"Solid";
			this->chb_solid->UseVisualStyleBackColor = true;
			this->chb_solid->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_solid_CheckedChanged);
			// 
			// chb_aabb
			// 
			this->chb_aabb->AutoSize = true;
			this->chb_aabb->Checked = true;
			this->chb_aabb->CheckState = System::Windows::Forms::CheckState::Checked;
			this->chb_aabb->Location = System::Drawing::Point(150, 110);
			this->chb_aabb->Name = L"chb_aabb";
			this->chb_aabb->Size = System::Drawing::Size(118, 17);
			this->chb_aabb->TabIndex = 37;
			this->chb_aabb->Text = L"Draw bounding box";
			this->chb_aabb->UseVisualStyleBackColor = true;
			this->chb_aabb->CheckedChanged += gcnew System::EventHandler(this, &bsp_tree_control::chb_aabb_CheckedChanged);
			// 
			// min_square_trackbar
			// 
			this->min_square_trackbar->Location = System::Drawing::Point(13, 13);
			this->min_square_trackbar->Maximum = 1000;
			this->min_square_trackbar->Name = L"min_square_trackbar";
			this->min_square_trackbar->Size = System::Drawing::Size(214, 42);
			this->min_square_trackbar->TabIndex = 36;
			this->min_square_trackbar->TickFrequency = 50;
			this->min_square_trackbar->Scroll += gcnew System::EventHandler(this, &bsp_tree_control::min_square_trackbar_Scroll);
			// 
			// lbl_slimness_threshold
			// 
			this->lbl_slimness_threshold->AutoSize = true;
			this->lbl_slimness_threshold->Location = System::Drawing::Point(233, 62);
			this->lbl_slimness_threshold->Name = L"lbl_slimness_threshold";
			this->lbl_slimness_threshold->Size = System::Drawing::Size(13, 13);
			this->lbl_slimness_threshold->TabIndex = 49;
			this->lbl_slimness_threshold->Text = L"0";
			// 
			// lbl_min_square
			// 
			this->lbl_min_square->AutoSize = true;
			this->lbl_min_square->Location = System::Drawing::Point(233, 22);
			this->lbl_min_square->Name = L"lbl_min_square";
			this->lbl_min_square->Size = System::Drawing::Size(13, 13);
			this->lbl_min_square->TabIndex = 48;
			this->lbl_min_square->Text = L"0";
			// 
			// bsp_tree_control
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->lbl_slimness_threshold);
			this->Controls->Add(this->lbl_min_square);
			this->Controls->Add(this->chb_generated_portals);
			this->Controls->Add(this->chb_outer_edges);
			this->Controls->Add(this->slimness_threshold_trackbar);
			this->Controls->Add(this->chb_portals);
			this->Controls->Add(this->chb_portal_blanks);
			this->Controls->Add(this->chb_geometry);
			this->Controls->Add(this->chb_draw_divider);
			this->Controls->Add(this->chb_double_sided);
			this->Controls->Add(this->chb_solid);
			this->Controls->Add(this->chb_aabb);
			this->Controls->Add(this->min_square_trackbar);
			this->Name = L"bsp_tree_control";
			this->Size = System::Drawing::Size(272, 232);
			this->Load += gcnew System::EventHandler(this, &bsp_tree_control::bsp_tree_control_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->slimness_threshold_trackbar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->min_square_trackbar))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void chb_generated_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_outer_edges_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_double_sided_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_solid_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_geometry_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_portal_blanks_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_draw_divider_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void chb_aabb_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		private: System::Void min_square_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void slimness_threshold_trackbar_Scroll(System::Object^  sender, System::EventArgs^  e);
		private: System::Void bsp_tree_control_Load(System::Object^  sender, System::EventArgs^  e);
};
}
}
