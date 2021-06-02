////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_CONTROL_PANEL_H_INCLUDED
#define DIALOG_CONTROL_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace dialog_editor {
	ref class xray::editor::controls::drag_cursor_collection;

	public ref class dialog_control_panel : public xray::editor::controls::toolbar_panel_base
	{
		typedef xray::editor::controls::toolbar_panel_base super; 
	public:
		dialog_control_panel(xray::editor::controls::document_editor_base^ ed):xray::editor::controls::toolbar_panel_base(ed)
		{
			in_constructor();
		}

	protected:
		~dialog_control_panel()
		{
			if(components)
				delete components;
		}

		virtual void		OnGiveFeedback	(GiveFeedbackEventArgs^ e) override;

	private:
		void				in_constructor			();
		System::Void		single_node_mouse_down	(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void		quest_answ_mouse_down	(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);

	private: 
		System::Windows::Forms::PictureBox^	pictureBox1;
		System::Windows::Forms::PictureBox^	pictureBox2;
		xray::editor::controls::drag_cursor_collection^	m_cursors;
	}; // ref class dialog_control_panel
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_CONTROL_PANEL_H_INCLUDED