////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef IDE_H_INCLUDED
#define IDE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace model_editor {
	ref class model_editor;
	/// <summary>
	/// Summary for ide
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	ref class ide : public System::Windows::Forms::Form
	{
	public:
		ide( model_editor^ ed)
		:m_model_editor( ed )
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			in_constructor	();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ide()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::MenuStrip^  top_menu;
	protected: 

	protected: 



	public: WeifenLuo::WinFormsUI::Docking::DockPanel^  bottom_panel;
	private: 
	public: WeifenLuo::WinFormsUI::Docking::DockPanel^  main_dock_panel;
	public: System::Windows::Forms::Button^  apply_button;
	public: System::Windows::Forms::Button^  cancel_button;

	public: 


	private: 


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
			WeifenLuo::WinFormsUI::Docking::DockPanelSkin^  dockPanelSkin1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelSkin());
			WeifenLuo::WinFormsUI::Docking::AutoHideStripSkin^  autoHideStripSkin1 = (gcnew WeifenLuo::WinFormsUI::Docking::AutoHideStripSkin());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripSkin^  dockPaneStripSkin1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripSkin());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripGradient^  dockPaneStripGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient2 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient2 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient3 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripToolWindowGradient^  dockPaneStripToolWindowGradient1 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripToolWindowGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient4 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient5 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient3 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient6 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient7 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelSkin^  dockPanelSkin2 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelSkin());
			WeifenLuo::WinFormsUI::Docking::AutoHideStripSkin^  autoHideStripSkin2 = (gcnew WeifenLuo::WinFormsUI::Docking::AutoHideStripSkin());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient4 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient8 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripSkin^  dockPaneStripSkin2 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripSkin());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripGradient^  dockPaneStripGradient2 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient9 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient5 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient10 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPaneStripToolWindowGradient^  dockPaneStripToolWindowGradient2 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPaneStripToolWindowGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient11 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient12 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::DockPanelGradient^  dockPanelGradient6 = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanelGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient13 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			WeifenLuo::WinFormsUI::Docking::TabGradient^  tabGradient14 = (gcnew WeifenLuo::WinFormsUI::Docking::TabGradient());
			this->top_menu = (gcnew System::Windows::Forms::MenuStrip());
			this->bottom_panel = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
			this->main_dock_panel = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
			this->apply_button = (gcnew System::Windows::Forms::Button());
			this->cancel_button = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// top_menu
			// 
			this->top_menu->Location = System::Drawing::Point(0, 0);
			this->top_menu->Name = L"top_menu";
			this->top_menu->Size = System::Drawing::Size(513, 24);
			this->top_menu->TabIndex = 0;
			this->top_menu->Text = L"menuStrip1";
			// 
			// bottom_panel
			// 
			this->bottom_panel->ActiveAutoHideContent = nullptr;
			this->bottom_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->bottom_panel->DockBackColor = System::Drawing::SystemColors::Control;
			this->bottom_panel->Location = System::Drawing::Point(0, 438);
			this->bottom_panel->Name = L"bottom_panel";
			this->bottom_panel->Size = System::Drawing::Size(513, 36);
			dockPanelGradient1->EndColor = System::Drawing::SystemColors::ControlLight;
			dockPanelGradient1->StartColor = System::Drawing::SystemColors::ControlLight;
			autoHideStripSkin1->DockStripGradient = dockPanelGradient1;
			tabGradient1->EndColor = System::Drawing::SystemColors::Control;
			tabGradient1->StartColor = System::Drawing::SystemColors::Control;
			tabGradient1->TextColor = System::Drawing::SystemColors::ControlDarkDark;
			autoHideStripSkin1->TabGradient = tabGradient1;
			dockPanelSkin1->AutoHideStripSkin = autoHideStripSkin1;
			tabGradient2->EndColor = System::Drawing::SystemColors::ControlLightLight;
			tabGradient2->StartColor = System::Drawing::SystemColors::ControlLightLight;
			tabGradient2->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripGradient1->ActiveTabGradient = tabGradient2;
			dockPanelGradient2->EndColor = System::Drawing::SystemColors::Control;
			dockPanelGradient2->StartColor = System::Drawing::SystemColors::Control;
			dockPaneStripGradient1->DockStripGradient = dockPanelGradient2;
			tabGradient3->EndColor = System::Drawing::SystemColors::ControlLight;
			tabGradient3->StartColor = System::Drawing::SystemColors::ControlLight;
			tabGradient3->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripGradient1->InactiveTabGradient = tabGradient3;
			dockPaneStripSkin1->DocumentGradient = dockPaneStripGradient1;
			tabGradient4->EndColor = System::Drawing::SystemColors::ActiveCaption;
			tabGradient4->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
			tabGradient4->StartColor = System::Drawing::SystemColors::GradientActiveCaption;
			tabGradient4->TextColor = System::Drawing::SystemColors::ActiveCaptionText;
			dockPaneStripToolWindowGradient1->ActiveCaptionGradient = tabGradient4;
			tabGradient5->EndColor = System::Drawing::SystemColors::Control;
			tabGradient5->StartColor = System::Drawing::SystemColors::Control;
			tabGradient5->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripToolWindowGradient1->ActiveTabGradient = tabGradient5;
			dockPanelGradient3->EndColor = System::Drawing::SystemColors::ControlLight;
			dockPanelGradient3->StartColor = System::Drawing::SystemColors::ControlLight;
			dockPaneStripToolWindowGradient1->DockStripGradient = dockPanelGradient3;
			tabGradient6->EndColor = System::Drawing::SystemColors::GradientInactiveCaption;
			tabGradient6->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
			tabGradient6->StartColor = System::Drawing::SystemColors::GradientInactiveCaption;
			tabGradient6->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripToolWindowGradient1->InactiveCaptionGradient = tabGradient6;
			tabGradient7->EndColor = System::Drawing::Color::Transparent;
			tabGradient7->StartColor = System::Drawing::Color::Transparent;
			tabGradient7->TextColor = System::Drawing::SystemColors::ControlDarkDark;
			dockPaneStripToolWindowGradient1->InactiveTabGradient = tabGradient7;
			dockPaneStripSkin1->ToolWindowGradient = dockPaneStripToolWindowGradient1;
			dockPanelSkin1->DockPaneStripSkin = dockPaneStripSkin1;
			this->bottom_panel->Skin = dockPanelSkin1;
			this->bottom_panel->TabIndex = 4;
			// 
			// main_dock_panel
			// 
			this->main_dock_panel->ActiveAutoHideContent = nullptr;
			this->main_dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->main_dock_panel->DockBackColor = System::Drawing::SystemColors::Control;
			this->main_dock_panel->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingSdi;
			this->main_dock_panel->Location = System::Drawing::Point(0, 24);
			this->main_dock_panel->Name = L"main_dock_panel";
			this->main_dock_panel->Size = System::Drawing::Size(513, 414);
			dockPanelGradient4->EndColor = System::Drawing::SystemColors::ControlLight;
			dockPanelGradient4->StartColor = System::Drawing::SystemColors::ControlLight;
			autoHideStripSkin2->DockStripGradient = dockPanelGradient4;
			tabGradient8->EndColor = System::Drawing::SystemColors::Control;
			tabGradient8->StartColor = System::Drawing::SystemColors::Control;
			tabGradient8->TextColor = System::Drawing::SystemColors::ControlDarkDark;
			autoHideStripSkin2->TabGradient = tabGradient8;
			dockPanelSkin2->AutoHideStripSkin = autoHideStripSkin2;
			tabGradient9->EndColor = System::Drawing::SystemColors::ControlLightLight;
			tabGradient9->StartColor = System::Drawing::SystemColors::ControlLightLight;
			tabGradient9->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripGradient2->ActiveTabGradient = tabGradient9;
			dockPanelGradient5->EndColor = System::Drawing::SystemColors::Control;
			dockPanelGradient5->StartColor = System::Drawing::SystemColors::Control;
			dockPaneStripGradient2->DockStripGradient = dockPanelGradient5;
			tabGradient10->EndColor = System::Drawing::SystemColors::ControlLight;
			tabGradient10->StartColor = System::Drawing::SystemColors::ControlLight;
			tabGradient10->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripGradient2->InactiveTabGradient = tabGradient10;
			dockPaneStripSkin2->DocumentGradient = dockPaneStripGradient2;
			tabGradient11->EndColor = System::Drawing::SystemColors::ActiveCaption;
			tabGradient11->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
			tabGradient11->StartColor = System::Drawing::SystemColors::GradientActiveCaption;
			tabGradient11->TextColor = System::Drawing::SystemColors::ActiveCaptionText;
			dockPaneStripToolWindowGradient2->ActiveCaptionGradient = tabGradient11;
			tabGradient12->EndColor = System::Drawing::SystemColors::Control;
			tabGradient12->StartColor = System::Drawing::SystemColors::Control;
			tabGradient12->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripToolWindowGradient2->ActiveTabGradient = tabGradient12;
			dockPanelGradient6->EndColor = System::Drawing::SystemColors::ControlLight;
			dockPanelGradient6->StartColor = System::Drawing::SystemColors::ControlLight;
			dockPaneStripToolWindowGradient2->DockStripGradient = dockPanelGradient6;
			tabGradient13->EndColor = System::Drawing::SystemColors::GradientInactiveCaption;
			tabGradient13->LinearGradientMode = System::Drawing::Drawing2D::LinearGradientMode::Vertical;
			tabGradient13->StartColor = System::Drawing::SystemColors::GradientInactiveCaption;
			tabGradient13->TextColor = System::Drawing::SystemColors::ControlText;
			dockPaneStripToolWindowGradient2->InactiveCaptionGradient = tabGradient13;
			tabGradient14->EndColor = System::Drawing::Color::Transparent;
			tabGradient14->StartColor = System::Drawing::Color::Transparent;
			tabGradient14->TextColor = System::Drawing::SystemColors::ControlDarkDark;
			dockPaneStripToolWindowGradient2->InactiveTabGradient = tabGradient14;
			dockPaneStripSkin2->ToolWindowGradient = dockPaneStripToolWindowGradient2;
			dockPanelSkin2->DockPaneStripSkin = dockPaneStripSkin2;
			this->main_dock_panel->Skin = dockPanelSkin2;
			this->main_dock_panel->TabIndex = 6;
			// 
			// apply_button
			// 
			this->apply_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->apply_button->Location = System::Drawing::Point(357, 444);
			this->apply_button->Name = L"apply_button";
			this->apply_button->Size = System::Drawing::Size(75, 23);
			this->apply_button->TabIndex = 7;
			this->apply_button->Text = L"OK";
			this->apply_button->UseVisualStyleBackColor = true;
			// 
			// cancel_button
			// 
			this->cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancel_button->Location = System::Drawing::Point(438, 444);
			this->cancel_button->Name = L"cancel_button";
			this->cancel_button->Size = System::Drawing::Size(75, 23);
			this->cancel_button->TabIndex = 8;
			this->cancel_button->Text = L"Cancel";
			this->cancel_button->UseVisualStyleBackColor = true;
			// 
			// ide
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(513, 474);
			this->ControlBox = false;
			this->Controls->Add(this->cancel_button);
			this->Controls->Add(this->apply_button);
			this->Controls->Add(this->main_dock_panel);
			this->Controls->Add(this->bottom_panel);
			this->Controls->Add(this->top_menu);
			this->MainMenuStrip = this->top_menu;
			this->MinimizeBox = false;
			this->Name = L"ide";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Model editor";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	protected:
		void						in_constructor		( );
		model_editor^				m_model_editor;
};
} // namespace model_editor
} // namespace xray

#endif // #ifndef IDE_H_INCLUDED