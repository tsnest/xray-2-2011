#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray
{
namespace model_editor {
	ref class sectors_controls; 
	/// <summary>
	/// Summary for sectors_view
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class sectors_view : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		sectors_view(void)
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~sectors_view()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  top_panel;
	private: System::Windows::Forms::Panel^  bottom_panel;
	private: System::Windows::Forms::Panel^  sectors_view_panel;
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
			this->top_panel = (gcnew System::Windows::Forms::Panel());
			this->bottom_panel = (gcnew System::Windows::Forms::Panel());
			this->sectors_view_panel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// top_panel
			// 
			this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->top_panel->Location = System::Drawing::Point(0, 0);
			this->top_panel->Name = L"top_panel";
			this->top_panel->Size = System::Drawing::Size(292, 76);
			this->top_panel->TabIndex = 0;
			// 
			// bottom_panel
			// 
			this->bottom_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->bottom_panel->Location = System::Drawing::Point(0, 445);
			this->bottom_panel->Name = L"bottom_panel";
			this->bottom_panel->Size = System::Drawing::Size(292, 109);
			this->bottom_panel->TabIndex = 1;
			// 
			// sectors_view_panel
			// 
			this->sectors_view_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->sectors_view_panel->Location = System::Drawing::Point(0, 76);
			this->sectors_view_panel->Name = L"sectors_view_panel";
			this->sectors_view_panel->Size = System::Drawing::Size(292, 369);
			this->sectors_view_panel->TabIndex = 2;
			// 
			// sectors_view
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 554);
			this->Controls->Add(this->sectors_view_panel);
			this->Controls->Add(this->bottom_panel);
			this->Controls->Add(this->top_panel);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->Name = L"sectors_view";
			this->Text = L"sectors_view";
			this->ResumeLayout(false);

		}
#pragma endregion
	public:
		void		set_property_container			( xray::editor::wpf_controls::property_container^ cont );
	private:
		void		in_constructor					();
	private:
		sectors_controls^									m_sectrors_controls;
		xray::editor::wpf_controls::property_grid::host^	m_property_grid_host;
	};
}
}