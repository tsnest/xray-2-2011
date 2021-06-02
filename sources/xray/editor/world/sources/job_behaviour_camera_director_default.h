////////////////////////////////////////////////////////////////////////////
//	Created		: 23.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_CAMERA_DIRECTOR_DEFAULT_H_INCLUDED
#define JOB_BEHAVIOUR_CAMERA_DIRECTOR_DEFAULT_H_INCLUDED

#include "job_behaviour_base.h"
#include "project_items.h"

namespace xray {
namespace editor {

ref class object_job;
ref class logic_event_properties_container;

public ref class job_behaviour_camera_director_default: public job_behaviour_base{

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;
	
public:
	job_behaviour_camera_director_default( object_job^ parent_job );

	[DisplayNameAttribute("Camera")]
	property System::String^					active_camera
	{
		System::String^							get( ){ 
			if ( m_active_camera == nullptr )
				return "";
			return m_active_camera->get_full_name();
		}
		void									set( System::String^ ){}
	}
	[DisplayNameAttribute("Transition")]
	property System::String^					transition;

private:
	project_item_base^							m_active_camera;

public:
	virtual void	load			( configs::lua_config_value const& config ) override;
	virtual void	save			( configs::lua_config_value& config )		override;
	virtual void	get_properties	( wpf_property_container^ to_container )	override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override;

	void			on_select_camera		( wpf_controls::property_editors::property^ prop, Object^ filter );
	void			selected_camera_loaded	( project_item_base^ track );

}; // class job_behaviour_camera_track


} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_CAMERA_DIRECTOR_DEFAULT_H_INCLUDED