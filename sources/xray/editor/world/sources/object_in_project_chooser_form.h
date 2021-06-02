////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_IN_PROJECT_CHOOSER_FORM_H_INCLUDED
#define OBJECT_IN_PROJECT_CHOOSER_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	ref class project;

	/// <summary>
	/// Summary for object_in_project_chooser_form
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class object_in_project_chooser_form : public System::Windows::Forms::Form
	{
	public:
		object_in_project_chooser_form(void)
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
		~object_in_project_chooser_form()
		{
			if (components)
			{
				delete components;
			}
		}
	public: xray::editor::controls::tree_view^  treeView;
	private: System::Windows::Forms::Button^  ok_button;
	private: System::Windows::Forms::Button^  cancel_button;
	public: 

	public: 

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
			this->treeView = (gcnew xray::editor::controls::tree_view());
			this->ok_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// treeView
			// 
			this->treeView->AllowDrop = true;
			this->treeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView->auto_expand_on_filter = false;
			this->treeView->BackColor = System::Drawing::SystemColors::Window;
			this->treeView->filter_visible = false;
			this->treeView->FullRowSelect = true;
			this->treeView->Indent = 27;
			this->treeView->is_multiselect = true;
			this->treeView->is_selectable_groups = true;
			this->treeView->is_selection_or_groups_or_items = false;
			this->treeView->ItemHeight = 20;
			this->treeView->keyboard_search_enabled = false;
			this->treeView->LabelEdit = true;
			this->treeView->Location = System::Drawing::Point(-2, -2);
			this->treeView->Name = L"treeView";
			this->treeView->PathSeparator = L"/";
			this->treeView->Size = System::Drawing::Size(228, 297);
			this->treeView->Sorted = true;
			this->treeView->source = nullptr;
			this->treeView->TabIndex = 4;
			this->treeView->selected_items_changed += gcnew System::EventHandler<controls::tree_view_selection_event_args^>(this, &object_in_project_chooser_form::treeView_selected_items_changed);
			// 
			// ok_button
			// 
			this->ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->ok_button->Location = System::Drawing::Point(71, 298);
			this->ok_button->Name = L"ok_button";
			this->ok_button->Size = System::Drawing::Size(75, 23);
			this->ok_button->TabIndex = 5;
			this->ok_button->Text = L"OK";
			this->ok_button->UseVisualStyleBackColor = true;
			this->ok_button->Click += gcnew System::EventHandler(this, &object_in_project_chooser_form::ok_button_Click);
			// 
			// cancel_button
			// 
			this->cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancel_button->Location = System::Drawing::Point(152, 298);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(75, 23);
			this->cancel_button->TabIndex = 6;
			this->cancel_button->Text = L"Cancel";
			this->cancel_button->UseVisualStyleBackColor = true;
			this->cancel_button->Click += gcnew System::EventHandler(this, &object_in_project_chooser_form::cancel_button_Click);
			// 
			// object_in_project_chooser_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(230, 322);
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->ok_button);
			this->Controls->Add(this->treeView);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->Name = L"object_in_project_chooser_form";
			this->Text = L"object_in_project_chooser_form";
			this->ResumeLayout(false);

		}
#pragma endregion
	public:
		property System::String^	selected_path;
		void						initialize						( project^ p, System::String^ current, System::String^ filter );
		private: System::Void		treeView_selected_items_changed	( System::Object^  sender, controls::tree_view_selection_event_args^ e );
		private: System::Void		ok_button_Click					( System::Object^  sender, System::EventArgs^ e );
		private: System::Void		cancel_button_Click				( System::Object^  sender, System::EventArgs^ e );
};

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_IN_PROJECT_CHOOSER_FORM_H_INCLUDED