////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_H_INCLUDED
#define JOB_BEHAVIOUR_H_INCLUDED

#include "attribute.h"

namespace xray {
namespace editor {

ref class object_job;
ref class logic_event_properties_container;

public ref class job_behaviour_base abstract {

typedef xray::editor::wpf_controls::property_container				wpf_property_container;
typedef xray::editor::wpf_controls::hypergraph::properties_node		wpf_hypergpaph_node;

public: 
	job_behaviour_base( object_job^ parent_job );
public: 
	virtual property System::String^			behaviour_type;
	virtual property System::String^			behaviour_name;
	virtual property object_job^				parent_job;
private:
	System::Windows::Point						m_logic_node_position;
public:
	
	property logic_event_properties_container^	events_container
	{
		logic_event_properties_container^		get( ){ return m_events_container; }
	}
	virtual property wpf_hypergpaph_node^		hypergraph_node
	{
		wpf_hypergpaph_node^					get()
		{
			return m_hypergraph_node;
		}
		void									set( wpf_hypergpaph_node^ hypergraph_node )
		{
			m_hypergraph_node = hypergraph_node;
			behaviour_name = hypergraph_node->id;
		}
	}

	property System::Windows::Point				logic_node_position
	{
		void									set( System::Windows::Point value ) { m_logic_node_position = value; }
		System::Windows::Point					get( ){ return ( hypergraph_node != nullptr ) ? hypergraph_node->position : m_logic_node_position; }
	}
	
	
public:
	virtual void load			( configs::lua_config_value const& config );
	virtual void save			( configs::lua_config_value& config );

	virtual void get_properties	( wpf_property_container^ to_container );

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( );
	 
	virtual void initialize		( ){};

protected:
	logic_event_properties_container^			m_events_container;
	wpf_hypergpaph_node^						m_hypergraph_node;

	[DisplayNameAttribute("Cycled")]
	property bool cycled;
	[DisplayNameAttribute("Length")]
	property float length;
}; // class job_behaviour_base


} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_H_INCLUDED