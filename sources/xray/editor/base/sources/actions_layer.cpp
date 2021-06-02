////////////////////////////////////////////////////////////////////////////
//	Created		: 29.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "actions_layer.h"
#include "action.h"
#include "action_shortcut.h"

namespace xray{
namespace editor_base{
	
actions_layer::actions_layer ( )
{
	m_action_registry = gcnew Dictionary<String^, action^>( );
}
actions_layer::actions_layer( u32 capacity )
{
	m_action_registry = gcnew Dictionary<String^, action^>( capacity );
}

void actions_layer::register_action ( xray::editor_base::action^ action, String^ shorcuts )
{
	R_ASSERT	( action != nullptr );
	R_ASSERT	( !m_action_registry->ContainsKey( action->name( ) ) );

	action->m_default_shortcut			= shorcuts;
	m_action_registry->Add				( action->name( ), action );

	if ( shorcuts == "" )
		return;
	
	array<String^ >^ segments = shorcuts->Split('|');
	
	for each( String^ str_shortcut in segments )
		add_action_shortcut( action->name( ), str_shortcut );
}

void actions_layer::unregister_action ( String^ name )
{
	if( m_action_registry->ContainsKey( name ) )
	{
		action^ action				= m_action_registry[name];
		m_action_registry->Remove	( name );

		delete action;
	}
}

bool actions_layer::action_exists ( String^ name )
{
	return ( m_action_registry->ContainsKey( name ) );
}

action^ actions_layer::get_action ( String^ name )
{
	action^ action;
	m_action_registry->TryGetValue( name, action );

	R_ASSERT( action );
	
	return action;
}

inline List<String^>^ actions_layer::get_actions_names_list	( )
{
	List<String^>^ result = gcnew List<String^>( );
	for each ( String^ action_name in m_action_registry->Keys )
		result->Add( action_name );

	return result;
}

List<String^>^ actions_layer::get_action_keys_list ( String^ name )
{
	R_ASSERT					( action_exists ( name ) );
	return m_action_registry[name]->get_action_keys_list( );
}

List<String^>^ actions_layer::get_action_keys_list_with_contexts ( String^ name )
{
	R_ASSERT				( action_exists( name ) );
	return m_action_registry[name]->get_action_keys_list_with_contexts( );
}

bool actions_layer::add_action_shortcut ( String^ name, String^ combination )
{
	R_ASSERT				( action_exists( name ) );
	R_ASSERT				( combination->Length );

	action^ action			= m_action_registry[name];

	for each( action_shortcut^ shortcut in action->m_shortcuts )
	{
		if( shortcut->m_combination == combination )
		{
			R_ASSERT( false, "An action with the specified name and keys is already registered!" );
			return false;
		}
	}

	action->m_shortcuts->Add	( gcnew action_shortcut( action, combination ) );

	return				true;
}


bool actions_layer::remove_action_shortcut ( String^ name, String^ combination )
{
	R_ASSERT				( action_exists ( name ) );
	action^ action			= m_action_registry[name];

	u32 count = action->m_shortcuts->Count;
	for ( u32 i = 0; i < count; ++i )
	{
		if( action->m_shortcuts[i]->m_combination == combination )
		{
			action->m_shortcuts->RemoveAt( i );
			return true;
		}
	}

	return false;
}

void actions_layer::reset_to_defaults ( )
{
	for each( action^ action in m_action_registry->Values )
	{
		if( action->m_shortcuts != nullptr )
			action->m_shortcuts->Clear( );

		if( action->m_default_shortcut == "" )
			continue;

		array<String^>^ segments = action->m_default_shortcut->Split( '|' );

		for each( String^ str_shortcut in segments )
			add_action_shortcut( action->name( ), str_shortcut );
	}
}

bool actions_layer::get_actions_by_shortcut ( String^ combination, [Out] List<String^>^% actions, [Out] List<String^>^% shortcuts )
{
	actions										= gcnew List<String^>( );
	shortcuts									= gcnew List<String^>( );
	action_shortcut^ shortcut					= gcnew action_shortcut( nullptr, combination );
	List<action_shortcut^>^ available_shortcuts = find_action_shortcuts_by_first_keys( shortcut->m_first_keys );

	for ( int i = 0; i < available_shortcuts->Count; i++ )
	{
		actions->Add			( available_shortcuts[i]->m_action->name( ) );
		String^ shortcut_str	= available_shortcuts[i]->m_first_keys;

		if( available_shortcuts[i]->m_second_keys != nullptr ) 
			shortcut_str += "" + available_shortcuts[i]->m_second_keys;

		shortcuts->Add ( shortcut_str );
	}

	return actions->Count > 0;
}

bool actions_layer::get_actions_by_shortcut ( String^ combination, [Out] List<String^>^% actions )
{
	actions										= gcnew List<String^>( );
	action_shortcut^ shortcut					= gcnew action_shortcut( nullptr, combination );
	List<action_shortcut^>^ available_shortcuts = find_action_shortcuts_by_first_keys( shortcut->m_first_keys );

	for ( int i = 0; i < available_shortcuts->Count; i++ )
		if( available_shortcuts[i]->m_second_keys == shortcut->m_second_keys )
			actions->Add ( available_shortcuts[i]->m_action->name( ) );

	return actions->Count > 0;
}

List<action_shortcut^>^ actions_layer::find_action_shortcuts_by_first_keys( String^ first_keys )
{
	List<action_shortcut^>^ result_lst = gcnew List<action_shortcut^>( );

	for each ( action^ action in m_action_registry->Values )
	{
		if( action->m_shortcuts == nullptr )
			continue;

		for each( action_shortcut^ shortcut in action->m_shortcuts )
		{
			if ( shortcut->m_first_keys == first_keys )
				result_lst->Add( shortcut );
		}
	}
	return result_lst;
}

} // namespace editor_base
} // namespace xray