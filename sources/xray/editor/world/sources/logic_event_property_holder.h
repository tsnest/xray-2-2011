////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_SIGNAL_PROPERTY_H_INCLUDED
#define LOGIC_SIGNAL_PROPERTY_H_INCLUDED

#include "logic_event_subscriber.h"

using xray::editor::wpf_controls::property_descriptor;
using namespace System::Collections;

namespace xray {
namespace editor {

ref class job_behaviour_base;
ref class object_scene;
interface class logic_event_action_base;
ref class logic_event_action_switch;
ref class logic_event_action_raise_event;

public ref class logic_event_property_holder : xray::editor::wpf_controls::i_property_value, logic_event_subscriber {
public:
	logic_event_property_holder( job_behaviour_base^ parent_behaviour ) ;

private: 
	~logic_event_property_holder( ) ;

public:
	virtual void			set_value		(System::Object^ value);
	virtual System::Object	^get_value		();

	System::String^			get_property_type();
	void					set_property_type(System::String^ value);

	void					refresh_attributes();

	void					load			( configs::lua_config_value const& config );
	void					save			( configs::lua_config_value config );

	void					add_switch_action		( logic_event_action_switch^		action );
	void					add_raise_event_action	( logic_event_action_raise_event^	action );
	void					remove_raise_event_action	( logic_event_action_raise_event^	action );

	void					remove_switch_action();

	void					on_parent_job_parent_changed( object_scene^ scene );
	Generic::IEnumerable<System::String^>^ get_available_scenes_events( System::String^ value_type );

	virtual void 			on_subscribed_logic_event_deletion		( logic_event^ event );
	virtual void			on_subscribed_logic_event_name_changed	( logic_event^ event );

	void					add_logic_links	( );

	virtual property System::Type^			value_type
	{
		System::Type^ get();
	}

	property property_descriptor^	own_property_descriptor
	{	
		property_descriptor^		get( ){ return m_own_property_descriptor; }
		void						set( property_descriptor^ value );	
	}
	
	property job_behaviour_base^	parent_behaviour
	{	
		job_behaviour_base^		get( ){ return m_parent_behaviour; }		
	}

	property System::Single					timer_value
	{
		System::Single						get(){return m_timer_value;}
		void								set( System::Single value ){m_timer_value = value;}
	}

	property Generic::List<logic_event_action_base^>^	actions_list
	{
		Generic::List<logic_event_action_base^>^		get(){ return m_actions_list; }
	}




private:
	property_descriptor^		m_own_property_descriptor;
	System::String^				m_selected_event;
	System::Object^				m_property_value;
	job_behaviour_base^			m_parent_behaviour;

	System::Single				m_timer_value;

	logic_event_action_switch^		m_action_switch;
	logic_event_action_raise_event^ m_action_raise_event;

	Generic::List<logic_event_action_base^>^ m_actions_list;

	editor::wpf_controls::property_editors::attributes::text_box_extension_attribute^ m_timer_attribute;




}; // class logic_event_property_holder

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_SIGNAL_PROPERTY_H_INCLUDED