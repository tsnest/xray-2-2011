////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_VIEWER_TOOLBAR_PANEL_H_INCLUDED
#define ANIMATION_VIEWER_TOOLBAR_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace animation_editor {
	ref class xray::editor::controls::drag_cursor_collection;

	public ref class animation_viewer_toolbar_panel : public xray::editor::controls::toolbar_panel_base
	{
		typedef xray::editor::controls::toolbar_panel_base super; 
	public:
		animation_viewer_toolbar_panel(xray::editor::controls::document_editor_base^ ed):xray::editor::controls::toolbar_panel_base(ed)
		{
			in_constructor();
		}

	protected:
		~animation_viewer_toolbar_panel()
		{
			if(components)
				delete components;
		}

		virtual void		OnGiveFeedback	(GiveFeedbackEventArgs^ e) override;

	private:
		void				in_constructor			();
		System::Void		animation_node_mouse_down	(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void		operator_node_mouse_down	(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void		root_node_mouse_down		(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);

	private: 
		System::Windows::Forms::PictureBox^	pictureBox1;
		System::Windows::Forms::PictureBox^	pictureBox2;
		System::Windows::Forms::PictureBox^	pictureBox3;
		xray::editor::controls::drag_cursor_collection^	m_cursors;
	}; // ref class animation_viewer_toolbar_panel
} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_VIEWER_TOOLBAR_PANEL_H_INCLUDED