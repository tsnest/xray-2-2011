////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_EDITOR_FORM_H_INCLUDED
#define ANIMATION_EDITOR_FORM_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;

namespace xray {
namespace animation_editor {

	public ref class animation_editor_form : public System::Windows::Forms::Form
	{
	public:
		animation_editor_form(System::String^ name)
		:m_name(name)
		{
			InitializeComponent();
			this->Name = m_name;
			active_content = nullptr;
		}

		property WeifenLuo::WinFormsUI::Docking::DockPanel^ main_dock_panel
		{
			WeifenLuo::WinFormsUI::Docking::DockPanel^ get () {return m_main_dock_panel;};
		}
		property WeifenLuo::WinFormsUI::Docking::IDockContent^ active_content;

	protected:
		~animation_editor_form()
		{
			if (components)
				delete components;
		}

	private:
		System::ComponentModel::Container^			components;
		System::String^								m_name;
		WeifenLuo::WinFormsUI::Docking::DockPanel^	m_main_dock_panel;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
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
			this->m_main_dock_panel = (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
			this->SuspendLayout();
			// 
			// m_main_dock_panel
			// 
			this->m_main_dock_panel->ActiveAutoHideContent = nullptr;
			this->m_main_dock_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_main_dock_panel->DockBackColor = System::Drawing::SystemColors::Control;
			this->m_main_dock_panel->DockLeftPortion = 0.5;
			this->m_main_dock_panel->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingWindow;
			this->m_main_dock_panel->Location = System::Drawing::Point(0, 0);
			this->m_main_dock_panel->Name = L"m_main_dock_panel";
			this->m_main_dock_panel->Size = System::Drawing::Size(284, 262);
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
			this->m_main_dock_panel->Skin = dockPanelSkin2;
			this->m_main_dock_panel->TabIndex = 0;
			// 
			// animation_editor_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 262);
			this->Controls->Add(this->m_main_dock_panel);
			this->Name = L"animation_editor_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Animation editor";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->ResumeLayout(false);

		}
#pragma endregion
	};
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_EDITOR_FORM_H_INCLUDED
