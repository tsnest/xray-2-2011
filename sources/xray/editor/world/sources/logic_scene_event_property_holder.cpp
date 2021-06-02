////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_job.h"
#include "logic_event.h"
#include "object_scene.h"
#include "job_behaviour_base.h"
#include "logic_scene_event_property_holder.h"
#include "logic_event_action_base.h"
#include "logic_event_action_start_scene.h"
#include "logic_event_action_raise_event.h"
#include "logic_editor_panel.h"

 
 using namespace System::Collections;
 using namespace System;
 using namespace xray::editor::wpf_controls;
 
 namespace xray {
 namespace editor {
 
 logic_scene_event_property_holder::logic_scene_event_property_holder( object_scene^ parent_scene )
 {
  	m_parent_scene				= parent_scene;
	m_selected_event			= "#StartScene";
	m_property_value			= nullptr;
	m_actions_list				= gcnew Generic::List<logic_event_action_base^>();	
	event_handler_type			= "parent_view_handler";

	
	System::String^ event_name = "event" ;

	wpf_controls::property_descriptor^ prop_descriptor = gcnew wpf_controls::property_descriptor( event_name, "", "", "", this );

	prop_descriptor->add_button		( "X", "", gcnew Action<button^>( m_parent_scene, &object_scene::remove_event_handler_from_parent_view_clicked ) )->tag = this;

	this->own_property_descriptor = prop_descriptor;

	this->own_property_descriptor->tag = this;

	

	prop_descriptor->is_read_only = false;
	this->refresh_attributes();	

	if ( m_parent_scene->get_event_names_list()->Count > 0 && this->get_property_type() == "" )
		this->set_property_type(m_parent_scene->get_event_names_list()[0]);

	hypergraph::node_property_attribute^ node_attr = gcnew hypergraph::node_property_attribute( false, true );
	node_attr->type_id = "logic_scene";
	prop_descriptor->dynamic_attributes->add( node_attr );
	prop_descriptor->dynamic_attributes->add( gcnew property_editors::attributes::custom_property_attribute(
		gcnew System::Func<System::Collections::Generic::List<System::String^>^>( m_parent_scene, &object_scene::get_event_names_list ) , 
		gcnew System::Func<System::String^>  ( this, &logic_scene_event_property_holder::get_property_type) , 
		gcnew System::Action<System::String^>( this, &logic_scene_event_property_holder::set_property_type)
		));
 }
 
 logic_scene_event_property_holder::~logic_scene_event_property_holder( ) 
 {
 
 }
 
 System::String^	logic_scene_event_property_holder::get_property_type( )
 {
	return m_selected_event;
 }
 
 void			logic_scene_event_property_holder::set_property_type(System::String^ value)
 {
	if ( value == nullptr )
		return;

	if (m_selected_event == value)
		return;
	
	m_selected_event = value; 
 	
	refresh_attributes(); 
 }
 
 void			logic_scene_event_property_holder::set_value		( System::Object^ value )
 {
	 if ( m_action_raise_event != nullptr ){
		remove_raise_event_action( m_action_raise_event );
	}

	if ( value->ToString( ) == "none" )
		return;
	
	add_raise_event_action( gcnew logic_event_action_raise_event( m_parent_scene, parent_scene->parent_scene->get_event_by_name((System::String^)value) ) );
 }
 System::Object^ logic_scene_event_property_holder::get_value		()
 {
	 if (event_handler_type == "own_view_handler")
		 return "";

	 if (m_action_raise_event == nullptr)
		 return "none";

	 return m_action_raise_event->raised_event_name;
 }
 
 System::Type^	logic_scene_event_property_holder::value_type::get()
 {
	return System::String::typeid;
 }
 
 
 void	logic_scene_event_property_holder::refresh_attributes()
 {
	
 }

 void logic_scene_event_property_holder::load			( configs::lua_config_value const& config )
 {
	if ( config.value_exists("editor_event_name") )
		m_selected_event = gcnew System::String(config["editor_event_name"]);
	else
		m_selected_event = gcnew System::String(config["event_name"]);

	if ( m_selected_event == "StartScene" )
		m_selected_event = "#StartScene";

	if ( config.value_exists("handler_type") )
		event_handler_type = gcnew System::String(config["handler_type"]);
	
	configs::lua_config_iterator actions_begin = config["actions"].begin();
	configs::lua_config_iterator actions_end = config["actions"].end();
	for ( ;actions_begin != actions_end ; actions_begin++){
		System::String^ action_type = gcnew System::String( (*actions_begin)["action_type"] );

		if (action_type == "raise_event")
		{
			m_action_raise_event = gcnew logic_event_action_raise_event( m_parent_scene );
			m_action_raise_event->load(*actions_begin);
			add_raise_event_action(m_action_raise_event);
		}
		else
		{
			logic_event_action_start_scene^ action = gcnew logic_event_action_start_scene(this);
			action->load( *actions_begin );
			add_start_scene_action ( action );
		}
	}
 }
 void logic_scene_event_property_holder::save			( configs::lua_config_value config )
 {
	config["editor_event_name"] = unmanaged_string(m_selected_event).c_str();
	config["event_name"] = unmanaged_string(m_selected_event).c_str();
	
	config["handler_type"] = unmanaged_string(event_handler_type).c_str();

	bool is_global = false;

	if ( m_parent_scene->get_event_by_name(m_selected_event) != nullptr )
		is_global = true;

	config["is_global"] = is_global;
	config["actions"].create_table();
	int index = 0;
	for each( logic_event_action_base^ action in m_actions_list )
	{
		action->save(config["actions"][unmanaged_string("action" + index).c_str()]);
		index++;
	}	
}

void logic_scene_event_property_holder::add_start_scene_action		( logic_event_action_start_scene^		action )
{
	m_actions_list->Add(action);
}

void logic_scene_event_property_holder::add_raise_event_action		( logic_event_action_raise_event^		action )
{
	m_action_raise_event = action;
	m_actions_list->Add(action);
	m_action_raise_event->raised_event->subscribe_on_changed( this );
}

void logic_scene_event_property_holder::remove_raise_event_action		( logic_event_action_raise_event^ /*action*/ )
{
	m_actions_list->Remove( m_action_raise_event );
	m_action_raise_event->raised_event->unsubscribe_on_changed( this );
	delete m_action_raise_event;
	m_action_raise_event = nullptr;
}

void logic_scene_event_property_holder::own_property_descriptor::set( property_descriptor^ value )
{
 	if ( m_own_property_descriptor != nullptr )
 		ASSERT( "Trying to set property descriptor twice!" );
 	m_own_property_descriptor = value; 
}

void logic_scene_event_property_holder::on_parent_job_parent_changed( object_scene^ )
{

}

Generic::IEnumerable<System::String^>^ logic_scene_event_property_holder::get_available_scenes_events( System::String^ value_type )
{
	Generic::List<System::String^>^ scenes_events = (Generic::List<System::String^>^)parent_scene->parent_scene->get_children_by_type( value_type );
	scenes_events->Insert(0, "none");
	scenes_events->Remove( "#StartScene" );
	return scenes_events;
}

void logic_scene_event_property_holder::remove_start_scene_action	( object_scene^ scene )
{
	for each( logic_event_action_base^ action in m_actions_list )
	{
		if ( action->GetType() == logic_event_action_start_scene::typeid ){
			logic_event_action_start_scene^ start_scene_action = safe_cast<logic_event_action_start_scene^>(action);
			if ( scene == start_scene_action->scene_start_to ){
				m_actions_list->Remove( start_scene_action );		
				delete action;
				return;
			}
		}
	}
}



void logic_scene_event_property_holder::remove_logic_links			( )
{
	object_scene^ logic_view_owner = ( event_handler_type == "own_view_handler" ) ? parent_scene : parent_scene->parent_scene;

	Generic::List<logic_event_action_base^> actions( m_actions_list );
	for each( logic_event_action_base^ action in actions )
	{	
		if ( action->GetType() == logic_event_action_start_scene::typeid ){
			logic_event_action_start_scene^ start_scene_action = safe_cast<logic_event_action_start_scene^>(action);
			link^ link_to_remove = nullptr;
			for each( link^ lnk in logic_view_owner->own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->links->Values)
			{
				if ( lnk->source_tag == this && lnk->destination_tag == start_scene_action->scene_start_to )
					link_to_remove = lnk;
			}
			logic_view_owner->own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->remove_link( link_to_remove->id );

			delete action;
		}
	}
}

void logic_scene_event_property_holder::add_logic_links				( )
{
	object_scene^ logic_view_owner = ( event_handler_type == "own_view_handler" ) ? parent_scene : parent_scene->parent_scene;
	for each( logic_event_action_base^ action in m_actions_list )
	{	
		if ( action->GetType() == logic_event_action_start_scene::typeid ){
			logic_event_action_start_scene^ start_scene_action = safe_cast<logic_event_action_start_scene^>(action);
			logic_view_owner->own_logic_view->cached_logic->logic_view->scenes_view->logic_hypergraph->create_link( own_property_descriptor, start_scene_action->scene_start_to->own_logic_node_in_parent_view );
		}
	}
}

void logic_scene_event_property_holder::on_before_remove			( )
{
	remove_logic_links			( );
	m_actions_list->Clear();
}


void 			logic_scene_event_property_holder::on_subscribed_logic_event_deletion		( logic_event^ subscribed_event )
{
	if ( m_action_raise_event != nullptr )
	{
		if ( subscribed_event == m_action_raise_event->raised_event )
			set_value("none");
	}
	own_property_descriptor->update( );
}

void 			logic_scene_event_property_holder::on_subscribed_logic_event_name_changed	( logic_event^ /*event*/ )
{
	own_property_descriptor->update( );
}

} // namespace editor
} // namespace xray

