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
	/// Summary for sectors_controls
	/// </summary>
	public ref class sectors_controls : public System::Windows::Forms::UserControl
	{
	public:
		sectors_controls(void)
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
		~sectors_controls()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  chb_sectors;
	protected: 
	private: System::Windows::Forms::CheckBox^  chb_cuboids;
	private: System::Windows::Forms::CheckBox^  chb_delimited_edges;
	private: System::Windows::Forms::CheckBox^  chb_draw_all_portals;
	private: System::Windows::Forms::CheckBox^  chb_draw_hanged;
	private: System::Windows::Forms::CheckBox^  chb_solid_sectors;


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
			this->chb_sectors = (gcnew System::Windows::Forms::CheckBox());
			this->chb_cuboids = (gcnew System::Windows::Forms::CheckBox());
			this->chb_delimited_edges = (gcnew System::Windows::Forms::CheckBox());
			this->chb_draw_all_portals = (gcnew System::Windows::Forms::CheckBox());
			this->chb_draw_hanged = (gcnew System::Windows::Forms::CheckBox());
			this->chb_solid_sectors = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// chb_sectors
			// 
			this->chb_sectors->AutoSize = true;
			this->chb_sectors->Location = System::Drawing::Point(20, 63);
			this->chb_sectors->Name = L"chb_sectors";
			this->chb_sectors->Size = System::Drawing::Size(88, 17);
			this->chb_sectors->TabIndex = 5;
			this->chb_sectors->Text = L"Draw sectors";
			this->chb_sectors->UseVisualStyleBackColor = true;
			this->chb_sectors->CheckedChanged += gcnew System::EventHandler(this, &sectors_controls::chb_sectors_CheckedChanged);
			// 
			// chb_cuboids
			// 
			this->chb_cuboids->AutoSize = true;
			this->chb_cuboids->Location = System::Drawing::Point(20, 39);
			this->chb_cuboids->Name = L"chb_cuboids";
			this->chb_cuboids->Size = System::Drawing::Size(91, 17);
			this->chb_cuboids->TabIndex = 4;
			this->chb_cuboids->Text = L"Draw cuboids";
			this->chb_cuboids->UseVisualStyleBackColor = true;
			this->chb_cuboids->CheckedChanged += gcnew System::EventHandler(this, &sectors_controls::chb_cuboids_CheckedChanged);
			// 
			// chb_delimited_edges
			// 
			this->chb_delimited_edges->AutoSize = true;
			this->chb_delimited_edges->Location = System::Drawing::Point(20, 15);
			this->chb_delimited_edges->Name = L"chb_delimited_edges";
			this->chb_delimited_edges->Size = System::Drawing::Size(127, 17);
			this->chb_delimited_edges->TabIndex = 3;
			this->chb_delimited_edges->Text = L"Draw delimited edges";
			this->chb_delimited_edges->UseVisualStyleBackColor = true;
			this->chb_delimited_edges->CheckedChanged += gcnew System::EventHandler(this, &sectors_controls::chb_delimited_edges_CheckedChanged);
			// 
			// chb_draw_all_portals
			// 
			this->chb_draw_all_portals->AutoSize = true;
			this->chb_draw_all_portals->Location = System::Drawing::Point(152, 15);
			this->chb_draw_all_portals->Name = L"chb_draw_all_portals";
			this->chb_draw_all_portals->Size = System::Drawing::Size(98, 17);
			this->chb_draw_all_portals->TabIndex = 6;
			this->chb_draw_all_portals->Text = L"Draw all portals";
			this->chb_draw_all_portals->UseVisualStyleBackColor = true;
			this->chb_draw_all_portals->CheckedChanged += gcnew System::EventHandler(this, &sectors_controls::chb_draw_all_portals_CheckedChanged);
			// 
			// chb_draw_hanged
			// 
			this->chb_draw_hanged->AutoSize = true;
			this->chb_draw_hanged->Location = System::Drawing::Point(152, 39);
			this->chb_draw_hanged->Name = L"chb_draw_hanged";
			this->chb_draw_hanged->Size = System::Drawing::Size(124, 17);
			this->chb_draw_hanged->TabIndex = 7;
			this->chb_draw_hanged->Text = L"Draw hanged portals";
			this->chb_draw_hanged->UseVisualStyleBackColor = true;
			this->chb_draw_hanged->CheckedChanged += gcnew System::EventHandler(this, &sectors_controls::chb_draw_hanged_CheckedChanged);
			// 
			// chb_solid_sectors
			// 
			this->chb_solid_sectors->AutoSize = true;
			this->chb_solid_sectors->Location = System::Drawing::Point(152, 63);
			this->chb_solid_sectors->Name = L"chb_solid_sectors";
			this->chb_solid_sectors->Size = System::Drawing::Size(86, 17);
			this->chb_solid_sectors->TabIndex = 8;
			this->chb_solid_sectors->Text = L"Solid sectors";
			this->chb_solid_sectors->UseVisualStyleBackColor = true;
			this->chb_solid_sectors->CheckedChanged += gcnew System::EventHandler(this, &sectors_controls::chb_solid_sectors_CheckedChanged);
			// 
			// sectors_controls
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->chb_solid_sectors);
			this->Controls->Add(this->chb_draw_hanged);
			this->Controls->Add(this->chb_draw_all_portals);
			this->Controls->Add(this->chb_sectors);
			this->Controls->Add(this->chb_cuboids);
			this->Controls->Add(this->chb_delimited_edges);
			this->Name = L"sectors_controls";
			this->Size = System::Drawing::Size(295, 96);
			this->Load += gcnew System::EventHandler(this, &sectors_controls::sectors_controls_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		System::Void chb_delimited_edges_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void chb_cuboids_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void chb_sectors_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void sectors_controls_Load(System::Object^  sender, System::EventArgs^  e);
		System::Void chb_draw_all_portals_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void chb_draw_hanged_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void chb_solid_sectors_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
};
}
}