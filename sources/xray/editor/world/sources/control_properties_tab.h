#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {
ref class editor_control_base;
	/// <summary>
	/// Summary for control_properties_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class control_properties_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		control_properties_tab(void)
		{
			InitializeComponent	();
			in_constructor		();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~control_properties_tab()
		{
			if (components)
			{
				delete components;
			}
		}
		void 		in_constructor		();
		xray::editor::controls::property_grid^	m_property_grid;
	public:
		void		show_properties		(editor_control_base^ o);
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
			// control_properties_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(162, 348);
			this->Name = L"control_properties_tab";
			this->Text = L"control_properties_tab";
			this->ResumeLayout(false);

		}
#pragma endregion
	};
} // editor
} // xray
