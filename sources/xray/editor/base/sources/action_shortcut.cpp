////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "action_shortcut.h"
#include "action.h"

namespace xray{
namespace editor_base{

//////////////////////////////////////				I N I T I A L I Z E				//////////////////////////////////////////

action_shortcut::action_shortcut( action^ action, String^ combination )
{
	m_action				= action;
	m_combination			= combination;

	excract_shortcut_pair	( combination, m_first_keys, m_second_keys, m_context );
}

//////////////////////////////////////			P U B L I C   M E T H O D S			//////////////////////////////////////////

String^ action_shortcut::joined_keys( )
{
	if( m_second_keys == nullptr)
		return m_first_keys;
	else
		return m_first_keys + ", " + m_second_keys; 
}

String^ action_shortcut::joined_keys_with_context( )
{
	if( m_second_keys == nullptr )
		return m_first_keys + "(" + m_context + ")";
	else
		return m_first_keys + ", " + m_second_keys + "(" + m_context + ")"; 
}

//////////////////////////////////////			P R I V A T E   M E T H O D S			//////////////////////////////////////////

void action_shortcut::excract_shortcut_pair	( String^ combination, String^% keys_first, String^% keys_second, String^% context )
{
	cli::array<String^>^ keys_and_context = combination->Split( '(' );
	if ( keys_and_context->Length > 1 )
		context = keys_and_context[1]->Substring( 0, keys_and_context[1]->Length - 1 );
	else
		context = "Global";

	array<String^>^ keys = keys_and_context[0]->Split(',');

	keys_first = nullptr;
	if( keys->Length == 0 || keys->Length > 2)
	{
		ASSERT( false, "Incorrect keys combination." );
		return;
	}

	// Arrange keys in a correct order and case.
	keys_first	= arrange_keys( keys[0] );

	// The same with the second key combination
	keys_second = nullptr;
	if( keys->Length == 2 )
	{
		ASSERT( keys[1] != "" );
		keys_second = arrange_keys( keys[1] );
		ASSERT( keys_second != nullptr && keys_second != "" );
	}
}

String^ action_shortcut::arrange_keys ( String^ keys_string )
{
	array<System::String^>^	split			= keys_string->Split( '+' );
	System::Collections::ArrayList^ keys	= gcnew System::Collections::ArrayList();

	Keys modifiers = Keys::None;

	for( int i = 0; i < split->Length; ++i )
	{
		System::String^ s = split[i];
		Keys key;
		if (  !s->Trim( )->Equals( "" ) && ( key = try_convert( s ) ) != Keys::None )
		{
			if( ( key & Keys::Modifiers ) != Keys::None )
			{
				modifiers = modifiers|(key&Keys::Modifiers);
			}
			else if( ( key & Keys::KeyCode ) != Keys::None && !keys->Contains( key & Keys::KeyCode ) )
				keys->Add( key&Keys::KeyCode );
		}
		else
			return nullptr;
	}

	return combine_string( modifiers,  keys );
}

Keys action_shortcut::try_convert ( String^ str )
{
	Keys key = Keys::None;
	try
	{
		key = *( (Keys^)s_key_converter.ConvertFromString( str ) );
	}
	catch(...)
	{
		return Keys::None;
	}

	return key;
}

String^ action_shortcut::combine_string ( Keys modifiers, ArrayList^ keys )
{
	String^ result = "";
	keys->Sort( );
	
	if( keys->Count > 0 )
		result = s_key_converter.ConvertToString( modifiers | ( (Keys)keys[0] ) );
	else
		result = s_key_converter.ConvertToString( modifiers );

	for ( int i = 1; i < keys->Count; ++i  )
	{
		result += "+" + s_key_converter.ConvertToString( keys[i] );
	}

	return result;
}

} // namespace editor
} // namespace xray