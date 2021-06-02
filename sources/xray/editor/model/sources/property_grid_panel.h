////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_GRID_PANEL_H_INCLUDED
#define PROPERTY_GRID_PANEL_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace model_editor {
	/// <summary>
	/// Summary for object_surfaces_panel
	/// </summary>
	ref class property_grid_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		property_grid_panel( )
		{
			InitializeComponent	( );
			in_constructor		( );
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~property_grid_panel()
		{
			if (components)
			{
				delete components;
			}
		}

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
			this->SuspendLayout();
			// 
			// object_surfaces_panel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(269, 294);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"object_surfaces_panel";
			this->Text = L"Surfaces";
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
		void						in_constructor			( );

		xray::editor::wpf_controls::property_grid::host^ m_property_grid_host;
	public:
		void			set_property_container	( xray::editor::wpf_controls::property_container^ cont );
		void			refresh_properties		( );
	};

	ref class collision_property_grid_panel : public property_grid_panel
	{};
} // namespace model_editor
} // namespace xray

#endif // #ifndef PROPERTY_GRID_PANEL_H_INCLUDED