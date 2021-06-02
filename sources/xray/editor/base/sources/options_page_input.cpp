#include "pch.h"
#include "options_page_input.h"
#include "input_engine.h"
#include "input_keys_holder.h"
#include <xray/editor/base/i_context_manager.h>

using System::Windows::Forms::Keys;

namespace xray {
namespace editor_base {

bool options_page_input::changed()
{
	return m_changes_list.Count > 0;
}

bool options_page_input::accept_changes( )
{
	for each (action_item_change^ ch in m_changes_list)
	{
		if( ch->m_change_type == enum_action_chage::add )
			m_input_engine->add_action_shortcut( ch->action_name, ch->action_shortcut);
		else 
		if( ch->m_change_type == enum_action_chage::remove )
			m_input_engine->remove_action_shortcut( ch->action_name, ch->action_shortcut);
		else
			ASSERT( false, "There is no such command." );
	}

	m_changes_list.Clear	( );
	m_initialized			= false;
	m_input_engine->save	( );
	return					true;	
}

void options_page_input::cancel_changes()
{
	m_changes_list.Clear	( );
	m_initialized			= false;
}


void options_page_input::initialize( )
{
	ASSERT							( m_changes_list.Count==0 );
	m_name_to_action.Clear			( );

	List<String^>^ gui_action_names	= m_input_engine->get_actions_names_list();
	gui_action_names->Sort			( );

	// build m_name_to_action dictionary
	for each ( String^ action_name in gui_action_names )
	{
		List<String^>^ shortcuts		= m_input_engine->get_action_keys_list_with_contexts( action_name );
		action_item_list^ new_items		= gcnew action_item_list( shortcuts->Count );
		
		for each ( String^ shortcut in shortcuts )
			new_items->Add		( gcnew action_item( action_name, shortcut ) );

		m_name_to_action[action_name]	= new_items;
	}

	contexts_combo_box->Items->Clear( );

	List<String^>^ contexts			= m_input_engine->contexts_manager->get_contexts_list();

	for each( String^ context in contexts)
		contexts_combo_box->Items->Add( context );

	contexts_combo_box->SelectedIndex	= 0;
	m_initialized						= true;
	enable_shortcuts_cb					( false );
	enable_assigned_cb					( false );
}

void options_page_input::activate_page( )
{
	filter_text_box->Text		= "";
	if(!m_initialized)
		initialize();

	m_keys_count		= 0;
	m_pass				= 0;

	update_actions_list	( );
	Visible				= true;
}

void options_page_input::deactivate_page( )
{
	Visible						= false;
	m_active_action				= "";
	clear_shortcut_text_box		( );
}

System::String^	options_page_input::exctract_first_keys( System::String^ keys )
{
	keys	= keys->Split('(')[0];
	int ind = keys->IndexOf(',');

	if( ind != -1 )
		return keys->Remove(ind);
	else
		return keys;
}

void options_page_input::actionslistBox_SelectedIndexChanged(System::Object^, System::EventArgs^ )
{	
	shortcuts_combo_box->Items->Clear();

	if( actions_list_box->SelectedIndex < 0 )
	{
		m_active_action = "";
	}else
	{
		m_active_action				= (System::String^)actions_list_box->Items[actions_list_box->SelectedIndex];
		action_item_list^ shortcuts = m_name_to_action[m_active_action];

		if( shortcuts != nullptr ) 
		{
			for each(action_item^ aitem in shortcuts)
				shortcuts_combo_box->Items->Add( aitem->action_shortcut );
		}
	}

	enable_shortcuts_cb					( shortcuts_combo_box->Items->Count!=0 );
}

void options_page_input::remove_button_Click( System::Object^ sender, System::EventArgs^ )
{
	if( shortcuts_combo_box->Text != "" && m_active_action != "" )
	{
		remove_action_shortcut				( m_active_action, shortcuts_combo_box->Text );
		actionslistBox_SelectedIndexChanged	( sender, nullptr );
		update_assigned_to_combo			( );
	}
}

bool options_page_input::add_action_shortcut( System::String^ name, System::String^ keys )
{
	action_item_list^ list			= nullptr;

	if( m_name_to_action.ContainsKey( name ) )
	{
		list = m_name_to_action[name];
	}else
	{
		list					= gcnew action_item_list();
		m_name_to_action[name]	= list;
	}

	for( int i = 0; i < list->Count; ++i )
		if( list[i]->action_shortcut == keys )
			return false;

	action_item^ new_item		= gcnew action_item( name, keys );
	list->Insert				( 0, new_item );

	m_changes_list.Add( gcnew action_item_change( enum_action_chage::add, name, keys ) );

	return true;
}

void options_page_input::remove_action_shortcut( System::String^ name, System::String^ keys )
{
	if( m_name_to_action.ContainsKey( name) )
	{
		action_item_list^ list	= m_name_to_action[name];

		for( int i = 0; i < list->Count; ++i )
		{
			if( list[i]->action_shortcut == keys )
			{
				list->RemoveAt(i);
				break;
			}
		}
	}
	m_changes_list.Add( gcnew action_item_change( enum_action_chage::remove, name, keys ) );
}

void options_page_input::shortcut_text_box_KeyUp(System::Object^, System::Windows::Forms::KeyEventArgs^  e) 
{
	e->Handled						= true;
	execute_key_up					( );
}

void options_page_input::shortcut_text_box_KeyDown( System::Object^, System::Windows::Forms::KeyEventArgs^  e) 
{
	e->Handled						= true;

	System::String^ tmp_str = "";
	int keys_count = m_input_engine->keys_holder->get_current_pressed_keys( tmp_str );

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
	}else
		execute_key_down	();

