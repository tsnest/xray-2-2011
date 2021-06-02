////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGIC_EVENT_PROPERTIES_CONTAINER_H_INCLUDED
#define LOGIC_EVENT_PROPERTIES_CONTAINER_H_INCLUDED

using namespace System::Collections;

namespace xray {
namespace editor {

ref class job_behaviour_base;
ref class logic_event_property_holder;

public ref class logic_event_properties_container {

typedef xray::editor::wpf_controls::property_container_collection	wpf_property_container_collection;

public: 
	logic_event_properties_container( job_behaviour_base^ parent_behaviour );

	
private:
	wpf_controls::property_container_base^	m_container;
	job_behaviour_base^						m_parent_behaviour;

public:
	Generic::List<logic_event_property_holder^>^ m_events_list;
	
public:
	bool add_property		( );
	bool remove_property	( System::Object^  property_id);
	void remove_property	( logic_event_property_holder^ holder );
	bool move_property		( System::Int32 property_id, System::String^ order );
	void load				( configs::lua_config_value const& config );
	void save				( configs::lua_config_value config );
	void add_to_container	( logic_event_property_holder^ holder );


public:
	property wpf_controls::property_container_base^ property_container
	{
		wpf_controls::property_container_base^		get( ){ return m_container; }
	}
	
};//class property_collection_holder

} // namespace editor
} // namespace xray

#endif // #ifndef LOGIC_EVENT_PROPERTIES_CONTAINER_H_INCLUDED