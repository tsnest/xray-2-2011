////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_CAMERA_TRACK_H_INCLUDED
#define JOB_BEHAVIOUR_CAMERA_TRACK_H_INCLUDED

#include "job_behaviour_base.h"
#include "project_items.h"

namespace xray {
namespace editor {

ref class object_job;
ref class logic_event_properties_container;

public ref class job_behaviour_camera_track: public job_behaviour_base{

typedef xray::editor::wpf_controls::property_container				wpf_property_container;
typedef xray::editor::wpf_controls::property_container_collection	wpf_property_container_collection;

public:
	job_behaviour_camera_track( object_job^ parent_job );

	[DisplayNameAttribute("FovX")]
	property float								fov_x;
	[DisplayNameAttribute("FovY")]
	property float								fov_y;
	[DisplayNameAttribute("NearPlane")]
	property float								near_plane;
	[DisplayNameAttribute("Cyclic")]
	property bool								cyclic;
	[DisplayNameAttribute("FarPlane")]
	property float								far_plane;
	[DisplayNameAttribute("Track")]
	property System::String^					camera_track
	{
		System::String^							get( ){ 
														if ( m_current_camera_track == nullptr )
															return "";
														return m_current_camera_track->get_full_name();
												}
		void									set( System::String^ ){}
	}

private:
	project_item_base^							m_current_camera_track;

public:
	virtual void	load			( configs::lua_config_value const& config ) override;
	virtual void	save			( configs::lua_config_value& config )		override;
	virtual void	get_properties	( wpf_property_container^ to_container )	override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override;

	void			on_select_camera_track	( wpf_controls::property_editors::property^ prop, Object^ filter );
	void			selected_track_loaded	( project_item_base^ track );
	
}; // class job_behaviour_camera_track


public ref class job_behaviour_camera_free_fly: public job_behaviour_base
{
public:
	job_behaviour_camera_free_fly( object_job^ parent_job ):job_behaviour_base(parent_job){ };

}; // class job_behaviour_camera_free_fly

} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_CAMERA_TRACK_H_INCLUDED