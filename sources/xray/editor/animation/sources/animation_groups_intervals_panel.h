////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_INTERVALS_PANEL_H_INCLUDED
#define ANIMATION_GROUPS_INTERVALS_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace xray::editor::wpf_controls::animation_setup;
using namespace xray::editor::wpf_controls::hypergraph;

namespace xray {
namespace animation_editor {
	
	ref class animation_groups_document;
	ref class animation_node_interval;
	ref class animation_node_clip_instance;

	public ref class animation_groups_intervals_panel: public System::Windows::Forms::Form
	{
	public:
		typedef Generic::List<animation_node_interval^> intervals_list;

	public:
		animation_groups_intervals_panel(animation_groups_document^ d);
		~animation_groups_intervals_panel();
		void				show				(node^ n);
		void				animations_count	(u32 val);
		void				animation_loaded	(animation_node_clip_instance^ new_clip);

	private: 
		System::ComponentModel::Container^	components;
		ProgressBar^						progressBar1;
		Label^								label1;
		Panel^								panel2;
		Panel^								panel3;
		CheckedListBox^						checkedListBox1;
		Panel^								panel5;
		Panel^								panel7;
		Panel^								panel6;
		Panel^								panel4;
		Label^								label2;
		Button^								button2;
		Button^								button1;
		intervals_list^						m_intervals;
		node^								m_node;
		animation_groups_document^			m_document;

	private:
		void				elements_enabled	(bool val);
		void				on_form_closing		(System::Object^, System::Windows::Forms::FormClosingEventArgs^ e);
		void				on_ok_clicked		(System::Object^, System::EventArgs^);
		void				on_cancel_clicked	(System::Object^, System::EventArgs^);
		void				InitializeComponent	()
		{
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->panel6 = (gcnew System::Windows::Forms::Panel());
			this->panel7 = (gcnew System::Windows::Forms::Panel());
			this->panel5 = (gcnew System::Windows::Forms::Panel());
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			this->panel4->SuspendLayout();
			this->panel6->SuspendLayout();
			this->SuspendLayout();

			// progressBar1
			this->progressBar1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->progressBar1->Location = System::Drawing::Point(0, 16);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(455, 14);
			this->progressBar1->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar1->TabIndex = 1;
			this->progressBar1->Value = 100;

			// label1
			this->label1->AutoSize = true;
			this->label1->Dock = System::Windows::Forms::DockStyle::Top;
			this->label1->Location = System::Drawing::Point(0, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(170, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Please wait. Processing intervals...";

			// panel2
			this->panel2->Controls->Add(this->progressBar1);
			this->panel2->Controls->Add(this->label1);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel2->Location = System::Drawing::Point(0, 0);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(455, 30);
			this->panel2->TabIndex = 3;

			// panel3
			this->panel3->Controls->Add(this->checkedListBox1);
			this->panel3->Controls->Add(this->panel5);
			this->panel3->Controls->Add(this->panel7);
			this->panel3->Controls->Add(this->panel6);
			this->panel3->Controls->Add(this->panel4);
			this->panel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel3->Location = System::Drawing::Point(0, 30);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(455, 192);
			this->panel3->TabIndex = 4;

			// panel4
			this->panel4->Controls->Add(this->label2);
			this->panel4->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel4->Location = System::Drawing::Point(0, 0);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(455, 20);
			this->panel4->TabIndex = 1;

			// panel6
			this->panel6->Controls->Add(this->button2);
			this->panel6->Controls->Add(this->button1);
			this->panel6->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->panel6->Location = System::Drawing::Point(0, 169);
			this->panel6->Name = L"panel6";
			this->panel6->Size = System::Drawing::Size(455, 23);
			this->panel6->TabIndex = 3;

			// panel7
			this->panel7->Dock = System::Windows::Forms::DockStyle::Right;
			this->panel7->Location = System::Drawing::Point(435, 20);
			this->panel7->Name = L"panel7";
			this->panel7->Size = System::Drawing::Size(20, 149);
			this->panel7->TabIndex = 4;

			// panel5
			this->panel5->Dock = System::Windows::Forms::DockStyle::Left;
			this->panel5->Location = System::Drawing::Point(0, 20);
			this->panel5->Name = L"panel5";
			this->panel5->Size = System::Drawing::Size(20, 149);
			this->panel5->TabIndex = 5;

			// checkedListBox1
			this->checkedListBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Location = System::Drawing::Point(20, 20);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(415, 139);
			this->checkedListBox1->TabIndex = 6;

			// label2
			this->label2->AutoSize = true;
			this->label2->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->label2->Location = System::Drawing::Point(0, 7);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(144, 13);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Detected animation intervals:";

			// button1
			this->button1->Dock = System::Windows::Forms::DockStyle::Right;
			this->button1->Location = System::Drawing::Point(380, 0);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Cancel";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &animation_groups_intervals_panel::on_cancel_clicked);

			// button2
			this->button2->Dock = System::Windows::Forms::DockStyle::Right;
			this->button2->Location = System::Drawing::Point(305, 0);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"OK";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &animation_groups_intervals_panel::on_ok_clicked);

			// animation_groups_intervals_panel
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(455, 222);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->panel2);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &animation_groups_intervals_panel::on_form_closing);
			this->Name = L"animation_groups_intervals_panel";
			this->Text = L"Intervals panel";
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel3->ResumeLayout(false);
			this->panel4->ResumeLayout(false);
			this->panel4->PerformLayout();
			this->panel6->ResumeLayout(false);
			this->ResumeLayout(false);
		}
	};
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_INTERVALS_PANEL_H_INCLUDED

