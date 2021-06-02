////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_light_static.h"
#include "object_light.h"
#include "object_job.h"
#include "project_items.h"

namespace xray {
namespace editor {

job_behaviour_light_static::job_behaviour_light_static( object_job^ parent_job )
:job_behaviour_base(parent_job)
{
	object_light^ jobs_selected_object = (parent_job->selected_job_resource != nullptr) ? safe_cast< object_light^ >( parent_job->selected_job_resource->get_object( ) ) : nullptr ;

	enabled		= true;
	color		= wpf_controls::color_rgb(1.f,1.f,1.f,1.f);
	intensity 	= 5;
	range		= 5;

	if ( jobs_selected_object != nullptr )
	{
		xrayColor	selected_color = jobs_selected_object->color;
		color		= wpf_controls::color_rgb(selected_color.r,selected_color.g,selected_color.b, 1.0f);
		intensity	= jobs_selected_object->intensity;
		range		= jobs_selected_object->range;
	}
}

void job_behaviour_light_static::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);
	enabled		= 	config["enabled"];
	
	if (config.value_exists("range"))
		range		= 	config["range"];

	if (config.value_exists("intensity"))
		intensity	= 	config["intensity"];
	
	if (config.value_exists("color"))
	{
		math::float4 float_color = config["color"];
		color		= 	wpf_controls::color_rgb( float_color.x, float_color.y, float_color.z, 1.0f );
	}	
}

void job_behaviour_light_static::save( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);
	config["enabled"]	= enabled;
	config["range"]		= range;
	config["intensity"] = intensity;
	config["color"]		= math::float4( (float)(color.r), (float)(color.g), (float)(color.b), 1.0f );
}

void job_behaviour_light_static::get_properties	( wpf_property_container^ to_container )
{
	wpf_controls::property_descriptor^ prop_descriptor = gcnew wpf_controls::property_descriptor( this, "enabled" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew wpf_controls::property_descriptor( this, "range" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew wpf_controls::property_descriptor( this, "intensity" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew wpf_controls::property_descriptor( this, "color" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	job_behaviour_base::get_properties( to_container );
}


} // namespace editor
} // namespace xray
