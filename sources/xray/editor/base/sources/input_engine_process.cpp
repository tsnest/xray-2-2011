////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_engine.h"
#include "input_keys_holder.h"

#include "action_single.h"
#include "action_shortcut.h"
#include "actions_layer.h"

#include <xray/editor/base/i_context_manager.h>

using namespace System;

namespace xray {
namespace editor_base {

//////////////////////////////////////			P U B L I C   M E T H O D S			//////////////////////////////////////////

void				input_engine::execute				( )
{
	if( m_active_shortcuts == nullptr )
	{
		for each( String^ combination in m_input_key_holder->key_combinations )
		{
			if( process_combination( combination ) )
				break;
		}
	}
	
	if ( m_active_shortcuts == nullptr )
		return;

	if( !m_waiting_for_second_combination )
	{
		do
		{
			String^ current_keys_string = m_input_key_holder->get_current_pressed_keys( );

			if( m_active_shortcut == current_keys_string )
			{
				m_active_shortcuts[m_active_shortcut_index]->m_action->execute( );
			}
			else
			{
				m_active_shortcuts[m_active_shortcut_index]->m_action->release( );
				m_active_shortcuts						= nullptr;
				m_active_shortcut						= nullptr;
				m_active_shortcut_index					= -1;

				process_combination( current_keys_string );
				break;
			}
		}while( m_input_key_holder->has_unprocessed_keys( ) );

	}
	else
	{
		for each( String^ combination in m_input_key_holder->key_combinations )
		{
			for( int i = m_active_shortcut_index; i < m_active_shortcuts->Count; ++i )
			{
				ASSERT( m_active_shortcuts[i]->m_second_keys != nullptr );

				action_shortcut^ shortcut = m_active_shortcuts[i];

				if( ( combination == shortcut->m_second_keys ) && 
					m_context_manager->context_fit( shortcut->m_context ) && 
					shortcut->m_action->enabled( ) && 
					shortcut->m_action->capture( ) )
				{
					m_active_shortcut_index				= i;
					m_waiting_for_second_combination	= false;
					
					break;
				}
			}
		}
	}
}

//////////////////////////////////////			P R I V A T E   M E T H O D S			//////////////////////////////////////////

bool				input_engine::process_combination	( String^ combination )
{
	if( m_paused )
		return		false;

	if ( combination == "None" )
		return		false;

	System::Windows::Input::KeyboardDevice^ device		= System::Windows::Input::Keyboard::PrimaryDevice;
	System::Windows::IInputElement^			target		= device->Target;

	if( target != nullptr && xray::editor::wpf_controls::input::RaiseInputCommandEvent( combination, target ) )
		return		true;

	List<action_shortcut^>^ available_shortcuts;

	for ( s32 i = m_active_action_layers->Count - 1; i >= 0; --i )
	{
		available_shortcuts = m_active_action_layers[i]->find_action_shortcuts_by_first_keys( combination );

		if( available_shortcuts->Count == 0 )
			continue;
	
		for( int i = 0; i < available_shortcuts->Count; ++i )
		{
			action_shortcut^ shortcut = available_shortcuts[i];
			
			if( shortcut->m_second_keys != nullptr )
			{
				m_waiting_for_second_combination	= true;
				m_active_shortcut_index				= i;
				m_active_shortcuts					= available_shortcuts;
				m_first_keys_released				= false;

				return		true;
			}
		
			else if( m_context_manager->context_fit( shortcut->m_context ) && shortcut->m_action->enabled( ) )
			{
				if( shortcut->m_action->capture( ) )
				{
					m_active_shortcuts 			= available_shortcuts;
					m_active_shortcut_index		= i;
					m_active_shortcut				= combination;

					return		true;
				}
			}
		}
	}
	return		false;
}

} // namespace editor_base
} // namespace xray