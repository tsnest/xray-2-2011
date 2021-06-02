#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace editor {
	class property_holder;

	/// <summary>
	/// Summary for window_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class window_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		window_tab( System::String^ tab_id, xray::editor::property_holder& property_holder)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			in_constructor	( tab_id, property_holder );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~window_tab()
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
			// window_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(277, 273);
			this->Name = L"window_tab";
			this->ResumeLayout(false);

		}
#pragma endregion
private:
	void	in_constructor	( System::String^ tab_id, xray::editor::property_holder& property_holder );

private:
	xray::editor::controls::property_grid^	m_property_grid;
};

} // namespace editor
} // namespace xray