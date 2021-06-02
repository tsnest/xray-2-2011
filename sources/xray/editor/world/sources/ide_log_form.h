////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef IDE_LOG_FORM_H_INCLUDED
#define IDE_LOG_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	/// <summary>
	/// Summary for ide_log_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ide_log_form : public System::Windows::Forms::Form
	{
	public:
		ide_log_form()
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
		~ide_log_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  listBox1;
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
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->listBox1->Location = System::Drawing::Point(0, 0);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(404, 264);
			this->listBox1->TabIndex = 0;
			// 
			// ide_log_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(404, 264);
			this->Controls->Add(this->listBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->Name = L"ide_log_form";
			this->Text = L"Log view";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ide_log_form::ide_log_form_FormClosing);
			this->ResumeLayout(false);

		}
#pragma endregion
	public:
		void			error_message_out			( System::String^ );
		void			clear_error_messages		( );
	
			void		ide_log_form_FormClosing	( System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e );
	};
} // namespace editor
} // namespace xray

#endif // #ifndef IDE_LOG_FORM_H_INCLUDED