////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_world.h"
#include "gui_binder.h"
#include "action_engine.h"
#include "input_engine.h"
#include "action.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections;

namespace xray {	
namespace editor {

gui_binder::gui_binder	( action_engine^ action_engine):
m_action_engine			( action_engine )
{
	ASSERT( m_action_engine != nullptr );
}

Void gui_binder::bind( ToolStripMenuItem^ menu_item, String^ action_name )
{
	action^ a = m_action_engine->get_action( action_name );
	if( a == nullptr )
	{
		ASSERT( false );
		return;
	}
	menu_item->Text		= a->text();
	menu_item->Click	+= gcnew System::EventHandler(this, &gui_binder::gui_control_click);
	menu_item->Paint	+= gcnew System::Windows::Forms::PaintEventHandler(this, &gui_binder::gui_menu_item_paint);
	m_menu_items.Add	( menu_item, action_name );
}

Void gui_binder::bind( System::Windows::Forms::ToolStripButton^ button, System::String^ action_name  )
{
	action^ a = m_action_engine->get_action( action_name );
	if( a == nullptr )
	{
		ASSERT( false );
		return;
	}
	
	button->Text			= a->text();
	button->Click += gcnew System::EventHandler(this, &gui_binder::gui_control_click);

	m_buttons.Add( button, action_name );
}

void gui_binder::gui_menu_drop_down(System::Object^ sender, System::EventArgs^)
{
	ToolStripMenuItem^ parent_item = safe_cast<ToolStripMenuItem^>(sender);
	
	for each (ToolStripMenuItem^ sub_item in parent_item->DropDownItems)
		if(m_menu_items.Contains(sub_item))
			update_menu_item ( sub_item );
}

void gui_binder::gui_control_click(System::Object^ sender, System::EventArgs^)
{
	 if(m_menu_items.Contains(sender))
		 m_action_engine->execute( safe_cast<System::String^>(m_menu_items[sender]) );
	 else
	 if(m_buttons.Contains(sender))
		 m_action_engine->execute( safe_cast<System::String^>(m_buttons[sender]) );
	 else
		 ASSERT(false, "unregistered control click");
}

void gui_binder::gui_menu_item_paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^)
{
	ToolStripMenuItem^ item = safe_cast<ToolStripMenuItem^>(sender);

	String^ action_name = dynamic_cast<String^>(m_menu_items[item]);
	ASSERT( action_name != nullptr );

	action^ action = m_action_engine->get_action(action_name);

	item->Enabled = action->enabled();
 
	Generic::List<String^>^ shortcuts = m_action_engine->get_input_engine()->get_action_keys_list( action_name );
 	if( shortcuts->Count > 0 )
 	{
		item->ShortcutKeyDisplayString = shortcuts[0];
	}
	else
		item->ShortcutKeyDisplayString = "";
}

void gui_binder::update_menu_item(ToolStripMenuItem^ menu_item)
{
	ASSERT					(m_menu_items.Contains(menu_item));
	String^ action_name		= safe_cast<String^>(m_menu_items[menu_item]);
	action^ action			= m_action_engine->get_action(action_name);

	menu_item->Enabled		= action->enabled();

	xray::editor::checked_state item_check_state	= action->checked_state();
	menu_item->Checked		= (item_check_state!=xray::editor::checked_state::uncheckable);

	if(item_check_state!=xray::editor::checked_state::uncheckable)
	{
		menu_item->Checked		= true;
		menu_item->CheckState	= (item_check_state==xray::editor::checked_state::checked)	?
									System::Windows::Forms::CheckState::Checked : 
									System::Windows::Forms::CheckState::Unchecked; 
	}else
		menu_item->Checked		= false;
}

void gui_binder::update_button(ToolStripButton^ button_item)
{
	ASSERT					(m_buttons.Contains(button_item));
	String^ action_name		= safe_cast<String^>(m_buttons[button_item]);
	update_button			(button_item, action_name);
}

void gui_binder::update_button(ToolStripButton^ button_item, System::String^ action_name)
{
	action^ action			= m_action_engine->get_action(action_name);

	button_item->Enabled	= action->enabled();
	xray::editor::checked_state item_check_state	= action->checked_state();
	button_item->Checked	= (item_check_state==xray::editor::checked_state::checked);
}

void gui_binder::update_items()
{
	for each (System::Collections::DictionaryEntry^ i in m_buttons) 
	{
		System::String^ action_name		= safe_cast<System::String^>	(i->Value);
		ToolStripButton^ button			= safe_cast<ToolStripButton^>	(i->Key);
		update_button					(button, action_name);
	}
}

} // namespace xray	
} // namespace editor
