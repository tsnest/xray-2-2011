////////////////////////////////////////////////////////////////////////////
//	Created		: 06.06.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "job_behaviour_light_anim.h"
#include "logic_event_property_holder.h"
#include "logic_event_properties_container.h"
#include "object_light.h"
#include "object_job.h"
#include "project_items.h"

namespace xray {
namespace editor {

job_behaviour_light_anim::job_behaviour_light_anim( object_job^ parent_job )
:job_behaviour_base(parent_job)
{

	m_animation_length = 1.f;

	object_light^ jobs_selected_object = (parent_job->selected_job_resource != nullptr) ? safe_cast< object_light^ >( parent_job->selected_job_resource->get_object( ) ) : nullptr ;
	
	wpf_controls::color_rgb			default_color				= wpf_controls::color_rgb( 1.f, 1.f, 1.f, 1.f);
	wpf_controls::float_curve_key^	default_intensity_value 	= gcnew wpf_controls::float_curve_key();
	wpf_controls::float_curve_key^	default_range_value			= gcnew wpf_controls::float_curve_key();

	if ( jobs_selected_object != nullptr )
	{
		xrayColor selected_color = jobs_selected_object->color;
		default_color = wpf_controls::color_rgb(selected_color.r,selected_color.g,selected_color.b, 1.0f);
		default_intensity_value->position_y = jobs_selected_object->intensity;
		default_range_value->position_y		= jobs_selected_object->range;
	}

	color_curve				= gcnew wpf_controls::color_curve();
	color_curve->keys->Add( gcnew wpf_controls::color_curve_key( 0.f, default_color ) );
	color_curve->max_time	= length;
	
	intensity_and_range_list = gcnew Generic::List<wpf_controls::float_curve^>();

	wpf_controls::float_curve^ intensity_curve		= gcnew wpf_controls::float_curve();
	intensity_curve->name = "Intensity";
	intensity_curve->color = System::Windows::Media::Colors::Red;
	intensity_curve->keys->Add(default_intensity_value);
	intensity_and_range_list->Add(intensity_curve);

	wpf_controls::float_curve^ range_curve			= gcnew wpf_controls::float_curve();
	range_curve->keys->Add(default_range_value);
	range_curve->name	= "Range";
	range_curve->color = System::Windows::Media::Colors::Blue;
	intensity_and_range_list->Add(range_curve);
}

System::Collections::Generic::List<System::String^>^  job_behaviour_light_anim::get_available_events_list( )
{
	System::Collections::Generic::List<System::String^>^ ret_lst = job_behaviour_base::get_available_events_list();

	ret_lst->Insert( 0, "animation_end" );

	return ret_lst;
}

void job_behaviour_light_anim::load			( configs::lua_config_value const& config )
{
	job_behaviour_base::load(config);

	cycled		=	config["cycled"];
	length		= 	config["length"];

	intensity_and_range_list->Clear();

	configs::lua_config_value curve_config = config[ "Intensity" ];
	wpf_controls::float_curve^ intensity_curve = editor_base::curves_serializer::load_float_curve_from_config( curve_config );
	intensity_curve->name = "Intensity";
	intensity_curve->color = System::Windows::Media::Colors::Red;
	intensity_and_range_list->Add(intensity_curve);

	curve_config = config[ "Range" ];
	wpf_controls::float_curve^ range_curve = editor_base::curves_serializer::load_float_curve_from_config( curve_config );
	range_curve->name	= "Range";
	range_curve->color = System::Windows::Media::Colors::Blue;
	intensity_and_range_list->Add(range_curve);

	curve_config = config[ "color_curve" ];
	color_curve = editor_base::curves_serializer::load_color_curve_from_config( curve_config );
	color_curve->max_time = length;
	
}

void job_behaviour_light_anim::save( configs::lua_config_value& config )
{
	job_behaviour_base::save(config);

	config["cycled"] = cycled;
	config["length"] = length;	
	for each( wpf_controls::float_curve^ curve in intensity_and_range_list )
	{
		configs::lua_config_value curve_config = config[ unmanaged_string( curve->name ).c_str( ) ];
		editor_base::curves_serializer::synchronize_float_curve_config( curve, curve_config );
	}

	configs::lua_config_value color_curve_config = config[ "color_curve" ];	
	editor_base::curves_serializer::synchronize_color_curve_config( color_curve, color_curve_config );	
}

void job_behaviour_light_anim::get_properties	( wpf_property_container^ to_container )
{
	property_descriptor^ prop_descriptor = gcnew property_descriptor( this, "cycled" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "length" );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "color_curve" );
	wpf_controls::property_editors::attributes::color_curve_editor_attribute^ color_curve_attr = gcnew wpf_controls::property_editors::attributes::color_curve_editor_attribute();
	on_length_changed += gcnew System::Action<System::Double>( color_curve_attr, &wpf_controls::property_editors::attributes::color_curve_editor_attribute::set_limit ) ;
	prop_descriptor->dynamic_attributes->add( color_curve_attr );

	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	prop_descriptor = gcnew property_descriptor( this, "intensity_and_range_list" );
	
	wpf_controls::property_editors::attributes::float_curve_editor_attribute^ float_curve_attr = gcnew wpf_controls::property_editors::attributes::float_curve_editor_attribute();
	on_length_changed += gcnew System::Action<System::Double>( float_curve_attr, &wpf_controls::property_editors::attributes::float_curve_editor_attribute::set_right_limit ) ;
	float_curve_attr->initial_right_limit	= length;
	float_curve_attr->initial_bottom_limit	= 0;
	float_curve_attr->initial_left_limit	= 0;
		
	prop_descriptor->dynamic_attributes->add( float_curve_attr );
	prop_descriptor->dynamic_attributes->add( gcnew editor::wpf_controls::hypergraph::node_property_attribute( false, false ));	
	to_container->properties->add(prop_descriptor);

	job_behaviour_base::get_properties( to_container );
}



} // namespace editor
} // namespace xray
