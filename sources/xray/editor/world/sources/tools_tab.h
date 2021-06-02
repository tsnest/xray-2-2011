////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TOOLS_TAB_H_INCLUDED
#define TOOLS_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace editor {

	/// <summary>
	/// Summary for tools_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class tools_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		tools_tab(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			m_prev_active_tab=0;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~tools_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^  tab_control;

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
			this->tab_control = (gcnew System::Windows::Forms::TabControl());
			this->SuspendLayout();
			// 
			// tab_control
			// 
			this->tab_control->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tab_control->Location = System::Drawing::Point(0, 0);
			this->tab_control->Multiline = true;
			this->tab_control->Name = L"tab_control";
			this->tab_control->SelectedIndex = 0;
			this->tab_control->Size = System::Drawing::Size(284, 467);
			this->tab_control->TabIndex = 0;
			this->tab_control->SelectedIndexChanged += gcnew System::EventHandler(this, &tools_tab::tab_control_SelectedIndexChanged);
			// 
			// tools_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 467);
			this->Controls->Add(this->tab_control);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"tools_tab";
			this->TabText = L"tools_tab";
			this->Text = L"tools_tab";
			this->ResumeLayout(false);

		}
#pragma endregion
	public:
	void		add_tab			( pcstr name, System::Windows::Forms::UserControl^ tab);
	void		activate_tab	( int idx );
	protected:
	int			m_prev_active_tab;
	void		tab_control_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	};
}
}

#endif // #ifndef TOOLS_TAB_H_INCLUDED