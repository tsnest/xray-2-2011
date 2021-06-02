////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_NPC_PATROL_H_INCLUDED
#define JOB_BEHAVIOUR_NPC_PATROL_H_INCLUDED

#include "job_behaviour_base.h"

namespace xray {
namespace editor {

ref class object_job;

public ref class job_behaviour_npc_patrol : job_behaviour_base {

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;

public:
	job_behaviour_npc_patrol( object_job^ parent_job );

	~job_behaviour_npc_patrol( )
	{
		DELETE( m_animation_behaviour_config );
	}

	virtual void load( configs::lua_config_value const& config ) override;

	virtual void save( configs::lua_config_value& config ) override;

	virtual void get_properties	( wpf_property_container^ to_container )	override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override; 

private:
	void on_select_patrol	( wpf_controls::property_editors::property^ , Object^ );
	void on_behaviour_config_loaded( resources::queries_result& data );

public:
	[System::ComponentModel::DisplayNameAttribute("patrol path")]
	property System::String^		selected_patrol;

private:
	
	configs::lua_config_ptr*	m_animation_behaviour_config;

}; // class job_behaviour_light_static

} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_NPC_PATROL_H_INCLUDED