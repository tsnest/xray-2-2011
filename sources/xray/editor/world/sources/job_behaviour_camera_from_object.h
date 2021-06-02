////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_CAMERA_FROM_OBJECT_H_INCLUDED
#define JOB_BEHAVIOUR_CAMERA_FROM_OBJECT_H_INCLUDED

#include "job_behaviour_base.h"
#include "project_items.h"

namespace xray {
namespace editor {

ref class object_job;

public ref class job_behaviour_camera_from_object: public job_behaviour_base{

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;
	typedef xray::editor::wpf_controls::property_container_collection	wpf_property_container_collection;
	typedef xray::editor::wpf_controls::hypergraph::node				wpf_hypergpaph_node;

public:
	job_behaviour_camera_from_object( object_job^ parent_job );

public: 
	
	[DisplayNameAttribute("FovX")]
	property float								fov_x;
	[DisplayNameAttribute("FovY")]
	property float								fov_y;
	[DisplayNameAttribute("NearPlane")]
	property float								near_plane;
	[DisplayNameAttribute("FarPlane")]
	property float								far_plane;
	[DisplayNameAttribute("Object")]
	property System::String^					from_object
	{
		System::String^							get( ){ 
			if ( m_current_object == nullptr )
				return "";
			return m_current_object->get_full_name();
		}
		void									set( System::String^ ){}
	}

private:
	project_item_base^							m_current_object;

public:
	virtual void	load			( configs::lua_config_value const& config ) override;
	virtual void	save			( configs::lua_config_value& config )		override;
	virtual void	get_properties	( wpf_property_container^ to_container )	override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override;


	void			on_select_object	( wpf_controls::property_editors::property^ prop, Object^ filter );
}; // class job_behaviour_camera_track


} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_CAMERA_FROM_OBJECT_H_INCLUDED