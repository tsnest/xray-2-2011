////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_TIME_BASED_LAYOUT_PANEL_H_INCLUDED
#define PARTICLE_TIME_BASED_LAYOUT_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace particle_editor {
		ref class particle_editor;
		ref class particle_graph_node;
		public ref class particle_time_based_layout_panel: public WeifenLuo::WinFormsUI::Docking::DockContent
		{
		public:
			particle_time_based_layout_panel(particle_editor^ ed):m_editor(ed)
			{
				InitializeComponent();
				in_constructor();
			}

		protected:
			~particle_time_based_layout_panel()
			{
				if(components)
					delete components;
			}
#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// particle_time_based_layout_panel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 273);
// 			this->DockAreas = static_cast<WeifenLuo::WinFormsUI::Docking::DockAreas>(
// 				WeifenLuo::WinFormsUI::Docking::DockAreas::DockLeft | 
// 				WeifenLuo::WinFormsUI::Docking::DockAreas::DockRight | 
// 				WeifenLuo::WinFormsUI::Docking::DockAreas::DockTop | 
// 				WeifenLuo::WinFormsUI::Docking::DockAreas::DockBottom | 
// 				WeifenLuo::WinFormsUI::Docking::DockAreas::Float);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"particle_time_based_layout_panel";
			this->Text = L"Time Layout";
			this->BackColor = SystemColors::Control;
			
			this->AutoScroll = true;
			
			m_time_layout_control = gcnew xray::editor::wpf_controls::time_layout_host();	
			m_time_layout_control->Location = System::Drawing::Point(0, 0);
			m_time_layout_control->Name = "host";
			m_time_layout_control->TabIndex = 1;
			m_time_layout_control->Text = "host";
			m_time_layout_control->Child = nullptr;
			m_time_layout_control->Dock = DockStyle::Fill;
			this->Controls->Add(m_time_layout_control);
			this->ResumeLayout(false);
		}
#pragma endregion

		protected: 
			particle_editor^					m_editor;
			particle_graph_node^				m_owner_node;
			xray::editor::wpf_controls::time_layout_host^		m_time_layout_control;
			System::ComponentModel::Container^	components;
		
		public:
			property particle_editor^			parent_editor{
				particle_editor^				get(){return m_editor;}
			}

		private:
			void								in_constructor();

		public:
			void								show_node(particle_graph_node^ node);
			void								clear_panel();
};// class particle_time_based_layout_panel

}//namespace particle_editor
}//namespace xray

#endif // #ifndef PARTICLE_TIME_BASED_LAYOUT_PANEL_H_INCLUDED