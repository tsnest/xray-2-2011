////////////////////////////////////////////////////////////////////////////
//	Created		: 21.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_keys_holder.h"
using namespace System::Windows::Forms;

namespace xray {
namespace editor {

Keys mouse_button_to_key( MouseButtons mouse_button )
{
	switch (mouse_button)
	{
	case MouseButtons::Left:
		return Keys::LButton;
	case MouseButtons::Right:
		return Keys::RButton;
	case MouseButtons::Middle:
		return Keys::MButton;
	case MouseButtons::XButton1:
		return Keys::XButton1;
	case MouseButtons::XButton2:
		return Keys::XButton2;
	default:
		return Keys::None;
	}
}

void input_keys_holder::register_key_event( KeyEventArgs^  e, bool bpress )
{
	if(bpress)
	{
		if( !m_held_keys.Contains( e->KeyCode ))
		{
			m_held_keys.Add( e->KeyCode);
			combination_changed	( );
		}

	}else
	{
		if( m_held_keys.Contains( e->KeyCode) )
		{
			m_held_keys.Remove	( e->KeyCode );
			combination_changed	( );
		}
	}
}

void input_keys_holder::register_mouse_event( System::Windows::Forms::MouseEventArgs^ e, bool bpress )
{
	Keys key = mouse_button_to_key(e->Button);
	if(bpress)
	{
		if(!m_held_keys.Contains(key))
		{
			m_held_keys.Add		(key);
			combination_changed	( );
		}
	}else
	{
 		if(m_held_keys.Contains(key))
		{
			m_held_keys.Remove	(key);
			combination_changed	( );
		}
	}
}

void input_keys_holder::reset( )
{
	m_held_keys.Clear();
}

System::Collections::ArrayList^ input_keys_holder::get_held_keys( )
{
	return %m_held_keys;
}

void extract_modifyer_keys( System::Collections::ArrayList^ keys, Keys %modifiers )
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

System::String^	input_keys_holder::combine_string( Keys modifiers, System::Collections::ArrayList^ keys )
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

int	input_keys_holder::get_current_keys_string( System::String^& combination )
{
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

void input_keys_holder::combination_changed( )
{
	System::String^			status;
	get_current_keys_string	( status );
	m_changed_delegate		( status );
}

void input_keys_holder::subscribe_on_changed( keys_combination_changed^ d )
{
	m_changed_delegate	+= d;
}

} // namespace editor
} // namespace xray
