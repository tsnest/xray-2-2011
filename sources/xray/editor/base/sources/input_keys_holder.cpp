////////////////////////////////////////////////////////////////////////////
//	Created		: 21.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_keys_holder.h"

using System::Collections::Generic::List;

namespace xray {
namespace editor_base {


void reset_input_keys_holder( System::Object^, System::EventArgs^ )
{
	input_keys_holder::ref->reset();
}

void input_keys_holder::create( )
{
	m_ref							= gcnew input_keys_holder( );
	Application::AddMessageFilter	( m_ref );
	m_ref->m_last_active_form		= Form::ActiveForm;
	Application::LeaveThreadModal	+= gcnew System::EventHandler( &reset_input_keys_holder );
}

input_keys_holder::input_keys_holder			( )
{
	m_iterator = gcnew keys_iterator ();
}

input_keys_holder::~input_keys_holder			( )
{
	Application::LeaveThreadModal -= gcnew System::EventHandler(&reset_input_keys_holder);
}

void		input_keys_holder::register_down_key_event		( Keys key )
{
	if( m_held_keys.Contains( key ) )
		return;

	bool do_not_add = false;
	
	for each( input_event e in m_input_events )
	{
		if(e.m_key->KeyData == key)
			do_not_add = e.m_key_down;
	}

	if( do_not_add )
		return;

	input_event		ev;
	ev.m_key		= gcnew KeyEventArgs(key);
	ev.m_key_down	= true;

	for each(input_handler^ h in m_external_input_handlers)
		if( h->on_key_down( ev.m_key ) )
			return; //handled

	m_input_events.Enqueue( ev );
	//LOGI_INFO( "keys_holder", unmanaged_string( "Key Down: " + key.ToString( ) ).c_str( ) );
}

void		input_keys_holder::register_up_key_event		( Keys key )
{
	input_event		ev;
	ev.m_key		= gcnew KeyEventArgs(key);
	ev.m_key_down	= false;

	for each(input_handler^ h in m_external_input_handlers)
		if( h->on_key_up( ev.m_key ) )
			return; //handled

	m_input_events.Enqueue( ev );
	//LOGI_INFO( "keys_holder", unmanaged_string( "Key Up: " + key.ToString( ) ).c_str( ) );
}

void		input_keys_holder::register_mouse_down_event	( Keys key )
{
	switch( key )
	{
		case Keys::LButton:		m_l_button_last_down = true; break;
		case Keys::RButton:		m_r_button_last_down = true; break;
		case Keys::MButton:		m_m_button_last_down = true; break;
		case Keys::XButton1:	m_x1_button_last_down = true; break;
		case Keys::XButton2:	m_x2_button_last_down = true; break;
	}

	input_event		ev;
	ev.m_key		= gcnew KeyEventArgs(key);
	ev.m_key_down	= true;

	m_input_events.Enqueue( ev );
	//LOGI_INFO( "keys_holder", unmanaged_string( "Mouse Down: " + key.ToString( ) ).c_str( ) );
}

void		input_keys_holder::register_mouse_up_event		( Keys key )
{
	switch( key )
	{
		case Keys::LButton:		m_l_button_last_down = false; break;
		case Keys::RButton:		m_r_button_last_down = false; break;
		case Keys::MButton:		m_m_button_last_down = false; break;
		case Keys::XButton1:	m_x1_button_last_down = false; break;
		case Keys::XButton2:	m_x2_button_last_down = false; break;
	}

	input_event		ev;
	ev.m_key		= gcnew KeyEventArgs(key);
	ev.m_key_down	= false;

	m_input_events.Enqueue( ev );
	//LOGI_INFO( "keys_holder", unmanaged_string( "Mouse Up: " + key.ToString( ) ).c_str( ) );
}

void		input_keys_holder::subscribe_input_handler		( input_handler^ h, bool bsubscribe )
{
	if(bsubscribe)
		m_external_input_handlers.Add( h );
	else
		m_external_input_handlers.Remove( h );
}


void		input_keys_holder::subscribe_on_changed			( keys_combination_changed^ d )
{
	m_changed_delegate	+= d;
}

void		input_keys_holder::reset						( )
{
	m_input_events.Clear	( );
	m_held_keys.Clear		( );
	combination_changed		( );
}

void		input_keys_holder::reset						( Object^ , EventArgs^ )
{
	reset( );
}

bool		input_keys_holder::PreFilterMessage				( System::Windows::Forms::Message% msg )
{
	System::Windows::Forms::Message m = msg;

	switch( (input_events)m.Msg )
	{
	case input_events::key_down:
	case input_events::sys_key_down:
		register_down_key_event( (Keys)(int)m.WParam & Keys::KeyCode );
		break;
	case input_events::key_up:
	case input_events::sys_key_up:
		register_up_key_event( (Keys)(int)m.WParam & Keys::KeyCode );
		break;
	case input_events::left_mouse_down:
		register_mouse_down_event( Keys::LButton );
		break;
	case input_events::left_mouse_up:
		register_mouse_up_event( Keys::LButton );
		break;
	case input_events::middle_mouse_down:
		register_mouse_down_event( Keys::MButton );
		break;
	case input_events::middle_mouse_up:
		register_mouse_up_event( Keys::MButton );
		break;
	case input_events::right_mouse_down:
		register_mouse_down_event( Keys::RButton );
		break;
	case input_events::right_mouse_up:
		register_mouse_up_event( Keys::RButton );
		break;
	case input_events::x_mouse_down:
		register_mouse_down_event( ( m.WParam.ToInt32( ) >> 16 == 1 ) ? Keys::XButton1 : Keys::XButton2 );
		break;
	case input_events::x_mouse_up:
		register_mouse_up_event( ( m.WParam.ToInt32( ) >> 16 == 1 ) ? Keys::XButton1 : Keys::XButton2 );
		break;
	case input_events::mouse_move:
		if( (Keys)(int)m.WParam == Keys::None )
		{
			if( m_l_button_last_down )
				register_mouse_up_event( Keys::LButton );

			if( m_r_button_last_down )
				register_mouse_up_event( Keys::RButton );

			if( m_m_button_last_down )
				register_mouse_up_event( Keys::MButton );

			if( m_x1_button_last_down )
				register_mouse_up_event( Keys::XButton1 );

			if( m_x2_button_last_down )
				register_mouse_up_event( Keys::XButton2 );
		}
		break;
	}

	//MK_CONTROL	0x0008
	//MK_LBUTTON	0x0001
	//MK_MBUTTON	0x0010
	//MK_RBUTTON	0x0002
	//MK_SHIFT		0x0004
	//MK_XBUTTON1	0x0020
	//MK_XBUTTON2	0x0040
	
	return false;
}



List<Keys>^	input_keys_holder::get_held_keys				( )
{
	if( Form::ActiveForm != m_last_active_form )
	{
		m_last_active_form = Form::ActiveForm;
		reset( );
	}
	return %m_held_keys;
}

void		extract_modifyer_keys							( ArrayList^ keys, Keys %modifiers )
{
	int ind = keys->IndexOf( Keys::ControlKey );
	if( ind != -1 ) 
	{
		modifiers = modifiers|Keys::Control;
		keys->RemoveAt( ind );
	}

	ind = keys->IndexOf( Keys::ShiftKey );
	if( ind != -1 ) 
	{
		modifiers = modifiers|Keys::Shift;
		keys->RemoveAt( ind );
	}

	ind = keys->IndexOf( Keys::Menu ); //AltKey
	if( ind != -1 ) 
	{
		modifiers = modifiers|Keys::Alt;
		keys->RemoveAt( ind );
	}
}

bool		input_keys_holder::peek_registered_key			( )
{
	bool ret_val = false;
	int count = m_held_keys.Count;

	for( ; m_input_events.Count > 0; )
	{
		input_event entry		= m_input_events.Dequeue( );
	
		if( entry.m_key_down )
		{
			if( !m_held_keys.Contains( entry.m_key->KeyData ) )
			{
				m_held_keys.Add		( entry.m_key->KeyData );

				Keys key = entry.m_key->KeyData;
				if( key != Keys::LShiftKey &&	key != Keys::RShiftKey &&
					key != Keys::LControlKey &&	key != Keys::RControlKey &&
					key != Keys::LMenu &&		key != Keys::RMenu )
				{
					ret_val = true;
				}
			}
			break;
		}else
		{
			m_held_keys.Remove	( entry.m_key->KeyData );
		}
	}

	if( count > 0 && m_held_keys.Count == 0 )
		ret_val = true;

	//if( ret_val )
	//	LOGI_INFO( "keys_holder", unmanaged_string( "Key Picked: " + ( ( m_held_keys.Count > 0 ) ? m_held_keys[m_held_keys.Count-1].ToString( ) : "empty" ) ).c_str( ) );

	return ret_val;
}

String^		input_keys_holder::combine_string				( Keys modifiers, ArrayList^ keys )
{
	System::String^ result = gcnew System::String("");
	keys->Sort();
	
	if( keys->Count > 0)
		result = m_key_converter.ConvertToString( modifiers|((Keys)keys[0]) );
	else
		result = m_key_converter.ConvertToString( modifiers );

	for ( int i = 1; i < keys->Count; ++i  )
	{
		result += "+" + m_key_converter.ConvertToString( keys[i] );
	}

	return result;
}

int			input_keys_holder::get_current_keys_string		( System::String^& combination )
{
	if( Form::ActiveForm != m_last_active_form )
	{
		m_last_active_form = Form::ActiveForm;
		reset( );
	}

	System::Collections::ArrayList keys			= %m_held_keys;

	Keys modifiers			= Keys::None;

	int count				= keys.Count;

	// Extract modifier keys to view them as modifiers (not keys).
	extract_modifyer_keys	( %keys, modifiers );
	count					-= keys.Count;

	// Create normalized key combination string to find it in the actions list.
	combination				= combine_string	( modifiers, %keys );

	return					keys.Count + count;
}

void		input_keys_holder::combination_changed			( )
{
	System::String^			status;
	get_current_keys_string	( status );
	m_changed_delegate		( status );
}

String^		input_keys_holder::get_current_pressed_keys		( )
{
	String^ combination;

	if( input_keys_holder::ref->peek_registered_key	( ) )
	{
		input_keys_holder::ref->get_current_keys_string	( combination );
		m_last_combination_string = combination;
		combination_changed	( );
	}
	else
	{
		input_keys_holder::ref->get_current_keys_string	( combination );
		return combination;
	}

	return m_last_combination_string;
}

Int32		input_keys_holder::get_current_pressed_keys		( String^& str )
{
	str = get_current_pressed_keys( );
	return m_held_keys.Count;
}

bool		input_keys_holder::has_unprocessed_keys			( )
{
	return m_input_events.Count > 0;
}
////////////////////////////////////    I T E R A T O R     /////////////////////////////////////////

Object^		input_keys_holder::keys_iterator::Current::get	( )
{
	return m_current_string;
}

bool		input_keys_holder::keys_iterator::MoveNext		( )
{
	bool ret_val = false;

	if( input_keys_holder::ref->m_input_events.Count > 0 )
	{
		ret_val = input_keys_holder::ref->peek_registered_key		( );

		if( ret_val )
		{
			String^ combination;
			input_keys_holder::ref->get_current_keys_string	( combination );
			input_keys_holder::ref->m_last_combination_string = combination;
			m_current_string = combination;
			input_keys_holder::ref->combination_changed	( );
		}
	}

	return ret_val;
}

void		input_keys_holder::keys_iterator::Reset			( )
{

}

} //namespace editor_base
} // namespace xray