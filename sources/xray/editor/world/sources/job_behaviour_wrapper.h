////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_WRAPPER_H_INCLUDED
#define JOB_BEHAVIOUR_WRAPPER_H_INCLUDED

#include "object_job.h"

using namespace System::Collections;

namespace xray {
namespace editor {


ref class job_behaviour_wrapper {

typedef xray::editor::wpf_controls::property_container				wpf_property_container;
typedef xray::editor::wpf_controls::hypergraph::properties_node		wpf_hypergpaph_node;

public:
	job_behaviour_wrapper( object_job^ parent_job, bool on_loading );

	static job_behaviour_base^			create_behaviour(System::String^ type , object_job^ parent_job );
	
private:
	object_job^							m_parent_job;
	wpf_property_container^ 			m_properties_container;
	job_behaviour_base^					m_current_behaviour;

public:
 	property job_behaviour_base^		current_behaviour
 	{	
 		job_behaviour_base^				get( ){ return m_current_behaviour;}
		void							set( job_behaviour_base^ value ){ m_current_behaviour = value; }
 	}
	property wpf_property_container^	properties_container
	{	
		wpf_property_container^			get( ){ return m_properties_container;}		
	}
	[DisplayNameAttribute("Behaviour")]
	property System::String^			behaviour_type
	{	
		System::String^					get( );
		void							set( System::String^ value );
	}

	property wpf_hypergpaph_node^		hypergraph_node;

}; // class job_behaviour_wrapper

} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_WRAPPER_H_INCLUDED