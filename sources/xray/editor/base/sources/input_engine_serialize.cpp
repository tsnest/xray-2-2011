////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_engine.h"
#include "action_single.h"
#include "action_shortcut.h"
#include "actions_layer.h"

using namespace System;
using Microsoft::Win32::RegistryKey;

namespace xray {
namespace editor_base {

#define default_shortcuts_ver "1.0"

RegistryKey^ get_sub_key( RegistryKey^ root, String^ name )
{
	RegistryKey^ result = root->OpenSubKey( name, true );
	if( !result )
		result			= root->CreateSubKey( name );

	return result;
}
bool input_engine::is_correct_settings_value( String^ value )
{
	array<String^>^ parts	= value->Split( '-' );
	String^ action_name		= parts[0];
	String^ action_shortcut = parts[1];

	if ( parts->Length == 1 ) //has no symbol '-' between name and value
		return false;

	if ( action_name->Length == 0 )
		return false;

	// it is valud record, when user un-binded default shortcut
	//if (action_shortcut->Length == 0)
	//	return false;

	if( !action_exists( action_name ) )
		return false;

	if( action_shortcut->Length )
	{
		parts = action_shortcut->Split( '(' );	
		
		if( parts->Length != 2 )
			return false;

		String^ last_symbol = parts[1]->Substring( parts[1]->Length - 1, 1 );

		if ( last_symbol != ")" )
			return false;
	}

	return true;
}

void input_engine::load( RegistryKey^ base_product_key )
{
	bool reset_to_default = true;

	m_registry_key				= get_sub_key( base_product_key, "keybindings" );
	RegistryKey^ registry_key	= m_registry_key;

	array<String^>^	value_names	= registry_key->GetValueNames();

	for each( String^ name in value_names )
	{
		String^ value				= safe_cast<String^>( registry_key->GetValue( name ) );
		if( name == "shortcuts_ver" )
		{
			String^ ver_value		= safe_cast<String^>( registry_key->GetValue( name ) );

			if( ver_value == default_shortcuts_ver )
				reset_to_default = false;

		}
		else if ( !is_correct_settings_value( value ) )
		{
			registry_key->DeleteValue( name );
		}
		else
		{
			array<String^>^ action_name_and_shortcut	= value->Split( '-' );
			
			String^ action_name = action_name_and_shortcut[0];

			if( m_default_actions_layer->m_action_registry->ContainsKey( action_name ) )
			{
				action^ r = m_default_actions_layer->m_action_registry[action_name];
				if( r->m_shortcuts != nullptr )
					r->m_shortcuts->Clear( );
			}
		}
	}

	value_names		= registry_key->GetValueNames( );
	for each( String^ name in value_names )
	{
		if( name == "shortcuts_ver" )
			continue;

		String^ value				= dynamic_cast<String^>( registry_key->GetValue( name ) );

		array<String^>^ str_pair	= value->Split( '-' );
		ASSERT								( str_pair->Length == 2 );
		add_action_shortcut					( str_pair[0]->Trim( ), str_pair[1]->Trim( ) );
	}

	if( reset_to_default )
		reset_to_defaults( );
}

void input_engine::save( )
{
	RegistryKey^ registry_key	= m_registry_key;
	array<String^>^	value_names	= registry_key->GetValueNames( );

	for each( String^ name in value_names )
		registry_key->DeleteValue( name );

	u32 index = 0;

	for each( action^ action in m_default_actions_layer->m_action_registry->Values )
	{
		if( action->m_shortcuts && action->m_shortcuts->Count )
		{
			for each( action_shortcut^ item in action->m_shortcuts )
			{		
				String^ binding_str			= item->m_action->name( ) + "-" + item->joined_keys_with_context( );
				registry_key->SetValue( index.ToString( "D4" ), binding_str );
				index++;
			}
		}
		else
		{
			if( action->m_default_shortcut->Length )
			{
				String^ binding_str			= action->name( ) + "-";
				registry_key->SetValue		( index.ToString( "D4" ), binding_str );
				index++;
			}
		}
	}

	registry_key->SetValue( "shortcuts_ver", default_shortcuts_ver );
}

} //namespace editor_base
} //namespace xray
