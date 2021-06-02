////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_PAINTER_CONTROL_TAB_H_INCLUDED
#define TERRAIN_PAINTER_CONTROL_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

	ref class tool_terrain;

	ref class terrain_modifier_bump;
	/// <summary>
	/// Summary for terrain_bump_control_tab
	/// </summary>
	public ref class terrain_painter_control_tab : public System::Windows::Forms::UserControl
	{
	public:
		terrain_painter_control_tab(tool_terrain^ t):m_tool(t),m_in_sync(false)
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
		~terrain_painter_control_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	void	on_control_activated		( editor_base::editor_control_base^ c );
	void	on_control_deactivated		( editor_base::editor_control_base^ c );

	void	in_constructor		( );
	tool_terrain^				m_tool;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::RadioButton^  second_layer_button;

	private: System::Windows::Forms::RadioButton^  first_layer_button;
	private: System::Windows::Forms::RadioButton^  base_layer_button;
	private: System::Windows::Forms::ListView^  textures_list_view;











	protected: 

	protected: 
		bool						m_in_sync;
	void	sync				(bool b_sync_ui);























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
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->second_layer_button = (gcnew System::Windows::Forms::RadioButton());
			this->first_layer_button = (gcnew System::Windows::Forms::RadioButton());
			this->base_layer_button = (gcnew System::Windows::Forms::RadioButton());
			this->textures_list_view = (gcnew System::Windows::Forms::ListView());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->second_layer_button);
			this->groupBox1->Controls->Add(this->first_layer_button);
			this->groupBox1->Controls->Add(this->base_layer_button);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(239, 37);
			this->groupBox1->TabIndex = 17;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"layer";
			// 
			// second_layer_button
			// 
			this->second_layer_button->AutoSize = true;
			this->second_layer_button->Location = System::Drawing::Point(97, 14);
			this->second_layer_button->Name = L"second_layer_button";
			this->second_layer_button->Size = System::Drawing::Size(31, 17);
			this->second_layer_button->TabIndex = 2;
			this->second_layer_button->Text = L"2";
			this->second_layer_button->UseVisualStyleBackColor = true;
			this->second_layer_button->CheckedChanged += gcnew System::EventHandler(this, &terrain_painter_control_tab::base_layer_button_CheckedChanged);
			// 
			// first_layer_button
			// 
			this->first_layer_button->AutoSize = true;
			this->first_layer_button->Checked = true;
			this->first_layer_button->Location = System::Drawing::Point(60, 14);
			this->first_layer_button->Name = L"first_layer_button";
			this->first_layer_button->Size = System::Drawing::Size(31, 17);
			this->first_layer_button->TabIndex = 1;
			this->first_layer_button->TabStop = true;
			this->first_layer_button->Text = L"1";
			this->first_layer_button->UseVisualStyleBackColor = true;
			this->first_layer_button->CheckedChanged += gcnew System::EventHandler(this, &terrain_painter_control_tab::base_layer_button_CheckedChanged);
			// 
			// base_layer_button
			// 
			this->base_layer_button->AutoSize = true;
			this->base_layer_button->Location = System::Drawing::Point(6, 14);
			this->base_layer_button->Name = L"base_layer_button";
			this->base_layer_button->Size = System::Drawing::Size(48, 17);
			this->base_layer_button->TabIndex = 0;
			this->base_layer_button->Text = L"base";
			this->base_layer_button->UseVisualStyleBackColor = true;
			this->base_layer_button->CheckedChanged += gcnew System::EventHandler(this, &terrain_painter_control_tab::base_layer_button_CheckedChanged);
			// 
			// textures_list_view
			// 
			this->textures_list_view->Activation = System::Windows::Forms::ItemActivation::OneClick;
			this->textures_list_view->Dock = System::Windows::Forms::DockStyle::Fill;
			this->textures_list_view->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->textures_list_view->HideSelection = false;
			this->textures_list_view->Location = System::Drawing::Point(0, 37);
			this->textures_list_view->MultiSelect = false;
			this->textures_list_view->Name = L"textures_list_view";
			this->textures_list_view->Size = System::Drawing::Size(239, 276);
			this->textures_list_view->TabIndex = 18;
			this->textures_list_view->UseCompatibleStateImageBehavior = false;
			this->textures_list_view->View = System::Windows::Forms::View::List;
			this->textures_list_view->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &terrain_painter_control_tab::textures_list_view_MouseDoubleClick);
			this->textures_list_view->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &terrain_painter_control_tab::textures_list_view_MouseClick);
			// 
			// terrain_painter_control_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->textures_list_view);
			this->Controls->Add(this->groupBox1);
			this->Name = L"terrain_painter_control_tab";
			this->Size = System::Drawing::Size(239, 313);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: 
		void sync							( System::Object^ sender, System::EventArgs^ e );
		void external_property_changed		( editor_base::editor_control_base^ c );
		bool select_texture					( System::String^ active_item );
		void textures_list_view_MouseDown	( System::Object^  sender, System::Windows::Forms::MouseEventArgs^ e );
		void textures_list_view_MouseClick	( System::Object^  sender, System::Windows::Forms::MouseEventArgs^ e );
		void base_layer_button_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		void textures_list_view_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
};
} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_PAINTER_CONTROL_TAB_H_INCLUDED