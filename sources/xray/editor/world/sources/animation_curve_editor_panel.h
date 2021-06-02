////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CURVE_EDITOR_PANEL_H_INCLUDED
#define ANIMATION_CURVE_EDITOR_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

typedef xray::editor::wpf_controls::animation_curve_editor animation_curve_editor_control;

namespace xray
{
	namespace editor 
	{
		ref class level_editor;

		public ref class animation_curve_editor_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
		{
		public:
			animation_curve_editor_panel( level_editor^ editor ):
				m_editor( editor )
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
			~animation_curve_editor_panel( )
			{
				if ( components )
				{
					delete components;
				}
			}

		private: System::Windows::Forms::Integration::ElementHost^  m_host;
		private: xray::editor::wpf_controls::animation_curve_editor^  animation_curve_editor_control1;
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
			void InitializeComponent( void )
			{
				this->m_host = (gcnew System::Windows::Forms::Integration::ElementHost());
				this->animation_curve_editor_control1 = (gcnew xray::editor::wpf_controls::animation_curve_editor( ) );
				this->SuspendLayout();
				// 
				// m_host
				// 
				this->m_host->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_host->Location = System::Drawing::Point(0, 0);
				this->m_host->Name = L"m_host";
				this->m_host->Size = System::Drawing::Size(687, 247);
				this->m_host->TabIndex = 1;
				this->m_host->Child = this->animation_curve_editor_control1;
				// 
				// animation_curve_editor_panel
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(687, 247);
				this->Controls->Add(this->m_host);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->HideOnClose = true;
				this->Name = L"animation_curve_editor_panel";
				this->Text = L"AnimationCurveEditor";
				this->ResumeLayout(false);

			}
		#pragma endregion

			level_editor^								m_editor;

		public:
			 property animation_curve_editor_control^			editor
			 {
				animation_curve_editor_control^		get( )
				{
					return animation_curve_editor_control1;
				}
			 }

		};
	} // namespace editor
} // namespace xray

#endif // #ifndef ANIMATION_CURVE_EDITOR_PANEL_H_INCLUDED