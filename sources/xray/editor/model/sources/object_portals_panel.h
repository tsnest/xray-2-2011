////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PORTALS_PANEL_H_INCLUDED
#define OBJECT_PORTALS_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace model_editor {
	ref class edit_object_mesh;
	/// <summary>
	/// Summary for object_portals_panel
	/// </summary>
	ref class object_portals_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		object_portals_panel( edit_object_mesh^ parent )
			:m_edit_object( parent )
		{
			in_constructor( );
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~object_portals_panel()
		{
			if (components)
			{
				delete components;
			}
		}

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
			this->SuspendLayout();
			// 
			// object_portals_panel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(269, 294);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"object_portals_panel";
			this->Text = L"Portals";
			this->ResumeLayout(false);

		}
#pragma endregion
		public:
			void		set_property_container			( xray::editor::wpf_controls::property_container^ cont );
		private:
			void		in_constructor					( );
			void		property_grid_host_key_press	(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
			xray::editor::wpf_controls::property_grid::host^	m_property_grid_host;
			edit_object_mesh^									m_edit_object;
};

} // namespace modeleditor
} // namespase xray

#endif // #ifndef OBJECT_PORTALS_PANEL_H_INCLUDED