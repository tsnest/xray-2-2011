////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "event_manager.h"
#include "logic_action.h"
#include "object_scene.h"

namespace stalker2 {

event_manager::event_manager( )
{}

void event_manager::load( configs::binary_config_value data )
{
	// data is ["events"] table in scene
	// temporary, this will be replaced with more efficient load procedure
	configs::binary_config::const_iterator it = data.begin();
	configs::binary_config::const_iterator it_e = data.end();
	for( ;it!=it_e; ++it )
	{
		pcstr id								= (*it)["name"];
		game_event								ev( id );
		events_registry_type::value_type pair	( ev, handler_list() );
		m_events_registry.insert				( pair );
	}
}

void event_manager::dump_event_manager( )
{
	LOG_INFO ( "\ndumping event_manager!!!!\n" );
	events_registry_type::iterator it	= m_events_registry.begin();
	events_registry_type::iterator it_e	= m_events_registry.end();

	for( ;it!=it_e; ++it)
	{
		// get copy of handlers list
		handler_list handlers				= it->second;
		handler_list::const_iterator hit	= handlers.begin();
		handler_list::const_iterator hit_e	= handlers.end();

		for(; hit!=hit_e; ++hit)
		{
			LOG_INFO( ( *hit ).m_description );
		}
	}
	LOG_INFO("\n");
}

bool event_manager::event_callback_item::operator < ( event_manager::event_callback_item const& other) const 
{
	return	m_callback->priority( ) < other.m_callback->priority( );
}

void event_manager::emit_event( game_event const& e, event_propagator const* filter )
{
	LOG_INFO("event raised : %s [%s]", e.m_event_id.c_str(), (filter==NULL)?"GLOBAL":"LOCAL" );
	events_registry_type::const_iterator it = m_events_registry.find(e);
	if(it==m_events_registry.end())
	{
		
		LOG_INFO("event not found in registry !!!!!!" );
		return;
	}

	// get copy of handlers list

	handler_list handlers				= it->second;

	handler_list::const_iterator hit	= handlers.begin();
	handler_list::const_iterator hit_e	= handlers.end();
	
	for(; hit!=hit_e; ++hit)
	{
		event_callback_item const& itm = (*hit);
		
		if(itm.m_filter==filter){
			itm.m_callback->execute(e);
		}
	}
}

void event_manager::subscribe_event_handler(	game_event const& e, 
												event_propagator const* filter, 
												logic_action_ptr& action,
												pcstr description, 
												bool registered_event )
{
	XRAY_UNREFERENCED_PARAMETER( registered_event );
	//events_registry_type::iterator found	= m_events_registry.find(e);
	//
	//if(found==m_events_registry.end())
	//{
	//	if( registered_event  )
	//	{
	//		LOG_ERROR( "event not registered, %s", e.m_event_id.c_str() );
	//		return;
	//	}else
	//	{	// force insertion on [] operator
	//	}
	//}
	handler_list& handlers					= m_events_registry[e];
	event_callback_item						item;
	item.m_callback							= action;
	item.m_filter							= filter;
	strings::copy							(item.m_description, description);
	handlers.push_back						( item );
	std::sort								( handlers.begin(), handlers.end() );
}

void event_manager::unsubscribe_event_handler( logic_action* action )
{
	events_registry_type::iterator it	= m_events_registry.begin();
	events_registry_type::iterator it_e	= m_events_registry.end();

	for( ;it!=it_e; ++it)
	{
		handler_list& handlers				= it->second;
		handler_list::iterator hit			= std::find( handlers.begin(), handlers.end(), action );
		if( hit!=handlers.end() )
		{
			handlers.erase						( hit );
			return;
		}
	}	
	R_ASSERT( false, "cannot find registered handler" );
}

void event_manager::clear( )
{
	m_events_registry.clear();
}

}