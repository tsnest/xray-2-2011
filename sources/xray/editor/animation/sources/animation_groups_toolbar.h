////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_TOOLBAR_H_INCLUDED
#define ANIMATION_GROUPS_TOOLBAR_H_INCLUDED

using namespace System::Windows::Forms;

namespace xray {
namespace animation_editor {

	ref class animation_groups_editor;

	public ref class animation_groups_toolbar: public xray::editor::controls::toolbar_panel_base
	{
		typedef xray::editor::controls::toolbar_panel_base super; 
	public:
		animation_groups_toolbar(xray::editor::controls::document_editor_base^ ed):xray::editor::controls::toolbar_panel_base(ed)
		{
			in_constructor();
		}

	protected:
		~animation_groups_toolbar()
		{
			if(components)
				delete components;
		}

		virtual void		OnGiveFeedback					(GiveFeedbackEventArgs^ e) override;

	private:
		void				in_constructor					();
		System::Void		animation_group_node_mouse_down	(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);

	private: 
		System::Windows::Forms::PictureBox^	pictureBox1;
		xray::editor::controls::drag_cursor_collection^	m_cursors;
	}; // class animation_groups_toolbar
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_TOOLBAR_H_INCLUDED