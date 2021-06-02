////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_H_INCLUDED
#define LOGIC_EVENT_H_INCLUDED

#include "logic_event_subscriber.h"

using namespace System;
using namespace xray::editor::wpf_controls;
using namespace xray::editor::wpf_controls::control_containers;

namespace xray {
namespace editor {


ref class object_scene;
ref class logic_event;
ref class logic_event_properties;
ref class project_item_base;

using System::Windows::Controls::Button;

public delegate void on_logic_event_name_changed	( logic_event^ );
public delegate void on_before_event_deletion		( logic_event^ );

public ref class logic_event : logic_event_subscriber {

typedef wpf_controls::logic_view::event_control				wpf_logic_event;
typedef System::Collections::Generic::List<logic_event^>	linked_events;

public:
	logic_event( object_scene^ parent_scene );	

public:
	property bool				is_public;
	
	property wpf_logic_event^	logic_event_view
	{
		wpf_logic_event^		get( ){ return m_logic_event_view; }
	}

	property System::String^	event_full_name
	{
		System::String^			get();
	}
	property object_scene^		parent_scene
	{
		object_scene^			get( ){ return m_parent_scene; }
	}
	
	property linked_events^		linked_to_me_events
	{
		linked_events^			get( ){ return m_linked_to_me_events; }
	}
	
private:
	object_scene^				m_parent_scene;
	wpf_logic_event^			m_logic_event_view;
	linked_events^				m_linked_events;
	linked_events^				m_linked_to_me_events;
	logic_event_properties^		m_properties_form;

	Collections::Generic::List<String^>^ m_loaded_link_names;
	
	on_before_event_deletion^		m_on_before_deletion;
	on_logic_event_name_changed^	m_on_event_name_changed;

private:
	void on_event_name_changed	( String^ new_name );
	void on_view_event_properties( );
	void add_link_clicked		( button^ );
	void remove_link_clicked	( button^ btn );

	property_container^	get_properties_container( );

	void linked_event_scene_loaded	( project_item_base^ scene );

public:
	void save( configs::lua_config_value config );
	void load( configs::lua_config_value config );

	void on_before_delete( );

	void subscribe_on_changed	( logic_event_subscriber^ subscriber );
	void unsubscribe_on_changed	( logic_event_subscriber^ subscriber );

	virtual void on_subscribed_logic_event_deletion		( logic_event^ event );
	virtual void on_subscribed_logic_event_name_changed	( logic_event^ event );
}; // class logic_event

} // namespace editor
} // namespace xray


#endif // #ifndef LOGIC_EVENT_H_INCLUDED