////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_SCENE_EVENT_PROPERTY_HOLDER_H_INCLUDED
#define LOGIC_SCENE_EVENT_PROPERTY_HOLDER_H_INCLUDED

#include "logic_event_subscriber.h"

using xray::editor::wpf_controls::property_descriptor;
using namespace System::Collections;



namespace xray {
namespace editor {

ref class job_behaviour_base;
ref class object_scene;
interface class logic_event_action_base;
ref class logic_event_action_start_scene;
ref class logic_event_action_raise_event;

public ref class logic_scene_event_property_holder : xray::editor::wpf_controls::i_property_value, logic_event_subscriber {
public:
	logic_scene_event_property_holder( object_scene^ parent_scene ) ;

private: 
	~logic_scene_event_property_holder( ) ;

public:
	virtual void			set_value		(System::Object^ value);
	virtual System::Object	^get_value		();

	System::String^			get_property_type();
	void					set_property_type(System::String^ value);

	void					refresh_attributes();

	void					load			( configs::lua_config_value const& config );
	void					save			( configs::lua_config_value config );

	void					add_start_scene_action		( logic_event_action_start_scene^		action );
	void					add_raise_event_action		( logic_event_action_raise_event^		action );
	void					remove_raise_event_action	( logic_event_action_raise_event^	action );
	void					remove_start_scene_action	( object_scene^ scene );
	void					on_before_remove			( );

	void					remove_logic_links			( );
	void					add_logic_links				( );

	void					on_parent_job_parent_changed( object_scene^ scene );
	Generic::IEnumerable<System::String^>^ get_available_scenes_events( System::String^ value_type );


	virtual void 			on_subscribed_logic_event_deletion		( logic_event^ event );
	virtual void			on_subscribed_logic_event_name_changed	( logic_event^ event );


	virtual property System::Type^			value_type
	{
		System::Type^ get();
	}

	property System::String^		event_handler_type;

	property property_descriptor^	own_property_descriptor
	{	
		property_descriptor^		get( ){ return m_own_property_descriptor; }
		void						set( property_descriptor^ value );	
	}
	
	property object_scene^	parent_scene
	{	
		object_scene^		get( ){ return m_parent_scene; }		
	}

	property Generic::List<logic_event_action_base^>^	actions_list
	{	
		Generic::List<logic_event_action_base^>^		get( ){ return m_actions_list; }		
	}
private:
	property_descriptor^						m_own_property_descriptor;
	System::String^								m_selected_event;
	System::Object^								m_property_value;
	object_scene^								m_parent_scene;
	logic_event_action_raise_event^				m_action_raise_event;
	Generic::List<logic_event_action_base^>^	m_actions_list;
}; // class logic_event_property_holder

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_SCENE_EVENT_PROPERTY_HOLDER_H_INCLUDED