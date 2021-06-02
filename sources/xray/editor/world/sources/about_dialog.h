////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ABOUT_DIALOG_H_INCLUDED
#define ABOUT_DIALOG_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

	/// <summary>
	/// Summary for about_dialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class about_dialog : public System::Windows::Forms::Form
	{
	public:
		about_dialog(void)
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
		~about_dialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  button_ok;
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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button_ok = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(31, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(184, 39);
			this->label1->TabIndex = 0;
			this->label1->Text = L"       X-Ray Engine v2.0 Editor\r\n\r\nCopyright(C) GSC Game World - 2010\r\n";
			// 
			// button_ok
			// 
			this->button_ok->Location = System::Drawing::Point(87, 98);
			this->button_ok->Name = L"button_ok";
			this->button_ok->Size = System::Drawing::Size(75, 23);
			this->button_ok->TabIndex = 1;
			this->button_ok->Text = L"Ok";
			this->button_ok->UseVisualStyleBackColor = true;
			this->button_ok->Click += gcnew System::EventHandler(this, &about_dialog::button_ok_Click);
			// 
			// about_dialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(250, 133);
			this->Controls->Add(this->button_ok);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"about_dialog";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"About X-Ray Editor";
			this->Load += gcnew System::EventHandler(this, &about_dialog::about_dialog_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: 
		System::Void button_ok_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void about_dialog_Load(System::Object^  sender, System::EventArgs^  e);
	};

} // namespace editor 
} // namespace xray

#endif // #ifndef ABOUT_DIALOG_H_INCLUDED