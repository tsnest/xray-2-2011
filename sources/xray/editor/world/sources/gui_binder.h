////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GUI_BINDER_H_INCLUDED
#define GUI_BINDER_H_INCLUDED

namespace xray {	
namespace editor {

class editor_world;

ref class action_engine;
ref class input_engine;

public ref class gui_binder
{
	typedef System::Windows::Forms::ToolStripMenuItem ToolStripMenuItem;
	typedef System::Windows::Forms::ToolStripButton ToolStripButton;
public:

	gui_binder( action_engine^ action_engine );

	void			bind					( ToolStripMenuItem^ menu_item, System::String^ action_name  );
	void			bind					( ToolStripButton^ control, System::String^ action_name  );
	void			update_menu_item		( ToolStripMenuItem^ item);
	void			update_button			( ToolStripButton^ item);
	void			update_button			(ToolStripButton^ button_item, System::String^ action_name);
	void			update_items			( );
	void			gui_menu_drop_down		(System::Object^  sender, System::EventArgs^  e);
private:

	void			gui_control_click		(System::Object^  sender, System::EventArgs^  e);
	void			gui_menu_item_paint		(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);

	action_engine^					m_action_engine;
	System::Collections::Hashtable	m_menu_items;
	System::Collections::Hashtable	m_buttons;
}; // class gui_binder

} // namespace xray	
} // namespace editor

#endif // #ifndef GUI_BINDER_H_INCLUDED