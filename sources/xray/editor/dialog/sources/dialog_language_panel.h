////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_LANGUAGE_PANEL_H_INCLUDED
#define DIALOG_LANGUAGE_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace dialog_editor {
	ref class dialog_editor_impl;
	ref class add_language_form;
	ref class remove_language_form;

	public ref class dialog_language_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
					dialog_language_panel		(dialog_editor_impl^ ed);
		void		refresh_languages			();
		void		add_new_language			(String^ loc, bool is_checked);
		void		set_active_language			(String^ loc);
		void		on_language_changed			(System::Object^ sender, System::EventArgs^ e);

		void		add_language				(System::Object^ , System::EventArgs^ );
		void		remove_language				(System::Object^ , System::EventArgs^ );

	protected:
		~dialog_language_panel()
		{
			if (components)
				delete components;
		}

	private:
		System::ComponentModel::Container^	components;
		System::Windows::Forms::GroupBox^	group_box;
		dialog_editor_impl^					m_editor;
		int									m_height;
		
		add_language_form^					m_add_lang_form;
		remove_language_form^				m_del_lang_form;

		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
		System::Windows::Forms::ToolStripMenuItem^	add_language_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	remove_language_menu_item;
	}; // ref class dialog_language_panel
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_LANGUAGE_PANEL_H_INCLUDED