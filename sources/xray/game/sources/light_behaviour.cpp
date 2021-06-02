////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "light_behaviour.h"
#include "object_scene.h"

namespace stalker2{

light_static_behaviour::light_static_behaviour( object_scene_job* owner, 
												configs::binary_config_value const& data, 
												pcstr name )
:super	( owner, data, name )
{
	m_state = data["enabled"];
	
	if ( m_state )
	{
		xray::math::float4 float_color = data["color"];
		m_light_color		= math::color_rgba(float_color.x, float_color.y, float_color.z, 1.0f);
		m_light_range		= data["range"];
		m_light_intensity	= data["intensity"];
	}
}

void light_static_behaviour::attach_to_object( object_controlled* o )
{
	super::attach_to_object		( o );
	m_light						= static_cast_checked<object_light*>( o );
	if(m_state)
	{
		m_light->add_to_scene	( );
		render::light_props& props	= m_light->props();
		props.intensity				= m_light_intensity;
		props.range					= m_light_range;
		props.color					= m_light_color;
		m_light->update_props		( ); 
	}
	else
	{
		m_light->remove_from_scene( );
	}
}

void light_static_behaviour::detach_from_object( object_controlled* o )
{
	super::detach_from_object( o );
}

light_anim_behaviour::light_anim_behaviour( object_scene_job* owner, 
											configs::binary_config_value const& data, 
											pcstr name )
:super					( owner, data, name ),
m_animation_end_raised	( false )
{
	m_track_intensity.load	( data["Intensity"] );
	m_track_range.load		( data["Range"] );
	m_color_anim.load		( data["color_curve"] );
	m_bcyclic				= data["cycled"];
	m_track_length			= data["length"];
}

void light_anim_behaviour::attach_to_object( object_controlled* o )
{
	super::attach_to_object		( o );
	m_light						= static_cast_checked<object_light*>( o );
	m_light->add_to_scene		( );

	m_initial_light_props		= m_light->props();
	m_animation_end_raised		= false;
	m_timer.start				( );
}

void light_anim_behaviour::detach_from_object( object_controlled* o )
{
	render::light_props& props	= m_light->props();
	props						= m_initial_light_props;
	m_light->update_props		( );

	super::detach_from_object	( o );
}

void light_anim_behaviour::tick( )
{
	float const time	= m_timer.get_elapsed_sec();

	float time_current	= (m_bcyclic) ? fmod(time, m_track_length) : time;
	
	float intensity		= m_track_intensity.evaluate( time_current, 0.0f);
	float range			= m_track_range.evaluate( time_current, 0.0f);

	xray::math::float4 float_color = m_color_anim.evaluate( time_current, xray::math::float4( 0.f, 0.f, 0.f, 0.f ) );
	
	render::light_props& props	= m_light->props();
	props.intensity				= intensity;
	props.range					= range;
	props.color					=  math::color_rgba(float_color.x, float_color.y, float_color.z, 1.0f);

	m_light->update_props		( );
	if(!m_bcyclic && time > m_track_length )
	{
		if(!m_animation_end_raised)
		{
			game_event ev("animation_end");
			m_owner->get_event_manager()->emit_event	( ev, m_light.c_ptr() );
			m_animation_end_raised = true;
		}
	}

}

}