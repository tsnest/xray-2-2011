////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_job.h"
#include "tool_scripting.h"
#include "level_editor.h"
#include "logic_event.h"
#include "object_scene.h"
#include "job_behaviour_base.h"
#include "logic_event_property_holder.h"
#include "logic_event_action_base.h"
#include "logic_event_action_switch.h"
#include "logic_event_action_raise_event.h"

 
 using namespace System::Collections;
 using namespace System;
 
 namespace xray {
 namespace editor {
 
 logic_event_property_holder::logic_event_property_holder( job_behaviour_base^ parent_behaviour )
 {
  	m_parent_behaviour			= parent_behaviour;
	m_selected_event			= "";
	m_property_value			= nullptr;
	m_actions_list				= gcnew Generic::List<logic_event_action_base^>();	
	m_timer_value				= 0;
	m_timer_attribute			= gcnew editor::wpf_controls::property_editors::attributes::text_box_extension_attribute(
		gcnew System::Func<Single>	( this, &logic_event_property_holder::timer_value::get ),
		gcnew System::Action<Single>	( this, &logic_event_property_holder::timer_value::set ));
 }
 
 logic_event_property_holder::~logic_event_property_holder( ) 
 {
 
 }
 
 System::String^	logic_event_property_holder::get_property_type( )
 {
 	return m_selected_event;
 }
 
 void			logic_event_property_holder::set_property_type(System::String^ value)
 {
	if ( value == nullptr )
		return;

	if (m_selected_event == value)
		return;
	
	if (m_selected_event == "on_time")
	{
		m_own_property_descriptor->dynamic_attributes->remove( m_timer_attribute );		
	}

	m_selected_event = value; 
 	
	if (m_selected_event == "on_time")
	{
		m_own_property_descriptor->dynamic_attributes->add( m_timer_attribute );
		timer_value = 0;
	}

	refresh_attributes(); 
 	/*set_value("none");*/
 }
 
 void			logic_event_property_holder::set_value		(System::Object^ value)
 {
	if ( m_action_raise_event != nullptr ){
		remove_raise_event_action( m_action_raise_event );
	}

	if ( value->ToString( ) == "none" )
		return;
	
	add_raise_event_action( gcnew logic_event_action_raise_event( m_parent_behaviour->parent_job, m_parent_behaviour->parent_job->parent_scene->get_event_by_name((System::String^)value) ) );
 }
 System::Object^ logic_event_property_holder::get_value		()
 {
	 if (m_action_raise_event == nullptr)
		 return "none";
	return m_action_raise_event->raised_event_name;
 }
 
 System::Type^	logic_event_property_holder::value_type::get()
 {
	return System::String::typeid;
 }
 
 
 void	logic_event_property_holder::refresh_attributes()
 {
	
 }

 void logic_event_property_holder::load			( configs::lua_config_value const& config )
 {
	if ( config.value_exists("editor_event_name") )
		m_selected_event = gcnew System::String(config["editor_event_name"]);
	else
		m_selected_event = gcnew System::String(config["event_name"]);


	if ( m_selected_event == "StartScene" )
		m_selected_event = "#StartScene";
	
	Generic::List< String^ >^ available_events  = m_parent_behaviour->get_available_events_list() ;

	if ( (bool)config["is_global"] && !available_events->Contains( m_selected_event ) )
	{
		::DialogResult result = m_parent_behaviour->parent_job->tool->get_level_editor()->ShowMessageBox( "Scene has no event with name : " + m_selected_event + ". Doyou want to add it?",
			MessageBoxButtons::YesNo,
			MessageBoxIcon::Exclamation );
		if ( result ==  ::DialogResult::Yes )
		{
			logic_event^ new_event = gcnew logic_event( m_parent_behaviour->parent_job->parent_scene );
			new_event->logic_event_view->event_name = m_selected_event;
			m_parent_behaviour->parent_job->parent_scene->add_event(new_event);
		}		
	}

	if ( m_selected_event == "on_time" ){
		String^ str_name = gcnew System::String(config["event_name"]);
		str_name = str_name->Substring( m_selected_event->Length, str_name->Length - m_selected_event->Length);
		timer_value = Single::Parse(str_name);			
	}

	configs::lua_config_iterator actions_begin = config["actions"].begin();
	configs::lua_config_iterator actions_end = config["actions"].end();
	for ( ;actions_begin != actions_end ; actions_begin++){
		System::String^ action_type = gcnew System::String( (*actions_begin)["action_type"] );
		 
		if (action_type == "switch_to_behaviour")
		{
			m_action_switch = gcnew logic_event_action_switch(this);
			m_action_switch->load(*actions_begin);
			add_switch_action(m_action_switch);
		}
		else if (action_type == "raise_event")
		{
			System::String^ raised_event_name = gcnew System::String( (*actions_begin)["raised_event"] );
			if ( m_parent_behaviour->parent_job->parent_scene->get_event_by_name( raised_event_name ) == nullptr )
			{
				::DialogResult result = m_parent_behaviour->parent_job->tool->get_level_editor()->ShowMessageBox( "Scene has no event with name : " + raised_event_name + ". Doyou want to add it?",
					MessageBoxButtons::YesNo,
					MessageBoxIcon::Exclamation );
				if ( result ==  ::DialogResult::Yes )
				{
					logic_event^ new_event = gcnew logic_event( m_parent_behaviour->parent_job->parent_scene );
					new_event->logic_event_view->event_name = raised_event_name;
					m_parent_behaviour->parent_job->parent_scene->add_event(new_event);
				}
				else
					continue;
			}
			m_action_raise_event = gcnew logic_event_action_raise_event(m_parent_behaviour->parent_job);
			m_action_raise_event->load(*actions_begin);

			add_raise_event_action(m_action_raise_event);
		}
	}
	if (m_selected_event == "on_time")
	{
		m_own_property_descriptor->dynamic_attributes->add( m_timer_attribute );		
	}
 }
 void logic_event_property_holder::save			( configs::lua_config_value config )
 {
	config["editor_event_name"] = unmanaged_string(m_selected_event).c_str();

	if ( m_selected_event == "on_time" ){
		config["event_name"] = unmanaged_string(m_selected_event + timer_value).c_str();
	}
	else
		config["event_name"] = unmanaged_string(m_selected_event).c_str();


	bool is_global = false;

	if ( parent_behaviour->parent_job->parent_scene->get_event_by_name(m_selected_event) != nullptr )
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

void logic_event_property_holder::add_switch_action		( logic_event_action_switch^ action )
{
	m_action_switch = action;
	m_actions_list->Add(action);
}

void logic_event_property_holder::add_raise_event_action		( logic_event_action_raise_event^ action )
{
	m_action_raise_event = action;
	m_actions_list->Add(action);
	m_action_raise_event->raised_event->subscribe_on_changed( this );
}
void logic_event_property_holder::remove_raise_event_action		( logic_event_action_raise_event^ /*action*/ )
{
	m_actions_list->Remove( m_action_raise_event );
	m_action_raise_event->raised_event->unsubscribe_on_changed( this );
	delete m_action_raise_event;
	m_action_raise_event = nullptr;
}

void logic_event_property_holder::own_property_descriptor::set( property_descriptor^ value )
{
	if ( m_own_property_descriptor != nullptr )
		ASSERT( "Trying to set property descriptor twice!" );
	m_own_property_descriptor = value;

	if ( m_selected_event == "on_time" )
	{
		m_own_property_descriptor->dynamic_attributes->add( m_timer_attribute );
	}

	if (m_parent_behaviour->parent_job->parent_scene != nullptr)
		on_parent_job_parent_changed( m_parent_behaviour->parent_job->parent_scene );
	else
		m_parent_behaviour->parent_job->on_parent_scene_changed += gcnew on_parent_scene_changed(this, &logic_event_property_holder::on_parent_job_parent_changed);	
}

void logic_event_property_holder::on_parent_job_parent_changed( object_scene^ ){
	m_own_property_descriptor->dynamic_attributes->add(gcnew editor::wpf_controls::property_editors::attributes::combo_box_items_attribute( 
												   "event",
												   gcnew System::Func< System::String^ , Generic::IEnumerable<System::String^>^>( this, &logic_event_property_holder::get_available_scenes_events ) 
												   )
												);	
}

Generic::IEnumerable<System::String^>^ logic_event_property_holder::get_available_scenes_events( System::String^ value_type )
{
	Generic::List<System::String^>^ scenes_events = (Generic::List<System::String^>^)parent_behaviour->parent_job->parent_scene->get_children_by_type( value_type );
	scenes_events->Insert( 0, "none" );
	scenes_events->Remove( "#SceneStart" );
	return scenes_events;
}

void logic_event_property_holder::remove_switch_action( )
{
	if (m_action_switch == nullptr)
		return;

	m_actions_list->Remove( m_action_switch );
	link^ link_to_remove = nullptr;
	for each( link^ lnk in m_parent_behaviour->parent_job->logic_view->logic_hypergraph->links->Values)
	{
		if ( lnk->source_tag == this )
			link_to_remove = lnk;
	}
	m_parent_behaviour->parent_job->logic_view->logic_hypergraph->remove_link( link_to_remove->id );

	delete m_action_switch;
	m_action_switch = nullptr;
}


void logic_event_property_holder::on_subscribed_logic_event_deletion		( logic_event^ subscribed_event )
{
	if ( m_action_raise_event != nullptr )
	{
		if ( subscribed_event == m_action_raise_event->raised_event )
			set_value("none");
	}
	own_property_descriptor->update( );
}

void logic_event_property_holder::on_subscribed_logic_event_name_changed	( logic_event^ /*event*/ )
{
	own_property_descriptor->update( );
}


void logic_event_property_holder::add_logic_links				( )
{
	for each( logic_event_action_base^ action in m_actions_list )
	{	
		if ( action->GetType() == logic_event_action_switch::typeid ){
			logic_event_action_switch^ switch_action = safe_cast<logic_event_action_switch^>(action);
			parent_behaviour->parent_job->logic_view->logic_hypergraph->create_link( own_property_descriptor, switch_action->behaviour_switch_to->hypergraph_node );	}
	}
}

} // namespace editor
} // namespace xray

