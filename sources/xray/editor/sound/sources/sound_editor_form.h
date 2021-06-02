////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_EDITOR_FORM_H_INCLUDED
#define SOUND_EDITOR_FORM_H_INCLUDED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace xray::editor::controls;

namespace xray {
namespace sound_editor {

	ref class sound_editor;
	ref class sound_document_editor_base;

	public ref class sound_editor_form: public System::Windows::Forms::Form
	{
	public:
		sound_editor_form				(sound_editor^ ed);
		~sound_editor_form				();

		property sound_document_editor_base^ multidocument_base
		{
			sound_document_editor_base^ get() {return m_multidocument_base;};
		}

	private:
		void view_drop_down				(Object^, EventArgs^);
		void tree_view_click			(Object^, EventArgs^);
		void instance_properties_click	(Object^, EventArgs^);
		void object_properties_click	(Object^, EventArgs^);
		void toolbar_click				(Object^, EventArgs^);
		void options_drop_down			(Object^, EventArgs^);
		void xaudio_click				(Object^, EventArgs^);
		void InitializeComponent		()
		{
			this->m_menu_view = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menu_view_tree_view = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menu_view_instance_properties = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menu_view_object_properties = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menu_view_toolbar = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menu_options = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menu_options_xaudio = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->SuspendLayout();
			// 
			// m_menu_view
			// 
			this->m_menu_view->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->m_menu_view_tree_view, 
				this->m_menu_view_instance_properties, this->m_menu_view_object_properties, this->m_menu_view_toolbar});
			this->m_menu_view->Name = L"m_menu_view";
			this->m_menu_view->Size = System::Drawing::Size(61, 20);
			this->m_menu_view->Text = L"&View";
			this->m_menu_view->DropDownOpening += gcnew System::EventHandler(this, &sound_editor_form::view_drop_down);
			// 
			// m_menu_view_tree_view
			// 
			this->m_menu_view_tree_view->Name = L"m_menu_view_tree_view";
			this->m_menu_view_tree_view->Size = System::Drawing::Size(174, 22);
			this->m_menu_view_tree_view->Text = L"&Tree view";
			this->m_menu_view_tree_view->Click += gcnew System::EventHandler(this, &sound_editor_form::tree_view_click);
			// 
			// m_menu_view_instance_properties
			// 
			this->m_menu_view_instance_properties->Name = L"m_menu_view_instance_properties";
			this->m_menu_view_instance_properties->Size = System::Drawing::Size(174, 22);
			this->m_menu_view_instance_properties->Text = L"&Instance properties";
			this->m_menu_view_instance_properties->Click += gcnew System::EventHandler(this, &sound_editor_form::instance_properties_click);
			// 
			// m_menu_view_object_properties
			// 
			this->m_menu_view_object_properties->Name = L"m_menu_view_object_properties";
			this->m_menu_view_object_properties->Size = System::Drawing::Size(174, 22);
			this->m_menu_view_object_properties->Text = L"&Object properties";
			this->m_menu_view_object_properties->Click += gcnew System::EventHandler(this, &sound_editor_form::object_properties_click);
			// 
			// m_menu_view_toolbar
			// 
			this->m_menu_view_toolbar->Name = L"m_menu_view_toolbar";
			this->m_menu_view_toolbar->Size = System::Drawing::Size(174, 22);
			this->m_menu_view_toolbar->Text = L"&Toolbar";
			this->m_menu_view_toolbar->Click += gcnew System::EventHandler(this, &sound_editor_form::toolbar_click);
			// 
			// m_menu_options
			// 
			this->m_menu_options->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->m_menu_options_xaudio});
			this->m_menu_options->Name = L"m_menu_options";
			this->m_menu_options->Size = System::Drawing::Size(61, 20);
			this->m_menu_options->Text = L"&Options";
			this->m_menu_options->DropDownOpening += gcnew System::EventHandler(this, &sound_editor_form::options_drop_down);
			// 
			// m_menu_options_xaudio
			// 
			this->m_menu_options_xaudio->Name = L"m_menu_options_xaudio";
			this->m_menu_options_xaudio->Size = System::Drawing::Size(237, 22);
			this->m_menu_options_xaudio->Text = L"&Use XAudio sound calculations";
			this->m_menu_options_xaudio->Click += gcnew System::EventHandler(this, &sound_editor_form::xaudio_click);
			// 
			// sound_editor_form
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 262);
			this->Name = L"sound_editor_form";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Sound editor";
			this->ResumeLayout(false);

		}

	private:
		System::ComponentModel::Container^	components;
		sound_editor^						m_editor;
		sound_document_editor_base^			m_multidocument_base;
		ToolStripMenuItem^					m_menu_view;
		ToolStripMenuItem^					m_menu_view_tree_view;
		ToolStripMenuItem^					m_menu_view_instance_properties;
		ToolStripMenuItem^					m_menu_view_object_properties;
		ToolStripMenuItem^					m_menu_view_toolbar;
		ToolStripMenuItem^					m_menu_options;
		ToolStripMenuItem^					m_menu_options_xaudio;
	}; // class sound_editor_form
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SOUND_EDITOR_FORM_H_INCLUDED