	shortcut_text_box->SelectionStart = shortcut_text_box->Text->Length;
}

void options_page_input::execute_key_down( )
{
	System::String^ tmp_str = "";
	int keys_count = m_input_engine->keys_holder->get_current_pressed_keys( tmp_str );

	tmp_str = tmp_str->Replace( "+None", "" );
	tmp_str = tmp_str->Replace( "None+", "" );

	if( m_pass >1 )
		clear_shortcut_text_box();

	if( m_keys_count < keys_count )
	{ 
		if( m_pass == 0 )
		{
			m_curr_shortcut_str					= tmp_str;
			shortcut_text_box->Text 			= "";
			shortcut_text_box->Text 			= m_curr_shortcut_str;
			shortcut_text_box->SelectionStart	= shortcut_text_box->Text->Length;
		}else
		{
			m_curr_shortcut_str					= exctract_first_keys( m_curr_shortcut_str );

			m_curr_shortcut_str					+= ", " + tmp_str;

			shortcut_text_box->Text				= "";
			shortcut_text_box->Text				= m_curr_shortcut_str;
			shortcut_text_box->SelectionStart	= shortcut_text_box->Text->Length;
		}

		m_keys_count = keys_count;
	}
}

void options_page_input::execute_key_up( )
{
	System::String^ tmp_str		= "";
	int keys_count				= m_input_engine->keys_holder->get_current_pressed_keys( tmp_str );

	if( keys_count == 0 && m_keys_count > 0 )
	{
		m_keys_count	= 0;
		m_pass			+= 1;
	}
}

void options_page_input::clear_shortcut_text_box( )
{
	m_pass								= 0;
	m_curr_shortcut_str					= "";
	shortcut_text_box->Text				= "";
	shortcut_text_box->Text				= m_curr_shortcut_str;
	shortcut_text_box->SelectionStart	= shortcut_text_box->Text->Length;
}

void options_page_input::shortcut_text_box_TextChanged( System::Object^, System::EventArgs^ )
{
	shortcut_text_box->SelectionStart	= shortcut_text_box->Text->Length;
	update_assigned_to_combo			( );
}

void options_page_input::shortcut_text_box_MouseDown( System::Object^, System::Windows::Forms::MouseEventArgs^ )
{
	if( !m_shortcut_bar_first_select )
		execute_key_down		( ); 

	m_shortcut_bar_first_select					= false;
	shortcut_text_box->SelectionStart			= shortcut_text_box->Text->Length;
}

void options_page_input::shortcut_text_box_MouseUp( System::Object^, System::Windows::Forms::MouseEventArgs^ )
{
	execute_key_up								( );
	shortcut_text_box->SelectionStart			= shortcut_text_box->Text->Length;
}

void options_page_input::update_assigned_to_combo( )
{
	assigned_to_comboBox->Items->Clear( );

	System::String^ filter_keys			= exctract_first_keys(shortcut_text_box->Text);

	for each ( action_item_list^ actions_list in m_name_to_action.Values)
	{
		for each ( action_item^ aitem in actions_list)
		{
			System::String^ fk = exctract_first_keys(aitem->action_shortcut);
			if(fk == filter_keys)
				assigned_to_comboBox->Items->Add( aitem->action_name + " (" + aitem->action_shortcut + ")");
		}
	}

	enable_assigned_cb(assigned_to_comboBox->Items->Count!=0);
}

void options_page_input::enable_shortcuts_cb( bool enable )
{
	if( enable )
	{
		shortcuts_combo_box->Enabled		= true;
		shortcuts_combo_box->SelectedIndex = 0;
	}else
	{
		shortcuts_combo_box->Items->Clear	( );
		shortcuts_combo_box->Items->Add	("No shortcut(s) assigned");
		shortcuts_combo_box->SelectedIndex = 0;
		shortcuts_combo_box->Enabled		= false;
	}

	shortcut_for_selected_label->Enabled = enable;
}

void options_page_input::enable_assigned_cb( bool enable )
{
	if( enable )
	{
		assigned_to_comboBox->Enabled		= true;
		assigned_to_comboBox->SelectedIndex = 0;
	}else
	{
		assigned_to_comboBox->Items->Clear	( );
		assigned_to_comboBox->Items->Add	("No action(s) assigned");
		assigned_to_comboBox->SelectedIndex = 0;
		assigned_to_comboBox->Enabled		= false;
	}

	asigned_to_label->Enabled = enable;
}

void options_page_input::assign_button_Click( System::Object^  sender, System::EventArgs^ )
{
	if( m_active_action != "" && shortcut_text_box->Text != "" )
	{
		if( !add_action_shortcut( m_active_action, shortcut_text_box->Text + "(" + contexts_combo_box->SelectedItem + ")") ) 
		{
			System::Windows::Forms::MessageBox::Show( this, "The shortcut is already assigned to the action.", 
									"Options", 
									System::Windows::Forms::MessageBoxButtons::OK );
		}
	}

	actionslistBox_SelectedIndexChanged	( sender, nullptr );
	clear_shortcut_text_box				( );
}

void options_page_input::update_actions_list( )
{
	actions_list_box->Items->Clear();
	System::String^ substring = filter_text_box->Text->ToLower();

	for each (System::String^ name in m_name_to_action.Keys )
	{
		if( name->ToLower()->IndexOf(substring) != -1 )
			actions_list_box->Items->Add( name );
	}
	
	actionslistBox_SelectedIndexChanged( nullptr, nullptr );
}

void options_page_input::filter_text_box_TextChanged( System::Object^, System::EventArgs^ )
{
	update_actions_list					( );
}

void options_page_input::shortcut_text_box_Enter(System::Object^, System::EventArgs^ )
{
}

void options_page_input::default_button_Click(System::Object^, System::EventArgs^ )
{
	m_input_engine->reset_to_defaults	( );
	m_initialized						= false;
	activate_page						( );
}

void options_page_input::shortcut_text_box_Leave( System::Object^, System::EventArgs^ ) 
{
	m_shortcut_bar_first_select			= true;
}

void options_page_input::shortcut_text_box_PreviewKeyDown( System::Object^, System::Windows::Forms::PreviewKeyDownEventArgs^ e ) 
{
	e->IsInputKey						= true;
}

void options_page_input::shortcut_text_box_MouseCaptureChanged( System::Object^, System::EventArgs^ )
{
	shortcut_text_box->Capture			= false;
}

} // namespace editor_base
} // namespace xray

