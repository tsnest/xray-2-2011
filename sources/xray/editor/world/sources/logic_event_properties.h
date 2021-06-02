////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_PROPERTIES_H_INCLUDED
#define LOGIC_EVENT_PROPERTIES_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
	namespace editor {

	/// <summary>
	/// Summary for logic_event_properties
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class logic_event_properties : public System::Windows::Forms::Form
	{
	public:
		logic_event_properties(void)
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
		~logic_event_properties()
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
		wpf_controls::property_grid::host^ m_property_grid_host;

	public:
		property wpf_controls::property_grid::host^ property_grid_host
		{
			wpf_controls::property_grid::host^		get( ){ return m_property_grid_host; }
		}

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"logic_event_properties";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;

			m_property_grid_host = gcnew wpf_controls::property_grid::host();
			m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;

			this->Controls->Add( m_property_grid_host );
        }
#pragma endregion
	};
}// namespace editor
}//namespace xray

#endif // #ifndef LOGIC_EVENT_PROPERTIES_H_INCLUDED
