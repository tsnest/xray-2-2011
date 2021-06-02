////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef JOB_BEHAVIOUR_LIGHT_STATIC_H_INCLUDED
#define JOB_BEHAVIOUR_LIGHT_STATIC_H_INCLUDED

#include "job_behaviour_base.h"
#include "attribute.h"

namespace xray {
namespace editor {

ref class object_job;

public ref class job_behaviour_light_static : job_behaviour_base {

	typedef xray::editor::wpf_controls::property_container				wpf_property_container;

public:
	job_behaviour_light_static( object_job^ parent_job );

	virtual void load( configs::lua_config_value const& config ) override;

	virtual void save( configs::lua_config_value& config ) override;

	virtual void get_properties	( wpf_property_container^ to_container )	override;

	[DisplayNameAttribute("Enabled")]
	property bool enabled;	
	[DisplayNameAttribute("Range")]
	property float range;	
	[DisplayNameAttribute("Intensity")]
	property float intensity;	
	[DisplayNameAttribute("Color")]
	property wpf_controls::color_rgb color;	



}; // class job_behaviour_light_static

} // namespace editor
} // namespace xray


#endif // #ifndef JOB_BEHAVIOUR_LIGHT_STATIC_H_INCLUDED