////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TOOLBAR_PANEL_BASE_H_INCLUDED
#define TOOLBAR_PANEL_BASE_H_INCLUDED

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
		namespace controls
		{
			ref class document_editor_base;

			public ref class toolbar_panel_base : public WeifenLuo::WinFormsUI::Docking::DockContent
			{
			public:
				toolbar_panel_base(document_editor_base^ ed, String^ panel_name):m_editor(ed)
				{
					InitializeComponent();
					this->Text = panel_name;
				}
				toolbar_panel_base(document_editor_base^ ed):m_editor(ed)
				{
					InitializeComponent();
				}

			private:
				toolbar_panel_base(){};

			protected:
				~toolbar_panel_base()
				{
					if(components)
						delete components;
				}

		#pragma region Windows Form Designer generated code
				void InitializeComponent(void)
				{
					this->SuspendLayout();
					// 
					// toolbar_panel_base
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->BackColor = System::Drawing::SystemColors::Control;
					this->ClientSize = System::Drawing::Size(292, 273);
					this->DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(((((WeifenLuo::WinFormsUI::Docking::DockAreas::Float | WeifenLuo::WinFormsUI::Docking::DockAreas::DockLeft) 
						| WeifenLuo::WinFormsUI::Docking::DockAreas::DockRight) 
						| WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop) 
						| WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom));
					this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
						static_cast<System::Byte>(204)));
					this->Name = L"toolbar_panel_base";
					this->Padding = System::Windows::Forms::Padding(3);
					this->Text = L"Toolbar";
					this->ResumeLayout(false);

				}
		#pragma endregion

			protected: 
				document_editor_base^			m_editor;
				System::ComponentModel::Container^	components;
			};// class toolbar_panel_base
		}//namespace controls
	}//namespace editor 
}//namespace xray

#endif // #ifndef TOOLBAR_PANEL_BASE_H_INCLUDED