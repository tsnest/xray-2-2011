////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "action_engine.h"
#include "editor_world.h"
#include "input_engine.h"
#include "action_single.h"


using namespace System;

namespace xray {
namespace editor {

action_engine::action_engine( editor::input_engine^ input_engine ) :
m_input_engine				( input_engine )
{

}
action_engine::~action_engine( )
{ 
	this->!action_engine(); 
}

action_engine::!action_engine( )
{
	Collections::IDictionaryEnumerator^ enm = m_actions.GetEnumerator();

	while( enm->MoveNext() )
	{
		action^ act = (action^) enm->Value;
		m_input_engine->unregister_action	( act->name(), false );
		delete act;
	}

}

System::Void action_engine::register_action	( action_single^ action )
{
	register_action	( action, nullptr );
}

System::Void action_engine::register_action	( action_single^ action, String^ shorcut )
{
	register_action	( action, shorcut, nullptr );
}

System::Void action_engine::register_action	( action_single^ action, String^ shorcut1, String^ shorcut2 )
{
	ASSERT( !m_actions.Contains( action->name() ) );
	ASSERT( action != nullptr );
	m_actions[action->name()] = (xray::editor::action^)action;

	if( shorcut1 != nullptr && shorcut1 != "" )
		m_input_engine->register_action( action, shorcut1 );
	else
		m_input_engine->register_action( action );

	if( shorcut2 != nullptr && shorcut2 != "" )
		m_input_engine->add_action_shortcut( action->name(), shorcut2 );
}

System::Void action_engine::register_action	( action_continuous^ action )
{
	register_action	( action, nullptr );
}

System::Void action_engine::register_action	( action_continuous^ action, String^ shorcut)
{
	register_action	( action, shorcut, nullptr );
}

System::Void action_engine::register_action	( action_continuous^ action, String^ shorcut1, String^ shorcut2)
{
	ASSERT( !m_actions.Contains( action->name() ) );
	ASSERT( action != nullptr );
	m_actions[action->name()] = (xray::editor::action^)action;

	if( shorcut1 != nullptr && shorcut1 != "" )
		m_input_engine->register_action( action, shorcut1 );
	else
		m_input_engine->register_action( action );

	if( shorcut2 != nullptr && shorcut2 != "" )
		m_input_engine->add_action_shortcut( action->name(), shorcut2);

}

System::Void action_engine::execute			( String^ name )
{
	if( !action_exists	( name ) )
	{
		ASSERT( false );
		return;
	}

	action^ a = (action^)m_actions[name];

	if( a->enabled() ) 
		a->execute();
}

bool action_engine::action_exists	( System::String^ name )
{
	if( m_actions.Contains( name ) )
		return true;

	return true;
}

action^ action_engine::get_action	( System::String^ name )
{
	if( !action_exists	( name ) )
	{
		ASSERT( false );
		return nullptr;
	}

	return (action^)m_actions[name];
}

inline	Collections::ICollection^ action_engine::get_actions_list( )
{
	return m_actions.Keys;
}

inline	input_engine^ action_engine::get_input_engine( )
{
	return m_input_engine;
}

} // namespace editor
} // namespace xray
