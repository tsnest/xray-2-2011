#include "pch.h"
#include "options_page_input.h"
#include "editor_world.h"
#include "action_engine.h"
#include "input_engine.h"
#include "window_ide.h"
#include "window_view.h"

namespace xray {
namespace editor {

bool options_page_input::changed()
{
	return m_changes_list.Count > 0;
}

bool options_page_input::accept_changes( )
{
	for( int i = 0; i < m_changes_list.Count; ++i )
	{
		if( m_changes_list[i]->change_id == enum_action_chage_id::enum_action_chage_add )
		{
			m_world.input_engine()->add_action_shortcut( m_changes_list[i]->name, m_changes_list[i]->keys);
		}
		else if( m_changes_list[i]->change_id == enum_action_chage_id::enum_action_chage_remove )
		{
			m_world.input_engine()->remove_action_shortcut( m_changes_list[i]->name, m_changes_list[i]->keys);
		}
		else
		{
			ASSERT( false, "There is no such command." );
		}
	}

	m_changes_list.Clear	();
	m_initialized			= false;
	m_world.input_engine()->save();
	return true;	
}

void options_page_input::cancel_changes()
{
	m_changes_list.Clear	();
	m_initialized			= false;
}


void options_page_input::initialize()
{
	ASSERT(m_changes_list.Count==0);
	m_name_to_action.Clear		();
	m_key_to_action.Clear		();

	//////////////////////////////////////////////////////////////////////////
	ICollection^ gui_actions			= m_world.action_engine()->get_actions_list();
	array<String^>^ gui_action_names	= gcnew array<String^>( gui_actions->Count );

	IEnumerator^ iter = gui_actions->GetEnumerator();
	int i = 0;
	while( iter->MoveNext() )
	{
		gui_action_names[i] = (String^)iter->Current;
		++i;
	}
	array<String^>::Sort( gui_action_names );

	Generic::List<String^> all_shortcuts;

	for ( i = 0; i< gui_action_names->Length; ++i )
	{
		Generic::List<String^>^ shortcuts		= m_world.input_engine()->get_action_keys_list( gui_action_names[i] );
		action_item_list^ new_items				= gcnew action_item_list( shortcuts->Count );
		m_name_to_action[gui_action_names[i]]	= new_items;
		
		for( int j = 0; j < shortcuts->Count ; ++j )
		{
			new_items->Add		( gcnew action_item( gui_action_names[i], shortcuts[j] ) );
			all_shortcuts.Add	( shortcuts[j] );
		} 
	}

	all_shortcuts.Sort			();

	Generic::List<String^>::Enumerator^ enm = all_shortcuts.GetEnumerator();
	String^ prev_key			= "";

	while( enm->MoveNext() )
	{
		if( enm->Current == prev_key )
			continue;

		prev_key = enm->Current;

		Generic::List<String^>^ actions;
		Generic::List<String^>^ shortcuts;
		m_world.input_engine()->get_actions_by_shortcut( enm->Current, actions, shortcuts );

		ASSERT( actions->Count != 0 );

		action_item_list^ list = gcnew action_item_list( actions->Count );

		for( int i = 0; i < actions->Count; ++i )
		{
			action_item_list^ names_list = m_name_to_action[actions[i]];
			for( int j = 0; j < names_list->Count; ++j )
			{
				if( names_list[j]->keys == enm->Current )
				{
					list->Add( names_list[j] );
					break;
				}
			}
		}
			m_key_to_action[exctract_first_keys(enm->Current)] = list;
	}

	m_initialized	= true;
}

void options_page_input::activate_page()
{
	filter_text_box->Text		= "";
	if(!m_initialized)
		initialize();

	m_keys_count		= 0;
	m_pass				= 0;

	update_actions_list	 ();
	Visible				= true;
}

void options_page_input::deactivate_page()
{
	Visible						= false;
	m_active_action				= "";
	clear_shortcut_text_box		();
}

String^	options_page_input::exctract_first_keys				( String^ keys )
{
	int ind = keys->IndexOf(',');
	if( ind != -1 )
		return keys->Remove(ind);
	else
		return keys;
}

Void options_page_input::actionslistBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
{	
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	shortcuts_combo_box->Items->Clear();

	if( actions_list_box->SelectedIndex < 0 )
	{
		m_active_action = "";
		return;
	}

	m_active_action = (String^)actions_list_box->Items[actions_list_box->SelectedIndex];

	action_item_list^ shortcuts = m_name_to_action[m_active_action];

	if( shortcuts != nullptr ) 
	{
		action_item_list::Enumerator^ iter = shortcuts->GetEnumerator();
		while( iter->MoveNext() )
		{
			shortcuts_combo_box->Items->Add( iter->Current->keys );
		}
	}

	if( shortcuts_combo_box->Items->Count > 0 )
	{
		shortcuts_combo_box->Enabled		= true;
		shortcuts_combo_box->SelectedIndex	= 0;
	}
	else
	{
		shortcuts_combo_box->Enabled		= false;
		shortcut_for_selected_label->Enabled = false;
	}

}
Void options_page_input::remove_button_Click		(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETER	( e );

	if( shortcuts_combo_box->Text != "" && m_active_action != "" )
	{
		remove_action_shortcut				( m_active_action, shortcuts_combo_box->Text );
		actionslistBox_SelectedIndexChanged	( sender, nullptr );
		update_assegned_to_combo			( );
	}
}

bool options_page_input::add_action_shortcut				( String^ name, String^  keys )
{
	action_item_list^ list = nullptr;
	if( m_name_to_action.ContainsKey( name ) )
		list = m_name_to_action[name];
	else
	{
		list = gcnew action_item_list();
		m_name_to_action[name] = list;
	}

	for( int i = 0; i < list->Count; ++i )
		if( list[i]->keys == keys )
			return false;

	action_item^ new_item = gcnew action_item( name, keys );
	list->Insert(0, new_item);

	String^ first_keys = exctract_first_keys( keys );


	if( m_key_to_action.ContainsKey( first_keys ) )
		list = m_key_to_action[first_keys];
	else
	{
		list = gcnew action_item_list();
		m_key_to_action[first_keys] = list;
	}

	if( list->Count == 0 )
		list->Add( new_item );
	else
	{
		int ind = 0;
		while( keys->Length > list[ind]->keys->Length )
			++ind;

		list->Insert( ind, new_item );
	}

	m_changes_list.Add( gcnew action_item_change( enum_action_chage_id::enum_action_chage_add, name, keys ) );

	return true;
}

System::Void options_page_input::remove_action_shortcut				( String^ name, String^  keys )
{
	if( m_name_to_action.ContainsKey( name) )
	{
		action_item_list^ list = m_name_to_action[name];
		for( int i = 0; i < list->Count; ++i )
		{
			if( list[i]->keys == keys )
			{
				list->RemoveAt(i);
				break;
			}
		}

		list = m_key_to_action[exctract_first_keys(keys)];
		for( int i = 0; i < list->Count; ++i )
		{
			if( list[i]->name == name && list[i]->keys == keys )
			{
				list->RemoveAt(i);
				break;
			}
		}
	}
	m_changes_list.Add( gcnew action_item_change( enum_action_chage_id::enum_action_chage_remove, name, keys ) );
}

Void options_page_input::shortcut_text_box_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS			( sender );
	m_input_keys_holder.register_key_event	( e, false );
	e->Handled								= true;
	execute_key_up							( );
}

Void options_page_input::shortcut_text_box_KeyDown		(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS			( sender );
	m_input_keys_holder.register_key_event	( e, true );
	e->Handled								= true;

	String^ tmp_str = "";
	int keys_count = m_input_keys_holder.get_current_keys_string( tmp_str );

	if( e->KeyCode == Keys::Back && keys_count == 1 )
	{
		if( m_pass > 0)
		{
			m_pass--;
			int ind = m_curr_shortcut_str->IndexOf(',');
			if( ind == -1 )
				ind = 0;
			m_curr_shortcut_str = m_curr_shortcut_str->Remove( ind );

			shortcut_text_box->Text = "";
			shortcut_text_box->Text = m_curr_shortcut_str;
			shortcut_text_box->SelectionStart = shortcut_text_box->Text->Length;
		}
	}
	else
		execute_key_down	();

	shortcut_text_box->SelectionStart = shortcut_text_box->Text->Length;
}

System::Void options_page_input::execute_key_down					()
{
	String^ tmp_str = "";
	int keys_count = m_input_keys_holder.get_current_keys_string( tmp_str );

	tmp_str = tmp_str->Replace( "+None", "" );
	tmp_str = tmp_str->Replace( "None+", "" );

	if( m_pass >1 )
	{
		clear_shortcut_text_box();
	}

	if( m_keys_count < keys_count )
	{ 
		if( m_pass == 0 )
		{
			m_curr_shortcut_str		= tmp_str;
			shortcut_text_box->Text = "";
			shortcut_text_box->Text = m_curr_shortcut_str;
			shortcut_text_box->SelectionStart = shortcut_text_box->Text->Length;
		}
		else
		{
			m_curr_shortcut_str = exctract_first_keys( m_curr_shortcut_str );

			m_curr_shortcut_str += ", " + tmp_str;

			shortcut_text_box->Text = "";
			shortcut_text_box->Text = m_curr_shortcut_str;
			shortcut_text_box->SelectionStart = shortcut_text_box->Text->Length;
		}
		m_keys_count = keys_count;
	}
}

System::Void options_page_input::execute_key_up						()
{
	String^ tmp_str = "";
	int keys_count = m_input_keys_holder.get_current_keys_string( tmp_str );

	if( keys_count == 0 && m_keys_count > 0 )
	{
		m_keys_count = 0;
		m_pass += 1;
	}
}

Void options_page_input::clear_shortcut_text_box			( )
{
	m_pass = 0;
	m_curr_shortcut_str					= "";
	shortcut_text_box->Text				= "";
	shortcut_text_box->Text				= m_curr_shortcut_str;
	shortcut_text_box->SelectionStart	= shortcut_text_box->Text->Length;
}

Void options_page_input::shortcut_text_box_TextChanged		(System::Object^  sender, System::EventArgs^  e) 
{
	XRAY_UNREFERENCED_PARAMETERS		( sender, e );

	shortcut_text_box->SelectionStart	= shortcut_text_box->Text->Length;
	update_assegned_to_combo			( );
}
Void options_page_input::shortcut_text_box_MouseDown		(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS				( sender );
	m_input_keys_holder.register_mouse_event	( e, true );

	if( !m_shortcut_bar_first_select )
		execute_key_down		( ); 

	m_shortcut_bar_first_select					= false;
	shortcut_text_box->SelectionStart			= shortcut_text_box->Text->Length;
}

Void options_page_input::shortcut_text_box_MouseUp			(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS				( sender, e );
	m_input_keys_holder.register_mouse_event	( e, false );

	execute_key_up								( );
	shortcut_text_box->SelectionStart			= shortcut_text_box->Text->Length;
}

Void options_page_input::update_assegned_to_combo			()
{
	assigned_to_comboBox->Items->Clear();

	String^ first_keys = exctract_first_keys(shortcut_text_box->Text);
	if( m_key_to_action.ContainsKey(first_keys) )
	{
		action_item_list^ actions =  m_key_to_action[first_keys];
		for ( int i = 0; i< actions->Count; ++i )
		{
			assigned_to_comboBox->Items->Add( actions[i]->name + " (" + actions[i]->keys + ")");
		}
		if( assigned_to_comboBox->Items->Count > 0) 
		{
			assigned_to_comboBox->Enabled = true;
			assigned_to_comboBox->SelectedIndex = 0;
		}
		else
		{
			assigned_to_comboBox->Enabled = false;
			asigned_to_label->Enabled = false;
		}

	}
}

Void options_page_input::assign_button_Click				(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETER	( e );

	if( m_active_action != "" && shortcut_text_box->Text != "" )
	{
		if( !add_action_shortcut				( m_active_action, shortcut_text_box->Text ) ) 
		{
			MessageBox::Show( this, "The shortcut is already assigned to the action.", 
									"Options", 
									MessageBoxButtons::OK );
		}
	}

	actionslistBox_SelectedIndexChanged( sender, nullptr );
	clear_shortcut_text_box();
}
System::Void options_page_input::update_actions_list				()
{
	actions_list_box->Items->Clear();

	key_to_action_list::Enumerator^ enm = m_name_to_action.GetEnumerator();
	while( enm->MoveNext() )
	{
		if( enm->Current.Key->IndexOf( filter_text_box->Text ) != -1 )
			actions_list_box->Items->Add( enm->Current.Key );
	}
	
	actionslistBox_SelectedIndexChanged( nullptr, nullptr );
}

System::Void options_page_input::filter_text_box_TextChanged			(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	update_actions_list	 ();
}

Void options_page_input::shortcut_text_box_Enter(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

//.	m_world.ide()->clear_key_list();
}

	
} // namespace xray
} // namespace editor

