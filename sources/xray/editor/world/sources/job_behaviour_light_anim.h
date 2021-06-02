////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_LIGHT_ANIM_H_INCLUDED
#define JOB_BEHAVIOUR_LIGHT_ANIM_H_INCLUDED

#include "job_behaviour_base.h"
#include "attribute.h"

namespace xray {
namespace editor {

ref class object_job;

public ref class job_behaviour_light_anim : job_behaviour_base {

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;

public:
	job_behaviour_light_anim( object_job^ parent_job );

	virtual void load( configs::lua_config_value const& config ) override;

	virtual void save( configs::lua_config_value& config ) override;

	virtual System::Collections::Generic::List<System::String^>^ get_available_events_list( ) override;

	virtual void get_properties	( wpf_property_container^ to_container )	override;

	[DisplayNameAttribute("Cycled")]
	property bool cycled;
	[DisplayNameAttribute("Length")]
	property float length
	{
		float	get( ){ return m_animation_length; }
		void	set( float value )
		{
			if ( value < 0 )
				value = 0;

			m_animation_length = value; 
			if ( on_length_changed != nullptr )
				on_length_changed( value ); 
		}
	}
	

	[DisplayNameAttribute("Color")]
	property wpf_controls::color_curve^				color_curve;
	
	[DisplayNameAttribute("Intensity and Range")]
	property System::Collections::Generic::List<wpf_controls::float_curve^>^ intensity_and_range_list;

	System::Action<System::Double>^	on_length_changed;

private:
	float m_animation_length;
	
}; // class job_behaviour_light_anim

} // namespace editor
} // namespace xray

#endif // #ifndef JOB_BEHAVIOUR_LIGHT_ANIM_H_INCLUDED