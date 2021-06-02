#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace editor {
ref class tool_compound;
ref class object_compound;
	/// <summary>
	/// Summary for compound_object_tab
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	enum enum_current_mode{mode_new, mode_edit_library/*, mode_edit_existing*/, mode_none};
	public ref class compound_object_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		compound_object_tab(tool_compound^ t):m_tool(t),m_current_object(nullptr),m_mode(mode_none)
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~compound_object_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	private: tool_compound^		m_tool;
			xray::editor::controls::property_grid^	m_property_grid;
			object_compound^	m_current_object;
			enum_current_mode	m_mode;
	private: System::Windows::Forms::ToolStrip^  toolStrip1;
	private: System::Windows::Forms::ToolStripButton^  btn_add_from_selection;
	private: System::Windows::Forms::ToolStripButton^  btn_del_selected;




	protected: 









	private: System::Windows::Forms::ToolStripButton^  btn_ok;
	private: System::Windows::Forms::ToolStripButton^  btn_cancel;




	private: System::Windows::Forms::ToolStripButton^  btn_properties;
	private: System::Windows::Forms::ToolStripButton^  btn_set_pivot;
	private: System::Windows::Forms::Panel^  panel;
	private: System::Windows::Forms::CheckedListBox^  objects_list_box;






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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(compound_object_tab::typeid));
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->btn_add_from_selection = (gcnew System::Windows::Forms::ToolStripButton());
			this->btn_del_selected = (gcnew System::Windows::Forms::ToolStripButton());
			this->btn_ok = (gcnew System::Windows::Forms::ToolStripButton());
			this->btn_cancel = (gcnew System::Windows::Forms::ToolStripButton());
			this->btn_properties = (gcnew System::Windows::Forms::ToolStripButton());
			this->btn_set_pivot = (gcnew System::Windows::Forms::ToolStripButton());
			this->panel = (gcnew System::Windows::Forms::Panel());
			this->objects_list_box = (gcnew System::Windows::Forms::CheckedListBox());
			this->toolStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// toolStrip1
			// 
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->btn_add_from_selection, 
				this->btn_del_selected, this->btn_ok, this->btn_cancel, this->btn_properties, this->btn_set_pivot});
			this->toolStrip1->Location = System::Drawing::Point(0, 0);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(283, 25);
			this->toolStrip1->TabIndex = 0;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// btn_add_from_selection
			// 
			this->btn_add_from_selection->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->btn_add_from_selection->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_add_from_selection.Image")));
			this->btn_add_from_selection->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->btn_add_from_selection->Name = L"btn_add_from_selection";
			this->btn_add_from_selection->Size = System::Drawing::Size(33, 22);
			this->btn_add_from_selection->Text = L"Add";
			this->btn_add_from_selection->Click += gcnew System::EventHandler(this, &compound_object_tab::on_add_from_selection);
			// 
			// btn_del_selected
			// 
			this->btn_del_selected->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->btn_del_selected->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_del_selected.Image")));
			this->btn_del_selected->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->btn_del_selected->Name = L"btn_del_selected";
			this->btn_del_selected->Size = System::Drawing::Size(28, 22);
			this->btn_del_selected->Text = L"Del";
			this->btn_del_selected->Click += gcnew System::EventHandler(this, &compound_object_tab::on_remove_selection);
			// 
			// btn_ok
			// 
			this->btn_ok->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->btn_ok->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_ok.Image")));
			this->btn_ok->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->btn_ok->Name = L"btn_ok";
			this->btn_ok->Size = System::Drawing::Size(26, 22);
			this->btn_ok->Text = L"Ok";
			this->btn_ok->Click += gcnew System::EventHandler(this, &compound_object_tab::on_ok_button);
			// 
			// btn_cancel
			// 
			this->btn_cancel->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->btn_cancel->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_cancel.Image")));
			this->btn_cancel->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->btn_cancel->Name = L"btn_cancel";
			this->btn_cancel->Size = System::Drawing::Size(47, 22);
			this->btn_cancel->Text = L"Cancel";
			this->btn_cancel->Click += gcnew System::EventHandler(this, &compound_object_tab::on_cancel_button);
			// 
			// btn_properties
			// 
			this->btn_properties->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->btn_properties->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_properties.Image")));
			this->btn_properties->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->btn_properties->Name = L"btn_properties";
			this->btn_properties->Size = System::Drawing::Size(64, 22);
			this->btn_properties->Text = L"Properties";
			this->btn_properties->Click += gcnew System::EventHandler(this, &compound_object_tab::on_properties);
			// 
			// btn_set_pivot
			// 
			this->btn_set_pivot->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->btn_set_pivot->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_set_pivot.Image")));
			this->btn_set_pivot->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->btn_set_pivot->Name = L"btn_set_pivot";
			this->btn_set_pivot->Size = System::Drawing::Size(54, 22);
			this->btn_set_pivot->Text = L"SetPivot";
			this->btn_set_pivot->Click += gcnew System::EventHandler(this, &compound_object_tab::on_set_pivot);
			// 
			// panel
			// 
			this->panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel->Location = System::Drawing::Point(0, 25);
			this->panel->Name = L"panel";
			this->panel->Size = System::Drawing::Size(283, 200);
			this->panel->TabIndex = 3;
			// 
			// objects_list_box
			// 
			this->objects_list_box->Dock = System::Windows::Forms::DockStyle::Fill;
			this->objects_list_box->FormattingEnabled = true;
			this->objects_list_box->Location = System::Drawing::Point(0, 225);
			this->objects_list_box->Name = L"objects_list_box";
			this->objects_list_box->Size = System::Drawing::Size(283, 229);
			this->objects_list_box->TabIndex = 4;
			// 
			// compound_object_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(283, 467);
			this->Controls->Add(this->objects_list_box);
			this->Controls->Add(this->panel);
			this->Controls->Add(this->toolStrip1);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::SizableToolWindow;
			this->HideOnClose = true;
			this->Name = L"compound_object_tab";
			this->ShowIcon = false;
			this->Text = L"compound_object_tab";
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		void	in_constructor();
	public:
		void	set_compound_object(object_compound^ object, enum_current_mode mode);
		void	close_internal();
		void	fill_object_props();
	private: System::Void on_add_from_selection(System::Object^  sender, System::EventArgs^  e);
	private: System::Void on_ok_button(System::Object^  sender, System::EventArgs^  e);
	private: System::Void on_cancel_button(System::Object^  sender, System::EventArgs^  e);
	private: System::Void on_remove_selection(System::Object^  sender, System::EventArgs^  e);
	private: System::Void on_selected(System::Object^  sender, System::EventArgs^  e);
	private: System::Void on_properties(System::Object^  sender, System::EventArgs^  e);
	private: System::Void on_set_pivot(System::Object^  sender, System::EventArgs^  e);
};
}
}
