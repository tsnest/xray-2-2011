////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SELECT_LIBRARY_CLASS_FORM_H_INCLUDED
#define SELECT_LIBRARY_CLASS_FORM_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Data;

namespace xray{
namespace editor {

	ref struct lib_item_base;
	ref class tool_base;
	ref class objects_library;
	ref class library_objects_tree;
	/// <summary>
	/// Summary for select_library_class_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class select_library_class_form : public System::Windows::Forms::Form
	{
	public:
		select_library_class_form( tool_base^ tool)
		:m_tool(tool)
		{
			InitializeComponent		( );
			in_constructor			( );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~select_library_class_form()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  bottom_panel;
	protected: 
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  add_panel;


	private: System::Windows::Forms::Button^  cancel_button;
	private: System::Windows::Forms::Button^  ok_button;

	private: System::Windows::Forms::TextBox^	name_textBox;


	private: 

	protected: 



	public: 
	private: 
	public: 
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->bottom_panel = (gcnew System::Windows::Forms::Panel());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->add_panel = (gcnew System::Windows::Forms::Panel());
			this->name_textBox = (gcnew System::Windows::Forms::TextBox());
			this->bottom_panel->SuspendLayout();
			this->panel1->SuspendLayout();
			this->add_panel->SuspendLayout();
			this->SuspendLayout();
			// 
			// bottom_panel
			// 
			this->bottom_panel->Controls->Add(this->cancel_button);
			this->bottom_panel->Controls->Add(this->ok_button);
			this->bottom_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->bottom_panel->Location = System::Drawing::Point(0, 364);
			this->bottom_panel->Name = L"bottom_panel";
			this->bottom_panel->Size = System::Drawing::Size(205, 33);
			this->bottom_panel->TabIndex = 14;
			// 
			// cancel_button
			// 
			this->cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->cancel_button->Location = System::Drawing::Point(127, 4);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(75, 26);
			this->cancel_button->TabIndex = 1;
			this->cancel_button->Text = L"Cancel";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &select_library_class_form::cancel_button_Click);
			// 
			// ok_button
			// 
			this->ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->ok_button->Location = System::Drawing::Point(46, 4);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(75, 26);
			this->ok_button->TabIndex = 0;
			this->ok_button->Text = L"Ok";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &select_library_class_form::ok_button_Click);
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->add_panel);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(205, 364);
			this->panel1->TabIndex = 15;
			// 
			// add_panel
			// 
			this->add_panel->Controls->Add(this->name_textBox);
			this->add_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->add_panel->Location = System::Drawing::Point(0, 342);
			this->add_panel->Name = L"add_panel";
			this->add_panel->Size = System::Drawing::Size(205, 22);
			this->add_panel->TabIndex = 15;
			// 
			// name_textBox
			// 
			this->name_textBox->Dock = System::Windows::Forms::DockStyle::Fill;
			this->name_textBox->Location = System::Drawing::Point(0, 0);
			this->name_textBox->Name = L"name_textBox";
			this->name_textBox->Size = System::Drawing::Size(205, 20);
			this->name_textBox->TabIndex = 1;
			this->name_textBox->TextChanged += gcnew System::EventHandler(this, &select_library_class_form::name_textBox_TextChanged);
			this->name_textBox->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &select_library_class_form::select_library_class_form_KeyUp);
			// 
			// select_library_class_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(205, 397);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->bottom_panel);
			this->Name = L"select_library_class_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"select library class";
			this->TopMost = true;
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &select_library_class_form::select_library_class_form_KeyUp);
			this->bottom_panel->ResumeLayout(false);
			this->panel1->ResumeLayout(false);
			this->add_panel->ResumeLayout(false);
			this->add_panel->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
public:
	void			initialize				( System::String^ initial, bool select_existing_only, System::String^ filter );

		property System::String^	selected_path
		{
			System::String^		get	( )			{ return name_textBox->Text; }
		}

protected:
		tool_base^					m_tool;
		library_objects_tree^		m_library_objects_tree;
		bool						m_chooser_mode;

		void			selected_path_changed	( System::String^ new_path, System::String^ prev_path );
		void			in_constructor			( );
		void			ok_button_Click			( System::Object^  sender, System::EventArgs^  e );
		void			cancel_button_Click		( System::Object^  sender, System::EventArgs^  e );
		void			name_textBox_TextChanged( System::Object^  sender, System::EventArgs^  e);
		void			select_library_class_form_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
		void			selected_item_double_clicked( lib_item_base^ item );
};

} // namespace editor
} // namespace xray

#endif // #ifndef SELECT_LIBRARY_CLASS_FORM_H_INCLUDED