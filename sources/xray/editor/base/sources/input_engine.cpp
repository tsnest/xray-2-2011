////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "input_engine.h"
#include "action_single.h"
#include "action_shortcut.h"
#include "actions_layer.h"

namespace xray {
namespace editor_base {

input_engine::input_engine	( input_keys_holder^  key_holder, i_context_manager^ context_manager ): 
	m_input_key_holder					( key_holder ),
	m_context_manager					( context_manager ),
	m_default_actions_layer				( gcnew actions_layer( 60 ) ),
	m_action_layers						( gcnew List<actions_layer^>( ) ),
	m_active_action_layers				( gcnew List<actions_layer^>( ) ),
	m_active_shortcut					( nullptr ),
	m_active_shortcuts					( nullptr ),
	m_active_shortcut_index				( -1 ),
	m_waiting_for_second_combination	( false ),
	m_first_keys_released				( true ),
	m_paused							( false )
{
	m_action_layers->Add		( m_default_actions_layer );
	m_active_action_layers->Add	( m_default_actions_layer );
}

input_engine::~input_engine	( )
{
	for each ( action^ action in m_default_actions_layer->m_action_registry->Values )
		delete action;

	m_default_actions_layer->m_action_registry->Clear	( );
}

void input_engine::execute_action ( String^ name )
{
	action^ action = get_action( name );

	if( action != nullptr && action->enabled( ) ) 
		action->do_it( );
}

void input_engine::execute_action ( action^ action )
{
	if( action != nullptr && action->enabled( ) ) 
		action->do_it( );
}

void input_engine::stop ( )
{
	ASSERT( !m_paused, "The engine is already stopped." );

	m_paused = true;
	if( m_active_shortcut )
	{
		m_active_shortcuts[m_active_shortcut_index]->m_action->release( );
		m_active_shortcut							= nullptr;
		m_active_shortcut_index					= -1;
	}
	m_waiting_for_second_combination			= false;
}

void input_engine::resume ( )
{
	ASSERT( m_paused, "The engine is not stopped." );
	m_paused = false;
}

void input_engine::register_action ( xray::editor_base::action^ action, String^ shorcuts )
{
	m_default_actions_layer->register_action( action, shorcuts );
}

void input_engine::unregister_action ( String^ name )
{
	m_default_actions_layer->unregister_action( name );
}

bool input_engine::action_exists ( String^ name )
{
	return m_default_actions_layer->action_exists( name );
}

action^ input_engine::get_action ( String^ name )
{
	return m_default_actions_layer->get_action( name );
}

inline List<String^>^ input_engine::get_actions_names_list	( )
{
	return m_default_actions_layer->get_actions_names_list( );
}

List<String^>^ input_engine::get_action_keys_list ( String^ name )
{
	return m_default_actions_layer->get_action_keys_list( name );
}

List<String^>^ input_engine::get_action_keys_list_with_contexts ( String^ name )
{
	return m_default_actions_layer->get_action_keys_list_with_contexts( name );
}

bool input_engine::add_action_shortcut ( String^ name, String^ combination )
{
	return m_default_actions_layer->add_action_shortcut( name, combination );
}

bool input_engine::remove_action_shortcut ( String^ name, String^ combination )
{
	return m_default_actions_layer->remove_action_shortcut( name, combination );
}

void input_engine::reset_to_defaults ( )
{
	m_default_actions_layer->reset_to_defaults( );
}

void input_engine::add_actions_layer ( actions_layer^ layer )
{
	R_ASSERT( !m_action_layers->Contains( layer ) );

	m_action_layers->Add			( layer );
}

void input_engine::remove_actions_layer ( actions_layer^ layer )
{
	m_action_layers->Remove			( layer );
}

void input_engine::activate_actions_layer ( actions_layer^ layer )
{
	R_ASSERT( !m_active_action_layers->Contains( layer ) );
	R_ASSERT( m_action_layers->Contains( layer ) );

	m_active_action_layers->Add		( layer );
}

void input_engine::deactivate_actions_layer	( actions_layer^ layer )
{
	m_active_action_layers->Remove	( layer );
}

//bool input_engine::get_actions_by_shortcut ( String^ combination, [Out] List<String^>^% actions, [Out] List<String^>^% shortcuts )
//{
//	return m_default_actions_layer->get_actions_by_shortcut( combination, actions, shortcuts );
//}
//
//bool input_engine::get_actions_by_shortcut ( String^ combination, [Out] List<String^>^% actions )
//{
//	return m_default_actions_layer->get_actions_by_shortcut( combination, actions );
//}


} // namespace editor_base
} // namespace xray