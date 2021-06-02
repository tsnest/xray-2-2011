////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGULATION_TESTER_H_INCLUDED
#define TRIANGULATION_TESTER_H_INCLUDED

#include "triangulation_tester_canvas.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray
{
	namespace editor
	{

		/// <summary>
		/// Summary for triangulation_tester
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class triangulation_tester : public WeifenLuo::WinFormsUI::Docking::DockContent
		{
		public:
			triangulation_tester(void)
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
			~triangulation_tester()
			{
				if (components)
				{
					delete components;
				}
			}

		private: xray::editor::triangulation_tester_canvas^		canvas;
		private: System::Windows::Forms::Panel^					top_panel;
		private: System::Windows::Forms::Button^				open_button;
		private: System::Windows::Forms::Button^				save_button;
		private: System::Windows::Forms::ComboBox^				triangulation_type;
		private: System::Windows::Forms::CheckBox^				show_bounding_triangle;
		private: System::Windows::Forms::CheckBox^				show_errors;
		private: System::Windows::Forms::Button^				clear_vertices_button;
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
				this->canvas	= ( gcnew xray::editor::triangulation_tester_canvas ( ) );
				this->top_panel	= ( gcnew System::Windows::Forms::Panel ( ) );
				this->open_button	= ( gcnew System::Windows::Forms::Button ( ) );
				this->save_button	= ( gcnew System::Windows::Forms::Button ( ) );
				this->triangulation_type	= ( gcnew System::Windows::Forms::ComboBox ( ) );
				this->show_bounding_triangle	= ( gcnew System::Windows::Forms::CheckBox ( ) );
				this->clear_vertices_button	= ( gcnew System::Windows::Forms::Button ( ) );
				this->show_errors	= ( gcnew System::Windows::Forms::CheckBox ( ) );

				this->SuspendLayout();
				//
				// canvas
				//
				this->canvas->Name = L"canvas";
				this->canvas->Size = System::Drawing::Size( 100, 100 );
				this->canvas->TabIndex = 1;
				this->canvas->Dock = System::Windows::Forms::DockStyle::Fill;
				//
				// top_panel
				//
				this->top_panel->Name = L"top_panel";
				this->top_panel->Size = System::Drawing::Size( 100, 25 );
				this->top_panel->TabIndex = 2;
				this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
				this->top_panel->Controls->Add( this->open_button );
				this->top_panel->Controls->Add( this->save_button );
				this->top_panel->Controls->Add( this->triangulation_type );
				this->top_panel->Controls->Add( this->show_bounding_triangle );
				this->top_panel->Controls->Add( this->clear_vertices_button );
				this->top_panel->Controls->Add( this->show_errors );

				//
				// button_1
				//
				this->open_button->Name = L"button_1";
				this->open_button->Text = "Load Vertices";
				this->open_button->Size = System::Drawing::Size( 80, 24 );
				this->open_button->Location = System::Drawing::Point( 0, 0 );
				this->open_button->TabIndex = 3;
				this->open_button->Click += gcnew  EventHandler(this, &triangulation_tester::open_button_onClick );

				//
				// button_2
				//
				this->save_button->Name = L"button_2";
				this->save_button->Text = "Save Vertices";
				this->save_button->Size = System::Drawing::Size( 80, 24 );
				this->save_button->Location = System::Drawing::Point( 90, 0 );
				this->save_button->TabIndex = 4;
				this->save_button->Click += gcnew  EventHandler(this, &triangulation_tester::save_button_onClick );

				this->triangulation_type->Name = L"combo_1";
				this->triangulation_type->Size = System::Drawing::Size( 80, 24 );
				this->triangulation_type->Location = System::Drawing::Point( 180, 0 );
				this->triangulation_type->TabIndex = 5;
				this->triangulation_type->Items->Add( "Regular" );
				this->triangulation_type->Items->Add( "Constrained" );
				this->triangulation_type->SelectedIndex = 0;
				this->triangulation_type->SelectionChangeCommitted += gcnew EventHandler( this, &triangulation_tester::triangulation_type_onChange );

				this->show_bounding_triangle->Name = L"check_1";
				this->show_bounding_triangle->Text = "Show bounding triangle";
				this->show_bounding_triangle->Size = System::Drawing::Size( 150, 24 );
				this->show_bounding_triangle->Location = System::Drawing::Point( 270, 0 );
				this->show_bounding_triangle->TabIndex = 6;
				this->show_bounding_triangle->Checked = false;
				this->show_bounding_triangle->CheckedChanged += gcnew  EventHandler(this, &triangulation_tester::show_bounding_triangle_onCheckedChanged );

				this->clear_vertices_button->Name = L"button_3";
				this->clear_vertices_button->Text = "Clear Vertices";
				this->clear_vertices_button->Size = System::Drawing::Size( 80, 24 );
				this->clear_vertices_button->Location = System::Drawing::Point( 430, 0 );
				this->clear_vertices_button->TabIndex = 7;
				this->clear_vertices_button->Click += gcnew  EventHandler(this, &triangulation_tester::clear_vertices_onClick );

				this->show_errors->Name = L"check_1";
				this->show_errors->Text = "Visualize Errors";
				this->show_errors->Size = System::Drawing::Size( 150, 24 );
				this->show_errors->Location = System::Drawing::Point( 510, 0 );
				this->show_errors->TabIndex = 8;
				this->show_errors->Checked = false;
				this->show_errors->CheckedChanged += gcnew  EventHandler(this, &triangulation_tester::show_errors_onCheckedChanged );

				// 
				// triangulation_tester
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(555, 369);
				this->Controls->Add(this->canvas);
				this->Controls->Add(this->top_panel);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->HideOnClose = true;
				this->Name = L"triangulation_tester";
				this->Text = L"triangulation_tester";
				this->ResumeLayout(false);
				this->PerformLayout();

			}
	#pragma endregion
		void open_button_onClick(System::Object^, System::EventArgs^ );
		void save_button_onClick(System::Object^, System::EventArgs^ );
		void show_bounding_triangle_onCheckedChanged (System::Object^, System::EventArgs^ );
		void show_errors_onCheckedChanged (System::Object^, System::EventArgs^ );
		void clear_vertices_onClick(System::Object^, System::EventArgs^ );
		void triangulation_type_onChange(System::Object^, System::EventArgs^ );

	};

	} // namespace editor
} // namespace xray

#endif // #ifndef TRIANGULATION_TESTER_H_INCLUDED